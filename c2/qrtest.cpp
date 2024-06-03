#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera" << std::endl;
        return -1;
    }

        cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
        cap.set(cv::CAP_PROP_FPS, 120);
        cap.set(cv::CAP_PROP_BRIGHTNESS, 50);
        cap.set(cv::CAP_PROP_CONTRAST, 70);
        cap.set(cv::CAP_PROP_EXPOSURE, 156);



    cv::QRCodeDetector qrDecoder;

    while (true) {
        cv::VideoCapture cap(0); // 0은 기본 카메라를 의미합니다.
        if (!cap.isOpened()) {
            throw std::runtime_error("카메라를 열 수 없습니다.");
        }
       
        cv::Mat frame;
        std::string qrData;
        cv::QRCodeDetector qrDecoder;
        std::string before_xy = " ";

        while (true) {
            cap >> frame; // 카메라에서 프레임 캡처
            if (frame.empty()) {
                throw std::runtime_error("프레임을 캡처할 수 없습니다.");
            }

            cv::Mat grayFrame;
            cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

            // 히스토그램 평활화 적용
            cv::Mat equalizedFrame;
            cv::equalizeHist(grayFrame, equalizedFrame);

            // QR 코드 디코딩
            cv::Mat bbox, rectifiedImage;
            qrData = qrDecoder.detectAndDecode(equalizedFrame, bbox, rectifiedImage);
        
        
        if (!data.empty()) {
            std::cout << "QR Code detected: " << data << std::endl;
            cv::putText(frame, data, cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
        }

        cv::imshow("QR Code Scanner", equalizedFrame);

        char c = (char)cv::waitKey(25);
        if (c == 27) {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
//g++ qr_recognition.cpp -o qrrecognition `pkg-config --cflags --libs opencv4`
