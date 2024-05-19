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

   
    cv::namedWindow("QR Code Scanner", cv::WINDOW_AUTOSIZE);


    cv::QRCodeDetector qrDecoder;

    while (true) {
        cv::Mat frame;
        cap >> frame; 

        if (frame.empty()) {
            std::cerr << "Error: Could not grab a frame" << std::endl;
            break;
        }

        std::string data = qrDecoder.detectAndDecode(frame);

        if (!data.empty()) {
            std::cout << "QR Code detected: " << data << std::endl;
            cv::putText(frame, data, cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
        }

        cv::imshow("QR Code Scanner", frame);

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
