#ifndef __PARSING_TYPES_HPP__
#define __PARSING_TYPES_HPP__

#include <cstddef>

typedef int TkType;
typedef size_t TkState;
typedef size_t PrRuleId;

#define TKTYPE_NONE -9
#define TKTYPE_FINAL -10
#define TKTYPE_BOF -11
#define TKTYPE_EOF -12

#endif