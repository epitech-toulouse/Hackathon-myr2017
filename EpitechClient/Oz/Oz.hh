#pragma once

#include "Oz/Motor.hh"
#include "Oz/Lidar.hh"
#include "Oz/Camera.hh"
#include "Oz/CPU.hh"
#include "Gateway/Gateway.hh"
#include "Oz/GPS.hh"

namespace Oz
{
  class Oz
  {
  private:
    Camera camera;
    Lidar lidar;
    Motor motor;
    CPU cpu;
    std::array<std::reference_wrapper<Unit>, 4> _iterable;
    GPS gps;

  public:
    typedef decltype(_iterable)::iterator iterator;

  public:
    explicit Oz(Gateway::Gateway &);
    const Camera & getCamera() const noexcept;
    const Lidar & getLidar() const noexcept;
    const Motor & getMotor() const noexcept;
    const CPU & getCPU() const noexcept;
    iterator begin() noexcept;
    iterator end() noexcept;
    const GPS & getGPS() const noexcept;

  };

}
