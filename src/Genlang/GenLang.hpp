#ifndef __GENLANG_HPP__
#define __GENLANG_HPP__

#include <memory>
#include <map>
#include <string>
#include "Wave/Synthesizer.hpp"
#include "Parsing/Language.hpp"
#include "Gen/IEgg.hpp"
#include "GenlangSynthesizer.hpp"
using namespace std;

////////////////////////////////////////////////

typedef shared_ptr<IEgg> GenLangResult;

extern bool g_isStartPresent;

////////////////////////////////////////////////

class GenLang
{
  public:
    GenLang();
    
    static void initVoiceIndexes(map<string,size_t> &);
    static size_t getVoiceIndex(const string &name);
    
    Value decodeIntoValue(const string &code);
    GenLangResult decode(const string &code);

  private:
    Language _language;

    static map<string,size_t> g_voiceNameToIndex;

};

#endif