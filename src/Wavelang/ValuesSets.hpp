#ifndef __doubleALUESSETS_HPP__
#define __doubleALUESSETS_HPP__

#include <cstddef>
#include <map>
#include <string>
#include <vector>
using namespace std;

////////////////////////////////////////////////

class ValuesSet
{
  public:
	ValuesSet(size_t capacity);
	bool contains(const double *addr) const;

	size_t size() const{return _vals.size();}

	void print();
  protected:
	vector<double> _vals;
};

////////////////////////////////////////////////

class ConstsSet : public ValuesSet
{
  public:
	ConstsSet(size_t capacity);
	
	const double *getOrAdd(const double &val);

  private:
	size_t _numVals;
	map<double, size_t> _valToIndex;
};

////////////////////////////////////////////////

class VarsSet : public ValuesSet
{
  public:
	VarsSet(size_t capacity);
	
	double *getMutable(size_t index);
	const double *get(size_t index);

};

////////////////////////////////////////////////

class ConstVarsSet : public ValuesSet
{
  public:
	ConstVarsSet(size_t capacity);
	
	const double *get(size_t index);

};

////////////////////////////////////////////////

class NamedVarsSet : public ValuesSet
{
  public:
	NamedVarsSet(size_t capacity);
	
	const double *getOrAdd(const string &id);

	string getNameOf(const double *addr);

  private:
	size_t _numVals;
	map<string, size_t> _nameToIndex;
	map<size_t, string> _indexToName;
};

////////////////////////////////////////////////

extern ConstVarsSet g_voiceInputVars;
extern VarsSet g_effectInputVars;
extern ConstsSet g_constDoubles;
extern NamedVarsSet g_namedVarsDoubles;

////////////////////////////////////////////////

#endif