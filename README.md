# bgslib

bgslib is a C++ header-only library for background subtraction. It provides a flexible and extensible framework for implementing and using various background subtraction algorithms.

## Features

- Header-only library for easy integration
- Factory pattern for creating algorithm instances
- Support for multiple background subtraction algorithms:
  - FrameDifference
  - StaticFrameDifference
  - AdaptiveBackgroundLearning
  - AdaptiveSelectiveBackgroundLearning
  - WeightedMovingMean
  - WeightedMovingVariance
- Easy parameter adjustment for algorithms
- Cross-platform compatibility (Windows, macOS, Linux)
- Performance metrics for real-time monitoring

## Requirements

- C++14 compatible compiler
- OpenCV 4.x
- CMake 3.10 or higher

## Installation

As bgslib is a header-only library, you don't need to compile it separately. Simply include the `bgslib.hpp` header in your project.

1. Clone the repository:

```bash
git clone https://github.com/andrewssobral/bgslib
```

2. Add the `include` directory to your project's include path.

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

## Examples

The library comes with several example applications demonstrating various features:

1. `list_algorithms`: Lists all available background subtraction algorithms.
2. `update_params`: Demonstrates how to update algorithm parameters.
3. `camera_stream`: Basic example of using the library with a camera stream.
4. `interactive_camera_stream`: Allows real-time parameter adjustment using keyboard controls.
5. `performance_metrics`: Displays performance metrics (FPS, processing time) while running the algorithm.

To build and run the examples:

```bash
./build.sh
./build/list_algorithms
./build/camera_stream
# ... and so on for other examples
```

## Extending the Library

To add a new background subtraction algorithm:

1. Create a new class that inherits from `bgslib::IBGS`.
2. Implement the required methods (`process`, `setParams`, `getParams`).
3. Register the new algorithm using the `bgs_register` macro.

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

## License

MIT License
