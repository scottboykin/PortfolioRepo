#include "PathGenerator.h"

namespace SBGame
{
    //static string[,] PathGenerator::GenerateRandomPath(int gridsize, int length, int numOfTurns)
    //{
    //    string[,] path;
    //    List<string> invalid = new List<string>();
    //    bool done = false;
    //    int startRow, startColumn;

    //    while (!done)
    //    {
    //        path = new string[gridsize, gridsize];

    //        do{
    //            startRow = r.Next(gridsize);
    //            startColumn = r.Next(gridsize);
    //        }while(invalid.Contains(""+startRow+","+startColumn));

    //        path[startRow, startColumn] = "S";
    //        if ((path = GeneratePath((string[,])path.Clone(), startRow, startColumn, length - 1, numOfTurns, gridsize, "start")) != null)
    //            return path;
    //        else
    //            invalid.Add("" + startRow + "," + startColumn);
    //        if (invalid.Count == (gridsize * gridsize))
    //            done = true;
    //    }
    //    return null;
    //}

    //static string[,] PathGenerator::GeneratePath(string[,] path, int row, int column, int length, int numOfTurns, int gridSize, string direction)
    //{
    //    bool done = false, triedTurn = false, triedStraight = false;
    //    int move;
    //    string[,] solution;
    //    List<int> invalid = new List<int>();

    //    if (numOfTurns == 0)
    //        triedTurn = true;

    //    if (length == 0)
    //    {
    //        if (numOfTurns != 0)
    //            return null;
    //        else
    //        {
    //            path[row, column] = "F";
    //            return path;
    //        }
    //    }

    //    if (direction == "start")
    //    {
    //        while (!done)
    //        {
    //            do
    //            {
    //                move = r.Next(4);
    //            } while (invalid.Contains(move));
    //            invalid.Add(move);

    //            switch (move)
    //            {
    //            case 0:
    //                if (CheckUp(path, row, column, gridSize))
    //                    if ((solution = GeneratePath((string[,])path.Clone(), row - 1, column, length - 1, numOfTurns, gridSize, "up")) != null)
    //                        return solution;
    //                break;
    //            case 1:
    //                if (CheckDown(path, row, column, gridSize))
    //                    if ((solution = GeneratePath((string[,])path.Clone(), row + 1, column, length - 1, numOfTurns, gridSize, "down")) != null)
    //                        return solution;
    //                break;
    //            case 2:
    //                if (CheckLeft(path, row, column, gridSize))
    //                    if ((solution = GeneratePath((string[,])path.Clone(), row, column - 1, length - 1, numOfTurns, gridSize, "left")) != null)
    //                        return solution;
    //                break;
    //            case 3:
    //                if (CheckRight(path, row, column, gridSize))
    //                    if ((solution = GeneratePath((string[,])path.Clone(), row, column + 1, length - 1, numOfTurns, gridSize, "right")) != null)
    //                        return solution;
    //                break;
    //            }
    //            if (invalid.Count == 4)
    //                done = true;
    //        }
    //    }

    //    else
    //    {
    //        while (!triedTurn || !triedStraight)
    //        {

    //            if ((!triedTurn && r.NextDouble() < (numOfTurns / length))||(triedStraight&&numOfTurns!=0)) //Turn
    //            {
    //                if (direction == "up" || direction == "down")
    //                {
    //                    done = false;
    //                    while (!done)
    //                    {
    //                        do
    //                        {
    //                            move = r.Next(2);
    //                        } while (invalid.Contains(move));
    //                        invalid.Add(move);

    //                        switch (move)
    //                        {
    //                        case 0:
    //                            if (CheckLeft(path, row, column, gridSize))
    //                            {
    //                                path[row, column] = "L";
    //                                if ((solution = GeneratePath((string[,])path.Clone(), row, column - 1, length - 1, numOfTurns - 1, gridSize, "left")) != null)
    //                                    return solution;
    //                            }
    //                            break;
    //                        case 1:
    //                            if (CheckRight(path, row, column, gridSize))
    //                            {
    //                                path[row, column] = "R";
    //                                if ((solution = GeneratePath((string[,])path.Clone(), row, column + 1, length - 1, numOfTurns - 1, gridSize, "right")) != null)
    //                                    return solution;
    //                            }
    //                            break;
    //                        }
    //                        if (invalid.Count == 2)
    //                        {
    //                            invalid.Clear();
    //                            triedTurn = true;
    //                            done = true;
    //                        }
    //                    }
    //                }

    //                else
    //                {
    //                    done = false;
    //                    while (!done)
    //                    {
    //                        do
    //                        {
    //                            move = r.Next(2);
    //                        } while (invalid.Contains(move));
    //                        invalid.Add(move);

    //                        switch (move)
    //                        {
    //                        case 0:
    //                            if (CheckUp(path, row, column, gridSize))
    //                            {
    //                                path[row, column] = "U";
    //                                if ((solution = GeneratePath((string[,])path.Clone(), row - 1, column, length - 1, numOfTurns - 1, gridSize, "up")) != null)
    //                                    return solution;
    //                            }
    //                            break;
    //                        case 1:
    //                            if (CheckDown(path, row, column, gridSize))
    //                            {
    //                                path[row, column] = "D";
    //                                if ((solution = GeneratePath((string[,])path.Clone(), row + 1, column, length - 1, numOfTurns - 1, gridSize, "down")) != null)
    //                                    return solution;
    //                            }
    //                            break;
    //                        }
    //                        if (invalid.Count == 2)
    //                        {
    //                            invalid.Clear();
    //                            triedTurn = true;
    //                            done = true;
    //                        }
    //                    }
    //                }
    //            }

    //            if(!triedStraight)
    //            {
    //                switch(direction)
    //                {
    //                case "up":
    //                    if (CheckUp(path, row, column, gridSize))
    //                    {
    //                        path[row, column] = "U";
    //                        if ((solution = GeneratePath((string[,])path.Clone(), row - 1, column, length - 1, numOfTurns, gridSize, "up")) != null)
    //                            return solution;
    //                    }
    //                    break;
    //                case "down":
    //                    if (CheckDown(path, row, column, gridSize))
    //                    {
    //                        path[row, column] = "D";
    //                        if ((solution = GeneratePath((string[,])path.Clone(), row + 1, column, length - 1, numOfTurns, gridSize, "down")) != null)
    //                            return solution;
    //                    }
    //                    break;
    //                case "left":
    //                    if (CheckLeft(path, row, column, gridSize))
    //                    {
    //                        path[row, column] = "L";
    //                        if ((solution = GeneratePath((string[,])path.Clone(), row, column - 1, length - 1, numOfTurns, gridSize, "left")) != null)
    //                            return solution;
    //                    }
    //                    break;
    //                case "right":
    //                    if (CheckRight(path, row, column, gridSize))
    //                    {
    //                        path[row, column] = "R";
    //                        if ((solution = GeneratePath((string[,])path.Clone(), row, column + 1, length - 1, numOfTurns, gridSize, "right")) != null)
    //                            return solution;
    //                    }
    //                    break;
    //                }
    //                triedStraight = true;
    //            }
    //        }

    //    }

    //    return null;
    //}

    //static bool PathGenerator::CheckUp(string[,] path, int row, int column, int gridSize)
    //{
    //    if (row - 1 < 0)
    //        return false;
    //    else if (path[row - 1, column] != null)
    //        return false;
    //    else
    //        return true;

    //}
    //static bool PathGenerator::CheckDown(string[,] path, int row, int column, int gridSize)
    //{
    //    if (row + 1 == gridSize)
    //        return false;
    //    else if (path[row + 1, column] != null)
    //        return false;
    //    else
    //        return true;
    //}
    //static bool PathGenerator::CheckLeft(string[,] path, int row, int column, int gridSize)
    //{
    //    if (column - 1 < 0)
    //        return false;
    //    else if (path[row, column - 1] != null)
    //        return false;
    //    else
    //        return true;
    //}
    //static bool PathGenerator::CheckRight(string[,] path, int row, int column, int gridSize)
    //{
    //    if (column + 1 == gridSize)
    //        return false;
    //    else if (path[row, column + 1] != null)
    //        return false;
    //    else
    //        return true;
    //}
}