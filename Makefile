# Phony targets
.PHONY: all clean build clean_build examples demos

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
demos: build frame_difference_stream adaptive_background_learning_stream adaptive_selective_bg_learning_stream weighted_moving_mean_stream

frame_difference_stream: build
	./build/frame_difference_stream

adaptive_background_learning_stream: build
	./build/adaptive_background_learning_stream

adaptive_selective_bg_learning_stream: build
	./build/adaptive_selective_bg_learning_stream

weighted_moving_mean_stream: build
	./build/weighted_moving_mean_stream

# Help target
help:
	@echo "Available targets:"
	@echo "  all               : Default target, same as 'build'"
	@echo "  clean             : Remove build directory"
	@echo "  build             : Configure and build the project"
	@echo "  clean_build       : Clean and then build the project"
	@echo "  examples          : Build and run all examples"
	@echo "  demos             : Build and run all demos"
	@echo "  list_algorithms   : Build and run list_algorithms example"
	@echo "  update_params     : Build and run update_params example"
	@echo "  camera_stream     : Build and run camera_stream example"
	@echo "  interactive_camera_stream : Build and run interactive_camera_stream example"
	@echo "  performance_metrics : Build and run performance_metrics example"
	@echo "  frame_difference_stream : Build and run frame_difference_stream demo"
	@echo "  adaptive_background_learning_stream : Build and run adaptive_background_learning_stream demo"
	@echo "  adaptive_selective_bg_learning_stream : Build and run adaptive_selective_bg_learning_stream demo"
	@echo "  weighted_moving_mean_stream : Build and run weighted_moving_mean_stream demo"
	@echo "  help              : Display this help message"
