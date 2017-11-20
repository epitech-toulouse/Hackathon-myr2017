#pragma once
#include "Oz/Unit.hh"

namespace Oz
{
  class Motor : public Unit
  {
  private:

  public:
    explicit Motor(void);
    void update() override;
  };
}
