#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <sstream>

#include <string>
#include <iostream>
#include <math.h>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetDir)
: GameWorld(assetDir)
{
    m_shipsDestroyed = 0;
    m_aliensOnScreen = 0;
    // make star here
}

StudentWorld::~StudentWorld(){
    cleanUp();
}

int StudentWorld::init()
{
    m_shipsDestroyed = 0;
    m_aliensOnScreen = 0;
    
    
    // Add the nBlaster
    nBlaster = new NachenBlaster(IID_NACHENBLASTER, 0, 128, 0, 1.0, 0, 50, 30, this);
    
    for(int i = 0; i < 30; i++){ // add 30 stars to the screen
        int xLoc = randInt(0, VIEW_WIDTH-1);
        int yLoc = randInt(0, VIEW_HEIGHT-1);
        actorList.push_back(new Star(IID_STAR, xLoc, yLoc, 0, ((double)randInt(5, 50))/100, 3));
    }
    return GWSTATUS_CONTINUE_GAME;
}


int StudentWorld::move()
{
    // Set the display text to display relevant info
    stringstream ss;
    ss << "Lives: " <<  getLives() << "  " << "Health: " << (int)(100*(((double)nBlaster->getHP())/50)) << "%" << "  " << "Score: " << getScore() << "  " << "Level: " << getLevel() << "  " << "Cabbages: " << (int)(100*(((double)(nBlaster->getCabPower())/30.0))) << "%" << "  " << "Torpedos: " << nBlaster->getTorpedo();
    string displayText = ss.str();
    setGameStatText(displayText);
    
    // Let the nachenblaster blast
    if((nBlaster)->isAlive()){
        (nBlaster)->doSomething();
    }
    if(!nBlaster->isAlive()){ // If nBlaster is dead, return
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    // Move the actors (excluding nBlaster)
    std::vector<Actor*>::iterator it = actorList.begin();
    for(; it != actorList.end(); it++){
        if((*it)->isAlive()){
            (*it)->doSomething();
            if(m_shipsDestroyed >= 6+(4*getLevel())){
                m_shipsDestroyed = 0;
                m_aliensOnScreen = 0;
                playSound(SOUND_FINISHED_LEVEL);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }
    
    // Add alien
    if(addAnotherAlien()){
        int S1 = 60;
        int S2 = 20 + getLevel()*.5;
        int S3 = 5 + getLevel()*10;
        int S = S1 + S2 + S3;
        int deter = randInt(1, S);
        if(deter >= 1 && deter <= S1){ // add smallgon
            Smallgon* s = new Smallgon(IID_SMALLGON, VIEW_WIDTH-1, randInt(0, VIEW_HEIGHT-1), 0, 1.5, 1, 5*(1+(getLevel()-1)*.1), 2.0, 250, this, 0);
            actorList.push_back(s);
            m_aliensOnScreen++;
        }
        else if(deter <= S2+S1){ // add smorgon
            Smoregon* s = new Smoregon(IID_SMOREGON, VIEW_WIDTH, randInt(0, VIEW_HEIGHT-1), 0, 1.5, 1, 5*(1+(getLevel()-1)*.1), 2.0, 250, this, 0);
            actorList.push_back(s);
            m_aliensOnScreen++;
        }
        else if(deter <= S){
            Snagglegon* s = new Snagglegon(IID_SNAGGLEGON, VIEW_WIDTH, randInt(0, VIEW_HEIGHT-1), 0, 1.5, 1, 10*(1+(getLevel()-1)*.1), 1.75, 1000, this, 1);
            actorList.push_back(s);
            m_aliensOnScreen++;
            // add snagglegon
        }
    }
    
    // Add a new star
    if(randInt(1, 15) == 1){
        int yLoc = randInt(0, VIEW_HEIGHT-1);
        actorList.push_back(new Star(IID_STAR, VIEW_WIDTH-1, yLoc, 0, ((double)randInt(5, 50))/100, 3));
    } //TODO: Make sure that rand(1,15) is [1,15] and not [1,14]
    
    if(m_shipsDestroyed >= 6+(4*getLevel())){
        m_shipsDestroyed = 0;
        m_aliensOnScreen = 0;
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    deleteDeadActors();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    std::vector<Actor*>::iterator it = actorList.begin();
    Actor* temp;
    if(nBlaster!=nullptr){
        delete nBlaster;
        nBlaster = nullptr;
    }
    while(it != actorList.end()){
        if(*it != nullptr){
            temp = *it;
            it = actorList.erase(it);
            delete temp;
        }
    }
    actorList.clear();
}

void StudentWorld::addToActorList(Actor* a){
    actorList.push_back(a);
}

void StudentWorld::alienKilled(){
    m_shipsDestroyed++;
    m_aliensOnScreen--;
}

void StudentWorld::alienLeftScreen(){
    m_aliensOnScreen--;
}

void StudentWorld::gotHealth(){
    nBlaster->setHP(nBlaster->getHP()+10);
    if(nBlaster->getHP()>50){
        nBlaster->setHP(50);
    }
}           

bool StudentWorld::addAnotherAlien(){ // not producing the correct number of ships
    int d = m_shipsDestroyed;
    int r = (6+(4*getLevel()))-d;
    int m = 4+(.5*getLevel());
    return min(m, r) > m_aliensOnScreen;
}

void StudentWorld::deleteDeadActors(){
    std::vector<Actor*>::iterator it = actorList.begin();
    while( it != actorList.end()){
        if(!(*it)->isAlive()){
            delete *it;
            actorList.erase(it);
        }
        else{
            it++;
        }
    }
}

Actor* StudentWorld::collidedWith(Actor* a){
    std::vector<Actor*>::iterator it = actorList.begin();
    //TODO: Check Nachen blaster collision here (not just actor list)
    if(nBlaster->isAlive()){
        if(distanceCalculate(a->getX(), a->getY(), (nBlaster)->getX(), (nBlaster)->getY()) < .75 * (a->getRadius() +(nBlaster)->getRadius())){
            return nBlaster;
        }
    }
    for(; it != actorList.end(); it++){
        if((*it)->isAlive() && (*it)->getImageID() != IID_STAR){
            if(distanceCalculate(a->getX(), a->getY(), (*it)->getX(), (*it)->getY()) < .75 * (a->getRadius() +(*it)->getRadius())){
                if((*it)->getImageID() != IID_STAR){
                    return *it;
                }
            }
        }
    }
    return nullptr;
}

bool StudentWorld::withinShootingDistance(Actor* shooter){
    return (nBlaster->getY()+4 >= shooter->getY() && nBlaster->getY()-4 <= shooter->getY() && nBlaster->getX() < shooter->getX());
}

double distanceCalculate(double x1, double y1, double x2, double y2)
{
    double x = x1 - x2;
    double y = y1 - y2;
    double dist;
    
    dist = pow(x,2)+pow(y,2);           //calculating distance by euclidean formula
    dist = sqrt(dist);                  //sqrt is function in math.h
    
    return dist;
}




