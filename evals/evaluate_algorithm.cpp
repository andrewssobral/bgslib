/**
 * @file evaluate_algorithm.cpp
 * @brief A flexible tool for evaluating background subtraction algorithms in the bgslib library.
 *
 * This script provides a command-line interface for evaluating various background subtraction
 * algorithms implemented in the bgslib library. It processes a dataset of input frames and
 * corresponding ground truth images, applying the specified algorithm and calculating
 * performance metrics such as True Positives (TP), False Positives (FP), True Negatives (TN),
 * False Negatives (FN), Recall, Precision, and F-score.
 *
 * The tool is designed to be flexible, allowing users to specify different algorithms,
 * datasets, and evaluation parameters through command-line arguments.
 *
 * Usage:
 * ./build/evaluate_algorithm [OPTIONS]
 *
 * Options:
 * --algorithm  : Specifies the algorithm to use (default: "FrameDifference")
 * --dataset    : Sets the base dataset path (default: "./datasets/ucsd/boats")
 * --frames     : Sets the frames directory name (default: "frames")
 * --groundtruth: Sets the groundtruth directory name (default: "groundtruth")
 * --extension  : Sets the file extension for images (default: ".png")
 * --delay      : Sets the delay between frames in milliseconds (default: 30)
 * --visual-debug: Enables visual debugging (optional)
 *
 * Examples:
 * 1. Run with default settings:
 *    ./build/evaluate_algorithm
 *
 * 2. Evaluate a different algorithm:
 *    ./build/evaluate_algorithm --algorithm WeightedMovingMean
 *
 * 3. Use a different dataset:
 *    ./build/evaluate_algorithm --dataset ./datasets/my_dataset --frames input --groundtruth gt
 *
 * 4. Change the frame delay and enable visual debugging:
 *    ./build/evaluate_algorithm --delay 500 --visual-debug
 *
 * 5. Combine multiple options:
 *    ./build/evaluate_algorithm --algorithm AdaptiveBackgroundLearning --dataset ./datasets/custom --frames images --groundtruth masks --extension .jpg --delay 100 --visual-debug
 *
 * This flexible design allows for easy evaluation of different algorithms on various datasets
 * without modifying the source code. Additional command-line options can be added in the future
 * to customize other aspects of the evaluation process.
 */

#include "bgslib.hpp"

#include <filesystem>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

std::vector<std::string> getFilesInDirectory(const std::string& directory, const std::string& extension) {
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (fs::is_regular_file(entry.path()) && entry.path().extension() == extension) {
            files.push_back(entry.path().string());
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}

void evaluateAlgorithm(const std::string& algorithmName, const std::string& framesDir, const std::string& groundtruthDir, 
                       const std::string& extension, int delay, bool visualDebug) {
    auto algorithm = bgslib::BGS_Factory::Instance()->Create(algorithmName);
    if (!algorithm) {
        std::cerr << "Failed to create " << algorithmName << " algorithm instance." << std::endl;
        return;
    }

    auto frameFiles = getFilesInDirectory(framesDir, extension);
    auto groundtruthFiles = getFilesInDirectory(groundtruthDir, extension);

    if (frameFiles.size() != groundtruthFiles.size()) {
        std::cerr << "Mismatch in number of frame and groundtruth files." << std::endl;
        return;
    }

    double TP = 0, FP = 0, TN = 0, FN = 0;

    for (size_t i = 0; i < frameFiles.size(); ++i) {
        cv::Mat frame = cv::imread(frameFiles[i], cv::IMREAD_GRAYSCALE);
        cv::Mat groundtruth = cv::imread(groundtruthFiles[i], cv::IMREAD_GRAYSCALE);
        cv::Mat fgMask, bgModel;

        algorithm->process(frame, fgMask, bgModel);

        for (int y = 0; y < frame.rows; ++y) {
            for (int x = 0; x < frame.cols; ++x) {
                bool isForeground = fgMask.at<uchar>(y, x) == 255;
                bool isGroundtruthForeground = groundtruth.at<uchar>(y, x) == 255;

                if (isForeground && isGroundtruthForeground) TP++;
                else if (isForeground && !isGroundtruthForeground) FP++;
                else if (!isForeground && !isGroundtruthForeground) TN++;
                else if (!isForeground && isGroundtruthForeground) FN++;
            }
        }

        if (visualDebug) {
            cv::imshow("Input Frame", frame);
            cv::imshow("Foreground Mask", fgMask);
            cv::imshow("Background Model", bgModel);
            cv::imshow("Ground Truth", groundtruth);
            
            int key = cv::waitKey(delay);
            if (key == 'q' || key == 27) { // 'q' or ESC key
                break;
            }
        }

        std::cout << "Processed frame " << (i + 1) << " / " << frameFiles.size() << "\r" << std::flush;
    }

    if (visualDebug) {
        cv::destroyAllWindows();
    }

    double recall = TP / (TP + FN);
    double precision = TP / (TP + FP);
    double fscore = 2 * precision * recall / (precision + recall);

    std::cout << "\nEvaluation Results for " << algorithmName << ":" << std::endl;
    std::cout << "TP: " << TP << std::endl;
    std::cout << "FP: " << FP << std::endl;
    std::cout << "TN: " << TN << std::endl;
    std::cout << "FN: " << FN << std::endl;
    std::cout << "Recall: " << recall << std::endl;
    std::cout << "Precision: " << precision << std::endl;
    std::cout << "F-score: " << fscore << std::endl;
}

int main(int argc, char* argv[]) {
    std::string algorithmName = "FrameDifference";
    std::string datasetPath = "./datasets/ucsd/boats";
    std::string framesDir = "frames";
    std::string groundtruthDir = "groundtruth";
    std::string extension = ".png";
    int delay = 30;
    bool visualDebug = false;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--algorithm" && i + 1 < argc) {
            algorithmName = argv[++i];
        } else if (arg == "--dataset" && i + 1 < argc) {
            datasetPath = argv[++i];
        } else if (arg == "--frames" && i + 1 < argc) {
            framesDir = argv[++i];
        } else if (arg == "--groundtruth" && i + 1 < argc) {
            groundtruthDir = argv[++i];
        } else if (arg == "--extension" && i + 1 < argc) {
            extension = argv[++i];
        } else if (arg == "--delay" && i + 1 < argc) {
            delay = std::stoi(argv[++i]);
        } else if (arg == "--visual-debug") {
            visualDebug = true;
        }
    }

    std::string fullFramesDir = datasetPath + "/" + framesDir;
    std::string fullGroundtruthDir = datasetPath + "/" + groundtruthDir;

    evaluateAlgorithm(algorithmName, fullFramesDir, fullGroundtruthDir, extension, delay, visualDebug);

    return 0;
}