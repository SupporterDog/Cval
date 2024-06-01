#include "track.h"
#include "alg.h"

int main() {
    if (wiringPiSetup() == -1) {
        printf("wiringPi setup failed\n");
        return 1;
    }

    pinMode(SENSOR_LEFT1, INPUT);
    pinMode(SENSOR_LEFT2, INPUT);
    pinMode(SENSOR_RIGHT1, INPUT);
    pinMode(SENSOR_RIGHT2, INPUT);

    YB_Pcb_Car car;
    get_i2c_device(&car, I2C_ADDR);

       srand(time(NULL));


    int control = 1;
    // Initialize the DGIST_OBJ map with some example scores
    for (int i = 0; i < MAP_ROW; ++i) {
        for (int j = 0; j < MAP_COL; ++j) {
            DGIST_OBJ.map[i][j].item.score = rand() % (MAX_SCORE + 1); // Random score between 0 and MAX_SCORE
        }
    }

    for (int num = 0; num < 10; ++num) {
        int opp_x = rand() % (MAP_COL), opp_y = rand() % (MAP_ROW); // Example opponent coordinates
        int my_x = rand() % (MAP_COL), my_y = rand() % (MAP_ROW); // Example my coordinates
    
        Point* my_point = &(Point) { my_x, my_y };
        int count;

        // 가장 쵝근 대가리 방향 설정
        int RECENT_HEAD_DIRECTION = (my_x == 0) ? DOWN : UP;

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
    }
    printf("Starting line following...\n");
    line_following(&car);

    return 0;
}