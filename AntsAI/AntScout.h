#pragma once

#include "Ant.h"

class AntScout : public Ant
{

public:

    AntScout( const AgentReport& report )
        :   Ant( report )
    {}

    virtual ~AntScout(){}

    virtual OrderCode update( const AgentReport& report );

private:



};