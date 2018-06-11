#include "Actor.h"
#include "StudentWorld.h"

#include <iostream>

//If a nBlaster crashes into the last ship on the level but that crash kills him does he die or pass the level?

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

///////////////////////////////
//          ACTOR            //
//////////////////////////////

class StudentWorld;

Actor::Actor() : GraphObject(0, 0, 0, 0, 0.0, 0){}

Actor::Actor(int imageID, int sX, int sY, int sDir, double size, int depth) : GraphObject(imageID, sX, sY, sDir, size, depth), m_imageID(imageID){}

Actor::~Actor(){}

void Actor::hasDied(){
    alive = false;
}

bool Actor::isAlive(){
    return alive;
}

int Actor::getImageID(){
    return m_imageID;
}

///////////////////////////////
//        EXPLOSION          //
//////////////////////////////

Explosion::Explosion(int sX, int sY) : Actor(IID_EXPLOSION, sX, sY, 0, 1, 0), m_numberOfTicks(0){}

void Explosion::doSomething(){
    m_numberOfTicks++;
    if(m_numberOfTicks > 4){
        hasDied();
        return;
    }
    setSize(getSize()*1.5);
}


///////////////////////////////
//          ALIEN           //
//////////////////////////////

Alien::Alien(int ID, int sX, int sY, int sDir, double size, int depth, int hp, StudentWorld* sWorld) : WorldExtender(ID, sX, sY, sDir, size, depth, sWorld), m_hp(hp){}


void Alien::setHP(int hp){
    m_hp = hp;
}

int Alien::getHP(){
    return m_hp;
}

void Alien::sufferDamage(int damageAmt){
    m_hp -= damageAmt;
    if(getHP() > 0){
        getWorld()->playSound(SOUND_BLAST);
    }
    else if(getHP() <= 0){
        if(getImageID() == IID_SMALLGON){
            getWorld()->alienKilled();
            hasDied();
            getWorld()->increaseScore(250);
            getWorld()->playSound(SOUND_DEATH);
            getWorld()->addToActorList(new Explosion(getX(), getY()));
        }
        else if(getImageID() == IID_SMOREGON){
            getWorld()->alienKilled();
            hasDied();
            getWorld()->increaseScore(250);
            getWorld()->playSound(SOUND_DEATH);
            getWorld()->addToActorList(new Explosion(getX(), getY()));
            if(randInt(1,2) == 1){
                if(randInt(1,2) == 1){
                    getWorld()->addToActorList(new TorpedoGoodie(getX(), getY(), getWorld()));
                }
                else{
                    getWorld()->addToActorList(new RepairGoodie(getX(), getY(), getWorld()));
                }
            }
        }
        else if(getImageID() == IID_SNAGGLEGON){
            getWorld()->alienKilled();
            hasDied();
            getWorld()->increaseScore(1000);
            getWorld()->playSound(SOUND_DEATH);
            getWorld()->addToActorList(new Explosion(getX(), getY()));
            if(randInt(1,6) == 1){
                getWorld()->addToActorList(new ExtraLifeGoodie(getX(), getY(), getWorld()));
            }
        }
        else if(getImageID() == IID_NACHENBLASTER){
            hasDied();
        }
    }
}



///////////////////////////////
//          ENEMIES          //
//////////////////////////////

Enemies::Enemies(int starID, int sX, int sY, int sDir, double size, int depth, int hp, double speed, int points, StudentWorld* sWorld, int tDir, int tDist) : Alien(starID, sX, sY, sDir, size, depth, hp, sWorld), m_moveSpeed(speed), m_points(points), m_travelDir(tDir), m_flightPath(tDist){}

void Enemies::setTravelDir(int dir){
    m_travelDir = dir;
}

int Enemies::getTravelDir(){
    return m_travelDir;
}

void Enemies::setFlightPath(int pathDist){
    m_flightPath = pathDist;
}

int Enemies::getFlightPath(){
    return m_flightPath;
}

void Enemies::flewOneUnit(){
    m_flightPath--;
}

void Enemies::setSpeed(int speed){
    m_moveSpeed = speed;
}

int Enemies::getSpeed(){
    return m_moveSpeed;
}


///////////////////////////////
//        WorldExtender        //
//////////////////////////////

WorldExtender::WorldExtender(int imageID, int sX, int sY, int sDir, double size, int depth, StudentWorld* sWorld) : Actor(imageID, sX, sY, sDir, size, depth), studentWorld(sWorld){}

StudentWorld* WorldExtender::getWorld(){
    return studentWorld;
}
//
//
//  /////////////////////////////
// //         GOODIES         //
///////////////////////////////
//
//Goodies::Goodies(int ID, int sX, int sY, StudentWorld* sWorld) : Actor(ID, sX, sY, 0, .5, 1), m_sWorld(sWorld) {}
//
//StudentWorld* Goodies::getWorld(){
//    return m_sWorld;
//}
//

///////////////////////////////
//           STAR            //
//////////////////////////////

Star::Star(int starID, int sX, int sY, int sDir, double size, int depth) : Actor(IID_STAR, sX, sY, 0, size, 3){} //TODO: Simplify this constructor to be simply default.

void Star::doSomething(){
    if(getX()+(2*getRadius()) > 0 && getX() < VIEW_WIDTH){
        moveTo(getX()-1, getY());
    }
    else{
        hasDied();
    }
}


///////////////////////////////
//      NACHENBLASTER       //
//////////////////////////////

NachenBlaster::NachenBlaster() : Alien(IID_NACHENBLASTER, 0, 128, 0, 1.0, 0, 0, nullptr), m_cabPower(30){}

NachenBlaster::NachenBlaster(int ID, int sX, int sY, int sDir, double size, int depth, int health, int cabPower, StudentWorld* sWorld) : Alien(ID, sX, sY, sDir, size, depth, health, sWorld), m_cabPower(cabPower){}

NachenBlaster::~NachenBlaster(){
}

void NachenBlaster::setCabPower(int pwr){
    m_cabPower = pwr;
}

int NachenBlaster::getCabPower(){
    return m_cabPower;
}

void NachenBlaster::incTorpedo(){
    m_numTorpedos+=5;
}

int NachenBlaster::getTorpedo(){
    return m_numTorpedos;
}

void NachenBlaster::doSomething(){
    int moveDir = 0;
    if(isAlive()){
        if(m_cabPower < 30){
            m_cabPower++;
        }
        if(getWorld()->getKey(moveDir)){
            switch(moveDir){
                case KEY_PRESS_LEFT:
                    if(getX()>6) moveTo(getX()-6, getY());
                    else moveTo(0, getY());
                    break;
                case KEY_PRESS_RIGHT:
                    if(getX()<VIEW_WIDTH-6) moveTo(getX()+6, getY());
                    else moveTo(VIEW_WIDTH-1, getY());
                    break;
                case KEY_PRESS_DOWN:
                    if(getY()>6) moveTo(getX(), getY()-6);
                    else moveTo(getX(), 0);
                    break;
                case KEY_PRESS_UP:
                    if(getY()<VIEW_HEIGHT-6) moveTo(getX(), getY()+6);
                    else moveTo(getX(), VIEW_HEIGHT-1);
                    break;
                case KEY_PRESS_TAB: // flatualance torpedo (pg. 24 part a)
                    if(m_numTorpedos > 0){
                        getWorld()->playSound(SOUND_TORPEDO);
                        getWorld()->addToActorList(new FlatulanceTorpedo(getX()+12, getY(), 180, getWorld(), true));
                        m_numTorpedos--;
                    }
                    break;
                case KEY_PRESS_SPACE: // cabbage (pg. 24 part a)
                    if(m_cabPower >= 5){
                        m_cabPower-=5;
                        getWorld()->playSound(SOUND_PLAYER_SHOOT);
                        getWorld()->addToActorList(new Cabbage(getX()+12, getY(), getWorld()));
                    }
                    break;
            }
        }
    }
}

///////////////////////////////
//         SMALLGON         //
//////////////////////////////


Smallgon::Smallgon(int ID, int sX, int sY, int sDir, double size, int depth, int hp, double speed, int points, StudentWorld* sWorld, int fpLength) : Enemies(ID, sX, sY, sDir, size, depth, hp, speed, points, sWorld, 0, fpLength){}

void Smallgon::doSomething(){
    if(isAlive()){ // Check if alive (#1)
        
        // Check boundries (#2)
        if(getX() < 0){
            getWorld()->alienLeftScreen();
            hasDied();
            return;
        }
        
        // Check collision (#3)
        if(getWorld()->collidedWith(this) != nullptr ){ // if the ship colides
            Actor* collidedActor = getWorld()->collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_NACHENBLASTER){ // collided with the nBlaster
                dynamic_cast<NachenBlaster*>(collidedActor)->sufferDamage(5);
                getWorld()->alienKilled();
                hasDied();
                getWorld()->increaseScore(250);
                getWorld()->playSound(SOUND_DEATH);
                getWorld()->addToActorList(new Explosion(getX(), getY()));
                return;
            }
            else if(collidedID == IID_CABBAGE){ // collided with cabbage
                sufferDamage(2);
            }
            else if(collidedID == IID_TORPEDO){ // collided with torpedo
                if(dynamic_cast<FlatulanceTorpedo*>(collidedActor)->firedByMe()){
                    sufferDamage(8);
                }
            }
        }
        
        
        // Determine the new flight path bc the old one has been exausted (#4)
        if(getY() >= VIEW_HEIGHT-1){ // (a)
            setTravelDir(-1);
            setFlightPath(randInt(1, 32)); // (d)
        }
        else if(getY() <= 0){ // (b)
            setTravelDir(1);
            setFlightPath(randInt(1, 32)); // (d)
        }
        else if(getFlightPath() == 0){ // (c)
            setTravelDir(randInt(-1, 1)); // TODO: Make sure this executes correctly
            setFlightPath(randInt(1, 32)); // (d)
        }
        
        // Fire if within distance
        if(getWorld()->withinShootingDistance(this)){
            int outOf = (20/(getWorld()->getLevel())+5); // CHANGE THIS TO DOUBLE TO REMOVE ERROR %
            if(randInt(1, outOf) == 1){
                Turnip* t = new Turnip(getX()-14, getY(), getWorld());
                getWorld()->addToActorList(t);
                getWorld()->playSound(SOUND_ALIEN_SHOOT);
                return;
            }
        }
        
        // Move the ship
        if(getTravelDir() == 1){
            moveTo(getX()-getSpeed(), getY()+getSpeed());
            flewOneUnit();
        }
        else if(getTravelDir() == 0){
            moveTo(getX()-getSpeed(), getY());
            flewOneUnit();
        }
        else if(getTravelDir() == -1){
            moveTo(getX()-getSpeed(), getY()-getSpeed());
            flewOneUnit();
        }
        
        
        //Check collision yet again
        if(getWorld()->collidedWith(this) != nullptr ){ // if the ship colides
            Actor* collidedActor = getWorld()->collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_NACHENBLASTER){ // collided with the nBlaster
                dynamic_cast<NachenBlaster*>(collidedActor)->sufferDamage(5);
                getWorld()->alienKilled();
                hasDied();
                getWorld()->increaseScore(250);
                getWorld()->playSound(SOUND_DEATH);
                getWorld()->addToActorList(new Explosion(getX(), getY()));
                return;
            }
            else if(collidedID == IID_CABBAGE){ // collided with cabbage
                sufferDamage(2);
            }
            else if(collidedID == IID_TORPEDO){ // collided with torpedo
                if(dynamic_cast<FlatulanceTorpedo*>(collidedActor)->firedByMe()){
                    sufferDamage(8);
                }
            }
        }
        
        //TODO: Check collision w/ nachenblaster again. Here at the end of doSomething()
    }
}

///////////////////////////////
//        SMOREGON          //
//////////////////////////////

Smoregon::Smoregon(int ID, int sX, int sY, int sDir, double size, int depth, int hp, double speed, int points, StudentWorld* sWorld, int fpLength) : Enemies(ID, sX, sY, sDir, size, depth, hp, speed, points, sWorld, 0, fpLength){}

void Smoregon::doSomething(){
    if(isAlive()){ // Check if alive (#1)
        
        // Check boundries (#2)
        if(getX() < 0){
            getWorld()->alienLeftScreen();
            hasDied();
            return;
        }
        
        // Check collision (#3)
        if(getWorld()->collidedWith(this) != nullptr ){ // if the ship colides
            Actor* collidedActor = getWorld()->collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_NACHENBLASTER){ // collided with the nBlaster
                dynamic_cast<NachenBlaster*>(collidedActor)->sufferDamage(5);
                getWorld()->alienKilled();
                hasDied();
                getWorld()->increaseScore(250);
                getWorld()->playSound(SOUND_DEATH);
                getWorld()->addToActorList(new Explosion(getX(), getY()));
                return;
            }
            else if(collidedID == IID_CABBAGE){ // collided with cabbage
                sufferDamage(2);
            }
            else if(collidedID == IID_TORPEDO){ // collided with torpedo
                if(dynamic_cast<FlatulanceTorpedo*>(collidedActor)->firedByMe()){
                    sufferDamage(8);
                }
            }
        }
        
        
        // Determine the new flight path bc the old one has been exausted (#4)
        if(getY() >= VIEW_HEIGHT-1){ // (a)
            setTravelDir(-1);
            setFlightPath(randInt(1, 32)); // (d)
        }
        else if(getY() <= 0){ // (b)
            setTravelDir(1);
            setFlightPath(randInt(1, 32)); // (d)
        }
        else if(getFlightPath() == 0){ // (c)
            setTravelDir(randInt(-1, 1)); // TODO: Make sure this executes correctly
            setFlightPath(randInt(1, 32)); // (d)
        }
        
        // Fire if within distance
        if(getWorld()->withinShootingDistance(this)){
            int outOf = (20/(getWorld()->getLevel())+5); // CHANGE THIS TO DOUBLE TO REMOVE ERROR %
            if(randInt(1, outOf) == 1){ // (a)
                Turnip* t = new Turnip(getX()-14, getY(), getWorld());
                getWorld()->addToActorList(t);
                getWorld()->playSound(SOUND_ALIEN_SHOOT);
                return;
            }
            else if(randInt(1, outOf) == 1){ // (b)
                setTravelDir(0);
                setFlightPath(VIEW_WIDTH);
                setSpeed(5);
            }
        }
        
        // Move the ship
        if(getTravelDir() == 1){
            moveTo(getX()-getSpeed(), getY()+getSpeed());
            flewOneUnit();
        }
        else if(getTravelDir() == 0){
            moveTo(getX()-getSpeed(), getY());
            flewOneUnit();
        }
        else if(getTravelDir() == -1){
            moveTo(getX()-getSpeed(), getY()-getSpeed());
            flewOneUnit();
        }
        
        //Check collision yet again
        if(getWorld()->collidedWith(this) != nullptr ){ // if the ship colides
            Actor* collidedActor = getWorld()->collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_NACHENBLASTER){ // collided with the nBlaster
                dynamic_cast<NachenBlaster*>(collidedActor)->sufferDamage(5);
                getWorld()->alienKilled();
                hasDied();
                getWorld()->increaseScore(250);
                getWorld()->playSound(SOUND_DEATH);
                getWorld()->addToActorList(new Explosion(getX(), getY()));
                return;
            }
            else if(collidedID == IID_CABBAGE){ // collided with cabbage
                sufferDamage(2);
            }
            else if(collidedID == IID_TORPEDO){ // collided with torpedo
                if(dynamic_cast<FlatulanceTorpedo*>(collidedActor)->firedByMe()){
                    sufferDamage(8);
                }
            }
        }
        
        //TODO: Check collision w/ nachenblaster again. Here at the end of doSomething()
    }
}


///////////////////////////////
//        Snagglegon         //
//////////////////////////////

Snagglegon::Snagglegon(int ID, int sX, int sY, int sDir, double size, int depth, int hp, double speed, int points, StudentWorld* sWorld, int fpLength) : Enemies(ID, sX, sY, sDir, size, depth, hp, speed, points, sWorld, -1, fpLength){}

void Snagglegon::doSomething(){
    if(isAlive()){ // Check if alive (#1)
        
        // Check boundries (#2)
        if(getX() < 0){
            getWorld()->alienLeftScreen();
            hasDied();
            return;
        }
        
        // Check collision (#3)
        if(getWorld()->collidedWith(this) != nullptr ){ // if the ship colides
            Actor* collidedActor = getWorld()->collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_NACHENBLASTER){ // collided with the nBlaster
                dynamic_cast<NachenBlaster*>(collidedActor)->sufferDamage(15);
                getWorld()->alienKilled();
                hasDied();
                getWorld()->increaseScore(250);
                getWorld()->playSound(SOUND_DEATH);
                getWorld()->addToActorList(new Explosion(getX(), getY()));
                return;
            }
            else if(collidedID == IID_CABBAGE){ // collided with cabbage
                sufferDamage(2);
            }
            else if(collidedID == IID_TORPEDO){ // collided with torpedo
                if(dynamic_cast<FlatulanceTorpedo*>(collidedActor)->firedByMe()){
                    sufferDamage(8);
                }
            }
        }
        
        
        // Determine the new flight path bc the old one has been exausted (#4)
        if(getY() >= VIEW_HEIGHT-1){ // (a)
            setTravelDir(-1);
        }
        else if(getY() <= 0){ // (b)
            setTravelDir(1);
        }
        
        // Fire if within distance
        if(getWorld()->withinShootingDistance(this)){
            int outOf = (15/(getWorld()->getLevel())+10); // TODO: CHANGE THIS TO DOUBLE TO REMOVE ERROR %
            if(randInt(1, outOf) == 1){
                FlatulanceTorpedo* t = new FlatulanceTorpedo(getX()-14, getY(), 180, getWorld(), false);
                getWorld()->addToActorList(t);
                getWorld()->playSound(SOUND_TORPEDO);
                return;
            }
        }
        
        // Move the ship
        if(getTravelDir() == 1){
            moveTo(getX()-getSpeed(), getY()+getSpeed());
        }
        else if(getTravelDir() == -1){
            moveTo(getX()-getSpeed(), getY()-getSpeed());
        }
        
        
        //Check collision yet again
        if(getWorld()->collidedWith(this) != nullptr ){ // if the ship colides
            Actor* collidedActor = getWorld()->collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_NACHENBLASTER){ // collided with the nBlaster
                dynamic_cast<NachenBlaster*>(collidedActor)->sufferDamage(15);
                getWorld()->alienKilled();
                hasDied();
                getWorld()->increaseScore(250);
                getWorld()->playSound(SOUND_DEATH);
                getWorld()->addToActorList(new Explosion(getX(), getY()));
                return;
            }
            else if(collidedID == IID_CABBAGE){ // collided with cabbage
                sufferDamage(2);
            }
            else if(collidedID == IID_TORPEDO){ // collided with torpedo
                if(dynamic_cast<FlatulanceTorpedo*>(collidedActor)->firedByMe()){
                    sufferDamage(8);
                }
            }
        }
    }
}



///////////////////////////////
//         CABBAGE          //
//////////////////////////////

Cabbage::Cabbage(int sX, int sY, StudentWorld* sWorld) : WorldExtender(IID_CABBAGE, sX, sY, 0, .5, 1, sWorld){}

void Cabbage::doSomething(){
    if(isAlive()){
        //Check the location of the cabbage
        if(getX() >= VIEW_WIDTH){
            hasDied();
            return;
        }
        
        //Check the collison again
        if(getWorld()->collidedWith(this) != nullptr ){
            Actor* collidedActor = getWorld()-> collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_SMALLGON || collidedID == IID_SMOREGON || collidedID == IID_SNAGGLEGON){ // the cabbage hits an enemy
                (dynamic_cast<Enemies*>(collidedActor))->sufferDamage(2);
                hasDied();
                return;
            }
        }
        
        // Move the cabbage
        moveTo(getX()+8, getY());
        setDirection(getDirection()+20);
        
        //Check collison again
        if(getWorld()->collidedWith(this) != nullptr ){
            Actor* collidedActor = getWorld()-> collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_SMALLGON || collidedID == IID_SMOREGON || collidedID == IID_SNAGGLEGON){
                (dynamic_cast<Enemies*>(collidedActor))->sufferDamage(2);
                hasDied();
                return;
            }
        }
    }
}


///////////////////////////////
//          TURNIP           //
//////////////////////////////

Turnip::Turnip(int sX, int sY, StudentWorld* sWorld) : WorldExtender(IID_TURNIP, sX, sY, 0, .5, 1, sWorld){}

void Turnip::doSomething(){
    if(isAlive()){
        if(getX()<0){
            hasDied();
            return;
        }
        
        //Check collision
        if(getWorld()->collidedWith(this) != nullptr ){
            Actor* collidedActor = getWorld()-> collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_NACHENBLASTER){
                (dynamic_cast<NachenBlaster*>(collidedActor))->sufferDamage(2);
                hasDied();
                return;
            }
        }
        
        //Move turnip
        moveTo(getX()-6, getY());
        setDirection(getDirection()+20);
        
        //Check collision yet again
        if(getWorld()->collidedWith(this) != nullptr ){
            Actor* collidedActor = getWorld()-> collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_NACHENBLASTER){
                (dynamic_cast<NachenBlaster*>(collidedActor))->sufferDamage(2);
                hasDied();
                return;
            }
        }
    }
}


///////////////////////////////
//     FLATUANCE TORPEDO    //
//////////////////////////////

FlatulanceTorpedo::FlatulanceTorpedo(int sX, int sY, int dir, StudentWorld* sWorld, bool firedByNBlaster) : WorldExtender(IID_TORPEDO, sX, sY, dir, .5, 1, sWorld), m_firedByNBlaster(firedByNBlaster){}

bool FlatulanceTorpedo::firedByMe(){
    return m_firedByNBlaster;
}

void FlatulanceTorpedo::doSomething(){
    if(isAlive()){
        if(getX() < 0 || getX() >= VIEW_WIDTH){
            hasDied();
            return;
        }
        // Check for collision
        if(getWorld()->collidedWith(this) != nullptr ){
            Actor* collidedActor = getWorld()-> collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(m_firedByNBlaster && (collidedID == IID_SMALLGON || collidedID == IID_SMOREGON || collidedID == IID_SNAGGLEGON)){
                (dynamic_cast<Enemies*>(collidedActor))->sufferDamage(8);
                hasDied();
                return;
            }
            else if(!m_firedByNBlaster && collidedID == IID_NACHENBLASTER){
                (dynamic_cast<NachenBlaster*>(collidedActor))->sufferDamage(8);
                hasDied();
                return;
            }
        }
        
        //Move the torpedo
        if(m_firedByNBlaster){
            moveTo(getX()+8, getY());
        }
        else{
            moveTo(getX()-8, getY());
        }
        
        // Check for collision
        if(getWorld()->collidedWith(this) != nullptr ){
            Actor* collidedActor = getWorld()-> collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(m_firedByNBlaster && (collidedID == IID_SMALLGON || collidedID == IID_SMOREGON || collidedID == IID_SNAGGLEGON)){
                (dynamic_cast<Enemies*>(collidedActor))->sufferDamage(8);
                hasDied();
                return;
            }
            else if(!m_firedByNBlaster && collidedID == IID_NACHENBLASTER){
                (dynamic_cast<NachenBlaster*>(collidedActor))->sufferDamage(8);
                hasDied();
                return;
            }
        }
    }
}

///////////////////////////////
//    EXTRA LIFE GOODIE     //
//////////////////////////////


ExtraLifeGoodie::ExtraLifeGoodie(int sX, int sY, StudentWorld* sWorld) : WorldExtender(IID_LIFE_GOODIE, sX, sY, 0, .5, 1, sWorld){}

void ExtraLifeGoodie::doSomething(){
    if(isAlive()){
        // Check to see if collided with anything
        if(getWorld()->collidedWith(this) != nullptr ){
            Actor* collidedActor = getWorld()-> collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_NACHENBLASTER){ // if collided with the nblaster
                getWorld()->increaseScore(100);
                hasDied();
                getWorld()->playSound(SOUND_GOODIE);
                getWorld()->incLives();
                return;
            }
        }
        
        // Move the goodie
        moveTo(getX()-.75, getY()-.75); // check this may need to change
        
        // Check to see if collided with anything yet again
        if(getWorld()->collidedWith(this) != nullptr ){
            Actor* collidedActor = getWorld()-> collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_NACHENBLASTER){ // if collided with the nblaster
                getWorld()->increaseScore(100);
                hasDied();
                getWorld()->playSound(SOUND_GOODIE);
                getWorld()->incLives();
                return;
            }
        }
    }
}


///////////////////////////////
//       REPAIR GOODIE      //
//////////////////////////////


RepairGoodie::RepairGoodie(int sX, int sY, StudentWorld* sWorld) : WorldExtender(IID_REPAIR_GOODIE, sX, sY, 0, .5, 1, sWorld) {}

void RepairGoodie::doSomething(){
    if(isAlive()){
        // Check to see if collided with anything
        if(getWorld()->collidedWith(this) != nullptr ){
            Actor* collidedActor = getWorld()-> collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_NACHENBLASTER){ // if collided with the nblaster
                getWorld()->increaseScore(100);
                hasDied();
                getWorld()->playSound(SOUND_GOODIE);
                getWorld()->gotHealth();
                return;
            }
        }
        
        // Move the goodie
        moveTo(getX()-.75, getY()-.75); // check this may need to change
        
        // Check to see if collided with anything yet again
        if(getWorld()->collidedWith(this) != nullptr ){
            Actor* collidedActor = getWorld()-> collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_NACHENBLASTER){ // if collided with the nblaster
                getWorld()->increaseScore(100);
                hasDied();
                getWorld()->playSound(SOUND_GOODIE);
                getWorld()->gotHealth();
                return;
            }
        }
    }
}


///////////////////////////////
//       TORPEDO GOODIE      //
//////////////////////////////


TorpedoGoodie::TorpedoGoodie(int sX, int sY, StudentWorld* sWorld) : WorldExtender(IID_TORPEDO_GOODIE, sX, sY, 0, .5, 1, sWorld) {}

void TorpedoGoodie::doSomething(){
    if(isAlive()){
        // Check to see if collided with anything
        if(getWorld()->collidedWith(this) != nullptr ){
            Actor* collidedActor = getWorld()->collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_NACHENBLASTER){ // if collided with the nblaster
                getWorld()->increaseScore(100);
                hasDied();
                getWorld()->playSound(SOUND_GOODIE);
                (dynamic_cast<NachenBlaster*>(collidedActor))->incTorpedo();
                return;
            }
        }
        
        // Move the goodie
        moveTo(getX()-.75, getY()-.75); // check this may need to change
        
        // Check to see if collided with anything yet again
        if(getWorld()->collidedWith(this) != nullptr ){
            Actor* collidedActor = getWorld()-> collidedWith(this);
            int collidedID = collidedActor->getImageID();
            if(collidedID == IID_NACHENBLASTER){ // if collided with the nblaster
                getWorld()->increaseScore(100);
                hasDied();
                getWorld()->playSound(SOUND_GOODIE);
                (dynamic_cast<NachenBlaster*>(collidedActor))->incTorpedo();
                return;
            }
        }
    }
}
