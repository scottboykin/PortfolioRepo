#pragma once

#include "Ant.h"

class MapTile;

class AntWorker : public Ant
{

public:

    AntWorker( const AgentReport& report )
        :   Ant( report )
        ,   claimedResource( nullptr )
    {}

    virtual ~AntWorker(){}

    virtual void ProcessDeath();

    virtual OrderCode update( const AgentReport& report );

private:

    MapTile* claimedResource;

};