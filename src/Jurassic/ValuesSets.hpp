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

class ChangingValuesSet : public ValuesSet
{
  public:
	ChangingValuesSet(size_t capacity);
	
	double *getOrAdd(const string &name);

  private:
	size_t _numVals;
	map<string, size_t> _nameToIndex;
};

////////////////////////////////////////////////

#endif