#ifndef __REALPMAP_HPP__
#define __REALPMAP_HPP__

#include "Realp.hpp"
#include <map>
using namespace std;

class RealpMap
{
  public:
	void set(crealp, crealp);
	crealp operator[](crealp);
	
	void print() const;

  private:
	map<crealp, crealp> _realMap;
};

#endif