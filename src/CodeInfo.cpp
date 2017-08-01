#include "CodeInfo.h"
#include <vector>
#include <cstdio>

using namespace hmo;
using namespace std;



const string CodeInfo::name = "HMO";
#if defined(CODEWINDOWS)
const string CodeInfo::platform = "Windows";
#elif defined(CODELINUX)
const string CodeInfo::platform = "Linux";
#elif defined(CODELINUX)
const string CodeInfo::platform = "Mac OS X";
#else
#error This operating system is unsupported.
#endif
const unsigned int CodeInfo::majorversion = CODEMAJORVER;
const unsigned int CodeInfo::minorversion = CODEMINORVER;

CodeInfo::CodeInfo(void)
{
    /* Nothing to do */ 
}

CodeInfo::~CodeInfo(void) 
{ 
    /* Nothing to do */ 
}

const string& CodeInfo::GetName(void)
{
    return name;
}

const string& CodeInfo::GetPlatform(void)
{
    return platform;
}

unsigned int CodeInfo::GetMajorVersion(void)
{
    return majorversion;
}


unsigned int CodeInfo::GetMinorVersion(void)
{
    return minorversion;
}

void CodeInfo::PrintCodeInfo(void)
{
    PrintCodeVersion();
    PrintPrologue();
}

void CodeInfo::PrintCodeVersion(void)
{
    // Print version.
    printf("%s %u.%u (%s)\n", name.c_str(), majorversion, minorversion, platform.c_str());
}

void CodeInfo::PrintPrologue(void)
{
    printf("Author: Dr. Jun Zhang, ZhangJunQcc@gmail.com\n");
}
