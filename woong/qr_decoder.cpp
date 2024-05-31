#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <zbar.h>
#include <iostream>
#include <cstring>

extern "C" {
    const char* decodeQRCodeFromCamera() {
        try {
            // OpenCV를 사용하여 카메라 캡처 초기화
            cv::VideoCapture cap(0); // 0은 기본 카메라를 의미합니다.
            if (!cap.isOpened()) {
                throw std::runtime_error("카메라를 열 수 없습니다.");
            }

            cv::Mat frame;
            std::string qrData;

            while (true) {
                cap >> frame; // 카메라에서 프레임 캡처
                if (frame.empty()) {
                    throw std::runtime_error("프레임을 캡처할 수 없습니다.");
                }

                // 그레이스케일로 변환
                cv::Mat gray;
                cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

                // ZBar 스캐너 초기화
                zbar::ImageScanner scanner;
                scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);

                // OpenCV 이미지를 ZBar 이미지로 변환
                zbar::Image zbarImage(gray.cols, gray.rows, "Y800", gray.data, gray.cols * gray.rows);

                // QR 코드 스캔
                int n = scanner.scan(zbarImage);
                if (n > 0) {
                    for (auto symbol = zbarImage.symbol_begin(); symbol != zbarImage.symbol_end(); ++symbol) {
                        qrData = symbol->get_data();
                        break; // 첫 번째 QR 코드 데이터만 반환
                    }
                    break; // QR 코드가 인식되면 루프 종료
                }

                // 프레임을 화면에 표시 (옵션)
                cv::imshow("QR Code Scanner", frame);
                if (cv::waitKey(30) >= 0) break; // 아무 키나 누르면 종료
            }

            // 결과를 복사하여 반환
            char* result = new char[qrData.size() + 1];
            std::strcpy(result, qrData.c_str());
            return result;

        } catch (const std::exception& e) {
            return nullptr;
        }
    }
}
