#include <iostream>
#include "Oz/Oz.hh"

namespace Oz {

  Oz::Oz(Gateway::Gateway & gateway) :
    camera { gateway },
    lidar {},
    motor { gateway },
    cpu {}
{
}
  
  Camera& Oz::getCamera(void)
  {
    return (this->camera);
  }

  Motor& Oz::getMotor(void)
  {
    return (this->motor);
  }

}
