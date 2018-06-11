//#include "newMap.h"
//
//Map::Map(){
//    m_listSize = 0;
//    m_max = DEFAULT_MAX_ITEMS;
//    m_mapList = new mapVals[DEFAULT_MAX_ITEMS];
//}
//
//
//Map::Map(int initSize){
//    m_listSize = 0;
//    m_max = initSize;
//    m_mapList = new mapVals[initSize];
//}
//
//// Copy constructor
//Map::Map(const Map& other){
//    m_max = other.m_max;
//    m_listSize = other.m_listSize;
//    m_mapList = new mapVals[m_max];
//    for(int i = 0; i < m_listSize; i++){
//        m_mapList[i].key = other.m_mapList[i].key;
//        m_mapList[i].val = other.m_mapList[i].val;
//    }
//}
//
//Map::~Map(){
//    delete [] m_mapList;
//}
//
//// Assignment operator
//Map& Map::operator=(const Map& map){
//    if(this != &map){
//        delete [] m_mapList;
//        m_listSize = map.m_listSize;
//        m_max = map.m_max;
//        m_mapList = new mapVals[m_max];
//        for(int i = 0; i < m_max; i++){
//            m_mapList[i].key = map.m_mapList[i].key;
//            m_mapList[i].val = map.m_mapList[i].val;
//        }
//    }
//    return *this;
//}
//
//int Map::size() const{
//    return m_listSize;
//}
//
//
//bool Map::empty() const{
//    return m_listSize == 0;
//}
//
//
//
//
//// If key is not equal to any key currently in the map, and if the
//// key/value pair can be added to the map, then do so and return true.
//// Otherwise, make no change to the map and return false (indicating
//// that either the key is already in the map, or the map has a fixed
//// capacity and is full).
//bool Map::insert(const KeyType& key, const ValueType& value){
//    if(m_listSize >= m_max){
//        return false;
//    }
//    for(int i = 0; i < m_listSize; i++){
//        if(m_mapList[i].key == key){
//            return false;
//        }
//    }
//    m_mapList[m_listSize].key = key;
//    m_mapList[m_listSize].val = value;
//    m_listSize++;
//    return true;
//}
//
//// If key is equal to a key currently in the map, then make that key no
//// longer map to the value it currently maps to, but instead map to
//// the value of the second parameter; return true in this case.
//// Otherwise, make no change to the map and return false.
//bool Map::update(const KeyType& key, const ValueType& value){
//    for(int i = 0; i < m_listSize; i++){
//        if(m_mapList[i].key == key){
//            m_mapList[i].val = value;
//            return true;
//        }
//    }
//    return false;
//}
//
//// If key is equal to a key currently in the map, then make that key no
//// longer map to the value it currently maps to, but instead map to
//// the value of the second parameter; return true in this case.
//// If key is not equal to any key currently in the map and if the
//// key/value pair can be added to the map, then do so and return true.
//// Otherwise, make no change to the map and return false (indicating
//// that the key is not already in the map and the map has a fixed
//// capacity and is full).
//bool Map::insertOrUpdate(const KeyType& key, const ValueType& value){
//    if(m_listSize >= m_max){
//        return false;
//    }
//    for(int i = 0; i < m_listSize; i++){
//        if(m_mapList[i].key == key){
//            m_mapList[i].val = value;
//            return true;
//        }
//    }
//    m_mapList[m_listSize].key = key;
//    m_mapList[m_listSize].val = value;
//    m_listSize++;
//    return true;
//}
//
//
//// If key is equal to a key currently in the map, remove the key/value
//// pair with that key from the map and return true.  Otherwise, make
//// no change to the map and return false.
//bool Map::erase(const KeyType& key){
//    for(int i = 0; i < m_listSize; i++){
//        if(m_mapList[i].key == key){
//            for(int j = i; j < m_listSize; j++){
//                if(j == m_listSize-1){
//                    m_mapList[j] = mapVals{}; // BOI THis may done fuck up
//                    m_listSize--;
//                }
//                else{
//                    m_mapList[j] = m_mapList[j+1];
//                }
//            }
//            return true;
//        }
//    }
//    return false;
//}
//
//
//// Return true if key is equal to a key currently in the map, otherwise
//// false.
//bool Map::contains(const KeyType& key) const{
//    for(int i = 0; i < m_listSize; i++){
//        if(m_mapList[i].key == key){
//            return true;
//        }
//    }
//    return false;
//}
//
//// If key is equal to a key currently in the map, set value to the
//// value in the map that that key maps to, and return true.  Otherwise,
//// make no change to the value parameter of this function and return
//// false.
//bool Map::get(const KeyType& key, ValueType& value) const{
//    for(int i = 0; i < m_listSize; i++){
//        if(m_mapList[i].key == key){
//            value = m_mapList[i].val;
//            return true;
//        }
//    }
//    return false;
//}
//
//
//// If 0 <= i < size(), copy into the key and value parameters the
//// key and value of one of the key/value pairs in the map and return
//// true.  Otherwise, leave the key and value parameters unchanged and
//// return false.  (See below for details about this function.)
//bool Map::get(int i, KeyType& key, ValueType& value) const{
//    if(i >= 0 && i < m_listSize){
//        key = m_mapList[i].key;
//        value = m_mapList[i].val;
//        return true;
//    }
//    return false;
//}
//
//
//// Exchange the contents of this map with the other one.
//void Map::swap(Map& other){
//    Map tempMap = *this;
//    *this = other;
//    other = tempMap;
//}
//
//
//
