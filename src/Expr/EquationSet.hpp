#ifndef __EQUATIONSET_HPP__
#define __EQUATIONSET_HPP__

#include "Common/Crash.hpp"
#include <string>
#include <map>
using namespace std;

class Rex;
struct RexProcessContext;
class IVoice;
class IEffect;

////////////////////////////////////////////////

class EquationSet
{
  public:
    bool contains(const string &name);
    void set(const string &name, Rex rex);
    Rex get(const string &name);

    void addMissingFrom(EquationSet &other);
    EquationSet copy() const;
    EquationSet operator+=(EquationSet &other);

    RexProcessContext process(vector<string> &outNames);
    void Validate(RexProcessContext &prog);

    bool isValidVoice();
    bool isValidEffect();
    shared_ptr<IVoice> toVoice();
    shared_ptr<IEffect> toEffect();

  protected:
    bool outDependsOnInputType(size_t type);
    vector<string> getUsedOutNames();

	  map<string, Rex> _equations;
};

////////////////////////////////////////////////

#endif