#ifndef    __RUNSTATUS_H__
#define    __RUNSTATUS_H__

#include <string>
#include <cstdarg>
#include <ctime>

namespace hmo {

using namespace std;

class RunStatus {
public:
    RunStatus(void);
    ~RunStatus(void);

    // Runtime status control.
    static void Start(int argc, char** argv);
    static unsigned int NormalTermination(void);
    static unsigned int ErrorTermination(const char* fmt, ...);
    
    static const unsigned int Succeed;
    static const unsigned int ErrorExit;

private:
    static time_t starttime;
    static time_t terminationtime;

    static string GetTime(time_t& lt);
    static void ProcessSignal(void);
};

void BlockTermSignal(int signal);

}

#endif //  __RUNSTATUS_H__
