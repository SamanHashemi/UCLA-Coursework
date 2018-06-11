#include "Map.h"

Map::Map(){
    m_listSize = 0;
}

Map::Map(const Map& other){ // Construct the map from another map
    if(other.head == nullptr){ // check the value to make sure not empty
        head = nullptr;
        return;
    }
    //create variables here
    m_listSize = other.m_listSize;
    Node *q = other.head;
    Node *p = new Node();
    Node* tail = q;
    
    // properly assign the values here
    p->data = q->data;
    tail = head = p;
    q = q->next;
    while(q != nullptr){ // itterate the list make changes to the nodes
        p = new Node();
        p->data = q ->data;
        q = q->next;
        tail->next = p;
        p->last = tail;
        tail = p;
    }
    head->last = nullptr; // assign last
}

// Deconstructor
Map::~Map(){
    Node* cur = head;
    while(cur != nullptr){
        Node* temp = cur->next;
        delete cur;
        cur = temp;
    }
}

// Assignment oper ator
Map& Map::operator=(const Map& map){
    // Deconstruct
    if(this != &map){
        Node* cur = head;
        while(cur != nullptr){
            Node* temp = cur->next;
            delete cur;
            cur = temp;
        }
        
        // Re-assign
        if(map.head == nullptr){
            head = nullptr;
        }
        m_listSize = map.m_listSize;
        Node *q = map.head;
        Node *p = new Node();
        Node* tail = q;
        
        p->data = q->data;
        tail = head = p;
        q = q->next;
        while(q != nullptr){
            p = new Node();
            p->data = q ->data;
            q = q->next;
            tail->next = p;
            p->last = tail;
            tail = p;
        }
        head->last = nullptr;

    }
    return *this;
}

// Return the number of elements in the list
int Map::size() const{
    return m_listSize;
}

// Return true if the list is empty
bool Map::empty() const{
    return m_listSize == 0;
}

// Insert the value the location
bool Map::insert(const KeyType& key, const ValueType& value){
    Node* last_node = new Node;
    if(head != nullptr){
        Node* cur = head;
        while(cur->next != nullptr){ // possibly change to cur->next
            if(cur->data.m_key == key){ // if the element is already in the list
                return false;
            }
            cur = cur->next;
        }
        // May need to remove this if statement
        if(cur->data.m_key == key){
            return false;
        }
        // move the values around and assign
        last_node->data.m_key = key;
        last_node->data.m_val = value;
        last_node->next = nullptr;
        last_node->last = cur;
        cur->next = last_node;
        m_listSize++;
    }
    else{
        // move the values around and assign
        last_node->data.m_key = key;
        last_node->data.m_val = value;
        last_node->next = nullptr;
        last_node->last = nullptr;
        head = last_node; // assign head to the node just created
        m_listSize++;
    }
    return true;

    
    // If key is not equal to any key currently in the map, and if the
    // key/value pair can be added to the map, then do so and return true.
    // Otherwise, make no change to the map and return false (indicating
    // that either the key is already in the map, or the map has a fixed
    // capacity and is full).
}

// Update the value at alocation
bool Map::update(const KeyType& key, const ValueType& value){
    if(head != nullptr){ // if the node is not empty
        Node* cur = head;
        while(cur != nullptr){ // iterate
            if(cur->data.m_key == key){
                cur->data.m_val = value;
                return true;
            }
            cur = cur->next;
        }
    }
    return false;

    // If key is equal to a key currently in the map, then make that key no
    // longer map to the value it currently maps to, but instead map to
    // the value of the second parameter; return true in this case.
    // Otherwise, make no change to the map and return false.
}

bool Map::insertOrUpdate(const KeyType& key, const ValueType& value){
    if(!contains(key)){ // the key is not in the list
        return insert(key, value); // add the value
    }
    return update(key, value); // change the value
    
    // If key is equal to a key currently in the map, then make that key no
    // longer map to the value it currently maps to, but instead map to
    // the value of the second parameter; return true in this case.
    // If key is not equal to any key currently in the map and if the
    // key/value pair can be added to the map, then do so and return true.
    // Otherwise, make no change to the map and return false (indicating
    // that the key is not already in the map and the map has a fixed
    // capacity and is full).
}

bool Map::erase(const KeyType& key){
    Node* cur = head;
    if(head == nullptr){ // if the list is empty
        return false;
    }
    else{
        
        while(cur != nullptr){ // itterate
            if(cur->data.m_key == key){
                if(m_listSize == 1){  // only one element
                    head = nullptr;
                }
                else if(cur->next == nullptr){ // if last element
                    cur->last->next = nullptr;
                }
                else if(cur->last == nullptr){ // > 1 element
                    head = cur->next;
                    cur->next->last = nullptr;
                }
                else{
                    cur->last->next = cur->next;
                    cur->next->last = cur->last;
                }
                delete cur;
                m_listSize--;
                return true;
            }
            cur = cur->next;
        }
    }
    return false;

    // If key is equal to a key currently in the map, remove the key/value
    // pair with that key from the map and return true.  Otherwise, make
    // no change to the map and return false.
}

bool Map::contains(const KeyType& key) const{
    Node* cur = head;
    while(cur != nullptr){ // iterate
        if(cur->data.m_key == key){ // check to seee if the key is there
            return true;
        }
        cur = cur->next;
    }
    return false;

    // Return true if key is equal to a key currently in the map, otherwise
    // false.
}

bool Map::get(const KeyType& key, ValueType& value) const{
    Node* cur = head;
    while(cur != nullptr){
        if(cur->data.m_key == key){
            value = cur->data.m_val; // assign if the key is there
            return true;
        }
        cur = cur->next;
    }
    return false;

    // If key is equal to a key currently in the map, set value to the
    // value in the map that that key maps to, and return true.  Otherwise,
    // make no change to the value parameter of this function and return
    // false.
}

bool Map::get(int i, KeyType& key, ValueType& value) const{
    Node* cur = head;
    if(i >= 0 && i < m_listSize){
        for(int count = 0; count < i; count++){
            cur = cur->next;
        }
        // assign values if the key exists
        key = cur->data.m_key;
        value = cur->data.m_val;
        return true;
    }
    return false;

    // If 0 <= i < size(), copy into the key and value parameters the
    // key and value of one of the key/value pairs in the map and return
    // true.  Otherwise, leave the key and value parameters unchanged and
    // return false.  (See below for details about this function.)
}

// Exchange the contents of this map with the other one.
void Map::swap(Map& other){
    // Swap the heads
    Node* temp_head = other.head;
    other.head = head;
    head = temp_head;
    
    // Swap the sizes
    int temp_size = m_listSize;
    m_listSize = other.m_listSize;
    other.m_listSize = temp_size;
}

void Map::dump(){
    Node* cur = head;
    if(cur != nullptr){
        while(cur != nullptr){
            std::cerr << cur->data.m_key << " " << cur->data.m_val << std::endl;
            cur = cur->next;
        }
    }
}

void subtract(const Map& m1, const Map& m2, Map& result){
    
    // Erase the result if it's not empty
    int result_size = result.size();
    KeyType result_key;
    ValueType result_value;
    
    for(int i = 0; i < result_size; i++){
        result.get(0, result_key, result_value);
        result.erase(result_key);
    }
    
    // create the new result list
    int m1_size = m1.size();
    KeyType m1_key;
    ValueType m1_val;

    for(int i = 0; i < m1_size; i++){
        m1.get(i, m1_key, m1_val);
        if(!m2.contains(m1_key)){
            result.insert(m1_key, m1_val);
        }
    }
}

bool combine(const Map& m1, const Map& m2, Map& result){
    
    // Erase the result if it's not empty
    int result_size = result.size();
    KeyType result_key;
    ValueType result_value;
    for(int i = 0; i < result_size; i++){
        result.get(0, result_key, result_value);
        result.erase(result_key);
    }
    
    // Combine the two
    int m1_size = m1.size();
    int m2_size = m2.size();
    KeyType m1_key, m2_key;
    ValueType m1_val, m2_val;
    bool notTheSameKey = false;
    
    // Go through the first list
    for(int i = 0; i < m1_size; i++){
        m1.get(i, m1_key, m1_val);
        if(m2.get(m1_key, m2_val)){
            if(m1_val == m2_val){
                result.insert(m1_key, m1_val);
            }
            else{
                notTheSameKey = true;
            }
        }
        else{
            result.insert(m1_key, m1_val);
        }
    }
    
    // Go through the second list
    for(int i = 0; i < m2_size; i++){
        m2.get(i, m2_key, m2_val);
        if(m1.get(m2_key, m1_val)){
            if(m2_val == m1_val){
                result.insert(m2_key, m2_val);
            }
            else{
                notTheSameKey = true;
            }
        }
        else{
            result.insert(m2_key, m2_val);
        }
    }
    if(notTheSameKey){
        return false;
    }
    return true;
}




