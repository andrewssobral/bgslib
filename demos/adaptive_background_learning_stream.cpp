#include "bgslib.hpp"

int main() {
    // Create an instance of AdaptiveBackgroundLearning algorithm
    auto adaptiveBG = bgslib::BGS_Factory::Instance()->Create("AdaptiveBackgroundLearning");
    if (!adaptiveBG) {
        std::cerr << "Failed to create AdaptiveBackgroundLearning algorithm instance." << std::endl;
        return -1;
    }

    // Set AdaptiveBackgroundLearning parameters
    adaptiveBG->setParams({
        {"alpha", "0.01"},
        {"threshold", "25"},
        {"enableThreshold", "true"},
        {"maxLearningFrames", "500"}
    });

    // Print AdaptiveBackgroundLearning parameters
    auto params = adaptiveBG->getParams();
    std::cout << "\nAdaptiveBackgroundLearning parameters:" << std::endl;
    for (const auto& param : params) {
        std::cout << param.first << ": " << param.second << std::endl;
    }

    // Open the default camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video capture" << std::endl;
        return -1;
    }

    cv::Mat frame, resizedFrame, fgMask, bgModel;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error capturing frame" << std::endl;
            break;
        }

        // Resize frame to 640x480
        cv::resize(frame, resizedFrame, cv::Size(640, 480));

        // Apply background subtraction
        adaptiveBG->process(resizedFrame, fgMask, bgModel);

        // Display the original frame, foreground mask, and background model
        cv::imshow("Original (Resized)", resizedFrame);
        cv::imshow("Foreground Mask", fgMask);
        cv::imshow("Background Model", bgModel);

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