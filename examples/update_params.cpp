#include "bgslib.hpp"

int main() {
    std::cout << "Using OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION << std::endl;

    // Create an instance of FrameDifference algorithm
    auto frameDiff = bgslib::BGS_Factory::Instance()->Create("FrameDifference");
    if (frameDiff) {
        // Get current parameters
        auto params = frameDiff->getParams();
        std::cout << "\nCurrent FrameDifference parameters:" << std::endl;
        for (const auto& param : params) {
            std::cout << param.first << ": " << param.second << std::endl;
        }

        // Set new parameters
        std::map<std::string, std::string> newParams = {
            {"enableThreshold", "false"},
            {"threshold", "20"}
        };
        frameDiff->setParams(newParams);

        // Get updated parameters
        params = frameDiff->getParams();
        std::cout << "\nUpdated FrameDifference parameters:" << std::endl;
        for (const auto& param : params) {
            std::cout << param.first << ": " << param.second << std::endl;
        }
    } else {
        std::cerr << "Failed to create FrameDifference algorithm instance." << std::endl;
    }

    return 0;
}
