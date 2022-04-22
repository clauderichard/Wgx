#include "Traverser.hpp"
#include "Common/All.hpp"
#include <iostream>
using namespace std;

////////////////////////////////////////////////

Value TravLeafValue::traverse(ITraverser *trav,
							  ParseTree tree)
{
	return _value;
}

////////////////////////////////////////////////

void Traverser::addNodePreFunc(PrRuleId ruleId,
							   void (*fun)())
{
	_nodeInstrs.at(ruleId)->_preFunc = fun;
}

Value Traverser::traverse(ParseTree tree)
{
	if (tree.isLeaf())
	{
		auto &tok = tree.getToken();
		auto instr = _leafInstrs.at(tok._type);
		return instr->traverse(this, tree);
	}
	else
	{
		auto instr = _nodeInstrs.at(tree.getRuleId());
		return instr->traverse(this, tree);
	}
}

////////////////////////////////////////////////
