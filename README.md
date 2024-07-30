# bgslib

bgslib is a powerful, header-only C++ library for background subtraction in computer vision applications. Background subtraction is a fundamental technique used in various fields such as video surveillance, traffic monitoring, and human-computer interaction to detect moving objects in video streams.

This library provides a flexible and extensible framework for implementing, using, and evaluating various background subtraction algorithms. It's designed to be easy to use while allowing for advanced customization and extension.

## Features

- **Header-only library**: Easy integration into existing projects
- **Multiple algorithms**: Includes several background subtraction techniques
- **Factory pattern**: Simplifies algorithm instantiation and management
- **Flexible parameter adjustment**: Easy tuning of algorithm parameters
- **Cross-platform compatibility**: Works on Windows, macOS, and Linux
- **Performance metrics**: Real-time monitoring of algorithm performance
- **Evaluation tools**: Assess algorithm effectiveness on datasets
- **Extensible design**: Easily add new background subtraction algorithms

### Included Algorithms

1. FrameDifference
2. StaticFrameDifference
3. AdaptiveBackgroundLearning
4. AdaptiveSelectiveBackgroundLearning
5. WeightedMovingMean
6. WeightedMovingVariance

## Requirements

- C++17 compatible compiler
- OpenCV 4.x
- CMake 3.10 or higher

## Installation

As bgslib is a header-only library, you don't need to compile it separately. Simply include the `bgslib.hpp` header in your project.

1. Clone the repository:
   ```bash
   git clone https://github.com/andrewssobral/bgslib.git
   ```

2. Add the `include` directory to your project's include path.

## Quick Start

To get started quickly with bgslib:

1. Include the header in your C++ file:
   ```cpp
   #include "bgslib.hpp"
   ```

2. Create an instance of an algorithm:
   ```cpp
   auto algorithm = bgslib::BGS_Factory::Instance()->Create("FrameDifference");
   ```

3. Process frames:
   ```cpp
   cv::Mat frame, fgMask, bgModel;
   // ... obtain frame from video or camera ...
   algorithm->process(frame, fgMask, bgModel);
   ```

## Building and Running Examples

The library comes with a Makefile that simplifies the build process and running of examples. Here are some common commands:

```bash
# Build the project
make build

# Build and run all examples
make examples

# Build and run all demos
make demos

# Run evaluations
make run_evals

# For more options
make help
```

## Usage

### Basic Usage

Here's a simple example of how to use bgslib with the FrameDifference algorithm:

```cpp
#include "bgslib.hpp"

int main() {
    // Create an instance of FrameDifference algorithm
    auto frameDiff = bgslib::BGS_Factory::Instance()->Create("FrameDifference");
    
    // Open a video capture
    cv::VideoCapture cap(0);
    cv::Mat frame, fgMask, bgModel;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // Apply background subtraction
        frameDiff->process(frame, fgMask, bgModel);

        // Display results
        cv::imshow("Original", frame);
        cv::imshow("Foreground Mask", fgMask);

        if (cv::waitKey(30) == 'q') break;
    }

    return 0;
}
```

### Adjusting Algorithm Parameters

You can adjust algorithm parameters using the `setParams` method:

```cpp
std::map<std::string, std::string> params = {
    {"enableThreshold", "true"},
    {"threshold", "20"}
};
frameDiff->setParams(params);
```

### Getting Current Parameters

To get the current parameters of an algorithm:

```cpp
auto currentParams = frameDiff->getParams();
for (const auto& param : currentParams) {
    std::cout << param.first << ": " << param.second << std::endl;
}
```

## Examples and Demos

The library includes several example applications and demos:

### Examples
1. `list_algorithms`: Lists all available background subtraction algorithms
2. `update_params`: Demonstrates how to update algorithm parameters
3. `camera_stream`: Basic example of using the library with a camera stream
4. `interactive_camera_stream`: Allows real-time parameter adjustment using keyboard controls
5. `performance_metrics`: Displays performance metrics (FPS, processing time) while running the algorithm

### Demos
- Separate demo files for each algorithm, showcasing their usage with a camera stream

### Building and Running Examples

To build and run the examples:

```bash
mkdir build && cd build
cmake ..
make
./list_algorithms
./camera_stream
# ... and so on for other examples and demos
```

## Evaluation Tool

The library includes an evaluation tool (`evaluate_algorithm.cpp`) that allows you to assess the performance of different algorithms on datasets. It calculates metrics such as True Positives, False Positives, Recall, Precision, and F-score.

To use the evaluation tool:

```bash
./build/evaluate_algorithm --algorithm FrameDifference --dataset ./path/to/dataset --frames frames --groundtruth groundtruth
```

Options:
- `--algorithm`: Specifies the algorithm to use (default: "FrameDifference")
- `--dataset`: Sets the base dataset path
- `--frames`: Sets the frames directory name (default: "frames")
- `--groundtruth`: Sets the groundtruth directory name (default: "groundtruth")
- `--extension`: Sets the file extension for images (default: ".png")
- `--delay`: Sets the delay between frames in milliseconds (default: 30)
- `--visual-debug`: Enables visual debugging (optional)

## Extending the Library

To add a new background subtraction algorithm:

1. Create a new class that inherits from `bgslib::IBGS`
2. Implement the required methods (`process`, `setParams`, `getParams`)
3. Register the new algorithm using the `bgs_register` macro

Example:

```cpp
class MyNewAlgorithm : public bgslib::IBGS {
public:
    MyNewAlgorithm() : IBGS("MyNewAlgorithm") {}
    void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel) override {
        // Implement your algorithm here
    }
    void setParams(const std::map<std::string, std::string>& params) override {
        // Set parameters
    }
    std::map<std::string, std::string> getParams() const override {
        // Return current parameters
    }
};

bgs_register(MyNewAlgorithm);
```

## Performance Considerations

When using bgslib, consider the following to optimize performance:

- Choose the appropriate algorithm for your use case. Some algorithms are faster but less accurate, while others are more robust but computationally intensive.
- Adjust algorithm parameters to balance between accuracy and speed.
- For real-time applications, consider processing frames at a lower resolution.
- Utilize OpenCV's GPU acceleration if available for your system.

## Troubleshooting

- If you encounter "OpenCV not found" errors, ensure that OpenCV is properly installed and its path is correctly set in your environment.
- For performance issues, try adjusting the parameters of the algorithms or using a more efficient algorithm for your specific use case.
- If you're having trouble with a specific dataset, ensure that the images are in a supported format and the directory structure matches what the evaluation tool expects.

## Contributing

Contributions to bgslib are welcome! Please feel free to submit pull requests, create issues or spread the word.
Here's how you can contribute:

1. Fork the repository on GitHub.
2. Create a new branch for your feature or bug fix.
3. Write your code and tests.
4. Ensure all tests pass and the code follows the project's coding style.
5. Submit a pull request with a clear description of your changes.

## Citing

If you use bgslib in your research, please cite it as follows:

```
@software{bgslib,
  author = {Cordolino Sobral, Andrews},
  title = {bgslib: A header-only C++ Background Subtraction Library},
  year = {2024},
  url = {https://github.com/andrewssobral/bgslib}
}
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- OpenCV community for their excellent computer vision library
- All contributors who have helped to improve this library

For more information and updates, please visit the [bgslib GitHub repository](https://github.com/andrewssobral/bgslib).