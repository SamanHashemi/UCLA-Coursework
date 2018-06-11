void listAll(const Class* c, string path)  // two-parameter overload
{
    if(c != nullptr){
        string toPass = path + c->name() ;
        cout << toPass << endl;
        if(c->subclasses().empty())
            return;
        toPass += "=>";
        vector<Class*>::const_iterator it = c->subclasses().begin();
        while((it) != c->subclasses().end()){
            listAll((*it), toPass);
            it++;
        }
    }
}
