#pragma once

#ifdef _WIN32
#include<Windows.h>
#endif

namespace SBUtil
{
    class RealTimeQuery
    {

    public:

        RealTimeQuery();
    };

    static double inverseFrequency;

    inline bool isFrequencyInitialized()
    {
        return inverseFrequency != 0.0;
    }

    inline double GetInverseFrequencyDouble()
    {
        return inverseFrequency;
    }

    double GetAbsoluteTimeSeconds();

    void initializeFrequency();
    
}