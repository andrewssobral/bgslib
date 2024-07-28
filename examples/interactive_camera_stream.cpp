#include "bgslib.hpp"

void printInstructions() {
    std::cout << "Keyboard controls:" << std::endl;
    std::cout << "  'q' - Quit the program" << std::endl;
    std::cout << "  't' - Toggle thresholding on/off" << std::endl;
    std::cout << "  '+' - Increase threshold value" << std::endl;
    std::cout << "  '-' - Decrease threshold value" << std::endl;
    std::cout << "  'p' - Print current parameters" << std::endl;
}

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

    printInstructions();

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

        // Handle keyboard input
        char key = (char)cv::waitKey(30);
        if (key == 'q') {
            break;
        } else if (key == 't') {
            auto params = frameDiff->getParams();
            bool enableThreshold = (params["enableThreshold"] == "true");
            frameDiff->setParams({{"enableThreshold", enableThreshold ? "false" : "true"}});
            std::cout << "Thresholding " << (enableThreshold ? "disabled" : "enabled") << std::endl;
        } else if (key == '+') {
            auto params = frameDiff->getParams();
            int threshold = std::stoi(params["threshold"]);
            threshold = std::min(threshold + 1, 255);
            frameDiff->setParams({{"threshold", std::to_string(threshold)}});
            std::cout << "Threshold increased to " << threshold << std::endl;
        } else if (key == '-') {
            auto params = frameDiff->getParams();
            int threshold = std::stoi(params["threshold"]);
            threshold = std::max(threshold - 1, 0);
            frameDiff->setParams({{"threshold", std::to_string(threshold)}});
            std::cout << "Threshold decreased to " << threshold << std::endl;
        } else if (key == 'p') {
            auto params = frameDiff->getParams();
            std::cout << "Current parameters:" << std::endl;
            for (const auto& param : params) {
                std::cout << "  " << param.first << ": " << param.second << std::endl;
            }
        }
    }

    // Release the camera and close windows
    cap.release();
    cv::destroyAllWindows();

    return 0;
}