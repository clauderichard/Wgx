#ifndef __IEVENTACTION_HPP__
#define __IEVENTACTION_HPP__

#include "EventInfo.hpp"
using namespace std;

////////////////////////////////////////////////

class IEventAction
{
  public:
	virtual ~IEventAction(){};
	virtual void execute(const EventInfo &) = 0;
};

////////////////////////////////////////////////

#endif