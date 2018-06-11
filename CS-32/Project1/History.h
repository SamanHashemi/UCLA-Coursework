#ifndef History_h
#define History_h

#include <iostream>
#include <string>

#include "globals.h"

class History
{
public:
    History(int nRows, int nCols);
    bool record(int r, int c);
    void display() const;
private:
    int m_rows;
    int m_cols;
    char m_counter[MAXROWS][MAXCOLS];
};

#endif /* History_h */
