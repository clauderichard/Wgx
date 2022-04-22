#ifndef __EVENT_HPP__
#define __EVENT_HPP__

#include "EventInfo.hpp"
#include "IEventAction.hpp"
#include <memory>
using namespace std;

////////////////////////////////////////////////

struct Event
{
	EventInfo _info;
	shared_ptr<IEventAction> _action;
};

////////////////////////////////////////////////

#endif