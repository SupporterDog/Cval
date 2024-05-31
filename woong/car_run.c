
#include "client.h"
// C++ 함수 선언 : qr_decoder.cpp를 쓰기 위함
#ifdef __cplusplus
extern "C" {
#endif

const char* decodeQRCodeFromCamera();

#ifdef __cplusplus
}
#endif


int main(int argc, char *argv[]) {
    // 클라이언트 프로그램의 진입점
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP> <PORT>\n", argv[0]);
        return 1;
    }

    const char* server_ip = argv[1];
    int server_port = atoi(argv[2]);

    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    printf("Connected to server %s:%d\n", server_ip, server_port);
    //SERVER CONNECTION 완료

    //Thread1 for 정보수신: update DGIST객체--> MAP and ENEMY
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receiveUpdates, NULL);
    pthread_join(recv_thread, NULL);
    /*
    //THread2 for CAR_RUN and CAMERA_MOVE
    pthread_t run_thread;
    pthread_create(&run_thread,NULL,,NULL);

    //Thread3 for claculating MAX_POINT and PATH
    pthread_t cal_thread;
    pthread_create(&cal_thread,NULL,NULL,NULL);

    //Thread4 for QR_CODE
    pthread_t qr_thread;
    pthread_create(&qr_thread,NULL,NULL,NULL);
    */

    //socketclose
    close(sock);
    return 0;
}
