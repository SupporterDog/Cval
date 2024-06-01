#ifndef ALL_HEADER_H
#define ALL_HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h> 
//For calpath
#include <stdbool.h>
#include <netinet/in.h>

#define MAX_QUEUE_SIZE (MAP_ROW * MAP_COL)

// 상수 정의
#define MAX_CLIENTS 2
#define _MAP_ROW 4
#define _MAP_COL 4
#define MAP_ROW (_MAP_ROW + 1)
#define MAP_COL (_MAP_COL + 1)
#define MAP_SIZE (MAP_COL * MAP_ROW)

#define LEFT 1
#define UP 2
#define RIGHT 3
#define DOWN 4

#define l_spin 1
#define straight 2
#define r_spin 3
#define turn 4

const int MAX_SCORE = 4; // Item max score
const int SETTING_PERIOD = 20; // Broadcast & Item generation period
const int INITIAL_ITEM = 10; // Initial number of item
const int INITIAL_BOMB = 4; // The number of bomb for each user
const int SCORE_DEDUCTION = 2; // The amount of score deduction due to bomb

// 클라이언트 정보 구조체 정의
typedef struct {
    int socket;
    struct sockaddr_in address;
    int row;
    int col;
    int score;
    int bomb;
} client_info;

// 아이템 상태 열거형 정의
enum Status {
    nothing, // 0
    item, // 1
    trap // 2
};

// 아이템 구조체 정의
typedef struct {
    enum Status status;
    int score;
} Item;

// 맵의 각 교차점을 나타내는 구조체 정의
typedef struct {
    int row;
    int col;
    Item item;
} Node;

// 전체 게임 상태를 나타내는 구조체 정의
typedef struct {
    client_info players[MAX_CLIENTS];
    Node map[MAP_ROW][MAP_COL];
} DGIST;

// 클라이언트 행동 열거형 정의
enum Action {
    move, // 0
    setBomb // 1
};

// 클라이언트 행동 구조체 정의
typedef struct {
    int row;
    int col;
    enum Action action;
} ClientAction;

// 함수 원형 정의
void* receiveUpdates(void* arg);

void updateGlobalVariables(DGIST* dgist,int my_sock);
void sendClientAction(int sock, pthread_mutex_t* lock, const char* coordinates, int action);

extern int sock;
extern pthread_mutex_t lock;
// 업데이트할 global DGIST object 변수 선언
extern DGIST* updatedDgist;


bool do_we_set_trap = false;


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

extern int my_index;
extern int met_Node;
extern int path_length;
extern int* pMovements;

bool isValid(Point p);

Point* Bangaljook(int opp_x, int opp_y, int my_x, int my_y, int* count);

Point* Find_MaxScorePoint(Point* StartPoint, Point* points, int count);

void copy_path(Path* dest, Path* src);


void find_paths(int row_moves, int column_moves, Path* path, int current_score, Path* best_path, int start_x, int start_y);

Point* find_best_road(Point* StartPoint, Point* EndPoint, int* path_length);

bool SetBomb_Checker(Point* currpoint, Point* opponentpoint);

void* Run_Algorithm(void* arg);

#endif // ALL_HEADER_H
