#include "RealTimeQuery.h"

namespace SBUtil
{
    RealTimeQuery::RealTimeQuery()
    {
        initializeFrequency();
    }

    void initializeFrequency()
    {
        #ifdef _WIN32
            LARGE_INTEGER frequency;
            QueryPerformanceFrequency( &frequency );
            inverseFrequency = 1.0 / (double)frequency.QuadPart;
        #endif
    }

    double GetAbsoluteTimeSeconds()
    {
        static RealTimeQuery rtq;

        double seconds = 0;

        #ifdef _WIN32
            LARGE_INTEGER counter;
            QueryPerformanceCounter( &counter );
            seconds = (double)counter.QuadPart * inverseFrequency;
        #endif

        return seconds;
    }
}