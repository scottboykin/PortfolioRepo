#pragma once

#include "Ant.h"

class AntSoldier : public Ant
{

public:

    AntSoldier( const AgentReport& report )
        :   Ant( report )
    {}

    virtual ~AntSoldier(){}

    virtual OrderCode update( const AgentReport& report );

private:



};