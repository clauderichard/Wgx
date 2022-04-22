#ifndef __TRANSMATRIX_HPP__
#define __TRANSMATRIX_HPP__

#include <unordered_map>
#include <set>
#include <iostream>
#include "TypeDefs.hpp"
using namespace std;

class TransMatrix
{
  public:
	TransMatrix(bool hasI = false);
	~TransMatrix();

	bool hasI();
	void addArrow(TkType a, TkType b);
	bool hasArrow(TkType a, TkType b) const;

	const set<TkType> &operator[](TkType);

	bool operator==(TransMatrix &other) const;

	TransMatrix operator*(TransMatrix &);

	// Modifies this object in-place!
	void selfPowInf();

  private:
	unordered_map<TkType, set<TkType>> _map;
	bool _hasI;

	// Get list of endpoints for a start point.
	// Initializes it as empty list if it's absent.
	set<TkType> &getBs(TkType a);

	TransMatrix selfSquare();
	size_t getNumNonzeroes() const;
};

ostream &operator<<(ostream &, TransMatrix &);

#endif