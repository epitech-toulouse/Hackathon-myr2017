#pragma once

#include <chrono>

/* Naïo Technologies, Technical Note, 3.2.7 */
static const size_t CAMERA_CAPTURE_WIDTH = 752 / 2;
static const size_t CAMERA_CAPTURE_HEIGHT = 480 / 2;
static const size_t CAMERA_CAPTURE_RESOLUTION = CAMERA_CAPTURE_WIDTH * CAMERA_CAPTURE_HEIGHT;
static const size_t CAMERA_BUFFER_SIZE = 0x400000;

/* Naïo Technologies, Technical Note, 5.2, Q2 */
static const size_t LIDAR_CAPTURE_RESOLUTION = 271;
static const double LIDAR_BEGIN_ANGLE = -45.0;
static const double LIDAR_END_ANGLE = 225.0;
static const uint16_t LIDAR_MIN_DISTANCE = 1;
static const uint16_t LIDAR_MAX_DISTANCE = 4000;

static const std::chrono::milliseconds WAIT_TIME_MS { 100 };

static const size_t GATEWAY_BUFFER_SIZE = 0x400000;

static const std::chrono::milliseconds SOCKET_RECONNECT_DELAY { 2000 };
