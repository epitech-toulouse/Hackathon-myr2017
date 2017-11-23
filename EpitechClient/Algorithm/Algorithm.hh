#pragma once

#include "Oz/Oz.hh"

namespace Algorithm
{

class Algorithm
{
public:
	explicit Algorithm(Oz::Oz &);
	void init();
	void update();

private:
	Oz::Oz & _oz;
};

}
