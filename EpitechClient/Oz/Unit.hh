#pragma once

namespace Oz
{

class Unit
{
public:
	virtual ~Unit() = default;
	virtual void update() = 0;
};

}
