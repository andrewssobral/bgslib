#ifndef BGSLIB_HPP
#define BGSLIB_HPP

#include <iostream>
#include <fstream>
#include <list>
#include <memory>
#include <string>
#include <functional>
#include <map>

#include <opencv2/opencv.hpp>

// Platform-specific includes and definitions
#if defined(_WIN32) || defined(_WIN64)
    #define BGSLIB_WINDOWS
    // Windows-specific includes, if any
#elif defined(__APPLE__)
    #define BGSLIB_MACOS
    // macOS-specific includes, if any
#elif defined(__linux__)
    #define BGSLIB_LINUX
    // Linux-specific includes, if any
#else
    #error "Unsupported platform"
#endif

#define DEBUG_OBJ_LIFE

#if !defined(quote)
#define quote(x) #x
#endif

#if !defined(debug_construction)
#if defined(DEBUG_OBJ_LIFE)
#define debug_construction(x) std::cout << "+" << quote(x) << "()" << std::endl
#else
#define debug_construction(x)
#endif
#endif

#if !defined(debug_destruction)
#if defined(DEBUG_OBJ_LIFE)
#define debug_destruction(x) std::cout << "-" << quote(x) << "()" << std::endl
#else
#define debug_destruction(x)
#endif
#endif

#if !defined(bgs_register)
#define bgs_register(x) static BGS_Register<x> register_##x(quote(x))
#endif

// bgslib namespace
namespace bgslib {

/**
 * @class IBGS
 * @brief Interface for background subtraction algorithms.
 *
 * This class defines the interface for all background subtraction algorithms
 * in the bgslib library. It provides common functionality and requires derived
 * classes to implement the process method.
 */
class IBGS {

private:
    friend std::ostream& operator<<(std::ostream& o, const std::shared_ptr<IBGS>& ibgs) {
        return ibgs.get()->dump(o);
    }
    friend std::ostream& operator<<(std::ostream& o, const IBGS *ibgs) {
        return ibgs->dump(o);
    }
    friend std::string to_string(const std::shared_ptr<IBGS>& ibgs) {
        std::ostringstream ss;
        ss << ibgs;
        return ss.str();
    }

public:
    virtual std::ostream& dump(std::ostream& o) const {
        return o << getAlgorithmName();
    }
    /**
     * @brief Gets the name of the algorithm.
     * @return The algorithm name as a string.
     */
    std::string getAlgorithmName() const {
        return algorithmName;
    }
    /**
     * @brief Constructs an IBGS object with a given algorithm name.
     * @param _algorithmName The name of the algorithm.
     */
    IBGS(const std::string _algorithmName) {
        // debug_construction(IBGS);
        algorithmName = _algorithmName;
    }
    /**
     * @brief Default constructor.
     */
    IBGS() {
        // debug_construction(IBGS);
        algorithmName = "";
    }
    /**
     * @brief Virtual destructor.
     */
    virtual ~IBGS() {
        // debug_destruction(IBGS);
    }
    /**
     * @brief Processes an input image to perform background subtraction.
     * @param img_input The input image.
     * @param img_foreground The output foreground mask.
     * @param img_background The output background model.
     */
    virtual void process(const cv::Mat &img_input, cv::Mat &img_foreground, cv::Mat &img_background) = 0;
    /**
     * @brief Set algorithm parameters.
     * @param params A map of parameter names and their values.
     */
    virtual void setParams(const std::map<std::string, std::string>& params) = 0;
    /**
     * @brief Get current algorithm parameters.
     * @return A map of parameter names and their current values.
     */
    virtual std::map<std::string, std::string> getParams() const = 0;

protected:
    std::string algorithmName; ///< The name of the algorithm.
    bool firstTime = true; ///< Flag indicating if this is the first frame.
    cv::Mat img_background; ///< The background model.
    cv::Mat img_foreground; ///< The foreground mask.
    /**
     * @brief Initializes output matrices.
     * @param img_input The input image.
     * @param img_outfg The output foreground mask.
     * @param img_outbg The output background model.
     */
    void init(const cv::Mat &img_input, cv::Mat &img_outfg, cv::Mat &img_outbg) {
        assert(img_input.empty() == false);
        // img_outfg = cv::Mat::zeros(img_input.size(), img_input.type());
        // img_outbg = cv::Mat::zeros(img_input.size(), img_input.type());
        img_outfg = cv::Mat::zeros(img_input.size(), CV_8UC1);
        img_outbg = cv::Mat::zeros(img_input.size(), CV_8UC3);
    }
};

/**
 * @class BGS_Factory
 * @brief Factory class for creating background subtraction algorithm instances.
 *
 * This class manages the creation of background subtraction algorithm instances.
 * It uses a singleton pattern to ensure only one factory exists in the application.
 */
class BGS_Factory {

private:
    BGS_Factory() {
        // debug_construction(BGS_Factory);
    }
    virtual ~BGS_Factory() {
        // debug_destruction(BGS_Factory);
    }
    std::map<std::string, std::function<IBGS*(void)>> factoryFunctionRegistry;

public:
    /**
     * @brief Get the singleton instance of the factory.
     * @return Pointer to the BGS_Factory instance.
     */
    static BGS_Factory* Instance() {
        static BGS_Factory factory;
        return &factory;
    }
    /**
     * @brief Create an instance of a background subtraction algorithm.
     * @param name The name of the algorithm to create.
     * @return A shared pointer to the created IBGS instance, or nullptr if creation fails.
     */
    std::shared_ptr<IBGS> Create(const std::string& name) {
        auto it = factoryFunctionRegistry.find(name);
        if (it != factoryFunctionRegistry.end()) {
            try {
                IBGS* instance = it->second();
                return std::shared_ptr<IBGS>(instance);
            } catch (const std::exception& e) {
                std::cerr << "Error creating instance of " << name << ": " << e.what() << std::endl;
                return nullptr;
            }
        } else {
            std::cerr << "Algorithm '" << name << "' not found in registry." << std::endl;
            return nullptr;
        }
    }
    /**
     * @brief Get the names of all registered algorithms.
     * @return A vector of strings containing the names of registered algorithms.
     */
    std::vector<std::string> GetRegisteredAlgorithmsName() {
        std::vector<std::string> algorithmsName;
        for (auto it = factoryFunctionRegistry.begin(); it != factoryFunctionRegistry.end(); ++it) {
            algorithmsName.push_back(it->first);
        }
        return algorithmsName;
    }
    /**
     * @brief Register a new algorithm factory function.
     * @param name The name of the algorithm.
     * @param classFactoryFunction The factory function to create instances of the algorithm.
     * @return True if registration was successful, false otherwise.
     */
    bool RegisterFactoryFunction(const std::string& name, std::function<IBGS*(void)> classFactoryFunction) {
        if (factoryFunctionRegistry.find(name) != factoryFunctionRegistry.end()) {
            std::cerr << "Warning: Overwriting existing factory function for '" << name << "'" << std::endl;
        }
        factoryFunctionRegistry[name] = classFactoryFunction;
        return true;
    }
};

template<class T>
class BGS_Register {

public:
    BGS_Register(const std::string className) {
        // debug_construction(BGS_Register);
        // register the class factory function
        BGS_Factory::Instance()->RegisterFactoryFunction(className, [](void) -> IBGS* { return new T(); });
    }
    virtual ~BGS_Register() {
        // debug_destruction(BGS_Register);
    }
};

namespace algorithms {

// FrameDifference algorithm
class FrameDifference : public IBGS {
private:
    bool enableThreshold;
    int threshold;

public:
    FrameDifference() : 
        IBGS("FrameDifference"),
        enableThreshold(true), threshold(15) {
        debug_construction(FrameDifference);
    }

    ~FrameDifference() {
        debug_destruction(FrameDifference);
    }

    void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel) override {
        init(img_input, img_output, img_bgmodel);

        if (img_background.empty()) {
            img_input.copyTo(img_background);
            return;
        }

        cv::absdiff(img_background, img_input, img_foreground);

        if (img_foreground.channels() == 3)
            cv::cvtColor(img_foreground, img_foreground, cv::COLOR_BGR2GRAY);

        if (enableThreshold)
            cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);

        img_foreground.copyTo(img_output);

        img_input.copyTo(img_background);
        img_background.copyTo(img_bgmodel);

        firstTime = false;
    }

    void setParams(const std::map<std::string, std::string>& params) override {
        for (const auto& param : params) {
            if (param.first == "enableThreshold") {
                enableThreshold = (param.second == "true");
            } else if (param.first == "threshold") {
                threshold = std::stoi(param.second);
            }
        }
    }

    std::map<std::string, std::string> getParams() const override {
        return {
            {"enableThreshold", enableThreshold ? "true" : "false"},
            {"threshold", std::to_string(threshold)}
        };
    }
};
bgs_register(FrameDifference);

// AdaptiveBackgroundLearning algorithm
class AdaptiveBackgroundLearning : public IBGS {
private:
    double alpha;
    int maxLearningFrames;
    long currentLearningFrame;
    double minVal;
    double maxVal;
    bool enableThreshold;
    int threshold;

public:
    AdaptiveBackgroundLearning() : 
        IBGS("AdaptiveBackgroundLearning"),
        alpha(0.05), maxLearningFrames(-1), currentLearningFrame(0), minVal(0.0),
        maxVal(1.0), enableThreshold(true), threshold(15) {
        debug_construction(AdaptiveBackgroundLearning);
    }

    ~AdaptiveBackgroundLearning() {
        debug_destruction(AdaptiveBackgroundLearning);
    }

    void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel) override {
        init(img_input, img_output, img_bgmodel);

        if (img_background.empty())
            img_input.copyTo(img_background);

        cv::Mat img_input_f, img_background_f, img_diff_f;
        img_input.convertTo(img_input_f, CV_32F, 1. / 255.);
        img_background.convertTo(img_background_f, CV_32F, 1. / 255.);
        cv::absdiff(img_input_f, img_background_f, img_diff_f);

        if ((maxLearningFrames > 0 && currentLearningFrame < maxLearningFrames) || maxLearningFrames == -1) {
            img_background_f = alpha * img_input_f + (1 - alpha) * img_background_f;
            img_background_f.convertTo(img_background, CV_8U, 255.0 / (maxVal - minVal), -minVal);
            
            if (maxLearningFrames > 0 && currentLearningFrame < maxLearningFrames)
                currentLearningFrame++;
        }

        img_diff_f.convertTo(img_foreground, CV_8UC1, 255.0 / (maxVal - minVal), -minVal);

        if (img_foreground.channels() == 3)
            cv::cvtColor(img_foreground, img_foreground, cv::COLOR_BGR2GRAY);

        if (enableThreshold)
            cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);

        img_foreground.copyTo(img_output);
        img_background.copyTo(img_bgmodel);

        firstTime = false;
    }

    void setParams(const std::map<std::string, std::string>& params) override {
        for (const auto& param : params) {
            if (param.first == "alpha") {
                alpha = std::stod(param.second);
            } else if (param.first == "maxLearningFrames") {
                maxLearningFrames = std::stoi(param.second);
            } else if (param.first == "enableThreshold") {
                enableThreshold = (param.second == "true");
            } else if (param.first == "threshold") {
                threshold = std::stoi(param.second);
            }
        }
    }

    std::map<std::string, std::string> getParams() const override {
        return {
            {"alpha", std::to_string(alpha)},
            {"maxLearningFrames", std::to_string(maxLearningFrames)},
            {"enableThreshold", enableThreshold ? "true" : "false"},
            {"threshold", std::to_string(threshold)}
        };
    }
};
bgs_register(AdaptiveBackgroundLearning);

// AdaptiveSelectiveBackgroundLearning algorithm
class AdaptiveSelectiveBackgroundLearning : public IBGS {
private:
    double alphaLearn;
    double alphaDetection;
    int learningFrames;
    long counter;
    double minVal;
    double maxVal;
    int threshold;

public:
    AdaptiveSelectiveBackgroundLearning() : 
        IBGS("AdaptiveSelectiveBackgroundLearning"),
        alphaLearn(0.05), alphaDetection(0.05), learningFrames(-1), 
        counter(0), minVal(0.0), maxVal(1.0), threshold(15) {
        debug_construction(AdaptiveSelectiveBackgroundLearning);
    }

    ~AdaptiveSelectiveBackgroundLearning() {
        debug_destruction(AdaptiveSelectiveBackgroundLearning);
    }

    void process(const cv::Mat &img_input_, cv::Mat &img_output, cv::Mat &img_bgmodel) override {
        init(img_input_, img_output, img_bgmodel);

        cv::Mat img_input;
        if (img_input_.channels() == 3)
            cv::cvtColor(img_input_, img_input, cv::COLOR_BGR2GRAY);
        else
            img_input_.copyTo(img_input);

        if (img_background.empty())
            img_input.copyTo(img_background);

        cv::Mat img_input_f, img_background_f, img_diff_f;
        img_input.convertTo(img_input_f, CV_32F, 1. / 255.);
        img_background.convertTo(img_background_f, CV_32F, 1. / 255.);
        cv::absdiff(img_input_f, img_background_f, img_diff_f);

        img_diff_f.convertTo(img_foreground, CV_8U, 255.0 / (maxVal - minVal), -minVal);

        cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);
        cv::medianBlur(img_foreground, img_foreground, 3);

        if (learningFrames > 0 && counter <= learningFrames) {
            img_background_f = alphaLearn * img_input_f + (1 - alphaLearn) * img_background_f;
            counter++;
        }
        else {
            for (int i = 0; i < img_input.rows; i++) {
                for (int j = 0; j < img_input.cols; j++) {
                    if (img_foreground.at<uchar>(i, j) == 0) {
                        img_background_f.at<float>(i, j) = alphaDetection * img_input_f.at<float>(i, j) + (1 - alphaDetection) * img_background_f.at<float>(i, j);
                    }
                }
            }
        }

        img_background_f.convertTo(img_background, CV_8UC1, 255.0 / (maxVal - minVal), -minVal);

        img_foreground.copyTo(img_output);
        img_background.copyTo(img_bgmodel);

        firstTime = false;
    }

    void setParams(const std::map<std::string, std::string>& params) override {
        for (const auto& param : params) {
            if (param.first == "alphaLearn") {
                alphaLearn = std::stod(param.second);
            } else if (param.first == "alphaDetection") {
                alphaDetection = std::stod(param.second);
            } else if (param.first == "learningFrames") {
                learningFrames = std::stoi(param.second);
            } else if (param.first == "threshold") {
                threshold = std::stoi(param.second);
            }
        }
    }

    std::map<std::string, std::string> getParams() const override {
        return {
            {"alphaLearn", std::to_string(alphaLearn)},
            {"alphaDetection", std::to_string(alphaDetection)},
            {"learningFrames", std::to_string(learningFrames)},
            {"threshold", std::to_string(threshold)}
        };
    }
};
bgs_register(AdaptiveSelectiveBackgroundLearning);

// WeightedMovingMean algorithm
class WeightedMovingMean : public IBGS {
private:
    cv::Mat img_input_prev_1;
    cv::Mat img_input_prev_2;
    bool enableWeight;
    bool enableThreshold;
    int threshold;

public:
    WeightedMovingMean() : 
        IBGS("WeightedMovingMean"),
        enableWeight(true), enableThreshold(true), threshold(15) {
        debug_construction(WeightedMovingMean);
    }

    ~WeightedMovingMean() {
        debug_destruction(WeightedMovingMean);
    }

    void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel) override {
        init(img_input, img_output, img_bgmodel);

        if (img_input_prev_1.empty()) {
            img_input.copyTo(img_input_prev_1);
            return;
        }

        if (img_input_prev_2.empty()) {
            img_input_prev_1.copyTo(img_input_prev_2);
            img_input.copyTo(img_input_prev_1);
            return;
        }

        cv::Mat img_input_f, img_input_prev_1_f, img_input_prev_2_f, img_background_f;
        img_input.convertTo(img_input_f, CV_32F, 1. / 255.);
        img_input_prev_1.convertTo(img_input_prev_1_f, CV_32F, 1. / 255.);
        img_input_prev_2.convertTo(img_input_prev_2_f, CV_32F, 1. / 255.);

        if (enableWeight)
            img_background_f = ((img_input_f * 0.5) + (img_input_prev_1_f * 0.3) + (img_input_prev_2_f * 0.2));
        else
            img_background_f = ((img_input_f) + (img_input_prev_1_f) + (img_input_prev_2_f)) / 3.0;

        img_background_f.convertTo(img_background, CV_8U, 255.0);

        cv::absdiff(img_input, img_background, img_foreground);

        if (img_foreground.channels() == 3)
            cv::cvtColor(img_foreground, img_foreground, cv::COLOR_BGR2GRAY);

        if (enableThreshold)
            cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);

        img_foreground.copyTo(img_output);
        img_background.copyTo(img_bgmodel);

        img_input_prev_1.copyTo(img_input_prev_2);
        img_input.copyTo(img_input_prev_1);

        firstTime = false;
    }

    void setParams(const std::map<std::string, std::string>& params) override {
        for (const auto& param : params) {
            if (param.first == "enableWeight") {
                enableWeight = (param.second == "true");
            } else if (param.first == "enableThreshold") {
                enableThreshold = (param.second == "true");
            } else if (param.first == "threshold") {
                threshold = std::stoi(param.second);
            }
        }
    }

    std::map<std::string, std::string> getParams() const override {
        return {
            {"enableWeight", enableWeight ? "true" : "false"},
            {"enableThreshold", enableThreshold ? "true" : "false"},
            {"threshold", std::to_string(threshold)}
        };
    }
};
bgs_register(WeightedMovingMean);

} // namespace algorithms

} // namespace bgslib

#endif // BGSLIB_HPP
