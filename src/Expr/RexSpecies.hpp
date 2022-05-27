#ifndef __REXSPECIES_HPP__
#define __REXSPECIES_HPP__

#include <vector>
#include "Rex.hpp"
#include "EquationSet.hpp"
#include "Jurassic/RealFunc.hpp"
#include "Jurassic/ITrex.hpp"
using namespace std;

////////////////////////////////////////////////

class RexConst : public IRexImpl
{
  public:
	RexConst(double val);
    virtual ~RexConst();

    crealp processImpl(RexProcessContext &prog, Rex self);
    
  private:
    double _val;
};

////////////////////////////////////////////////

class RexVarName : public IRexImpl
{
  public:
    RexVarName(const string &name);
    virtual ~RexVarName();

    crealp processImpl(RexProcessContext &prog, Rex self);
    
    virtual bool dependsOnInputTypeImpl(size_t type, RexProcessContext &prog, Rex self);

  private:
    string _name;
};

////////////////////////////////////////////////

class RexInputVar : public IRexImpl
{
  public:
	  RexInputVar(size_t typeIndex, size_t index);
    virtual ~RexInputVar();

    virtual crealp processImpl(RexProcessContext &prog, Rex self);
    
    virtual bool dependsOnInputTypeImpl(size_t type, RexProcessContext &prog, Rex self);
    
  private:
    size_t _typeIndex;
    size_t _index;
};

////////////////////////////////////////////////

class RexCustomFunArg : public IRexImpl
{
  public:
	  RexCustomFunArg(size_t index);
    virtual ~RexCustomFunArg();

    crealp processImpl(RexProcessContext &prog, Rex self);

  private:
    size_t _index;
};

////////////////////////////////////////////////

class RexCustomFunCall : public IRexImpl
{
  public:
	  RexCustomFunCall(const string &funName, vector<Rex> args);
    virtual ~RexCustomFunCall();

    Rex copyWithSubstitutionsImpl(RexSubstitutions &subs, Rex self);

    crealp processImpl(RexProcessContext &prog, Rex self);
    
    virtual bool dependsOnInputTypeImpl(size_t type, RexProcessContext &prog, Rex self);

  private:
    string _funName;
    vector<Rex> _args;
};

////////////////////////////////////////////////

class RexTrex : public IRexImpl
{
  public:
	  RexTrex(shared_ptr<ITrex> trex, vector<Rex> args);
    virtual ~RexTrex();

    Rex copyWithSubstitutionsImpl(RexSubstitutions &subs, Rex self);

    crealp processImpl(RexProcessContext &prog, Rex self);
    
    virtual bool dependsOnInputTypeImpl(size_t type, RexProcessContext &prog, Rex self);

  private:
    shared_ptr<ITrex> _trex;
    vector<Rex> _args;
};

////////////////////////////////////////////////

class RexTrexNew : public IRexImpl
{
  public:
	  RexTrexNew(vector<Rex> args);
    virtual ~RexTrexNew();

    Rex copyWithSubstitutionsImpl(RexSubstitutions &subs, Rex self);

    crealp processImpl(RexProcessContext &prog, Rex self);
    
    virtual bool dependsOnInputTypeImpl(size_t type, RexProcessContext &prog, Rex self);

    virtual ITrex *contructTrex() = 0;
    virtual RexTrexNew *copy(vector<Rex> &newArgs) = 0;

  protected:
    vector<Rex> _args;
};

////////////////////////////////////////////////

class RexTrexFunc1 : public RexTrexNew
{
  public:
	  RexTrexFunc1(realfunc1 f, Rex a);
    virtual ~RexTrexFunc1();

    virtual ITrex *contructTrex();
    virtual RexTrexNew *copy(vector<Rex> &newArgs);
  private:
    realfunc1 _f;
};

////////////////////////////////////////////////

#endif