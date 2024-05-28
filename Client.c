
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "client.h"

int sock;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* receiveUpdates(void* arg) {
    DGIST dgist;
    while (1) {
        pthread_mutex_lock(&lock);
        if (recv(sock, &dgist, sizeof(DGIST), 0) <= 0) {
            printf("Connection closed by server.\n");
            close(sock);
            exit(0);
        }
        pthread_mutex_unlock(&lock);

        // 글로벌 변수 업데이트
        updateGlobalVariables(&dgist);
    }
    return NULL;
}

// 글로벌 변수 선언
Node globalMap[MAP_ROW][MAP_COL];
client_info globalPlayers[MAX_CLIENTS];


//좌표축과 아이템 점수를 업데이트
void* updateGlobalVariables(DGIST* dgist) {
    pthread_mutex_lock(&lock);
    // 맵 업데이트
   int index = 0;
    // 맵 업데이트
    for (int i = 0; i < MAP_ROW; i++) {
        for (int j = 0; j < MAP_COL; j++) {
            globalMap[i][j] = dgist->map[i][j];
        }
    }

    // 플레이어 정보 업데이트
    for (int i = 0; i < MAX_CLIENTS; i++) {
        globalPlayers[i] = dgist->players[i];
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}


// 클라이언트 액션을 서버로 전송하는 함수 ex: "11",1 -->(1,1)이동 , 폭탄 설치
void sendClientAction(int sock, pthread_mutex_t* lock, const char* coordinates, int action) {
    ClientAction clientAction;

    // 좌표 문자열을 파싱하여 row와 col로 변환
    if (sscanf(coordinates, "%1d%1d", &clientAction.row, &clientAction.col) != 2) {
        printf("Invalid coordinates format.\n");
        return;
    }

    clientAction.action = action;

    pthread_mutex_lock(lock);
    if (send(sock, &clientAction, sizeof(ClientAction), 0) <= 0) {
        printf("Failed to send action to server.\n");
    }
    pthread_mutex_unlock(lock);
}

// 클라이언트 프로그램의 진입점
int main(int argc, char *argv[]) {
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

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receiveUpdates, NULL);

    while (1) {
        char coordinates[3];
        int action;
        action=0;
        coordinates[0]='3';
        coordinates[1]='3';
        coordinates[2]='\n';
        sendClientAction(sock, &lock, coordinates, action);
    }
   

    close(sock);
    return 0;
}
