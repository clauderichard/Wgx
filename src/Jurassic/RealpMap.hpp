#ifndef __REALPMAP_HPP__
#define __REALPMAP_HPP__

#include "Realp.hpp"
#include <map>
#include <set>
using namespace std;

class RealpMap
{
  public:
	void set(crealp, crealp);
	crealp operator[](crealp);
	void flagAsChanging(crealp);
	bool isChanging(crealp);
	
	void print() const;

  private:
	map<crealp, crealp> _realMap;
	std::set<crealp> _changingValues;
};

#endif