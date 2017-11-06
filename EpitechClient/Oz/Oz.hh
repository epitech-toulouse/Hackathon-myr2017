#include "Oz/Motor.hh"
#include "Oz/Lidar.hh"
#include "Oz/Camera.hh"
#include "Oz/CPU.hh"
#include "Gateway/Gateway.hh"

namespace Oz
{

  class Oz
  {
  private:
    Camera camera;
    Lidar lidar;
    Motor motor;
    CPU cpu;

  public:
    explicit Oz(Gateway::Gateway &);
    ~Oz() {}

    Camera& getCamera(void);
    /*Lidar getLidar(void);
    Motor getMotor(void);
    Motor getCPU(void);*/
  };

}
