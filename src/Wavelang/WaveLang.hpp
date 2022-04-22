#ifndef __WAVELANG_HPP__
#define __WAVELANG_HPP__

#include <memory>
#include <map>
#include <string>
#include "Parsing/Language.hpp"
#include "RealFunc.hpp"
#include "JurassicUniverse.hpp"
using namespace std;

////////////////////////////////////////////////

class IVoice;
class JurassicWorld;
class JurassicPark;
class JurassicUniverse;

typedef SynthesizerWithNames WaveLangResult;

struct Op1
{
	realfunc1 _f;
};
struct Op2
{
	realfunc2 _g;
};
struct OpAmbiguous
{
	realfunc1 _f;
	realfunc2 _g;
};

////////////////////////////////////////////////

class WaveLang
{
  public:
    WaveLang();
    Value decodeIntoValue(const string &code);
    WaveLangResult decode(const string &code);

  private:
    Language _language;

    int _minUnusedTkType;

    void registerVariable(const string &symbol, size_t index);
    void registerMutableVariable(const string &symbol, size_t index);
    void registerConstant(const string &symbol, double value);
    void registerDefaultPark(const string &bindingCode);
    void registerUnOp(const string &symbol,
      int priority, 
      realfunc1 theFunc);
    void registerBinOp(const string &symbol,
      int priority, 
      realfunc2 theFunc);
    void registerAmbOp(const string &symbol,
      int priority, 
      realfunc1 theFunc1,
      realfunc2 theFunc2);

};

#endif