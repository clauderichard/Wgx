
#ifndef __ACTIONPECIES_HPP__
#define __ACTIONPECIES_HPP__

#include "Egg/IEventAction.hpp"
#include <memory>
using namespace std;

////////////////////////////////////////////////

extern shared_ptr<IEventAction> g_egglangNoteAction;
extern shared_ptr<IEventAction> g_egglangRestAction;

void actionSpeciesInitialize();

////////////////////////////////////////////////

#endif