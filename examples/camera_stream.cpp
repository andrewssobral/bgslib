#include "bgslib.hpp"

int main() {
    // Create an instance of FrameDifference algorithm
    auto frameDiff = bgslib::BGS_Factory::Instance()->Create("FrameDifference");
    if (!frameDiff) {
        std::cerr << "Failed to create FrameDifference algorithm instance." << std::endl;
        return -1;
    }

    // Open the default camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video capture" << std::endl;
        return -1;
    }

    cv::Mat frame, fgMask, bgModel;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error capturing frame" << std::endl;
            break;
        }

        // Apply background subtraction
        frameDiff->process(frame, fgMask, bgModel);

        // Display the original frame and the foreground mask
        cv::imshow("Original", frame);
        cv::imshow("Foreground Mask", fgMask);

        // Exit if 'q' is pressed
        if (cv::waitKey(30) == 'q') {
            break;
        }
    }

    // Release the camera and close windows
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
