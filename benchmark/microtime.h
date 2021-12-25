

//Windows Version of GetMicro
#ifdef _WIN32 || _WIN64
#include <windows.h>
typedef unsigned __int64 microsec;

bool firstUse = true;
//time of first use in ticks
unsigned __int64 startTick;

microsec GetMicro()
{
    LARGE_INTEGER Counter;
    LARGE_INTEGER Frequency;
    QueryPerformanceCounter(&Counter);
    QueryPerformanceFrequency(&Frequency);
    if(firstUse)
    {
        firstUse = false;
        startTick = Counter.QuadPart;
    }
    unsigned __int64 frequ = Frequency.QuadPart;
    unsigned __int64 ticks = Counter.QuadPart - startTick;
    //microFrequ ist in der einheit Tick / sec
    //durch Multiplikation von ticks mit 1000000 ist die einheit am ende
    //in microsec ohne so gro�e rundungsfehler wie bei frequ/1000000
    unsigned __int64 _Time = ticks*1000000 / frequ ;
    return _Time;
}
#endif

#ifdef __APPLE__
#include <sys/types.h>
#include <sys/time.h>
typedef u_int64_t microsec;

bool FirstUse = true;
//time of first use in ticks
timeval StartTv;

double getDifference(timeval start, timeval end)
{
    double dstart = 0.0f, dend = 0.0f;
    dstart = (double)(start.tv_sec);
    dstart += (double)(start.tv_usec) / 1000000;
    dend = (double)(end.tv_sec);
    dend += (double)(end.tv_usec) / 1000000;
    return(dend-dstart);
}

microsec GetMicro()
{
    if(FirstUse)
    {
        FirstUse = false;
        gettimeofday(&StartTv,0);
    }
    timeval NowTv;
    gettimeofday(&NowTv,0);

    double dif = getDifference(StartTv,NowTv);
    //seconds to microseconds
    return (microsec)(dif * 1000000);
}
#endif
//Linux Version of GetMicro