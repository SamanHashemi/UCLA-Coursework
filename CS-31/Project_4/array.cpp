//
//  main.cpp
//  Project 4
//
//  Created by Saman Hashemipour on 11/2/17.
//  Copyright © 2017 Saman Hashemipour. All rights reserved.
//

#include <iostream>
#include <string>
#include <cassert>

using namespace std;

int appendToAll(string a[], int n, string value);
int lookup(const string a[], int n, string target);
int rotateLeft(string a[], int n, int pos);
int positionOfMax(const string a[], int n);
int countRuns(const string a[], int n);
int flip(string a[], int n);
int split(string a[], int n, string splitter);
int differ(const string a1[], int n1, const string a2[], int n2);
int lookupAny(const string a1[], int n1, const string a2[], int n2);
int subsequence(const string a1[], int n1, const string a2[], int n2);


int main()
{
    string h[7] = { "selina", "reed", "diana", "tony", "", "logan", "peter"};
    assert(lookup(h, 7, "logan") == 5);
    assert(lookup(h, 7, "diana") == 2);
    assert(lookup(h, 2, "diana") == -1);
    assert(positionOfMax(h, 7) == 3);
    
    string g[4] = { "selina", "reed", "peter", "sue" };
    assert(differ(h, 4, g, 4) == 2);
    assert(appendToAll(g, 4, "?") == 4 && g[0] == "selina?" && g[3] == "sue?");
    assert(rotateLeft(g, 4, 1) == 1 && g[1] == "peter?" && g[3] == "reed?");
    
    string e[4] = { "diana", "tony", "", "logan" };
    assert(subsequence(h, 7, e, 4) == 2);
    
    string d[8] = { "reed", "reed", "reed", "tony", "tony", "sam", "tony", "tony" };
    assert(countRuns(d, 8) == 4);
    
    string f[3] = { "peter", "diana", "steve" };
    assert(lookupAny(h, 7, f, 3) == 2);
    assert(flip(f, 3) == 3 && f[0] == "steve" && f[2] == "peter");
    
    string test[3] = { "lol", "lol", "lol"};
    assert(split(test, 3, "lol") == 3);
    
    cout << "All tests succeeded" << endl;
}

int appendToAll(string a[], int n, string value){
    for(int i = 0; i < n; i++){
        a[i] += value;
    }
    return n;
}

int lookup(const string a[], int n, string target){
    for(int i = 0; i < n; i++){
        if(target == a[i]){
            return i;
        }
    }
    return -1;
}

int positionOfMax(const string a[], int n){
    int curMaxPos = -1;
    string curMaxWord = "";
    for(int i = 0; i < n; i++){
        if(i == 0){
            curMaxPos = 0;
            curMaxWord = a[0];
        }
        else if(a[i] >= curMaxWord){
            if(a[i] == curMaxWord){
                curMaxWord = a[i];
            }
            else{
                curMaxWord = a[i];
                curMaxPos = i;
            }
        }
    }
    return curMaxPos;
}

int rotateLeft(string a[], int n, int pos){
    string lastWord = a[pos];
    for(int i = pos; i < n-1; i++){
        a[i] = a[i+1];
    }
    a[n-1] = lastWord;
    return pos;
}

int countRuns(const string a[], int n){
    int i = 0;
    int numRuns = 0;
    string curWord = "";
    while(i < n){
        if(i == 0){
            curWord = a[i];
            numRuns++;
            i++;
        }
        else if(curWord == a[i]){
            i++;
        }
        else{
            curWord = a[i];
            numRuns++;
            i++;
        }
    }
    return numRuns;
}

int flip(string a[], int n){
    int j = 0; // Counter for the varaible at the beginning
    string temp = ""; // Temp value for the string at a[i]
    for(int i = n-1; i >= n/2; i--){ // t************************TEST************ >=n/2 VS. >n/2
        temp = a[i]; // Save a[i]
        a[i] = a[j]; // Assign a[i] to the mirror location
        a[j] = temp; // Assign the saved location to a[j]
        j++; // Increase the mirror location by 1
    }
    return n;
}

int differ(const string a1[], int n1, const string a2[], int n2){
    int i = 0;
    while(i < n1 && i < n2){
        if(a1[i] == a2[i]){
            i++;
        }
        else{
            break;
        }
    }
    return i;
}

int subsequence(const string a1[], int n1, const string a2[], int n2){
    int i = 0;
    int foundLoc = 0;
    bool found = false;
    while(i < n1){
        if(a1[i] == a2[0]){
            found = true;
            foundLoc = i;
            for(int j = 0; j < n2; j++){
                if(a1[i+j] != a2[j])
                    found = false;
            }
        }
        if(found) return foundLoc;
        i++;
    }
    return -1;
}


int lookupAny(const string a1[], int n1, const string a2[], int n2){
    for(int i = 0; i < n1; i++){
        for(int j = 0; j < n2; j++){
            if(a1[i] == a2[j])
                return i;
        }
    }
    return -1;
}

int split(string a[], int n, string splitter){
    int i = n-1;
    int j = 0;
    string temp = "";
    bool foundLess = false;
    while(i >= 0){
        while(a[i] < splitter && j < i){
            foundLess = true;
            while(a[j] < splitter && j < i) j++;
            temp = a[i];
            a[i] = a[j];
            a[j] = temp;
            j++;
        }
        if(a[j] < splitter && j < n) j++;
        i--;
    }
    if(foundLess) return j;
    else return n;
}







