#include <iostream>

using namespace std;

int main()
{
    //Declare all variables
    int age;
    int zoneBounds;
    double totalFare;
    string destination;
    string student;
    
    //Get user input
    cout << "Age of rider: ";
    cin >> age;
    cin.ignore(99999, '\n');
    if(age < 0){
        cout << "---" << endl;
        cout << "The age must not be negative" << endl;
        return 1;
    }
    cout << "Student? (y/n): ";
    getline(cin, student);
    
    if(student != "y" && student != "n"){
        cout << "---" << endl;
        cout << "You must enter y or n" <<endl;
        return 1;
    }
    cout << "Destination: ";
    getline(cin, destination);
    if(destination == "" || destination == " "){
        cout << "---" << endl;
        cout << "You must enter a destination" << endl;
        return 1;
    }
    cout << "Number of zone boundaries crossed: ";
    cin >> zoneBounds;
    if(zoneBounds < 0){
        cout << "---" << endl;
        cout << "The number of zone boundaries crossed must not be negative" << endl;
        return 1;
    }
    cout << "---" << endl;
    
    //Calculate Fare
    if(age >= 65 && zoneBounds == 0){
        totalFare = 0.45;
    }
    else if((age < 18 || student == "y") && (zoneBounds == 0 || zoneBounds == 1)){
        totalFare = 0.65;
    }
    else if(age >= 65){
        totalFare = 0.25 * zoneBounds;
        totalFare += 0.55;
    }
    else{
        totalFare = 0.55 * zoneBounds;
        totalFare += 1.35;
    }
    
    cout.setf(ios::fixed);
    cout.precision(2);
    cout << "The fare to " << destination << " is $" << totalFare << endl;
    
}
