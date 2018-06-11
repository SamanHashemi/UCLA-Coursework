// MyHash.h

// Skeleton for the MyHash class template.  You must implement the first seven
// member functions; we have implemented the eighth.

#include <string>
#include <functional>
#include <iostream>

template<typename KeyType, typename ValueType>
class MyHash
{
public:
    MyHash(double maxLoadFactor = 0.5);
    ~MyHash();
    void reset();
    void associate(const KeyType& key, const ValueType& value);
    int getNumItems() const;
    double getLoadFactor() const;

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const MyHash*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    MyHash(const MyHash&) = delete;
    MyHash& operator=(const MyHash&) = delete;

private:
    int m_numElements = 0; // the current number of elements in the hash table
    int m_numBuckets = 100;
    double m_curLF = 0; // the current load factor
    double m_maxLF; // the max load factor until you must reallcat
    
    unsigned int getBucketNumber(const KeyType& k) const;
    
    // The Bucket
    struct Node{
        KeyType key;
        ValueType value;
        Node *next;
    };
    
    Node **m_table = new Node*[m_numBuckets];
};


//Testing: Check to see if the default (0.5) load factor is being used when nothing is passed to construcor
template <typename KeyType, typename ValueType>
MyHash<KeyType,ValueType>::MyHash(double maxLoadFactor) : m_maxLF(maxLoadFactor) // NOTE: Should be okay, 99% certain
{
    for(int i = 0; i < m_numBuckets; i++)
    {
        m_table[i] = nullptr;
    }
    if(m_maxLF <= 0){ // prevent the max load factor from being 0 or negative
        m_maxLF = 0.5;
    }
    else if(m_maxLF > 2){ // prevent the max load factor from exceeding 2
        m_maxLF = 2.0;
    }
    else
        m_maxLF = maxLoadFactor;
}


// Deconstruct the hash table and remove all values
template <typename KeyType, typename ValueType>
MyHash<KeyType,ValueType>::~MyHash(){ // NOTE: Should be okay, 99% certain (minus the possible creation of "value" to ptr
    // Delete all buckets
    for(int i = 0; i < m_numBuckets; i++)
    {
        Node *curNode = m_table[i]; // First node of the bucket
        // Delete all nodes and their corresponding grossness
        while(curNode != nullptr)
        {
            Node *temp = curNode;
            // TODO: Check deallocation here if you decide to make "value" (in node) a pointer
            curNode = curNode->next;
            delete temp;
        }
    }
    delete [] m_table; // TODO: Check deallocation here
}

// Reset the hash table with new values
template <typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::reset(){ // NOTE: Should be okay 90% certain (minus the possible creation of "value" to ptr
    // Delete all buckets
    for(int i = 0; i < m_numBuckets; i++)
    {
        Node *curNode = m_table[i];
        while(curNode != nullptr)
        {
            Node *temp = curNode;
            // TODO: Check deallocation here if you decide to make "value" (in node) a pointer
            curNode = curNode->next;
            delete temp;
        }
    }
    delete [] m_table; // TODO: Check deallocation here
    m_numElements = 0;
    m_numBuckets = 100;
    m_table = new Node*[100];
    for (int i = 0; i < m_numBuckets; i++)
        m_table[i] = nullptr;
}


template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::associate(const KeyType &key, const ValueType &value) {
    unsigned int hash(const KeyType& k);
    
    ValueType* value_pointer = find(key);
    
    if (value_pointer == nullptr){
        m_numElements++;
        
        //If the load factor has been exceeded
        if (getLoadFactor() > m_maxLF)
        {
            Node **tempTable = new Node*[m_numBuckets * 2]; // Create the double sized table
            
            // Initilize the values
            for (int i = 0; i < 2 * m_numBuckets; i++)
                tempTable[i] = nullptr;
            
            // Re-hash all value
            for (int i = 0; i < m_numBuckets; i++)
            {
                // Go through and add to the
                Node* curNode = m_table[i];
                while (curNode != nullptr)
                {
                    Node* tempTableNode = tempTable[hash(curNode->key) % (m_numBuckets * 2)]; // node to the new list
                    Node* tempNode = new Node; // placeholder node
                    tempNode->next = tempTableNode; // place holder gets the current node being hashed to
                    tempNode->key = curNode->key;
                    tempNode->value = curNode->value;
                    tempTable[hash(tempNode->key) % (m_numBuckets * 2)] = tempNode;
                    tempNode = tempNode->next;
                }
            }
            
            // Create the new node of the value
            Node* tempTableNode= tempTable[hash(key) % (m_numBuckets*2)];
            Node* tempNode = new Node;
            tempNode->next = tempTableNode;
            tempNode->key = key;
            tempNode->value = value;
            tempTable[hash(key) % (m_numBuckets*2)] = tempNode;
            
            // Delete the old values and reassin the new ones
            for (int i = 0; i < m_numBuckets; i++)
            {
                Node* cur = m_table[i];
                while (cur != nullptr){
                    Node* temp = cur;
                    cur = cur->next;
                    delete temp;
                }
            }
            delete[] m_table;
            m_table = tempTable;
        }
        else // if the load facter was fine
        {
            int bucketNum = getBucketNumber(key);
            Node* x = m_table[bucketNum];
            Node* begin = new Node;
            begin->next = x;
            begin->key = key;
            begin->value = value;
            m_table[bucketNum] = begin;
        }
    }
    else // the key already existed
    {
        *value_pointer = value;
    }
}

//TODO: May need to make this "const" spec and the provided skeleton differ
template <typename KeyType, typename ValueType>
const ValueType* MyHash<KeyType, ValueType>::find(const KeyType &key) const // NOTE: Should be okay, 99% certain
{
    Node *curNode = m_table[getBucketNumber(key)];
    
    if(curNode == nullptr)
        return nullptr;
    
    while(curNode != nullptr)
    {
        if(curNode->key == key)
        {
            return &(curNode->value);
        }
        curNode = curNode->next;
    }
    return nullptr;
}

// Return the number of elements in the list
template <typename KeyType, typename ValueType>
int MyHash<KeyType, ValueType>::getNumItems() const{ // NOTE: Is okay, 100% certain
    return m_numElements;
}

// Return the current load factor
template <typename KeyType, typename ValueType>
double MyHash<KeyType, ValueType>::getLoadFactor() const // NOTE: Is okay, 100% certain
{
    return (double)(((double)m_numElements)/((double)m_numBuckets));
}

template <typename KeyType, typename ValueType>
unsigned int MyHash<KeyType, ValueType>::getBucketNumber(const KeyType& k) const
{
    unsigned int hash(const KeyType& k);
    return hash(k)%m_numBuckets;
}
