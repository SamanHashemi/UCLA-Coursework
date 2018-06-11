#ifndef MAP_INCLUDE
#define MAP_INCLUDE

#include <string>
#include <iostream>

const int DEFAULT_MAX_ITEMS = 250;

using KeyType = std::string;
using ValueType = double;



class Map
{
public:
    Map();
    Map(const Map& m);
    Map& operator=(const Map& map);
    ~Map();
    bool empty() const;
    int size() const;
    bool insert(const KeyType& key, const ValueType& value);
    bool update(const KeyType& key, const ValueType& value);
    bool insertOrUpdate(const KeyType& key, const ValueType& value);
    bool erase(const KeyType& key);
    bool contains(const KeyType& key) const;
    bool get(const KeyType& key, ValueType& value) const;
    bool get(int i, KeyType& key, ValueType& value) const;
    void swap(Map& other);
    bool combine(const Map& m1, const Map& m2, Map& result);
    void subtract(const Map& m1, const Map& m2, Map& result);
    void dump();
private:
    // Struct for the value pairs in map
    struct m_pairs{
        KeyType m_key;
        ValueType m_val;
    };
    
    // Struct for the nodes to contain
    struct Node{
        Node* next;
        Node* last;
        m_pairs data;
    };
    
    Node* head = nullptr;
    int m_listSize = 0;
};

void subtract(const Map& m1, const Map& m2, Map& result);
bool combine(const Map& m1, const Map& m2, Map& result);

#endif

