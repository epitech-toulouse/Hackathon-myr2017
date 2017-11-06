#pragma once

#include <chrono>

static const size_t CAMERA_CAPTURE_WIDTH = 752 / 2;
static const size_t CAMERA_CAPTURE_HEIGHT = 480 / 2;
static const size_t CAMERA_CAPTURE_RESOLUTION = CAMERA_CAPTURE_WIDTH * CAMERA_CAPTURE_HEIGHT;
static const size_t CAMERA_BUFFER_SIZE = 0x400000;

static const size_t LIDAR_CAPTURE_RESOLUTION = 271;
static const double LIDAR_BEGIN_ANGLE = -45.0;
static const double LIDAR_END_ANGLE = 225.0;

static const std::chrono::milliseconds WAIT_TIME_MS { 100 };

static const size_t GATEWAY_BUFFER_SIZE = 0x400000;
