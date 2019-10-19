#include <iostream>
using namespace std;

int main()
{
    const char* str = "C+ B+ B- A C A- B- C- B+ D B";
    int count = 0;
    const char* p;
    for (p = str; *p != '\0'  &&  count < 6; p++)
        if (*p == ' ')
            count++;
    for ( ; *p != '\0'  &&  *p != ' '; p++)
        cout << *p;
    cout << '\n';
}

//#include <iostream>
//#include <string>
//using namespace std;
//
//int main () {
//    int array[1000];
//    array['1'] = 50;
//    cout << array[49] << endl;
//}
//#include <iostream>
//
//using namespace std;
//
//void changer(int &num);
//
//#include <iostream>
//using namespace std;
//
////This function takes as input a pointer to the first element
////of a c-string and a pointer to the last element of a c-string
////(including 0 byte) and outputs a new c-string that reverses
////the order of characters in the original string
//
//void changePointer(int* first, int* second)
//{
//    *first = 5;
//    cout << *first << endl;
//}
//
//
////This function takes as input a c-string containing a number and
////the number of characters in the array (not including 0 byte)
////It outputs an integer representation of that number.
//int convertStringToInt(char number[], int len)
//{
//    int theInt = 0;
//    int theTenMult = 1;
//    for(int i = len-1; i >= 0; i--){
//        theInt += (number[i] - '0')*theTenMult;
//        theTenMult *= 10;
//    }
//    return theInt;
//}
//
////Write very basic test code to test your functions
//int main()
//{
//    int array[1000];
//    array['c'] = 50;
//    cout << (int)'c' << endl;
//    cout << array['c'] << endl;
//}
//
//
//
//
////#include <iostream>
////#include <cstring>
////
////using namespace std;
////
//////const char* findTheChar(const char* str, char chr);
//////
//////const char* findTheChar(const char* str, char chr)
//////{
//////    for (int k = 0; *(str+k) != 0; k++)
//////        if (*(str+k) == chr)
//////            return (str+k);
//////
//////    return nullptr;
//////}
//////
//////int main(){
//////    const char str[5] = {'a', 'b', 'c', 'd', 'e'};
//////    char c = 'm';
//////    cout << findTheChar(str, c);
//////}
//////
//////
////
//////bool strequal(const char* str1[], const char* str2[]);
//////
//////bool strequal(const char str1[], const char str2[])
//////{
//////    while (*str1 != 0  ||  *str2 != 0)
//////    {
//////
//////        if (*str1 != *str2){  // compare corresponding characters
//////            return false;
//////        }
//////        str1++;            // advance to the next character
//////        str2++;
//////
//////    }
//////    return true;   // the ended the same and both reached 0 together
//////}
//////
//////
//////int main(){
//////    char a[15] = "Zou";
//////    char b[15] = "Zhou";
//////
//////    if(strequal(a,b)){
//////        cout << " same" << endl;
//////    }
//////}
//////
//////
////
////
////
////
////
////
////
////
////
//////
//////void removeS(char*);
//////
//////void removeS(char* msg){
//////    int i = 0;
//////    while(*(msg+i) != 0){
//////        if(*(msg+i) == 's' || *(msg+i) == 'S'){
//////            for(int j = i; *(msg+j) != 0; j++){
//////                *(msg+j) = *(msg+j+1);
//////            }
//////            i--;
//////        }
//////        i++;
//////    }
//////}
//////
//////
//////int main()
//////{
//////    char msg[50] = "sssssssssssssss.";
//////    removeS(msg);
//////    cout << msg << endl;  // prints   he'll be a male prince.
//////}
//////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
//////int* maxwell(int* a, int* b);
//////void swap1(int* a, int* b);
//////void swap2(int* a, int* b);
//////
//////int* maxwell(int* a, int* b) // return the pointer of the int with the larger value
//////{
//////    if (*a > *b) // if a is greater than b, return a ptr
//////        return a;
//////    else // if b is greater than a, return b ptr
//////        return b;
//////}
//////
//////void swap1(int* a, int* b) // swap the location of a and b (a = b's location and visa versa)
//////{
//////    int* temp = a;
//////    a = b;
//////    b = temp;
//////}
//////
//////void swap2(int* a, int* b) // swap the values of a and b, locatiosn stay the same but the values change
//////{
//////    int temp = *a;
//////    *a = *b;
//////    *b = temp;
//////}
////
//////int main()
//////{
//////    int array[6] = { 5, 3, 4, 17, 22, 19 };
//////    int* ptr = maxwell(array, &array[2]); // the ptr points at location 0 of the array
//////    *ptr = -1; // the value at pos 0 (el 1) is now -1
//////    ptr += 2; // make the pointer point to the location at pos 2 (element 3)
//////    ptr[1] = 9; // the pointer at location 3 (element 4) is now 9 instead of 17
//////    *(array+1) = 79; // the value at position 2 of the array (element 3 is now 79)
//////
//////    for (int i = 0; i < 6; i++)
//////        cout << array[i] << " "; // print all the values of the array
//////    cout << endl;
//////
//////    cout << &array[5] - ptr << endl; // print the location of position 5 - the position of ptr (pos 2)
//////
//////    swap1(&array[0], &array[1]); // swap pos[0] now points to pos[1] and visa versa
//////
//////    for (int i = 0; i < 6; i++)
//////        cout << array[i] << " "; // print all the values of the array
//////    cout << endl;
//////
//////    swap2(array, &array[2]); // swap pos[0] value (3) and pos[2] value (4)
//////
//////    for (int i = 0; i < 6; i++)
//////        cout << array[i] << endl; // print all the values of the array
//////}
//////
////
////
////
////
////
////
////
////
////
////
////void printArray(double* first, int n);
////
////
////int main(){
////    double* cat;
////    double mouse[5];
////    cat = mouse+4; // cat is now at location 4 (element 5)
////    *cat = 25; // the value at location 4 is now 25
////    printArray(mouse, 5);
////
////    *(mouse+3) = 42; // the value at location 3 is now 42
////    cat -= 3; // cat is now pointing to location 1 (element 2)
////    printArray(mouse, 5);
////
////    cat[1] = 54; // the value at location 2 (element 3) is now 54
////    printArray(mouse, 5);
////
////    cat[0] = 27; // the value at location 1 (element 2) is now
////    printArray(mouse, 5);
////
////    bool b = *cat == *(cat+1);
////
////    bool d = cat == mouse;
////
////    cout << "b is: " << b << endl;
////    cout << "d is: " << d << endl;
////
////
////    printArray(mouse, 5);
////}
////
////void printArray(double* first, int n){
////    for(int i = 0; i < n; i++){
////        cout << *(first+i) << " ";
////    }
////    cout << endl;
////}
////

