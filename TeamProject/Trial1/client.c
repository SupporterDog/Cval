#include "all_header.h"

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
        updateGlobalVariables(&dgist,sock);
        usleep(300000);
    }
    return NULL;
}



//좌표축과 아이템 점수를 업데이트
void updateGlobalVariables(DGIST* dgist,int my_sock) {
    pthread_mutex_lock(&lock);
    //DGIST객체 업데이트
    // 깊은 복사
    if (updatedDgist == NULL) {
        updatedDgist = malloc(sizeof(DGIST));
    }
    memcpy(updatedDgist, dgist, sizeof(DGIST));
	
    client_info client;

    printf("==========PRINT PLYAERS==========\n");
	for(int i=0; i < MAX_CLIENTS; i++){
		client = updatedDgist->players[i];
		printf("++++++++++Player %d++++++++++\n",i+1);
		printf("Location: (%d,%d)\n",client.row, client.col);
		printf("Score: %d\n",client.score);
		printf("Bomb: %d\n",client.bomb);
	}
	printf("==========PRINT DONE==========\n");
    pthread_mutex_unlock(&lock);
}


// 클라이언트 액션을 서버로 전송하는 함수
void sendClientAction(int sock, pthread_mutex_t* lock, const char* coordinates, int action) {
    ClientAction clientAction;

    // 좌표 문자열을 파싱하여 row와 col로 변환
    if (sscanf(coordinates, "%1d%1d", &clientAction.row, &clientAction.col) != 2) {
        printf("Invalid coordinates format.\n");
        return;
    }

    // action 값을 enum으로 변환하여 설정
    if (action == 0) {
        clientAction.action = move;
    } else if (action == 1) {
        clientAction.action = setBomb;
    } else {
        printf("Invalid action value.\n");
        return;
    }

    // 뮤텍스 잠금
    pthread_mutex_lock(lock);

    // 서버로 데이터 전송
    if (send(sock, &clientAction, sizeof(ClientAction), 0) <= 0) {
        printf("Failed to send action to server.\n");
    }

    // 뮤텍스 잠금 해제
    pthread_mutex_unlock(lock);
}
