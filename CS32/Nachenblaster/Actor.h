#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

class StudentWorld;


///////////////////////////////
//          ACTOR           //
//////////////////////////////

class Actor : public GraphObject{
public:
    Actor();
    Actor(int imageID, int sX, int sY, int sDir, double size, int depth);
    virtual ~Actor();
    
    virtual bool isAlive();
    void hasDied();
    int getImageID();
//    virtual bool isActivated(int x, int y); // use only for goodies
    
    virtual void doSomething() = 0;
    
private:
    StudentWorld* studentWorld;
    int m_imageID;
    bool alive = true;
};

///////////////////////////////
//      WORLD EXTENDER       //
//////////////////////////////

class WorldExtender : public Actor{
public:
    WorldExtender(int imageID, int sX, int sY, int sDir, double size, int depth, StudentWorld* sWorld);
    StudentWorld* getWorld();
private:
    StudentWorld* studentWorld;
};

///////////////////////////////
//         Explosion         //
//////////////////////////////


class Explosion : public Actor{
public:
    Explosion(int sX, int sY);
    virtual void doSomething();
private:
    int m_numberOfTicks;
};


///////////////////////////////
//          ALIEN           //
//////////////////////////////

class Alien : public WorldExtender{
public:
    Alien(int starID, int sX, int sY, int sDir, double size, int depth, int hp, StudentWorld* sWorld);
    int getHP();
    void setHP(int hp);
    void sufferDamage(int damageAmt);
private:
    int m_hp;
};


///////////////////////////////
//         ENEMIES           //
//////////////////////////////

class Enemies : public Alien{
public:
    Enemies(int starID, int sX, int sY, int sDir, double size, int depth, int hp, double speed, int points, StudentWorld* sWorld, int tDir, int flightPath);
    
    void setTravelDir(int dir);
    int getTravelDir();
    
    void setFlightPath(int dist);
    void flewOneUnit();
    int getFlightPath();
    
    
    void setSpeed(int speed);
    int getSpeed();
    
private:
    double m_moveSpeed;
    int m_flightPath;
    int m_travelDir; // 1 is up and left, 0 is left, -1 is down and left
    int m_points;
};

///////////////////////////////
//      NACHENBLASTER       //
//////////////////////////////

class NachenBlaster : public Alien{
public:
    NachenBlaster();
    NachenBlaster(int starID, int sX, int sY, int sDir, double size, int depth, int hp, int cabPwr, StudentWorld* sWorld);
    virtual ~NachenBlaster();
    
    virtual void doSomething();
    
    int getCabPower();
    void setCabPower(int pwr);
    
    void incTorpedo();
    int getTorpedo();
    
private:
    int m_numTorpedos = 0;
    int m_cabPower = 30;
};



///////////////////////////////
//         SMALLGON          //
//////////////////////////////

class Smallgon : public Enemies{
public:
    Smallgon(int ID, int sX, int sY, int sDir, double size, int depth, int hp, double speed, int points, StudentWorld* sWorld, int fpLength);
    virtual void doSomething();
private:
};

///////////////////////////////
//        SMOREGON          //
//////////////////////////////

class Smoregon : public Enemies{
public:
    Smoregon(int ID, int sX, int sY, int sDir, double size, int depth, int hp, double speed, int points, StudentWorld* sWorld, int fpLength);
    virtual void doSomething();
private:
};


///////////////////////////////
//        Snagglegon         //
//////////////////////////////

class Snagglegon : public Enemies{
public:
    Snagglegon(int ID, int sX, int sY, int sDir, double size, int depth, int hp, double speed, int points, StudentWorld* sWorld, int fpLength);
    
    virtual void doSomething();
private:
};


///////////////////////////////
//           STAR            //
//////////////////////////////

class Star : public Actor{
public:
    Star(int starID, int sX, int sY, int sDir, double size, int depth);
    virtual void doSomething();
    // may need to add "alive" variable here
private:
};


///////////////////////////////
//         CABBAGE          //
//////////////////////////////

class Cabbage : public WorldExtender{
public:
    Cabbage(int sX, int sY, StudentWorld* sWorld);
    virtual void doSomething();

private:
};


///////////////////////////////
//          TURNIP           //
//////////////////////////////

class Turnip : public WorldExtender{
public:
    Turnip(int sX, int sY, StudentWorld* sWorld);
    
    virtual void doSomething();
private:
};



///////////////////////////////
//     FLATLUANCE TORPEDO    //
//////////////////////////////

class FlatulanceTorpedo : public WorldExtender {
public:
    FlatulanceTorpedo(int sX, int sY, int dir, StudentWorld* sWorld, bool fbnb);
    
    virtual void doSomething();
    
    bool firedByMe();
private:
    bool m_firedByNBlaster;
};



///////////////////////////////
//     EXTRA LIFE GOODIE     //
//////////////////////////////

class ExtraLifeGoodie : public WorldExtender{
public:
    ExtraLifeGoodie(int sX, int sY, StudentWorld* sWorld);
    virtual void doSomething();
private:
};

///////////////////////////////
//       REPAIR GOODIE      //
//////////////////////////////

class RepairGoodie : public WorldExtender{
public:
    RepairGoodie(int sX, int sY, StudentWorld* sWorld);
    virtual void doSomething();
private:
};


///////////////////////////////
//       TORPEDO GOODIE      //
//////////////////////////////

class TorpedoGoodie : public WorldExtender{
public:
    TorpedoGoodie(int sX, int sY, StudentWorld* sWorld);
    virtual void doSomething();
private:
};


#endif // ACTOR_H_
