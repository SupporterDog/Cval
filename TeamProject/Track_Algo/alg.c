#include "alg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAX_SCORE = 4; // Item max score
const int SETTING_PERIOD = 20; // Boradcast & Item generation period
const int INITIAL_ITEM = 10; // Initial number of item
const int INITIAL_BOMB = 4; // The number of bomb for each user
const int SCORE_DEDUCTION = 2; // The amount of score deduction due to bomb

bool do_we_set_trap = false;

DGIST DGIST_OBJ;

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

        if (DGIST_OBJ.map[currpoint->x][currpoint->y].item.score == MAX_SCORE) {
            if ( currpoint->x == StartPoint->x && currpoint->y == StartPoint->y ) {}
            else { return currpoint; }
        }

        if (DGIST_OBJ.map[currpoint->x][currpoint->y].item.score > currmaxscore) {
            returnpoint = currpoint;
            currmaxscore = DGIST_OBJ.map[currpoint->x][currpoint->y].item.score;
        }

        for (int i = 0; i < 4; ++i) {
            Point nextpoint = { currpoint->x + directions[i].x, currpoint->y + directions[i].y };
            if (isValid(nextpoint, points, count) && !visited[nextpoint.x][nextpoint.y]) {
                visited[nextpoint.x][nextpoint.y] = true;
                queue[rear++] = (QueueNode){ nextpoint, current.distance + 1 };
            }
        }
    }
    return returnpoint;
}

void copy_path(Path* dest, Path* src) {
    dest->length = src->length;
    dest->score = src->score;
    memcpy(dest->points, src->points, src->length * sizeof(Point));
}

void find_paths(int row_moves, int column_moves, Path* path, int current_score, Path* best_path, int start_x, int start_y) {
    int max_score = -10;
    if (row_moves == 0 && column_moves == 0) {
        if (current_score > max_score) {
            max_score = current_score;
            copy_path(best_path, path);
        }
    }
    if (row_moves > 0) {
        (*path).points[(*path).length] = (Point){ (*path).points[(*path).length - 1].x + 1, (*path).points[(*path).length - 1].y };
        (*path).length++;
        start_x++;
        find_paths(row_moves - 1, column_moves, path, current_score + DGIST_OBJ.map[start_x][start_y].item.score, best_path, start_x, start_y);
        (*path).length--;
        start_x--;
    }
    if (column_moves > 0) {
        (*path).points[(*path).length] = (Point){ (*path).points[(*path).length - 1].x, (*path).points[(*path).length - 1].y + 1 };
        (*path).length++;
        start_y++;
        find_paths(row_moves, column_moves - 1, path, current_score + DGIST_OBJ.map[start_x][start_y].item.score, best_path, start_x, start_y);
        (*path).length--;
        start_y--;
    }
}

Point* find_best_road(Point* StartPoint, Point* EndPoint, int* path_length) {
    int row_moves = (*EndPoint).x - (*StartPoint).x;
    int column_moves = (*EndPoint).y - (*StartPoint).y;
    Path path, best_path;
    path.length = 1;
    path.score = DGIST_OBJ.map[(*StartPoint).x][(*StartPoint).y].item.score;
    path.points[0] = *StartPoint;

    find_paths(row_moves, column_moves, &path, path.score, &best_path, (*StartPoint).x, (*StartPoint).y);

    Point* return_points = (Point*)malloc(best_path.length * sizeof(Point));
    memcpy(return_points, best_path.points, best_path.length * sizeof(Point));
    *path_length = best_path.length;
    return return_points;
}

int* getDirection(Point* road, int path_length) {
    int* Dir = (int*)malloc((path_length - 1) * sizeof(int));
    for (int i = 1; i < path_length; ++i) {
        if (road[i].x - road[i-1].x == 1) Dir[i-1] = DOWN;
        if (road[i].x - road[i-1].x == -1) Dir[i-1] = UP;
        if (road[i].y - road[i-1].y == 1) Dir[i-1] = RIGHT;
        if (road[i].y - road[i-1].y == -1) Dir[i-1] = LEFT;
    }
    return Dir;
}

int* getDirection_for_Mov(int* Dir, int path_length, int recent_head_dir) {
    int* dirs_for_movs = (int*)malloc(path_length * sizeof(int));
    for (int i = 0; i < path_length - 1; ++i) {
        int turn = Dir[i] - recent_head_dir;
        if (turn == -3 || turn == 1) dirs_for_movs[i] = r_spin;
        else if (turn == 3 || turn == -1) dirs_for_movs[i] = l_spin;
        else dirs_for_movs[i] = straight;
        recent_head_dir = Dir[i];
    }
    dirs_for_movs[path_length - 1] = turn;
    return dirs_for_movs;
}

int* getMovement(int* dirs_for_movs, int path_length) {
    int* movs = (int*)malloc(path_length * sizeof(int));
    for (int i = 0; i < path_length; ++i) {
        if (dirs_for_movs[i] == l_spin) movs[i] = 1;
        else if (dirs_for_movs[i] == straight) movs[i] = 2;
        else if (dirs_for_movs[i] == r_spin) movs[i] = 3;
    }
    return movs;
}

int SetBomb_Checker(Point* currpoint, Point* opponentpoint) {
    int returnvalue = 0;
    if (abs(currpoint->x - opponentpoint->x) + abs(currpoint->y - opponentpoint->y) == 1) {
        returnvalue = 1;
    }
    return returnvalue;
}
