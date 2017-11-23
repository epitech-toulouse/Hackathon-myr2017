#pragma once

#include <string>
#include <functional>
#include <ApiCodec/ApiCommandPacket.hpp>
#include <cstdlib>
#include "exceptions.hh"
#include "constants.hh"
#include "Algorithm/Algorithm.hh"
#include "Display/Display.hh"
#include "Gateway/Gateway.hh"
#include "Oz/Oz.hh"
#include "System.hh"
#include "Arguments.hh"

namespace Playground
{

  class Playground
  {
  private:
    Arguments args;
    System system;
    Gateway::Gateway gateway;
    Oz::Oz oz;
    Display::Display display;
    Algorithm::Algorithm algorithm;

    int run(void);
    std::thread _update_thread();

  public:
    explicit Playground(int argc, char ** argv);
    ~Playground() {};

    Oz::Oz & getOz(void);
    Gateway::Gateway & getGateway(void);
    Display::Display & getDisplay(void);
  };

}
