//If the start location is equal to the ending location, then we've
//solved the maze, so return true.
//Mark the start location as visted.
//For each of the four directions,
//If the location one step in that direction (from the start
//                                            location) is unvisited,
//then call pathExists starting from that location (and
//                                                  ending at the same ending location as in the
//                                                  current call).
//If that returned true,
//then return true.
//Return false.

#include <string>
#include <iostream>
using namespace std;

bool pathExists(string maze[], int nRow, int nCol, int sr, int sc, int er, int ec){
    maze[sr][sc] = '*';
    if(sr==er && sc==ec)
        return true;
    if(sc != nCol-1 && maze[sr][sc+1] == '.'){ // check to the right
        if (pathExists(maze, nRow, nCol, sr, sc+1, er, ec) == true)
            return true;
    }
    if(sc != 0 && maze[sr][sc-1] == '.'){ // check the left
        if(pathExists(maze, nRow, nCol, sr, sc-1, er, ec) == true)
            return true;
    }
    if(sr != nRow+1 && maze[sr+1][sc] == '.'){ // check to the down
        if (pathExists(maze, nRow, nCol, sr+1, sc, er, ec) == true)
            return true;
    }
    if(sr != 0 && maze[sr-1][sc] == '.'){ // check the up
        if(pathExists(maze, nRow, nCol, sr-1, sc, er, ec) == true)
            return true;
    }
    return false;
}


