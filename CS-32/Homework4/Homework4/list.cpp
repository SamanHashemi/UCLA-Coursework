//#include <iostream>
//#include <string>
//#include <vector>
//
//using namespace std;
//
//class Class
//{
//public:
//    Class(string nm) : m_name(nm) {}
//    string name() const { return m_name; }
//    const vector<Class*>& subclasses() const { return m_subclasses; }
//    void add(Class* d) { m_subclasses.push_back(d); }
//    ~Class();
//private:
//    string m_name;
//    vector<Class*> m_subclasses;
//};
//
//Class::~Class()
//{
//    for (size_t k = 0; k < m_subclasses.size(); k++)
//        delete m_subclasses[k];
//}
//
//void listAll(const Class* c, string path)  // two-parameter overload
//{
//    if(c != nullptr){
//        string toPass = path + c->name() ;
//        cout << toPass << endl;
//        if(c->subclasses().empty())
//            return;
//        toPass += "=>";
//        vector<Class*>::const_iterator it = c->subclasses().begin();
//        while((it) != c->subclasses().end()){
//            listAll((*it), toPass);
//            it++;
//        }
//    }
//}
//
//void listAll(const Class* c)  // one-parameter overload
//{
//    if (c != nullptr)
//        listAll(c, "");
//}
//
//int main()
//{
//    Class* d1 = new Class("Snagglegon");
//    listAll(d1);
//    cout << "====" << endl;
//    d1->add(new Class("PutridSnagglegon"));
//    Class* d2 = new Class("Alien");
//    d2->add(new Class("Smallgon"));
//    d2->add(d1);
//    Class* d3 = new Class("Goodie");
//    d3->add(new Class("ExtraLifeGoodie"));
//    d3->add(new Class("RepairGoodie"));
//    d3->add(new Class("TorpedoGoodie"));
//    listAll(d3);
//    cout << "====" << endl;
//    Class* d4 = new Class("Actor");
//    d4->add(d2);
//    d4->add(new Class("Cabbage"));
//    d4->add(d3);
//    listAll(d4);
//    delete d4;
//}
//
