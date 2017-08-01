#include "RunStatus.h"
#include "CodeInfo.h"
#include <string>
#include <csignal>
#include <cstdarg>
#include <ctime>
#include <cstdlib>
#include <cstdio>

using namespace hmo;
using namespace std;

const unsigned int RunStatus::Succeed = 0x00000000;
const unsigned int RunStatus::ErrorExit = 0x00000001;

time_t RunStatus::terminationtime;

RunStatus::RunStatus(void) 
{ 
    /* Nothing to do */ 
}

RunStatus::~RunStatus(void)
{
    /* Nothing to do */ 
}

void RunStatus::Start(int argc, char** argv)
{
    setvbuf(stdout, NULL, _IONBF, 0); // No buffer!
    ProcessSignal(); // Blocking signals
    // Begin!
    CodeInfo::PrintCodeInfo();
    printf("Command line: \" ");
    for(int i = 0; i < argc; ++i)
    {
        printf("%s ", argv[i]);
    }
    printf("\"\n");
    printf("\n");
}

unsigned int RunStatus::NormalTermination(void)
{
    printf("\n");
    printf("===== (^ _ ^) =====\n");
    printf("Normal termination at %s.\n", GetTime(terminationtime).c_str());
    
    return Succeed;
}

unsigned int RunStatus::ErrorTermination(const char* fmt, ...)
{
    va_list args;
    char buffer[2048];
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    printf("\n");
    printf("===== (@ o @) =====\n");
    printf("Error occurs: %s\n", buffer);
    printf("Error termination at %s.\n", GetTime(terminationtime).c_str());
    
    exit(ErrorExit);
    return ErrorExit;
}

string RunStatus::GetTime(time_t& lt)
{
    time(&lt);
    string timestr(asctime(localtime(&lt)));
    return timestr.substr(0, timestr.find('\n'));
}

void RunStatus::ProcessSignal(void)
{
    signal(SIGINT, hmo::BlockTermSignal);
    signal(SIGILL, hmo::BlockTermSignal);
    signal(SIGFPE, hmo::BlockTermSignal);
    signal(SIGSEGV, hmo::BlockTermSignal);
    signal(SIGTERM, hmo::BlockTermSignal);
    signal(SIGABRT, hmo::BlockTermSignal);
}

void hmo::BlockTermSignal(int signal)
{
    string siginfo;
    switch (signal)
    {
	case SIGINT:
	{
	    siginfo.assign("SIGINT: Interrupt");
	    break;
	}
	case SIGILL:
	{
	    siginfo.assign("SIGILL: Illegal instruction");
	    break;
	}
	case SIGFPE:
	{
	    siginfo.assign("SIGFPE: Floating point exception");
	    break;
	}
	case SIGSEGV:
	{
	    siginfo.assign("SIGSEGV: Segment violation");
	    break;
	}
	case SIGTERM:
	{
	    siginfo.assign("SIGTERM: Software termination from kill");
	    break;
	}
	case SIGABRT:
	{
	    siginfo.assign("SIGABRT: Termination by abort call");
	    break;
	}
	default:
	{
	    siginfo.assign("Unknown signal");
	    break;
	}
    }
    RunStatus::ErrorTermination("Recieve signal: %s (Value: %d).", siginfo.c_str(), signal);
}

