#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>

#define MAX_CLIENTS 2
#define _MAP_ROW 4
#define _MAP_COL 4
#define MAP_ROW (_MAP_ROW + 1)
#define MAP_COL (_MAP_COL + 1)
#define MAP_SIZE (MAP_COL * MAP_ROW)
#define MAX_QUEUE_SIZE (MAP_ROW * MAP_COL)

#define LEFT 1
#define UP 2
#define RIGHT 3
#define DOWN 4

#define l_spin 1
#define straight 2
#define r_spin 3
#define turn 4

const int MAX_SCORE = 4; // Item max score
const int SETTING_PERIOD = 20; //Boradcast & Item generation period
const int INITIAL_ITEM = 10; //Initial number of item
const int INITIAL_BOMB = 4; //The number of bomb for each user
const int SCORE_DEDUCTION = 2; //The amount of score deduction due to bomb

bool do_we_set_trap = false;

typedef struct {
    int socket;
    struct sockaddr_in address;
    int row;
    int col;
    int score;
    int bomb;
} client_info;

enum Status {
    nothing, //0
    item, //1
    trap //2
};

typedef struct {
    enum Status status;
    int score;
} Item;

typedef struct {
    int row;
    int col;
    Item item;
} Node;

typedef struct {
    client_info players[MAX_CLIENTS];
    Node map[MAP_ROW][MAP_COL];
} DGIST;
DGIST DGIST_OBJ;

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point points[_MAP_ROW * _MAP_COL];
    int length;
    int score;
} Path;

typedef struct {
    Point point;
    int distance;
} QueueNode;

//방문한 교차점에 함정을 설치하고 싶으면 1, 그렇지 않으면 0으로 설정하면 돼요.
enum Action {
    move, //0
    setBomb, //1
};

//서버에게 소켓을 통해 전달하는 구조체에요.
//QR에서 읽어온 숫자 2개를 row, col에 넣고 위의 enum Action을 참고해서 action 값을 설정하세요.
typedef struct {
    int row;
    int col;
    enum Action action;
} ClientAction;

Point* Bangaljook(int opp_x, int opp_y, int my_x, int my_y, int* count) {
    static Point all_points[25] = {
        {0,0}, {0,1}, {0,2}, {0,3}, {0,4},
        {1,0}, {1,1}, {1,2}, {1,3}, {1,4},
        {2,0}, {2,1}, {2,2}, {2,3}, {2,4},
        {3,0}, {3,1}, {3,2}, {3,3}, {3,4},
        {4,0}, {4,1}, {4,2}, {4,3}, {4,4}
    };

    float bangal_x = (opp_x + my_x) / 2.0;
    float bangal_y = (opp_y + my_y) / 2.0;
    float geeoolgii;

    if (my_x == opp_x && my_y == opp_y) {
        *count = 25; // Return all points if the coordinates are the same
        Point* temp_points = (Point*)malloc(25 * sizeof(Point));
        for (int i = 0; i < 25; ++i) {
            temp_points[i] = all_points[i];
        }
        return temp_points;
    }
    else if (my_x == opp_x) {
        geeoolgii = 10000; // Vertical bisector slope is infinite
    }
    else if (my_y == opp_y) {
        geeoolgii = 0; // Horizontal bisector slope is 0
    }
    else {
        float dx = my_x - opp_x;
        float dy = my_y - opp_y;
        geeoolgii = -dx / dy;
    }

    int my_part = (my_y >= geeoolgii * (my_x - bangal_x) + bangal_y) ? 1 : -1;
    Point* return_points = (Point*)malloc(25 * sizeof(Point));
    *count = 0;

    for (int i = 0; i < 25; ++i) {
        int x = all_points[i].x;
        int y = all_points[i].y;
        int part = (y >= geeoolgii * (x - bangal_x) + bangal_y) ? 1 : -1;
        // 현재 위치는 포함시키지 않는다
        if (part == my_part && (x != my_x || y != my_y)) {
            return_points[(*count)++] = all_points[i];
        }
    }

    return_points = (Point*)realloc(return_points, (*count) * sizeof(Point));
    return return_points;
}

bool isValid(Point p, Point* points, int count) {
    if (p.x < 0 || p.y < 0 ) {return false;}
    if (p.x > MAP_ROW || p.y > MAP_COL) { return false; }
    for (int i = 0; i < count ; ++i) {
        if (points[i].x == p.x && points[i].y == p.y) { return true; }
    }
    return false;
}

// StartPoint로부터 BFS 를 통해 points 배열에 있는 포인트 struct 중 가까운 점부터 차례대로 방문하여 주어진 지점의 점수가 4면 해당 지점을 즉시 반환하고 
// 그렇지 않으면 최고 점수를 가진 지점을 반환하는 함수
Point* Find_MaxScorePoint(Point* StartPoint, Point* points, int count) {
    Point directions[4] = { {0,1}, {1,0}, {0,-1}, {-1,0} };
    bool visited[MAP_ROW][MAP_COL] = { false };
    QueueNode queue[MAX_QUEUE_SIZE];
    int front = 0, rear = 0;

    queue[rear++] = (QueueNode){ *StartPoint, 0 };
    visited[(*StartPoint).x][(*StartPoint).y] = true;

    Point* returnpoint = NULL;
    int currmaxscore = -1;

    while (front < rear) {
        QueueNode current = queue[front++];
        Point* currpoint = &(current.point);

        // 점수가 4면 해당 지점을 반환
        if (DGIST_OBJ.map[currpoint->x][currpoint->y].item.score == MAX_SCORE) {
            if ( currpoint->x == StartPoint->x && currpoint->y == StartPoint->y ) {}
            else { return currpoint; }
        }

        // 현재 점수가 최고 점수보다 크면 갱신
        if (DGIST_OBJ.map[currpoint->x][currpoint->y].item.score > currmaxscore) {
            returnpoint = currpoint;
            currmaxscore = DGIST_OBJ.map[currpoint->x][currpoint->y].item.score;
        }

        // 다음 지점을 큐에 추가
        for (int i = 0; i < 4; ++i) {
            Point nextpoint = { currpoint->x + directions[i].x, currpoint->y + directions[i].y };
            if (isValid(nextpoint, points, count) && !visited[nextpoint.x][nextpoint.y]) {
                visited[nextpoint.x][nextpoint.y] = true;
                queue[rear++] = (QueueNode){ nextpoint, current.distance + 1 };
            }
        }
    }
    // 점수가 4인 게 없다면 최대 점수를 가진 포인트를 반환한다
    return returnpoint;
}


//포인터 복사
void copy_path(Path* dest, Path* src) {
    dest->length = src->length;
    dest->score = src->score;
    memcpy(dest->points, src->points, src->length * sizeof(Point));
}
//최적의 경로 탐색해서 best_path에 저장
void find_paths(int row_moves, int column_moves, Path* path, int current_score, Path* best_path, int start_x, int start_y) {
    int max_score = -10;
    if (row_moves == 0 && column_moves == 0) {
        // 경로가 완성된 경우
        if (current_score > max_score) {
            max_score = current_score;
            copy_path(best_path, path);
        }
    }
    //x방향 양의 이동
    if (row_moves > 0) {
        (*path).points[(*path).length] = (Point){ (*path).points[(*path).length - 1].x + 1, (*path).points[(*path).length - 1].y };
        (*path).length++;
        start_x++;
        find_paths(row_moves - 1, column_moves, path, current_score + DGIST_OBJ.map[start_x][start_y].item.score, best_path, start_x, start_y);
        (*path).length--;
        start_x--;
    }
    //y방향 양의 이동
    if (column_moves > 0) {
        (*path).points[(*path).length] = (Point){ (*path).points[(*path).length - 1].x, (*path).points[(*path).length - 1].y + 1 };
        (*path).length++;
        start_y++;
        find_paths(row_moves, column_moves - 1, path, current_score + DGIST_OBJ.map[start_x][start_y].item.score, best_path, start_x, start_y);
        (*path).length--;
        start_y--;
    }
    //x방향 음의 이동
    if (row_moves < 0) {
        (*path).points[(*path).length] = (Point){ (*path).points[(*path).length - 1].x - 1, (*path).points[(*path).length - 1].y };
        (*path).length++;
        start_x--;
        find_paths(row_moves + 1, column_moves, path, current_score + DGIST_OBJ.map[start_x][start_y].item.score, best_path, start_x, start_y);
        (*path).length--;
        start_x++;
    }
    //y방향 음의 이동
    if (column_moves < 0) {
        (*path).points[(*path).length] = (Point){ (*path).points[(*path).length - 1].x, (*path).points[(*path).length - 1].y - 1 };
        (*path).length++;
        start_y--;
        find_paths(row_moves, column_moves + 1, path, current_score + DGIST_OBJ.map[start_x][start_y].item.score, best_path, start_x, start_y);
        (*path).length--;
        start_y++;
    }
}

Point* find_best_road(Point* StartPoint, Point* EndPoint, int* path_length) {
    int start_x = (*StartPoint).x;
    int start_y = (*StartPoint).y;
    int end_x = (*EndPoint).x;
    int end_y = (*EndPoint).y;
    int row_move = end_x - start_x;
    int column_move = end_y - start_y;
    Path best_path;
    Path initial_path;
    initial_path.length = 1;
    initial_path.points[0] = *StartPoint;
    initial_path.score = 0;
    //경로 찾아서 best_path에 저장
    find_paths(row_move, column_move, &initial_path, 0, &best_path, start_x, start_y);
    *path_length = best_path.length;
    //best_path의 정보를 Point*로 변환
    Point* highest = (Point*)malloc((abs(row_move) + abs(column_move) + 1) * sizeof(Point));
    for (int i = 0; i < abs(row_move) + abs(column_move) + 1;i++) {
        highest[i] = best_path.points[i];
    }
    return highest;
}

int* getDirection(Point* road, int path_length) {
    int* returnvec = (int*)malloc((path_length - 1) * sizeof(int));
    if (returnvec == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < path_length - 1; ++i) {
        Point currpoint = road[i];
        Point nextpoint = road[i + 1];
        if (currpoint.x == nextpoint.x && nextpoint.y == currpoint.y + 1) {
            returnvec[i] = RIGHT;
        }
        else if (currpoint.x == nextpoint.x && nextpoint.y == currpoint.y - 1) {
            returnvec[i] = LEFT;
        }
        else if (currpoint.y == nextpoint.y && nextpoint.x == currpoint.x + 1) {
            returnvec[i] = DOWN;
        }
        else if (currpoint.y == nextpoint.y && nextpoint.x == currpoint.x - 1) {
            returnvec[i] = UP;
        }
    }
    return returnvec;
}

int* getDirection_for_Mov(int* Dir, int path_length, int recent_head_dir) {
    int* returnvec = (int*)malloc(sizeof(int) * (path_length));
    returnvec[0] = recent_head_dir;
    for (int i = 1; i < path_length; ++i) {
        returnvec[i] = Dir[i - 1]; 
    }
    return returnvec;
}

int* getMovement(int* dirs_for_movs, int path_length) {
    int* returnvec = malloc(sizeof(int) * (path_length - 1));
    for (int i = 0; i < path_length - 1; ++i) {
        int firstdir = dirs_for_movs[i];
        int seconddir = dirs_for_movs[i + 1];
        if (firstdir == LEFT) {
            if (seconddir == UP) { returnvec[i] = r_spin; }
            else if (seconddir == DOWN) { returnvec[i] = l_spin; }
            else if (seconddir == LEFT) { returnvec[i] = straight; }
            else if (seconddir == RIGHT) { returnvec[i] = turn; }
        }
        else if (firstdir == UP) {
            if (seconddir == UP) { returnvec[i] = straight; }
            else if (seconddir == DOWN) { returnvec[i] = turn; }
            else if (seconddir == LEFT) { returnvec[i] = l_spin; }
            else if (seconddir == RIGHT) { returnvec[i] = r_spin; }
        }
        else if (firstdir == RIGHT) {
            if (seconddir == UP) { returnvec[i] = l_spin; }
            else if (seconddir == DOWN) { returnvec[i] = r_spin; }
            else if (seconddir == LEFT) { returnvec[i] = turn; }
            else if (seconddir == RIGHT) { returnvec[i] = straight; }
        }
        else if (firstdir == DOWN) {
            if (seconddir == UP) { returnvec[i] = turn; }
            else if (seconddir == DOWN) { returnvec[i] = straight; }
            else if (seconddir == LEFT) { returnvec[i] = r_spin; }
            else if (seconddir == RIGHT) { returnvec[i] = l_spin; }
        }
    }
    return returnvec;
}


// 내 포인트와 상대 포인트, 내가 다음에 갈 포인트를 바탕으로 폭탄을 세팅할지 말지를 알 수 있다.
int SetBomb_Checker(Point* currpoint, Point* opponentpoint) {
    
    Point directions[4] = { {0,1}, {1,0}, {0,-1}, {-1,0} };
    for (int i = 0; i < 4; ++i ) {
        Point* nextpoint = &(Point) {currpoint->x + directions[i].x, currpoint->y + directions[i].y};
        if (DGIST_OBJ.map[(*nextpoint).x][(*nextpoint).y].item.score == 4) {
            if (i == 0 || i == 2) {
                if ((*currpoint).x == (*nextpoint).x && (*nextpoint).x == (*opponentpoint).x && (*opponentpoint).y - (*currpoint).y == (*currpoint).y - (*nextpoint).y) {
                    return 1;
                }
            }
            if (i == 1 || i == 3) {
                if ((*currpoint).y == (*nextpoint).y && (*nextpoint).y == (*opponentpoint).y && (*opponentpoint).x - (*currpoint).x == (*currpoint).x - (*nextpoint).x) {
                    return 1;
                }
            }
        }
    }
    return 0;

}


int main() {

    int opp_x; 
    int opp_y;
    int my_x;
    int my_y;

    Point* my_point;
    int count;
    // 가장 쵝근 대가리 방향 설정
    int RECENT_HEAD_DIRECTION;
    // 상대보다 빨리 접근 가능한 좌표들
    Point* reachable_points = Bangaljook(opp_x, opp_y, my_x, my_y, &count);
    // 맥스 스코어 포인트
    Point* max_score_point = Find_MaxScorePoint(&(Point) { my_x, my_y }, reachable_points, count);
    // 맥스 스코어 포인트로 가는 옵티멀 길 찾기
    int path_length;
    Point* local_optimal_path;
    local_optimal_path = find_best_road(my_point, max_score_point, &path_length);
    // Direction---------------------------------------
    int* Directions;
    Directions = getDirection(local_optimal_path, path_length);
    // Direction for Moves(Add Recent_direction in front of Directions) ----------------------------
    int* Dirs_for_Movs; 
    Dirs_for_Movs = getDirection_for_Mov(Directions, path_length, RECENT_HEAD_DIRECTION);
    // Find the nex Movements for the given best path --------------------------
    int* Movements;
    Movements = getMovement(Dirs_for_Movs, path_length);
    // 마지막 대가리 방향 업데이트 ----------------------------------
    RECENT_HEAD_DIRECTION = Directions[path_length - 2];
    
    free(reachable_points);
    free(Directions);
    free(Dirs_for_Movs);
    free(Movements);

    return 0;
}
