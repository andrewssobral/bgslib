#include <chrono>
#include <numeric>
#include <deque>

#include "bgslib.hpp"

class PerformanceMetrics {
private:
    std::deque<double> frameTimings;
    const size_t maxSamples = 100;
    std::chrono::high_resolution_clock::time_point lastFrameTime;

public:
    PerformanceMetrics() {
        lastFrameTime = std::chrono::high_resolution_clock::now();
    }

    void update() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double frameTime = std::chrono::duration<double, std::milli>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;

        frameTimings.push_back(frameTime);
        if (frameTimings.size() > maxSamples) {
            frameTimings.pop_front();
        }
    }

    double getAverageFPS() const {
        if (frameTimings.empty()) return 0.0;
        double avgFrameTime = std::accumulate(frameTimings.begin(), frameTimings.end(), 0.0) / frameTimings.size();
        return 1000.0 / avgFrameTime;
    }

    double getInstantFPS() const {
        if (frameTimings.empty()) return 0.0;
        return 1000.0 / frameTimings.back();
    }
};

void displayPerformanceMetrics(cv::Mat& frame, const PerformanceMetrics& metrics) {
    std::stringstream ss;
    ss << "Avg FPS: " << std::fixed << std::setprecision(2) << metrics.getAverageFPS();
    cv::putText(frame, ss.str(), cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
    
    ss.str("");
    ss << "Instant FPS: " << std::fixed << std::setprecision(2) << metrics.getInstantFPS();
    cv::putText(frame, ss.str(), cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
}

int main() {
    auto frameDiff = bgslib::BGS_Factory::Instance()->Create("FrameDifference");
    if (!frameDiff) {
        std::cerr << "Failed to create FrameDifference algorithm instance." << std::endl;
        return -1;
    }

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video capture" << std::endl;
        return -1;
    }

    PerformanceMetrics metrics;
    cv::Mat frame, fgMask, bgModel;

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error capturing frame" << std::endl;
            break;
        }

        auto start = std::chrono::high_resolution_clock::now();
        
        frameDiff->process(frame, fgMask, bgModel);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        metrics.update();
        displayPerformanceMetrics(frame, metrics);

        std::stringstream ss;
        ss << "Process time: " << duration.count() / 1000.0 << " ms";
        cv::putText(frame, ss.str(), cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);

        cv::imshow("Original with Metrics", frame);
        cv::imshow("Foreground Mask", fgMask);

        if (cv::waitKey(30) == 'q') {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}
