#ifndef __ITREX_HPP__
#define __ITREX_HPP__

#include "Realp.hpp"
#include "RealpMap.hpp"
#include "CrealpNamer.hpp"
#include <vector>
using namespace std;

typedef vector<crealp *> TrexArgList;

// Real-valued function of something else.
// If you want bool-valued, you could have a convention of
//   positive double means true, negative means false.
class ITrex
{
  public:
	ITrex();
	virtual ~ITrex();

	////////////////////////////////
	virtual void eval() = 0;
	virtual ITrex *copy() = 0;
	virtual TrexArgList getArgs() = 0;
	virtual bool introducesChange() const = 0;
	virtual void reset();
	////////////////////////////////
	virtual void print(CrealpNamer &n) const;
	virtual void initialize();
	////////////////////////////////

	ITrex *copy(RealpMap &);
	ITrex *copyAndAddToMap(RealpMap &);
	void updateArgs(RealpMap &);
	crealp getResult();
	void *getResult() const;

	
  protected:
	double _result;
};

#endif