#pragma once

namespace SBGame
{
    class PathGenerator
    {

    public:

        static void GenerateRandomPath();

        static bool CheckLeft();
        static bool CheckRight();
        static bool CheckUp();
        static bool CheckDown();

    private:



    };
}