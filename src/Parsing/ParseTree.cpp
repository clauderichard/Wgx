
#include "ParseTree.hpp"
#include "Token.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <sstream>
using namespace std;

////////////////////////////////////////////////

ParseTree::ParseTree()
	: _node(NULL)
{
}

ParseTree::ParseTree(Token &token)
	: _node(new ParseTreeNode())
{
	_node->_token = token;
}

ParseTree::ParseTree(PrRuleId ruleId,
					 TkType type,
					 vector<size_t> valuedChildIndexes)
	: _node(new ParseTreeNode())
{
	_node->_ruleId = ruleId;
	_node->_token._type = type;
	for (size_t i = 0; i < valuedChildIndexes.size(); i++)
	{
		_node->_valuedChildIndexes.push_back(valuedChildIndexes[i]);
	}
}

ParseTree::~ParseTree() {}

void ParseTree::addChild(ParseTree child)
{
	_node->_children.push_back(child._node);
}

bool ParseTree::isLeaf() const
{
	return _node->_children.size() == 0;
}

const Token &ParseTree::getToken() const
{
	return _node->_token;
}

PrRuleId ParseTree::getRuleId() const
{
	return _node->_ruleId;
}

size_t ParseTree::getNumChildren() const
{
	return _node->_children.size();
}

size_t ParseTree::getNumValuedChildren() const
{
	return _node->_valuedChildIndexes.size();
}

ParseTree ParseTree::getChild(size_t index) const
{
	return ParseTree(_node->_children[index]);
}

ParseTree ParseTree::getValuedChild(size_t index) const
{
	size_t childIndex = _node->_valuedChildIndexes[index];
	return ParseTree(_node->_children[childIndex]);
}

void ParseTree::print(const map<TkType, string> &tkTypeReprs, size_t spaces, bool printChildren) const
{
	auto &tok = getToken();
	for (size_t i = 0; i < spaces; i++)
	{
		cout << " ";
	}
	string rootRepr = "?";
	try
	{
		rootRepr = tkTypeReprs.at(tok._type);
	}
	catch (const std::exception &)
	{
	}
	cout << rootRepr;

	if (getNumChildren() == 0)
	{
		if (tok._type != TKTYPE_BOF && tok._type != TKTYPE_EOF)
		{
			//string tokstr = code.substr(tok._startIndex, tok._length);
			//cout << " (\"" << tokstr << "\")";
			cout << " (\"" << *tok._string << "\")";
		}
	}
	cout << endl;
	if (printChildren)
	{
		for (size_t i = 0; i < getNumChildren(); i++)
		{
			getChild(i).print(tkTypeReprs, spaces + 2);
		}
	}
}

ParseTree::ParseTree(shared_ptr<ParseTreeNode> node)
	: _node(node) {}

////////////////////////////////////////////////
