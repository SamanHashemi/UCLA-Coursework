#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
//#include "Actor.h"
#include <vector>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class NachenBlaster;
class Star;
class Actor;

double distanceCalculate(double x1, double y1, double x2, double y2);

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    Actor* collidedWith(Actor* a);
    bool withinShootingDistance(Actor* shooter);
    bool addAnotherAlien();
    void gotHealth();
    void alienKilled();
    void alienLeftScreen();
    void deleteDeadActors();
    void addToActorList(Actor* a);
    
    
private:
    int m_shipsDestroyed;
    int m_aliensOnScreen;
    
    bool m_hasBeenPlayed = true;
    
    std::vector<Actor*> actorList;
    NachenBlaster* nBlaster;
};



#endif // STUDENTWORLD_H_
