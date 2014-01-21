#pragma once

namespace SBGame
{
    class Map;

    class RTSGame
    {

    public:

        RTSGame();

        void updateSim();
        void updateDisplay();

    private:

        Map* m_map;

    };
}