void line_following(YB_Pcb_Car* car) {
    int left1, left2, right1, right2;

    while (1) {
        left1 = read_sensor(SENSOR_LEFT1);
        left2 = read_sensor(SENSOR_LEFT2);
        right1 = read_sensor(SENSOR_RIGHT1);
        right2 = read_sensor(SENSOR_RIGHT2);

        int sensor_state = (left1 << 3) | (left2 << 2) | (right2 << 1) | right1;

        switch (sensor_state) {
            case 0b1001:  // (H L L H) : 앞으로 직진
                Car_Run(car, 70, 70);
                break;
            case 0b1101:  // (H H L H): (H L L H ) 될때까지 조금씩 우회전하기
                while ((left1 == HIGH) && (left2 == HIGH) && (right2 == LOW) && (right1 == HIGH)) {
                    Car_Right(car, 50, 70);
                    delay(100);
                    left1 = read_sensor(SENSOR_LEFT1);
                    left2 = read_sensor(SENSOR_LEFT2);
                    right1 = read_sensor(SENSOR_RIGHT1);
                    right2 = read_sensor(SENSOR_RIGHT2);
                }
                break;
            case 0b1110:  // (H H H L): (H H L H) 될때까지 조금씩 우회전하기
                while ((left1 == HIGH) && (left2 == HIGH) && (right2 == HIGH) && (right1 == LOW)) {
                    Car_Right(car, 50, 70);
                    delay(100);
                    left1 = read_sensor(SENSOR_LEFT1);
                    left2 = read_sensor(SENSOR_LEFT2);
                    right1 = read_sensor(SENSOR_RIGHT1);
                    right2 = read_sensor(SENSOR_RIGHT2);
                }
                break;
            case 0b1100:  // (H H L L), (H H H L) : 로봇 본체의 몸통 중간 까지 직진후 90도 우회전하기
            case 0b1110:
                Car_Run(car, 70, 70);
                delay(500);  // 로봇의 몸통 중간까지 직진
                Car_Right(car, 70, 70);  // 90도 우회전
                delay(1000);
                break;
            default:
                Car_Stop(car);
                break;
        }
        delay(100);
    }
}
