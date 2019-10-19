//
//  main.cpp
//  Project 3
//
//  Created by Saman Hashemipour on 10/22/17.
//  Copyright © 2017 Saman Hashemipour. All rights reserved.
//

#include <iostream>
#include <cctype>
#include <string>

using namespace std;

char encodeNote(int, char, char);
bool hasCorrectSyntax(string);
int encodeSong(string, string&, int& badBeat);
int hasBadBeat(string);
string bracketPadding(string);

int main(int argc, const char * argv[])
{
    string instructions;
    string song;
    int badBeat;
    cout << "Please enter a song: ";
    getline(cin, song);
    int error = encodeSong(song, instructions, badBeat);
    if(error== 0)
        cout << "The instructions are \"" << instructions << "\"" << endl;
    else if(error== 1)
        cout << "There was improper syntax" << endl;
    else if(error== 2)
        cout << "There was a bad beat at beat #" << badBeat << endl;
    
}

int hasBadBeat(string song){
    int beatNumber = 1; // Count the number of beats in the song
    int i = 0; // Current Char Position
    while(i < song.length()){ // Go throug the song one char at a time
        if(song[i] == '/'){
            beatNumber++; // Another beat was encountered
            i++; // Go to next char
        }
         // Check for weird cases 'Cb6' and 'B#1'
        else if((i+3 < song.length()) && ((song[i] == 'C' && song[i+1] == 'b' && song[i+2] == '6') ||
           (song[i] == 'B' && song[i+1] == '#' && song[i+2] == '1'))){
            i+=3;
        }
         // Check for weird case of 'C6'
        else if(i+2 < song.length() && (song[i] == 'C' && song[i+1] == '6')){
            i+=2;
        }
        // 'Cb2' not acceptable, end function reutrn current beat
        else if(i+3 < song.length() && (song[i] == 'C' && song[i+1] == 'b' && song[i+2] == '2')){
            return beatNumber;
        }
        // Otherwise the case requirements for the notes
        else if(song[i] == 'A' || song[i] == 'B' || song[i] == 'C' || song[i] == 'D'
           || song[i] == 'E'|| song[i] == 'F' || song[i] == 'G'){
            i++;
            if(song[i] == 'b' || song[i] == '#'){
                i++;
                // None of these values are acceptable must have octave between 2 and 5
                if(song[i] == '0' || song[i] == '1' || song[i] == '6' || song[i] == '7' || song[i] == '8' || song[i] == '9'){
                    return beatNumber;
                }
                else if(song[i] == '2' || song[i] == '3' || song[i] == '4' || song[i] == '5'){
                    i++;
                }
            }
        }
        // No accidental but the octave was out of range
        else if(song[i] == '0' || song[i] == '1' || song[i] == '6' || song[i] == '7' || song[i] == '8' || song[i] == '9'){
            return beatNumber;
        }
        // Octave in range
        else if(song[i] == '2' || song[i] == '3' || song[i] == '4' || song[i] == '5'){
            i++;
        }
        // If anything else was encountered return the beat number
        else{
            return beatNumber;
        }
    }
    return 0;
}

bool hasCorrectSyntax(string song)
{
    bool isBlank = true; // Assume blank string
    for(int i = 0; i < song.length(); i++){ // Check if the string is blank
        if(song[i] != ' '){ // If non-whitespace is encountered
            isBlank = false;
        }
    }
    if(song == ""){ // Is the song string empty
        return true;
    }
    else if(isBlank == true){ // If the song is blank (has only whitespaces)
        return true;
    }
    else if(song[song.length()-1] != '/'){ // If the song doesn't end in '/'
        return false;
    }
    // If the first char is anything other than a song letter or a break
    else if(song[0] != 'A' && song[0] != 'B' && song[0] != 'C' && song[0] != 'D' &&
       song[0] != 'E' && song[0] != 'F' && song[0] != 'G' && song[0] != '/'){
        return false;
    }
    int i = 0;
    while(i < song.length()){
        // Check for the correct letter on the keyboard
        if(song[i] == 'A' || song[i] == 'B' || song[i] == 'C' || song[i] == 'D'
           || song[i] == 'E'|| song[i] == 'F' || song[i] == 'G'){
            i++;
            // Check if it's as sharp or flat
            if(song[i] == 'b' || song[i] == '#'){
                i++;
                if(isdigit(song[i])){ // If there is a digit that follows 0-9 keep going
                    i++;
                }
            }
            // If no accidental but there is a digit only increase count by 1
            else if(isdigit(song[i])){
                i++;
            }
        }
        // '/' is accpetable
        else if(song[i] == '/'){
            i++;
        }
        // Anything else encoutered return false
        else{
            return false;
        }
    }
    return true;
    
}

string bracketPadding(string chord){
    string temp = "[";
    temp += chord;
    return temp += "]";
}

int encodeSong(string song, string& instructions, int& badBeat)
{
    if(!hasCorrectSyntax(song)){
        return 1;
    }
    else if(hasBadBeat(song)){
        badBeat = hasBadBeat(song);
        return 2;
    }
    else{
        instructions = "";
        int i = 0;
        int notesInBeat = 0;
        string tempNoteString = "";
        string finalNoteString = "";
        int octave = 4;
        char noteLetter = ' ';
        char accidentalSign = ' ';
        while(i < song.size()){
            if(isalpha(song[i])){ // If the current char is a letter or a number
                notesInBeat++;
                noteLetter = song[i];
                i++;
                if(song[i] == 'b' || song[i] == '#'){ // Is accidental
                    accidentalSign = song[i]; // The accidental sign is assigned to the cur val
                    i++;
                    if(isdigit(song[i])){ // Is a digit
                        octave = (int)(song[i]-'0'); // The octave is assigned to the numerical repesentation of it's char
                        i++;
                    }
                }
                else if(isdigit(song[i])){ // Is digit with no accidental
                    octave = (int)(song[i]-'0'); // The octave is assigned to the numerical repesentation of it's char
                    i++;
                }
                tempNoteString += encodeNote(octave, noteLetter, accidentalSign);
                
                // Reset all note features
                octave = 4;
                noteLetter = ' ';
                accidentalSign = ' ';
            }
            else if(song[i] == '/'){ // End of a beat is encountered
                if(notesInBeat > 1){
                    instructions += bracketPadding(tempNoteString); // Pad notes in brackets if there was more than 1 note
                }
                // If two '/' were encountered back to back add a space
                else if((i < song.size() && i != 0 && i-1 >= 0) && song[i] == '/' && song[i-1] == '/'){
                        instructions += " ";
                    }
                else{
                    instructions += tempNoteString;
                }
                tempNoteString = ""; // Reset the note string to prevent duplication
                notesInBeat = 0; // New beat new number of notes
                i++;
            }
        }
        return 0;
    }
}

//
char encodeNote(int octave, char noteLetter, char accidentalSign)
{
    // This check is here solely to report a common CS 31 student error.
    if (octave > 9)
    {
        cerr << "********** encodeNote was called with first argument = "<< octave << endl;
    }
    
    // Convert Cb, C, C#/Db, D, D#/Eb, ..., B, B#
    //      to -1, 0,   1,   2,   3, ...,  11, 12
    
    int note;
    switch (noteLetter)
    {
        case 'C':  note =  0; break;
        case 'D':  note =  2; break;
        case 'E':  note =  4; break;
        case 'F':  note =  5; break;
        case 'G':  note =  7; break;
        case 'A':  note =  9; break;
        case 'B':  note = 11; break;
        default:   return ' ';
    }
    switch (accidentalSign)
    {
        case '#':  note++; break;
        case 'b':  note--; break;
        case ' ':  break;
        default:   return ' ';
    }
    
    // Convert ..., A#1, B1, C2, C#2, D2, ... to
    //         ..., -2,  -1, 0,   1,  2, ...
    
    int sequenceNumber = 12 * (octave - 2) + note;
    
    string keymap = "Z1X2CV3B4N5M,6.7/A8S9D0FG!H@JK#L$Q%WE^R&TY*U(I)OP";
    if (sequenceNumber < 0  ||  sequenceNumber >= keymap.size())
        return ' ';
    return keymap[sequenceNumber];
}
