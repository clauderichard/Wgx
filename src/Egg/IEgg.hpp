#ifndef __IEGG_HPP__
#define __IEGG_HPP__

#include "Common/Crash.hpp"
#include "Event.hpp"
#include <memory>
using namespace std;

////////////////////////////////////////////////

class IEgg
{
  public:
	virtual ~IEgg() {}
	
	virtual shared_ptr<IEgg> deepcopy() = 0;

	// reset to start.
	virtual void reset() = 0;

	// throw false if no event to pop.
	virtual Event pop() = 0;
};

////////////////////////////////////////////////

#endif