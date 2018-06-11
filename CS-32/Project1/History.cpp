#include "History.h"

History::History(int nRow, int nCol)
{
    m_rows = nRow;
    m_cols = nCol;
    
    // initilize the counter array
    for(int row = 0; row < MAXROWS; row++){
        for(int col = 0; col < MAXCOLS; col++){
            m_counter[row][col] = '.';
        }
    }
}

bool History::record(int r, int c)
{
    int row = r-1;
    int col = c-1;
    if(row < MAXROWS && col < MAXCOLS){
        if (m_counter[row][col] == '.'){
            m_counter[row][col] = 'A';
        }
        else if(m_counter[row][col] == 'Y' || m_counter[row][col] == 'Z'){
            m_counter[row][col] = 'Z';
        }
        else{
            m_counter[row][col]++;
        }
        return true;
    }
    else{
        return false;
    }
}

void History::display() const
{
    clearScreen();
    for(int r = 0; r < m_rows; r++){
        for(int c = 0; c < m_cols; c++){
            std::cout << (char)m_counter[r][c];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
