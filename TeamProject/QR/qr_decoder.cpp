#include <iostream>
#include <opencv2/opencv.hpp>
#include <pthread.h>
#include <unistd.h>
#include <stdexcept>

// 전역 변수 정의
pthread_mutex_t lock;

// 스레드에서 실행할 함수
void* decodeQRCodeThread(void* arg) {
    try {
        // OpenCV를 사용하여 카메라 캡처 초기화
        cv::VideoCapture cap(0); // 0은 기본 카메라를 의미합니다.
        if (!cap.isOpened()) {
            throw std::runtime_error("카메라를 열 수 없습니다.");
        }

        cv::Mat frame;
        std::string qrData;
        cv::QRCodeDetector qrDecoder;

        while (true) {
            cap >> frame; // 카메라에서 프레임 캡처
            if (frame.empty()) {
                throw std::runtime_error("프레임을 캡처할 수 없습니다.");
            }

            // QR 코드 디코딩
            cv::Mat bbox, rectifiedImage;
            qrData = qrDecoder.detectAndDecode(frame, bbox, rectifiedImage);

            if (!qrData.empty()) {
                pthread_mutex_lock(&lock);
                // 디코딩된 QR 코드 데이터를 처리
                std::cout << "QR Data: " << qrData << std::endl;
                pthread_mutex_unlock(&lock);
            }

            // 프레임을 화면에 표시 (옵션)
            // cv::imshow("QR Code Scanner", frame);
            // if (cv::waitKey(30) >= 0) break; // 아무 키나 누르면 종료

            usleep(300000); // 약간의 지연을 추가하여 CPU 사용량을 줄입니다.
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return NULL;
}

int main() {
    // 뮤텍스 초기화
    pthread_mutex_init(&lock, NULL);

    // 스레드 생성
    pthread_t thread;
    if (pthread_create(&thread, NULL, decodeQRCodeThread, NULL)) {
        std::cerr << "스레드를 생성할 수 없습니다." << std::endl;
        return 1;
    }

    // 메인 스레드가 decodeQRCodeThread가 종료될 때까지 기다림
    pthread_join(thread, NULL);

    // 뮤텍스 파괴
    pthread_mutex_destroy(&lock);

    return 0;
}
//g++ -o qr_decoder qr_decoder.cpp `pkg-config --cflags --libs opencv4` -lpthread
