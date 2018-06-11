//
//  MyHash.hpp
//  Project4
//
//  Created by Saman Hashemipour on 3/11/18.
//  Copyright © 2018 Saman Hashemipour. All rights reserved.
//


#ifndef MyHash_hpp
#define MyHash_hpp

#include <stdio.h>
#include <string>
#include <functional>
#include <iostream>


template <class KeyType, class ValueType>
class MyHash
{
public:
    MyHash(double maxLoadFactor = 0.5);
    ~MyHash();
    void reset();
    void associate(const KeyType& key, const ValueType& value);
    const ValueType* find(const KeyType& key) const;
    ValueType* find(const KeyType& key);
    int getNumItems() const;
    double getLoadFactor() const;
    
private:
    int m_numElements = 0; // the current number of elements in the hash table
    int m_numBuckets = 100;
    double m_curLF = 0; // the current load factor
    double m_maxLF; // the max load factor until you must reallcat
    
    unsigned int getBucketNumber(const KeyType& k) const{
        unsigned int hash(const KeyType& k);
        unsigned int bucketLoc = hash(k)%m_numBuckets;
        return bucketLoc;
    }
    
    // The Bucket
    struct Node{
        int numValues = 0; // always start with ending string
        int maxSize = 100;
        KeyType key;
        ValueType value;
        Node *next;
    };
    
    Node **m_table = new Node*[m_numBuckets];;
};

//Testing: Check to see if the default (0.5) load factor is being used when nothing is passed to construcor
template <typename KeyType, typename ValueType>
MyHash<KeyType,ValueType>::MyHash(double maxLoadFactor) : m_maxLF(maxLoadFactor){
    for(int i = 0; i < m_numBuckets; i++){
        m_table[i] = nullptr;
    }
    if(m_maxLF <= 0){ // prevent the max load factor from being 0 or negative
        m_maxLF = 0.5;
    }
    else if(m_maxLF > 2){ // prevent the max load factor from exceeding 2
        m_maxLF = 2.0;
    }
}


// Deconstruct the hash table and remove all values
template <typename KeyType, typename ValueType>
MyHash<KeyType,ValueType>::~MyHash(){
    // Delete all buckets
    for(int i = 0; i < m_numBuckets; i++){
        Node *curNode = m_table[i]; // First node of the bucket
        // Delete all nodes and their corresponding grossness
        while(curNode != nullptr){
//            delete curNode->value; // TODO: Check deallocation here
            Node *temp = curNode;
            curNode = curNode->next;
            delete temp;
        }
    }
    delete [] m_table; // TODO: Check deallocation here
}

// Reset the hash table with new values
template <typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::reset(){
    // Delete all buckets
    for(int i = 0; i < m_numBuckets; i++){
        Node *curNode = m_table[i];
        while(curNode->next != nullptr){
//            delete curNode->value; // TODO: Check deallocation here
            Node *temp = curNode;
            curNode = curNode->next;
            delete temp;
        }
    }
    delete [] m_table; // TODO: Check deallocation here
    m_numElements = 0;
    m_curLF = 0;
    *m_table = new Node[100];
}

// Add a new value to the hash table
template <typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::associate(const KeyType &key, const ValueType &value){
    
    bool keyExists = false; // if a value is alread in the array
    Node *curNode = m_table[getBucketNumber(key)];
    
    //Create a new node
    
    Node* nodeToAdd = new Node;
    nodeToAdd->key = key;
//    std::cout << value;
    nodeToAdd->value = value; // a string list
    nodeToAdd->next = nullptr;
    
    // if the bucket is empty
    if(curNode == nullptr){
        // Create the new node from the value and key
        m_table[getBucketNumber(key)] = nodeToAdd;
        m_numElements++;
    }
    else{
        // Get the starting node and go through the LL to check for duplicate strings
        while(curNode->next != nullptr){
            if(curNode->key == key){
//                delete curNode->value; // TODO: May need to change this to "delete [] curNode->value"
                curNode->value = value; // add value before the NULL
                keyExists = true;
                break;
            }
            curNode = curNode->next;
        }
        if(!keyExists){
            curNode->next = nodeToAdd;
            m_numElements++;
        }
    }
    
    // IF LOAD FACTOR WAS EXCEEDED
    if(getLoadFactor() > m_maxLF){
        Node **tempTable = new Node*[m_numBuckets*2];
        for(int i = 0; i < m_numBuckets; i++){ // go through the original array
            Node* curNode = m_table[i]; // save that node to a local var
            while(curNode != nullptr){ // go through all nodes in that list
                // Create a new node pointer with the values from the previous list
                Node *data = nullptr;
                data->key = curNode->key;
                data->value = curNode->value;
                
                if(&tempTable[getBucketNumber(data->key)] != nullptr){
                    Node *temp = tempTable[getBucketNumber(data->key)];
                    data->next = temp;
                    *tempTable[getBucketNumber(data->key)] = *data;
                }
                else{
                    *tempTable[getBucketNumber(data->key)] = *data;
                }
            }
        }
        
        delete [] tempTable;
        m_table = tempTable;
    }
}

// Find the corresponding bucket to

//TODO: May need to make this "const" spec and the provided skeleton differ
template <typename KeyType, typename ValueType>
ValueType* MyHash<KeyType, ValueType>::find(const KeyType &key){
    Node *curNode = m_table[getBucketNumber(key)];
    std::cout<<getBucketNumber(key);
    while(curNode != nullptr){
        if(curNode->key == key){
            return &(curNode->value);
        }
        curNode = curNode->next;
    }
    return nullptr;
}

// Return the number of elements in the list
template <typename KeyType, typename ValueType>
int MyHash<KeyType, ValueType>::getNumItems() const{
    return m_numElements;
}

// Return the current load factor
template <typename KeyType, typename ValueType>
double MyHash<KeyType, ValueType>::getLoadFactor() const{
    return (double)(((double)m_numElements)/((double)m_numBuckets));
}


#endif /* MyHash_hpp */
