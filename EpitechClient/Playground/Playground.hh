#pragma once

#include <string>
#include <functional>
#include <ApiCodec/ApiCommandPacket.hpp>
#include <cstdlib>
#include "exceptions.hh"
#include "constants.hh"
#include "Display/Display.hh"
#include "Gateway/Gateway.hh"
#include "Oz/Oz.hh"
#include "System.hh"

namespace Playground
{

  class Playground
  {
  private:
    System system;
    Gateway::Gateway gateway;
    Oz::Oz oz;
    Display::Display display;

    int run(void);

  public:
    explicit Playground(const std::string &, const std::string &, const std::string &);
    ~Playground() {};

    Oz::Oz & getOz(void);
    Gateway::Gateway & getGateway(void);
    Display::Display & getDisplay(void);
  };

}
