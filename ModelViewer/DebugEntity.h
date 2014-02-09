#pragma once

#include "Entity.h"
#include "DebugOutput.h"

namespace SBGraphics
{
    class DebugEntity : public DebugOutput
    {

    public:

        explicit DebugEntity( Entity& entity, float timeToDie = 1.0f );

        virtual void draw();

    private:

        Entity& m_entity;

    };
}