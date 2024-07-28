#include "bgslib.hpp"

int main() {
    // Create an instance of AdaptiveSelectiveBackgroundLearning algorithm
    auto adaptiveSelectiveBG = bgslib::BGS_Factory::Instance()->Create("AdaptiveSelectiveBackgroundLearning");
    if (!adaptiveSelectiveBG) {
        std::cerr << "Failed to create AdaptiveSelectiveBackgroundLearning algorithm instance." << std::endl;
        return -1;
    }

    // Set AdaptiveSelectiveBackgroundLearning parameters
    adaptiveSelectiveBG->setParams({
        {"alphaLearn", "0.05"},
        {"alphaDetection", "0.05"},
        {"learningFrames", "500"},
        {"threshold", "15"}
    });

    // Print AdaptiveSelectiveBackgroundLearning parameters
    auto params = adaptiveSelectiveBG->getParams();
    std::cout << "\nAdaptiveSelectiveBackgroundLearning parameters:" << std::endl;
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
        adaptiveSelectiveBG->process(resizedFrame, fgMask, bgModel);

        // Display parameters on the frame
        params = adaptiveSelectiveBG->getParams();
        int y = 20;
        for (const auto& param : params) {
            cv::putText(resizedFrame, param.first + ": " + param.second, 
                        cv::Point(10, y), cv::FONT_HERSHEY_SIMPLEX, 0.5, 
                        cv::Scalar(255, 255, 255), 1);
            y += 20;
        }

        // Display the input frame, foreground mask, and background model
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