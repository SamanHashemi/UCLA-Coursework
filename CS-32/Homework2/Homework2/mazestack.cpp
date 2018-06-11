//#include <stack>
//#include <string>
//#include <iostream>
//using namespace std;
//
//bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec);
//
//class Coord
//{
//public:
//    Coord(int rr, int cc) : m_r(rr), m_c(cc) {}
//    int r() const { return m_r; }
//    int c() const { return m_c; }
//private:
//    int m_r;
//    int m_c;
//};
//
//bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec){
//    stack<Coord> cStack;
//    cStack.push(Coord(sr,sc));
//    maze[sr][sc] = '*';
//    int count = 0;
//    while(!cStack.empty()){
//        Coord curCoords = cStack.top();
//        int curRow = curCoords.r();
//        int curCol = curCoords.c();
//        cStack.pop();
//        if(curRow == er && curCol == ec){
//            return true;
//        }
//        // Location to the ____ is valid
//        if(curCol < nCols-1 && maze[curRow][curCol+1] == '.'){ // East
//            cStack.push(Coord(curRow, curCol+1));
//            maze[curRow][curCol+1] = '*';
//        }
//        if(curRow < nRows-1 && maze[curRow+1][curCol] == '.'){ // South
//            cStack.push(Coord(curRow+1, curCol));
//            maze[curRow+1][curCol] = '*';
//        }
//        if(curCol > 0 && maze[curRow][curCol-1] == '.'){ // Weast
//            cStack.push(Coord(curRow, curCol-1));
//            maze[curRow][curCol-1] = '*';
//        }
//        if(curRow > 0 && maze[curRow-1][curCol] == '.'){ // Norf norf
//            cStack.push(Coord(curRow-1, curCol));
//            maze[curRow-1][curCol] = '*';
//        }
//    }
//    return false;
//}
//
//int main(){
//    string maze[10] = {
//        "XXXXXXXXXX",
//        "X...X..X.X",
//        "X..XX....X",
//        "X.X.XXXX.X",
//        "XXX......X",
//        "X...X.XX.X",
//        "X.X.X..X.X",
//        "X.XXXX.X.X",
//        "X..X...X.X",
//        "XXXXXXXXXX"
//    };
//    if (pathExists(maze, 10,10, 4,3, 1,8)){
//        cout << "Solvable!" << endl;
//    }
//    else{
//        cout << "Out of luck!" << endl;
//    }
//    return 0;
//}
//
//
//
