#ifndef __EGGLANG_HPP__
#define __EGGLANG_HPP__

#include <memory>
#include <map>
#include <string>
#include "Parsing/Language.hpp"
#include "Egg/IEgg.hpp"
#include "EgglangSynthesizer.hpp"
using namespace std;

////////////////////////////////////////////////

typedef shared_ptr<IEgg> EgglangResult;

extern bool g_isStartPresent;

////////////////////////////////////////////////
enum ForkerElemType
{
	ForkerElemType_Egg,
	ForkerElemType_OfficialEnd,
	ForkerElemType_Death
};
struct ForkerElem
{
	ForkerElemType _type;
	shared_ptr<IEgg> _egg;
};
////////////////////////////////////////////////

class Egglang
{
  public:
    Egglang();
    Language *getLang();
    
    static void initVoiceIndexes(map<string,size_t> &);
    static size_t getVoiceIndex(const string &name);
    
    Value decodeIntoValue(const string &code);
    EgglangResult decodeLine(const string &line);

  private:
    Language _language;

    static map<string,size_t> g_voiceNameToIndex;

};

#endif