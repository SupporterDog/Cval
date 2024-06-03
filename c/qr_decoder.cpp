#include "all_header.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <iostream>
#include <string>

// 스레드에서 실행할 함수

    int sock;
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
            std::string before_xy=" ";

            while (true) {

                cap >> frame; // 카메라에서 프레임 캡처
                if (frame.empty()) {
                    throw std::runtime_error("프레임을 캡처할 수 없습니다.");
                }

                cv:Mat gray;
                cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

                cv::Mat blurred;
                cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);

                cv::Mat resized;
                cv::resize(blurred, resized, cv::Size(). 2, 2, cv::rectifiedImage);

                // QR 코드 디코딩
                cv::Mat bbox, rectifiedImage;
                qrData = qrDecoder.detectAndDecode(resized, bbox, rectifiedImage);

                if (!qrData.empty() and qrData!=before_xy) {
                    printf("SCANNING QR CODE: ");
                    printf(qrData.c_str());
                    printf("  BEFORE QR CODE: ");
                    printf(before_xy.c_str());
                    printf("\n");
                    // 디코딩된 QR 코드 데이터를 처리
                    before_xy=qrData;
                    sendClientAction(sock, &lock, qrData.c_str(), 0);
                }


                // 프레임을 화면에 표시 (옵션)
                // cv::imshow("QR Code Scanner", frame);
                // if (cv::waitKey(30) >= 0) break; // 아무 키나 누르면 종료

                usleep(10000); // 약간의 지연을 추가하여 CPU 사용량을 줄입니다.
                //sleep(1)
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
        return NULL;
    }
