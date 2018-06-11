#include "Map.h"
#include <string>
#include <iostream>
#include <cassert>
using namespace std;

void test()
{
    Map test;
    Map other_test;
    Map result;
    ValueType v;
    // Add values to the maps
    assert(test.insert("Something", 1.1));
    assert(test.insert("Wow", 1.3));
    assert(test.insert("Grape", 1.5));
    assert(test.insert("Nothing", 1.3));
    assert(other_test.insert("Nothing", 1.3));
    assert(other_test.insert("new", 1.7));
    assert(other_test.insert("game", 1.19));
    assert(other_test.insert("time", 1.15));
    
    // Check if contains
    assert(other_test.contains("Nothing"));
    assert(!other_test.contains("SOmething"));
    
    //Check size
    assert(other_test.size() == 4);
    
    //Check erase
    assert(other_test.erase("Nothing"));
    assert(!other_test.erase("SHuldn't work"));
    
    //Check insert and update
    assert(other_test.insertOrUpdate("new", 1.9));
    assert(other_test.get("new", v) && v == 1.9); // check to see if value changed
    
    //Add new value
    assert(other_test.insertOrUpdate("unnew", 1.9));
    assert(other_test.get("unnew", v) && v == 1.9); // check to see if value added
    
    //Test combine
    assert(combine(other_test, test, result));
    
    assert(result.size() == 8); // should have 7 values
    
    //Test subtract
    subtract(other_test, test, result);
    assert(result.size() == 4);
    
    Map copy = Map(other_test); // test copy constructor
    
    assert(copy.get("unnew", v) && v == 1.9); // should have fred w this value
    assert(copy.get("new", v) && v == 1.9); // should have greg w this value
    
    other_test = test;
    
    assert(other_test.contains("Something")); // should now have the prev value of result
    
//    Map m;
//    Map n;
//    Map result;
//    ValueType v = 0;
//
//    assert(n.insert("Fred", 2.956)); // add a value to n
//    assert(n.insert("Ethel", 3.538)); // add a value to n
//    assert(n.insert("Grass", 2.956)); // add a value to n
//    assert(m.insert("Fred", 2.9523)); // add a value to m
//    assert(m.insert("Ethel", 3.53)); // add a value to m
//    //The previous statements were for the future code to test
//
//    assert((m.size() == 2));
//    assert(m.erase("Fred")); // erase a value in the list
//    assert(!m.erase("Dead")); // erase with value not in the list
//    assert((m.size() == 1)); // removed value with erased size should change
//    assert(!combine(m, n, result)); // similar value in n and m should not work but result should
//                                    //produce a result
//    subtract(n, m, result); // to check result
//    assert(result.contains("Grass")); // should contain grass
//    assert(result.contains("Fred")); // should also contain fred
//    assert((result.size() == 2)); // but nothing other than grass and fred
//
//    assert(!m.update("Fred", 2.2)); // Fred was erased should not update
//    assert(result.update("Fred", 2.2)); // exists in result should update
//    assert(result.get("Fred", v) && v == 2.2); // should have changed fred
//
//
//    assert(result.insertOrUpdate("Fred", 2.3)); // should change fred
//    assert(result.get("Fred", v) && v == 2.3); // should have changed fred
//
//    assert(result.insertOrUpdate("Greg", 2.72)); // SHould add a greg key with 2.72 for val
//    assert(result.contains("Greg")); // make sure greg exists in the thing
//    assert(result.get("Greg", v) && v == 2.72); // should have changed fred
//    assert(result.update("Greg", 2.75)); // SHould add a greg key with 2.72 for val
//    assert(result.get("Greg", v) && v == 2.75); // should have changed fred
//
//    m.swap(result); // should
//    assert(m.get("Fred", v) && v == 2.3); // should have fred w this value
//    assert(m.get("Greg", v) && v == 2.75); // should have greg w this value
//
//    assert((result.size() == 1)); // should have size of the new m
//    assert(result.contains("Ethel"));
//
//
//
//    Map copy = Map(m); // test copy constructor
//
//    assert(copy.get("Fred", v) && v == 2.3); // should have fred w this value
//    assert(copy.get("Greg", v) && v == 2.75); // should have greg w this value
//
//    m = result;
//
//    assert(m.contains("Ethel")); // should now have the prev value of result
}

int main()
{
    test();
    cout << "Passed all tests" << endl;
}



