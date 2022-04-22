#ifndef __TRAVERSER_HPP__
#define __TRAVERSER_HPP__

#include "Value.hpp"
#include "ParseTree.hpp"
#include <map>
using namespace std;

class ITraverser
{
  public:
	virtual ~ITraverser() {}
	virtual Value traverse(ParseTree tree) = 0;
};

////////////////////////////////////////////////

struct ITravInstruction
{
	virtual ~ITravInstruction() {}
	virtual Value traverse(ITraverser *trav,
						   ParseTree tree) = 0;
};

////////////////////////////////////////////////

template <typename R>
struct TravLeafFunc : public ITravInstruction
{
	R(*_f)
	(const string &);
	TravLeafFunc(R (*f)(const string &))
		: _f(f) {}
	Value traverse(ITraverser *trav,
				   ParseTree tree)
	{
		auto &tok = tree.getToken();
		auto arg = *tok._string;
		return Value::make<R>(_f(arg));
	}
};

////////////////////////////////////////////////

struct TravLeafValue : public ITravInstruction
{
	Value _value;
	TravLeafValue() {}
	TravLeafValue(Value val)
		: _value(val) {}
	Value traverse(ITraverser *trav,
				   ParseTree tree);
};

////////////////////////////////////////////////

struct ITravNodeInstruction
{
	void (*_preFunc)();
	ITravNodeInstruction()
		: _preFunc(NULL) {}
	virtual ~ITravNodeInstruction() {}
	Value traverse(ITraverser *trav,
				   ParseTree tree)
	{
		if (_preFunc != NULL)
			_preFunc();
		return traverse2(trav, tree);
	}
	virtual Value traverse2(ITraverser *trav,
							ParseTree tree) = 0;
};

////////////////////////////////////////////////

template <typename R>
struct TravNodeFunc0 : public ITravNodeInstruction
{
	R(*_f)
	();
	TravNodeFunc0(R (*f)())
		: _f(f) {}
	Value traverse2(ITraverser *trav,
					ParseTree tree)
	{
		return Value::make<R>(_f());
	}
};

////////////////////////////////////////////////

struct TravNodeFuncIdentity : public ITravNodeInstruction
{
	TravNodeFuncIdentity() {}
	Value traverse2(ITraverser *trav,
					ParseTree tree)
	{
		return trav->traverse(tree.getValuedChild(0));
	}
};

////////////////////////////////////////////////

template <typename R, typename A>
struct TravNodeFunc1 : public ITravNodeInstruction
{
	R(*_f)
	(A &);
	TravNodeFunc1(R (*f)(A &))
		: _f(f) {}
	Value traverse2(ITraverser *trav,
					ParseTree tree)
	{
		A a = trav->traverse(tree.getValuedChild(0)).get<A>();
		return Value::make<R>(_f(a));
	}
};

////////////////////////////////////////////////

template <typename R, typename A, typename B>
struct TravNodeFunc2 : public ITravNodeInstruction
{
	R(*_f)
	(A &, B &);
	TravNodeFunc2(R (*f)(A &, B &))
		: _f(f) {}
	Value traverse2(ITraverser *trav,
					ParseTree tree)
	{
		A a = trav->traverse(tree.getValuedChild(0)).get<A>();
		B b = trav->traverse(tree.getValuedChild(1)).get<B>();
		return Value::make<R>(_f(a, b));
	}
};

////////////////////////////////////////////////

template <typename R, typename A, typename B, typename C>
struct TravNodeFunc3 : public ITravNodeInstruction
{
	R(*_f)
	(A &, B &, C &);
	TravNodeFunc3(R (*f)(A &, B &, C &))
		: _f(f) {}

	Value traverse2(ITraverser *trav,
					ParseTree tree)
	{
		A a = trav->traverse(tree.getValuedChild(0)).get<A>();
		B b = trav->traverse(tree.getValuedChild(1)).get<B>();
		C c = trav->traverse(tree.getValuedChild(2)).get<C>();
		return Value::make<R>(_f(a, b, c));
	}
};

////////////////////////////////////////////////

template <typename R, typename A>
struct TravNodeFuncList1 : public ITravNodeInstruction
{
	PrRuleId _delimRuleId;
	R(*_f)
	(vector<A> &);
	TravNodeFuncList1(PrRuleId delimRuleId,
					  R (*f)(vector<A> &))
		: _delimRuleId(delimRuleId),
		  _f(f) {}
	void traverseStarRule(
		vector<A> &argVec,
		ITraverser *trav,
		ParseTree tree)
	{
		if (tree.getRuleId() == _delimRuleId)
		{
			return;
		}
		traverseStarRule(argVec, trav, tree.getChild(0));
		argVec.push_back(trav->traverse(tree.getChild(1)).get<A>());
	}
	Value traverse2(ITraverser *trav,
					ParseTree tree)
	{
		vector<A> argVec;
		traverseStarRule(argVec, trav, tree.getChild(0));
		return Value::make<R>(_f(argVec));
	}
};

////////////////////////////////////////////////

template <typename R, typename A, typename B>
struct TravNodeFuncList2 : public ITravNodeInstruction
{
	PrRuleId _initRuleId;
	R(*_f)
	(vector<A> &, vector<B> &);
	TravNodeFuncList2(PrRuleId initRuleId,
					  R (*f)(vector<A> &, vector<B> &))
		: _initRuleId(initRuleId),
		  _f(f) {}
		  
	void traverseStarRule(
		vector<A> &veca,
		vector<B> &vecb,
		ITraverser *trav,
		ParseTree tree)
	{
		if (tree.getRuleId() == _initRuleId)
		{
			veca.push_back(trav->traverse(tree.getValuedChild(0)).get<A>());
			return;
		}
		traverseStarRule(veca, vecb, trav, tree.getChild(0));
		veca.push_back(trav->traverse(tree.getChild(2)).get<A>());
		vecb.push_back(trav->traverse(tree.getChild(1)).get<B>());
	}
	
	Value traverse2(ITraverser *trav,
					ParseTree tree)
	{
		vector<A> veca;
		vector<B> vecb;
		traverseStarRule(veca, vecb, trav, tree.getChild(0));
		return Value::make<R>(_f(veca, vecb));
	}
};

////////////////////////////////////////////////

class Traverser : public ITraverser
{
  public:
	Value traverse(ParseTree tree);

	void addLeafInstr(TkType typ, ITravInstruction *ti)
	{
		_leafInstrs[typ].reset(ti);
	}
	void addNodeInstr(PrRuleId ruleId, ITravNodeInstruction *ti)
	{
		_nodeInstrs[ruleId].reset(ti);
	}

	template <typename R>
	void addLeafValue(TkType typ,
					  R val)
	{
		addLeafInstr(typ, new TravLeafValue(
							  Value::make<R>(val)));
	}

	template <typename R>
	void addLeafFunc(TkType typ,
					 R (*f)(const string &))
	{
		addLeafInstr(typ, new TravLeafFunc<R>(f));
	}

	void addNodeIdentityFunc(PrRuleId ruleId)
	{
		addNodeInstr(ruleId, new TravNodeFuncIdentity());
	}

	template <typename R>
	void addNodeFunc(PrRuleId ruleId,
					 R (*f)())
	{
		addNodeInstr(ruleId, new TravNodeFunc0<R>(f));
	}

	template <typename R, typename A>
	void addNodeFunc(PrRuleId ruleId,
					 R (*f)(A &))
	{
		addNodeInstr(ruleId, new TravNodeFunc1<R,A>(f));
	}

	template <typename R, typename A, typename B>
	void addNodeFunc(PrRuleId ruleId,
					 R (*f)(A &, B &))
	{
		addNodeInstr(ruleId, new TravNodeFunc2<R,A,B>(f));
	}

	template <typename R, typename A, typename B, typename C>
	void addNodeFunc(PrRuleId ruleId,
					 R (*f)(A &, B &, C &))
	{
		addNodeInstr(ruleId, new TravNodeFunc3<R,A,B,C>(f));
	}

	void addNodePreFunc(PrRuleId ruleId,
						void (*fun)());

	template <typename R, typename A>
	void addNodeListFunc(PrRuleId initRuleId,
						 PrRuleId listRuleId,
						 R (*f)(vector<A> &))
	{
		addNodeInstr(listRuleId, new TravNodeFuncList1<R,A>(
									 initRuleId, f));
	}

	template <typename R, typename A, typename B>
	void addNodeListFunc2(PrRuleId initRuleId,
						 PrRuleId appendRuleId,
						 R (*f)(vector<A> &, vector<B> &))
	{
		addNodeInstr(appendRuleId, new TravNodeFuncList2<R,A,B>(
									 initRuleId, f));
	}

  private:
	map<TkType, shared_ptr<ITravInstruction>> _leafInstrs;
	map<PrRuleId, shared_ptr<ITravNodeInstruction>> _nodeInstrs;
};

#endif