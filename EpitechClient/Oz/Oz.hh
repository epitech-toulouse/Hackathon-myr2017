#pragma once

#include "Oz/Motor.hh"
#include "Oz/Lidar.hh"
#include "Oz/Camera.hh"
#include "Oz/CPU.hh"
#include "Gateway/Gateway.hh"
#include "Oz/GPS.hh"
#include "Oz/Gyro.hh"
#include "Oz/ODO.hh"
#include "Oz/Actuator.hh"

namespace Oz
{
  class Oz
  {
  private:
    Camera camera;
    Lidar lidar;
    Motor motor;
    CPU cpu;
    GPS gps;
    Gyro gyro;
    ODO odo;
    Actuator actuator;
    std::array<std::reference_wrapper<Unit>, 8> _iterable;

  public:
    typedef decltype(_iterable)::iterator iterator;

  public:
    explicit Oz(Gateway::Gateway &);

    const Camera & getCamera() const noexcept;
    Camera & getCamera() noexcept;

    const Lidar & getLidar() const noexcept;
    Lidar & getLidar() noexcept;

    const Motor & getMotor() const noexcept;
    Motor & getMotor() noexcept;

    const CPU & getCPU() const noexcept;
    CPU & getCPU() noexcept;

    const GPS & getGPS() const noexcept;
    GPS & getGPS() noexcept;

    const Gyro & getGyro() const noexcept;
    Gyro & getGyro() noexcept;

    const ODO & getODO() const noexcept;
    ODO & getODO() noexcept;

    const Actuator & getActuator() const noexcept;
    Actuator & getActuator() noexcept;

    iterator begin() noexcept;
    iterator end() noexcept;
  };

}
