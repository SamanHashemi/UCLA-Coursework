//#include "CarMap.h"
//
//CarMap::CarMap(){
//    
//}
//
//bool CarMap::addCar(std::string license){
//    if(m.insert(license, 0)){
//        return true;
//    }
//    return false;
//}
//// If a car with the given license plate is not currently in the map,
//// and there is room in the map, add an entry for that car recording
//// that it has 0 gallons of gas din it, and return true.  Otherwise,
//// make no change to the map and return false.
//
//double CarMap::gas(std::string license) const{
//    double gas = 0.0;
//    if(!m.get(license, gas))
//        return -1;
//    return gas;
//}
//// If a car with the given license plate is in the map, return the
//// number of gallons of gas in its tank; otherwise, return -1.
//
//bool CarMap::addGas(std::string license, double gallons){
//    double curGas = gas(license);
//    if(gallons < 0){
//        return false;
//    }
//    m.update(license, (curGas + gallons));
//    return true;
//}
//// If no car with the given license plate is in the map or if
//// gallons is negative, make no change to the map and return
//// false.  Otherwise, increase the number of gallons of gas in the
//// indicated car by the gallons parameter and return true.
//
//bool CarMap::useGas(std::string license, double gallons){
//    double gasInCar = gas(license);
//    if(!m.contains(license) || gallons > gasInCar || gallons < 0){
//        return false;
//    }
//    m.update(license, (gasInCar - gallons));
//    return true;
//}
//// If no car with the given license plate is in the map or if
//// gallons is negative or if gallons > gas(), make no change to the
//// map and return false.  Otherwise, decrease the number of gallons
//// of gas in the indicated car by the gallons parameter and return
//// true.
//
//int CarMap::fleetSize() const{
//    return m.size();
//}  // Return the number of cars in the CarMap.
//
//void CarMap::print() const{
//    std::string l;
//    double v;
//    for(int i = 0; i < m.size(); i++){
//        m.get(i, l, v);
//        std::cout << l << " " << v << std::endl;
//    }
//}
//// Write to cout one line for every car in the map.  Each line
//// consists of the car's license plate, followed by one space,
//// followed by the number of gallons in that car's tank.  Write
//// no other text.
//
//
