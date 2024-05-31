#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <string.h>
#define MAX_CLIENTS 2

#define _MAP_ROW 4
#define _MAP_COL 4
#define MAP_ROW (_MAP_ROW + 1)
#define MAP_COL (_MAP_COL + 1)
#define MAP_SIZE (MAP_COL * MAP_ROW)
#define MAX_QUEUE_SIZE (MAP_ROW * MAP_COL)

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


bool isValid(Point p);

Point* Bangaljook(int opp_x, int opp_y, int my_x, int my_y, int* count);

Point* Find_MaxScorePoint(Point* StartPoint, Point* points, int count);

void copy_path(Path* dest, Path* src);


void find_paths(int row_moves, int column_moves, Path* path, int current_score, Path* best_path, int start_x, int start_y);

Point* find_best_road(Point* StartPoint, Point* EndPoint, int* path_length);

bool SetBomb_Checker(Point* currpoint, Point* opponentpoint);
