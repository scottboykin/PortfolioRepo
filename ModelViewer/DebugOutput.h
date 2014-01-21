#pragma once

namespace SBGraphics
{
    class DebugOutput
    {

    public:

        DebugOutput();
        virtual ~DebugOutput(){}

        virtual void update( float currentSimTime );
        virtual void draw();

        bool wantsToDie(){ return m_currentSimTime >= m_timeToDie && m_timeToDie != 0.0;  }

    protected:

        float m_currentSimTime;
        float m_timeToDie;

    };
}