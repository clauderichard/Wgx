#ifndef __PARSETREE_HPP__
#define __PARSETREE_HPP__

#include "Token.hpp"
#include <memory>
#include <vector>
#include <map>
using namespace std;

////////////////////////////////////////////////

struct ParseTreeNode
{
	Token _token;	  // Important only if this is a leaf.
	PrRuleId _ruleId; // Important only if this is not a leaf.
	vector<shared_ptr<ParseTreeNode>> _children;

	// Valued children are the ones for which the traverser cares about the values.
	// E.g. if a rule if EXPR <- LBRACKET X RBRACKET then
	// this list could be [1] because children 0 and 2 don't have a value.
	vector<size_t> _valuedChildIndexes;
};

////////////////////////////////////////////////

class ParseTree
{
  public:
	// constructors
	ParseTree();
	ParseTree(Token &);
	ParseTree(PrRuleId ruleId,
			  TkType type,
			  vector<size_t> valuedChildIndexes);
	~ParseTree();
	void addChild(ParseTree);

	// accessors
	bool isLeaf() const;
	const Token &getToken() const;
	PrRuleId getRuleId() const;
	size_t getNumChildren() const;
	size_t getNumValuedChildren() const;
	ParseTree getChild(size_t index) const;
	ParseTree getValuedChild(size_t index) const;

	void print(const map<TkType, string> &tkTypeReprs, size_t spaces) const;

  private:
	ParseTree(shared_ptr<ParseTreeNode> node);
	shared_ptr<ParseTreeNode> _node;
};

////////////////////////////////////////////////

#endif