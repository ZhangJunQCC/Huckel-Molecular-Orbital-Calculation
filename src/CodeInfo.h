#ifndef    __CODEINFO_H__
#define    __CODEINFO_H__

#include <string>
#include <vector>

namespace hmo {

using namespace std;

class CodeInfo {
public:
    CodeInfo(void);
    ~CodeInfo(void);

    static const string& GetName(void);
    static const string& GetPlatform(void);
    static unsigned int GetMajorVersion(void);
    static unsigned int GetMinorVersion(void);
    
    static void PrintCodeInfo(void);

private:
    static const string name;
    static const string platform;
    static const unsigned int majorversion;
    static const unsigned int minorversion;

    static void PrintCodeVersion(void);
    static void PrintPrologue(void);    
};

}

#endif //  __CODEINFO_H__
