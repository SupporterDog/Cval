#include "all_header.h"

int main() {

    int my_x = 4; int my_y = 4;
    int opp_x = 2; int opp_y = 3;
    int count = 0;
    Point* my_point = &(Point) { my_x, my_y };
    Point* opp_point = &(Point) { opp_x, opp_y };
    Point* reachable_points = Bangaljook(opp_x, opp_y, my_x, my_y, &count);  
    Point* max_score_point = Find_MaxScorePoint(my_point, reachable_points, count); 
    printf("Max Point : row (%d), col (%d)", max_score_point->x, max_score_point->y);
    Point* local_optimal_path = find_best_road(my_point, max_score_point, &path_length);
    printf("Optimal Path: ");
    for (int i = 0; i < path_length; ++i) {
        printf("(row: %d, col: %d) \t", local_optimal_path[i].x, local_optimal_path[i].y);
    }
    printf("\n");
    int* Directions = getDirection(local_optimal_path, path_length);
    int* Dirs_for_Movs = getDirection_for_Mov(Directions, path_length, RECENT_HEAD_DIRECTION);
    pMovements =  getMovement(Dirs_for_Movs, path_length);
    printf("Your moves: \n");
    for (int i = 0; i < path_length - 1; ++i) {
        printf("(%d) \t", pMovements[i]);
    }
    printf("\n");
    int RECENT_HEAD_DIRECTION = Directions[path_length - 2];
    printf("Recent head direction: %d \n", RECENT_HEAD_DIRECTION);
    met_Node = 0;

    free(reachable_points);
    free(Directions);
    free(Dirs_for_Movs);

    printf("Now Let's Check if the Car Actually Moves Well....");

    YB_Pcb_Car car;
    get_i2c_device(&car, I2C_ADDR);
    Car_Stop(&car);
    delay(10000);
    line_following(&car);


    return 0;
}
