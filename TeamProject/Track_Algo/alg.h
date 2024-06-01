#ifndef ALG_H
#define ALG_H

#include <stdbool.h>
#include <netinet/in.h>

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

extern const int MAX_SCORE;
extern const int SETTING_PERIOD;
extern const int INITIAL_ITEM;
extern const int INITIAL_BOMB;
extern const int SCORE_DEDUCTION;

extern bool do_we_set_trap;

typedef struct {
    int socket;
    struct sockaddr_in address;
    int row;
    int col;
    int score;
    int bomb;
} client_info;

enum Status {
    nothing,
    item,
    trap
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
extern DGIST DGIST_OBJ;

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

enum Action {
    move,
    setBomb,
};

typedef struct {
    int row;
    int col;
    enum Action action;
} ClientAction;

Point* Bangaljook(int opp_x, int opp_y, int my_x, int my_y, int* count);
bool isValid(Point p, Point* points, int count);
Point* Find_MaxScorePoint(Point* StartPoint, Point* points, int count);
void copy_path(Path* dest, Path* src);
void find_paths(int row_moves, int column_moves, Path* path, int current_score, Path* best_path, int start_x, int start_y);
Point* find_best_road(Point* StartPoint, Point* EndPoint, int* path_length);
int* getDirection(Point* road, int path_length);
int* getDirection_for_Mov(int* Dir, int path_length, int recent_head_dir);
int* getMovement(int* dirs_for_movs, int path_length);
int SetBomb_Checker(Point* currpoint, Point* opponentpoint);

#endif // ALG_H
