#pragma once
#include "Oz/Unit.hh"

namespace Oz
{
  class CPU : public Unit
  {
  private:


  public:
    explicit CPU(void);
    void update() override;
  };
}
