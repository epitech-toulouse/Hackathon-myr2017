#include <iostream>
#include "Oz/Oz.hh"

namespace Oz {

  Oz::Oz(Gateway::Gateway & gateway) :
    camera { gateway },
    lidar {},
    motor {},
    cpu {}
{
}
  
  Camera& Oz::getCamera(void)
  {
    return (this->camera);
  }

}
