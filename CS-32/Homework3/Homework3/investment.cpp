#include <iostream>
#include <string>
using namespace std;

//Your declarations and implementations would go here
class Investment{
public:
    Investment(string name, int price);
    virtual ~Investment(){}
    
    string name() const;
    int purchasePrice() const;
    
    virtual string description() const = 0;
    virtual bool fungible() const;
    
private:
    string m_name;
    string m_descrip;
    int m_price;
    bool m_fungible = false;
};

Investment::Investment(string name, int price){
    m_name = name;
    m_price = price;
}

int Investment::purchasePrice() const{
    return m_price;
}

string Investment::name() const{
    return m_name;
}

bool Investment::fungible() const{
    return m_fungible;
}



class Painting : public Investment{
public:
    Painting(string name, int price);
    virtual string description() const;
    ~Painting();
};


Painting::Painting(string name, int price):Investment(name, price){}

string Painting::description() const{
    return "painting";
}

Painting::~Painting(){
    cout << "Destroying " << name() << " a painting" << endl;
}


class Stock : public Investment{
public:
    Stock(string name, int price, string abrv);
    virtual string description() const;
    virtual bool fungible() const;
    ~Stock();
    
private:
    bool m_fungible = true;
    string m_abrv;
};

Stock::Stock(string name, int price, string abrv):Investment(name, price), m_abrv(abrv){}

string Stock::description() const{
    return "stock trading as " + m_abrv;
}

bool Stock::fungible() const{
    return true;
}

Stock::~Stock(){
    cout << "Destroying " << name() << ", a stock holding" << endl;
}


class House : public Investment{
public:
    House(string addr, int price);
    virtual string description() const;
    ~House();
    
private:
    bool m_fungible = false;
};

House::House(string addr, int price):Investment(addr, price){}

string House::description() const{
    return "house";
}

House::~House(){
    cout << "Destroying the house " << name() << endl;
}

