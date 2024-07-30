# Phony targets
.PHONY: all clean build clean_build examples demos evals run run_examples run_evals run_evals_visual_debug run_custom_eval

# Default target
all: build

# Clean target
clean:
	rm -rf build

# Build target
build:
	cmake -B build -S .
	cmake --build build --config Release

# Clean and build target
clean_build: clean build

# Examples targets
examples: build list_algorithms update_params camera_stream interactive_camera_stream performance_metrics

list_algorithms: build
	./build/list_algorithms

update_params: build
	./build/update_params

camera_stream: build
	./build/camera_stream

interactive_camera_stream: build
	./build/interactive_camera_stream

performance_metrics: build
	./build/performance_metrics

# Demos targets
demos: build frame_difference_stream static_frame_difference_stream adaptive_background_learning_stream adaptive_selective_bg_learning_stream weighted_moving_mean_stream weighted_moving_variance_stream

frame_difference_stream: build
	./build/frame_difference_stream

static_frame_difference_stream: build
	./build/static_frame_difference_stream

adaptive_background_learning_stream: build
	./build/adaptive_background_learning_stream

adaptive_selective_bg_learning_stream: build
	./build/adaptive_selective_bg_learning_stream

weighted_moving_mean_stream: build
	./build/weighted_moving_mean_stream

weighted_moving_variance_stream: build
	./build/weighted_moving_variance_stream

# Evaluation targets
evals: build evaluate_algorithm

evaluate_algorithm: build
	./build/evaluate_algorithm

run: run_examples run_evals

run_examples:
	./build/list_algorithms
	./build/update_params

DATASET_PATH ?= ./datasets/ucsd/boats
FRAMES_DIR ?= frames
GROUNDTRUTH_DIR ?= groundtruth
FILE_EXTENSION ?= .png
DELAY ?= 30
VISUAL_DEBUG ?= false

# Usage: make run_evals [DATASET_PATH="./datasets/ucsd/boats"] [FRAMES_DIR="frames"] [GROUNDTRUTH_DIR="groundtruth"] [FILE_EXTENSION=".png"] [DELAY=30] [VISUAL_DEBUG=true]
run_evals:
	./build/evaluate_algorithm --algorithm FrameDifference --dataset $(DATASET_PATH) --frames $(FRAMES_DIR) --groundtruth $(GROUNDTRUTH_DIR) --extension $(FILE_EXTENSION) $(if $(filter true,$(VISUAL_DEBUG)),--delay $(DELAY) --visual-debug,)
	./build/evaluate_algorithm --algorithm StaticFrameDifference --dataset $(DATASET_PATH) --frames $(FRAMES_DIR) --groundtruth $(GROUNDTRUTH_DIR) --extension $(FILE_EXTENSION) $(if $(filter true,$(VISUAL_DEBUG)),--delay $(DELAY) --visual-debug,)
	./build/evaluate_algorithm --algorithm AdaptiveBackgroundLearning --dataset $(DATASET_PATH) --frames $(FRAMES_DIR) --groundtruth $(GROUNDTRUTH_DIR) --extension $(FILE_EXTENSION) $(if $(filter true,$(VISUAL_DEBUG)),--delay $(DELAY) --visual-debug,)
	./build/evaluate_algorithm --algorithm AdaptiveSelectiveBackgroundLearning --dataset $(DATASET_PATH) --frames $(FRAMES_DIR) --groundtruth $(GROUNDTRUTH_DIR) --extension $(FILE_EXTENSION) $(if $(filter true,$(VISUAL_DEBUG)),--delay $(DELAY) --visual-debug,)
	./build/evaluate_algorithm --algorithm WeightedMovingMean --dataset $(DATASET_PATH) --frames $(FRAMES_DIR) --groundtruth $(GROUNDTRUTH_DIR) --extension $(FILE_EXTENSION) $(if $(filter true,$(VISUAL_DEBUG)),--delay $(DELAY) --visual-debug,)
	./build/evaluate_algorithm --algorithm WeightedMovingVariance --dataset $(DATASET_PATH) --frames $(FRAMES_DIR) --groundtruth $(GROUNDTRUTH_DIR) --extension $(FILE_EXTENSION) $(if $(filter true,$(VISUAL_DEBUG)),--delay $(DELAY) --visual-debug,)

run_evals_visual_debug:
	$(MAKE) run_evals DELAY=500 VISUAL_DEBUG=true

# Usage: make run_custom_eval EVAL_ARGS="--algorithm WeightedMovingMean --dataset ./my_dataset --delay 100 --visual-debug"
run_custom_eval:
	./build/evaluate_algorithm $(EVAL_ARGS)

# Help target
help:
	@echo "Available targets:"
	@echo "  all               : Default target, same as 'build'"
	@echo "  clean             : Remove build directory"
	@echo "  build             : Configure and build the project"
	@echo "  clean_build       : Clean and then build the project"
	@echo "  examples          : Build and run all examples"
	@echo "  demos             : Build and run all demos"
	@echo "  evals             : Build and run all evaluations"
	@echo "  run               : Run examples and evaluations"
	@echo "  run_evals         : Run evaluations with customizable parameters"
	@echo "  run_evals_visual_debug : Run evaluations with visual debugging enabled"
	@echo "  run_custom_eval   : Run a custom evaluation"
	@echo "  list_algorithms   : Build and run list_algorithms example"
	@echo "  update_params     : Build and run update_params example"
	@echo "  camera_stream     : Build and run camera_stream example"
	@echo "  interactive_camera_stream : Build and run interactive_camera_stream example"
	@echo "  performance_metrics : Build and run performance_metrics example"
	@echo "  frame_difference_stream : Build and run frame_difference_stream demo"
	@echo "  static_frame_difference_stream : Build and run static_frame_difference_stream demo"
	@echo "  adaptive_background_learning_stream : Build and run adaptive_background_learning_stream demo"
	@echo "  adaptive_selective_bg_learning_stream : Build and run adaptive_selective_bg_learning_stream demo"
	@echo "  weighted_moving_mean_stream : Build and run weighted_moving_mean_stream demo"
	@echo "  weighted_moving_variance_stream : Build and run weighted_moving_variance_stream demo"
	@echo "  evaluate_algorithm : Build and run evaluate_algorithm evaluation"
	@echo "  help              : Display this help message"