#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <iostream>
#include <string>

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
