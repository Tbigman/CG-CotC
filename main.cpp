#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>   

#define DEBUG true
#define RELEASE false

#define PI 3.14159265

#define MAP_WIDTH 23
#define MAP_HEIGHT 21

enum EntityType { SHIP = 0, BARREL = 1, CANNONBALL = 2, MINE = 3};
enum MoveType { NONE = -1, MOVE = 0, FIRE = 1, MINING = 2, PORT = 3, STARBOARD = 4, FASTER = 5, SLOWER = 6, WAIT = 7};

using namespace std;

class Point;
class Hexagon;
class Entity;
class Ship;
class Barrel;
class Mine;
class Cannonball;
class Move;
class Solution;

Ship *ships[6];
Ship *myShips[3];
Ship *myShipsSimu[3];
Ship *opShips[3];

Barrel *barrels[26];
Mine *mines[100];
Cannonball *cannonballs[100];

Hexagon *map[23][21];

Solution *solution;
Solution *solutionFight;

int myShipCount;
int opShipCount;
int totalShipCount;
int barrelCount;
int mineCount;
int cannonballCount;
int turn = 1;
int DIRECTIONS_EVEN[6][2] =  { { 1, 0 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 } };
int DIRECTIONS_ODD[6][2] =  { { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, 0 }, { 0, 1 }, { 1, 1 } };

int getIndFromId(int id, bool generalInd = false);
void populateMap();
void getInputEntities();
void resetBeforeInputs();
void initDefault();
int mod(int number);

int mod(int number) 
{
    int result = ((((number + 1) % 6) + 6) % 6);
    if (result == 0)
        return 5;
    else
        return result - 1;
    return ((((number + 1) % 6) + 6) % 6) - 1;
}


class Move {
public:
  Move(void){};
  Move(MoveType ptype, int px, int py) : type(ptype), x(px), y(py){};

  void set(MoveType ptype, int px, int py, int pscore = 0, string pextraText = "")
  {
		type = ptype;
		score = pscore;
		switch(ptype)
		{
		  case MOVE: 
		  case FIRE: x = px; y = py;
		  default: extraText = pextraText;
			break;
		}
  }
  void output()
  {
	switch(type)
	{
	  case MOVE:      cout << "MOVE"; break;
	  case FIRE:      cout << "FIRE"; break;
	  case MINING:    cout << "MINE"; break;
	  case PORT:      cout << "PORT"; break;
	  case STARBOARD: cout << "STARBOARD"; break;
	  case FASTER:    cout << "FASTER"; break;
	  case SLOWER:    cout << "SLOWER"; break;
	  case WAIT:      cout << "WAIT"; break;
	  case NONE:      cout << "WAIT"; break;
	}
	if(type == MOVE || type == FIRE) cout << " " << x << " " << y;
	if(!extraText.empty()) cout << " " << extraText;
	
	cout << endl;
  }     

  MoveType type;
  int x;
  int y;
  string extraText;
  int score;
};

class Solution {
public:
  Solution(void){};
  Solution(Move moves[]);

  Move *moves[3];
    
  void output()
  {
	for(int i = 0; i < 3; ++i)
	{		
		switch(moves[i]->type)
		{
		  case MOVE:      cout << "MOVE"; break;
		  case FIRE:      cout << "FIRE"; break;
		  case MINING:    cout << "MINE"; break;
		  case PORT:      cout << "PORT"; break;
		  case STARBOARD: cout << "STARBOARD"; break;
		  case FASTER:    cout << "FASTER"; break;
		  case SLOWER:    cout << "SLOWER"; break;
		  case WAIT:      cout << "WAIT"; break;
		}
		if(moves[i]->type == MOVE || moves[i]->type == FIRE) cout << " " << moves[i]->x << " " << moves[i]->y;
		if(!moves[i]->extraText.empty()) cout << " " << moves[i]->extraText;
		
		cout << endl;
	}		
  }     
};


class Point {

public:
  Point(){};
  Point(int px, int py) : x(px), y(py){}
  Point(const Point &ppoint)
  {
    this->x = ppoint.x;
    this->y = ppoint.y;
  }

  int x;
  int y;

  bool operator==(const Point &a)
  {
    return (this->x == a.x && this->y == a.y);
  }
  bool operator!=(const Point &a)
  {
    return !(*this == a);
  }
  int dist2(Point p) {
    return (this->x - p.x)*(this->x - p.x) + (this->y - p.y)*(this->y - p.y);
  }
  int dist2(int px, int py) {
    return (this->x - px)*(this->x - px) + (this->y - py)*(this->y - py);
  }

  int dist(Point p) {
    return sqrt(this->dist2(p));
  }
  int dist(int px, int py) {
    return sqrt(this->dist2(px, py));
  }
  void set(Point p)
  {
    this->x = p.x;
    this->y = p.y;
  }
  void set(int px, int py)
  {
    this->x = px;
    this->y = py;
  }               
  /*int distanceTo(CubeCoordinate dst) {
            return (abs(x - dst.x) +abs(y - dst.y) + abs(z - dst.z)) / 2;
        } */   
  double angle(Point targetPosition) 
  {
  	double dy = (targetPosition.y - this->y) * sqrt(3) / 2;
  	double dx = targetPosition.x - this->x + ((this->y - targetPosition.y) & 1) * 0.5;
  	double angle = -atan2(dy, dx) * 3 / PI;
  	if (angle < 0) {
  		angle += 6;
  	} else if (angle >= 6) {
  		angle -= 6;
  	}
  	return angle;
  }     
  
	bool isInsideMap() {
		return x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT;
	}
  Point neighbor(int orientation) 
  {
  	int newY, newX;
  	if (this->y % 2 == 1) {
  		newY = this->y + DIRECTIONS_ODD[orientation][1];
  		newX = this->x + DIRECTIONS_ODD[orientation][0];
  	} else {
  		newY = this->y + DIRECTIONS_EVEN[orientation][1];
  		newX = this->x + DIRECTIONS_EVEN[orientation][0];
  	}
	return Point(newX, newY);
  }
};

class Hexagon : public Point {
public:    
  Hexagon(void){};
  Hexagon(int x, int y): Point(x,y){};

  ~Hexagon(void){};
  
  int dist(Hexagon &p) {
  	return Point::dist(Point(p.x, p.y));
  }  
  int dist2(Hexagon &p) {
  	return Point::dist2(Point(p.x, p.y));
  }
   
  bool isBarrel = false;
  bool isMined = false;
  bool isTargeted = false;
  int cannonTurns = 0;
  
  Ship *currentShip;
  Mine *currentMine;
  Barrel *currentBarrel;
};

class Entity : public Point {
public:    
  Entity(void){};
  Entity(int pid, int x, int y, EntityType ptype = SHIP): Point(x,y), id(pid), type(ptype){};


  ~Entity(void){};
  
  int dist(Entity &p) {
  	return Point::dist(Point(p.x, p.y));
  }  
  int dist2(Entity &p) {
  	return Point::dist2(Point(p.x, p.y));
  }
  int dist(Point &p) {
  	return Point::dist(p);
  }  
  int dist2(Point &p) {
  	return Point::dist2(p);
  }
  
  virtual void updateWithInputs(int pid, int px, int py, EntityType ptype = SHIP)
  {
    id = pid;
    x = px;
    y = py;
	type = ptype;
  }

  int id;
  EntityType type;
};

class Barrel : public Entity {
public:
  Barrel(void){};
  Barrel(int pid, int px, int py, int pamount): 
    Entity(pid, px, py, BARREL), amount(pamount){};

  ~Barrel(void){};

  void updateWithInputs(int pid, int px, int py, int pamount)
  {
    Entity::updateWithInputs(pid, px, py, BARREL);
	amount = pamount;
  }

  void print()
  {
    cerr << "Barrel id: " << id << " Coord:" << x << "," << y << endl;
  }
  
  void reset()
  {
  }
  int amount;
};

class Mine : public Entity {
public:
  Mine(void){};
  Mine(int pid, int px, int py): 
    Entity(pid, px, py, MINE){};

  ~Mine(void){};

  void updateWithInputs(int pid, int px, int py)
  {
    Entity::updateWithInputs(pid, px, py, MINE);
  }

  void print()
  {
    cerr << "Mine id: " << id << " Coord:" << x << "," << y << endl;
  }
  
  void reset()
  {
  }
};

class Cannonball : public Entity {
public:
  Cannonball(void){};
  Cannonball(int pid, int px, int py, int psource, int pturns): 
    Entity(pid, px, py, CANNONBALL), source(psource), turns(pturns){};

  ~Cannonball(void){};

  void updateWithInputs(int pid, int px, int py, int psource, int pturns)
  {
    Entity::updateWithInputs(pid, px, py, CANNONBALL);
	source = psource;
	turns = pturns;
  }

  void print()
  {
    cerr << "Cannonball id: " << id << " Coord:" << x << "," << y << endl;
  }
  
  void reset()
  {
  }
  int source;
  int turns;
};


class Ship : public Entity {
public:
  Ship(void){};
  Ship(int pid, int px, int py, int protation, int pspeed, int pstock, int powner): 
    Entity(pid, px, py, SHIP), rotation(protation), speed(pspeed), stock(pstock), owner(powner){};

  ~Ship(void){};

  void updateWithInputs(int pid, int px, int py, int protation, int pspeed, int pstock, int powner)
  {
    Entity::updateWithInputs(pid, px, py, SHIP);
	rotation = protation;
	speed = pspeed;
	stock = pstock;
	owner = powner;
	front.set(px,py);
	back.set(px,py);
	center.set(px,py);
	switch(rotation)
	{
		case 0: front.x += 1;
				back.x -= 1;
			break;
		case 1: front.x += (py%2 == 0) ? 0 : 1;
				front.y -= 1;
				back.x -= (py%2 == 0) ? 1 : 0;
				back.y += 1;
			break;
		case 2: front.x -= (py%2 == 0) ? 1 : 0;
				front.y -= 1;
				back.x += (py%2 == 0) ? 0 : 1;
				back.y += 1;
			break;
		case 3: front.x -= 1;
				back.x += 1;
			break;
		case 4: front.x -= (py%2 == 0) ? 1 : 0;
				front.y += 1;
				back.x += (py%2 == 0) ? 0 : 1;
				back.y -= 1;
			break;
		case 5: front.x += (py%2 == 0) ? 0 : 1;
				front.y += 1;
				back.x -= (py%2 == 0) ? 1 : 0;
				back.y -= 1;
			break;
	}
  }
  
  void copy(Ship otherShip)
  {	  
	  x        = otherShip.x   ;
	  y        = otherShip.y   ;
	  front    = otherShip.front   ;
	  back     = otherShip.back    ;
	  center   = otherShip.center  ;
	  owner    = otherShip.owner   ;
	  rotation = otherShip.rotation;
	  speed    = otherShip.speed   ;
	  stock    = otherShip.stock   ;
	  canFire  = otherShip.canFire ;
  }
  
  
  void updatePosition(MoveType pmove)
  {
	// cerr << "la1 ";  print();
	Point currentPosition(this->x, this->y);
	Point nextP = currentPosition;
	if(pmove == SLOWER) --speed;
	if(pmove == FASTER) ++speed;
	if(speed < 0) speed = 0;
	if(speed > 2) speed = 2;
	switch(speed)
	{
		case 0: nextP = currentPosition;
		break;
		case 1: nextP = currentPosition.neighbor(rotation);
		break;
		case 2: nextP = currentPosition.neighbor(rotation);	
		        nextP = nextP.neighbor(rotation);	
		break;
	}	
	front = nextP.neighbor(mod(rotation));
	back = nextP.neighbor(mod(rotation+3));
					
	x = nextP.x;
	y = nextP.y;
	 cerr << "updatePosition to ";  print();
  }
  
  void updateRotation(MoveType movetype)
  {
	Point currentPosition(this->x, this->y);
	switch(movetype)
	{
		case PORT: front = currentPosition.neighbor(mod(rotation+1));
					back = currentPosition.neighbor(mod(rotation+4));
					rotation = mod(rotation+1);
		break;
		case STARBOARD: front = currentPosition.neighbor(mod(rotation-1));
						back = currentPosition.neighbor(mod(rotation-4));
						rotation = mod(rotation-1);
		break;
		case SLOWER:front = currentPosition.neighbor(rotation);
					back = currentPosition.neighbor(mod(rotation+3));
		break;
		case FASTER:front = currentPosition.neighbor(rotation);
					back = currentPosition.neighbor(mod(rotation+3));
		break;		
		case FIRE:	
		case NONE:	front = currentPosition.neighbor(rotation);
					back = currentPosition.neighbor(mod(rotation+3));
		break;		
	}	
  }
    
  bool dodgeBorders(MoveType &dodgeMove)
  {	  
	bool moveChanged = false;
	bool badMove = false;
	updatePosition(dodgeMove);
	if(!isInsideMap()) return moveChanged;
	updateRotation(dodgeMove);	
	Point nextPoint = front.neighbor(rotation);
	if(!nextPoint.isInsideMap()) badMove = true;
	if(badMove)
	{
		badMove = false;
		// Try Slower
		if(speed == 0) badMove = true;
		copy(*myShips[getIndFromId(id)]);;
		updatePosition(SLOWER);	
		updateRotation(SLOWER);	
		Point nextPoint = front.neighbor(rotation);
		if(!nextPoint.isInsideMap()) badMove = true;
		// if Slower move in a mine
		if(badMove)
		{
			badMove = false;
			// Try Faster
			if(speed == 2) badMove = true;
			copy(*myShips[getIndFromId(id)]);;
			updatePosition(FASTER);
			updateRotation(FASTER);	
			Point nextPoint = front.neighbor(rotation);
			if(!nextPoint.isInsideMap()) badMove = true;
			// if Faster move in a mine
			if(badMove)
			{
				badMove = false;
				// Try Left
				copy(*myShips[getIndFromId(id)]);;
				updatePosition(PORT);	
				updateRotation(PORT);	
				Point nextPoint = front.neighbor(rotation);
				if(!nextPoint.isInsideMap()) badMove = true;
				// if Left move in a mine
				if(badMove)
				{
					badMove = false;
					// Try Right
					copy(*myShips[getIndFromId(id)]);;
					updatePosition(STARBOARD);	
					updateRotation(STARBOARD);	
					Point nextPoint = front.neighbor(rotation);
					if(!nextPoint.isInsideMap()) badMove = true;
					// if Right move in a mine
					if(badMove)
					{				
						moveChanged = true;
						badMove = false;
						// Try Straight
						copy(*myShips[getIndFromId(id)]);;
						updatePosition(NONE);	
						updateRotation(NONE);	
						Point nextPoint = front.neighbor(rotation);
						if(!nextPoint.isInsideMap()) badMove = true;
						// if Straight move in a mine
						if(badMove)
						{
							 moveChanged = false;
							 cerr << "moveChanged borders FALSE " << endl;
						}else dodgeMove = NONE;		
					}else dodgeMove = STARBOARD;
				}else dodgeMove = PORT;	
			}else dodgeMove = FASTER;
		}else dodgeMove = SLOWER;	
	}	
	return moveChanged;
  }
  bool dodgeMineAndCannon(MoveType &dodgeMove, bool onlyCenter = false)
  {	  
	bool moveChanged = false;
	bool badMove = false;
	
	// Look what happen if we do FIRE
	// if something bad happen, set moveChanged to true because we must not do FIRE move this turn
	updatePosition(FIRE);	
	if(intoMine()) moveChanged = true;
	updateRotation(FIRE);	
	if(intoMine() || intoCannon(onlyCenter)) moveChanged = true;		
	Point nextPoint = front.neighbor(rotation);
	if(nextPoint.isInsideMap() && map[nextPoint.x][nextPoint.y]->isMined) moveChanged = true;
	
	copy(*myShips[getIndFromId(id)]);;
	updatePosition(dodgeMove);
	if(!isInsideMap()) return moveChanged;
	if(intoMine()) badMove = true;
	updateRotation(dodgeMove);	
	if(intoMine() || intoCannon(onlyCenter)) badMove = true;
	nextPoint = front.neighbor(rotation);
	if(nextPoint.isInsideMap() && map[nextPoint.x][nextPoint.y]->isMined) badMove = true;
	if(badMove)
	{
		moveChanged = true;
		badMove = false;
		// Try Slower
		if(speed == 0) badMove = true;
		copy(*myShips[getIndFromId(id)]);;
		updatePosition(SLOWER);	
		if(intoMine()) badMove = true;
		updateRotation(SLOWER);	
		if(intoMine() || intoCannon(onlyCenter)) badMove = true;
		Point nextPoint = front.neighbor(rotation);
		if(nextPoint.isInsideMap() && map[nextPoint.x][nextPoint.y]->isMined) badMove = true;
		// if Slower move in a mine
		if(badMove)
		{
			badMove = false;
			// Try Faster
			if(speed == 2) badMove = true;
			copy(*myShips[getIndFromId(id)]);;
			updatePosition(FASTER);
			if(intoMine()) badMove = true;
			updateRotation(FASTER);	
			if(intoMine() || intoCannon(onlyCenter)) badMove = true;
			Point nextPoint = front.neighbor(rotation);
			if(nextPoint.isInsideMap() && map[nextPoint.x][nextPoint.y]->isMined) badMove = true;
			// if Faster move in a mine
			if(badMove)
			{
				badMove = false;
				// Try Left
				copy(*myShips[getIndFromId(id)]);;
				updatePosition(PORT);	
				if(intoMine()) badMove = true;
				updateRotation(PORT);	
				if(intoMine() || intoCannon(onlyCenter)) badMove = true;
				Point nextPoint = front.neighbor(rotation);
				if(nextPoint.isInsideMap() && map[nextPoint.x][nextPoint.y]->isMined) badMove = true;
				// if Left move in a mine
				if(badMove)
				{
					badMove = false;
					// Try Right
					copy(*myShips[getIndFromId(id)]);;
					updatePosition(STARBOARD);	
					if(intoMine()) badMove = true;
					updateRotation(STARBOARD);	
					if(intoMine() || intoCannon(onlyCenter)) badMove = true;
					Point nextPoint = front.neighbor(rotation);
					if(nextPoint.isInsideMap() && map[nextPoint.x][nextPoint.y]->isMined) badMove = true;
					// if Right move in a mine
					if(badMove)
					{				
						badMove = false;
						// Try Straight
						copy(*myShips[getIndFromId(id)]);;
						updatePosition(NONE);	
						if(intoMine()) badMove = true;
						updateRotation(NONE);	
						if(intoMine() || intoCannon(onlyCenter)) badMove = true;		
						Point nextPoint = front.neighbor(rotation);
						if(nextPoint.isInsideMap() && map[nextPoint.x][nextPoint.y]->isMined) badMove = true;
						// if Straight move in a mine
						if(badMove)
						{
							 moveChanged = false;
							 cerr << "moveChanged FALSE " << endl;
						}else dodgeMove = NONE;		
					}else dodgeMove = STARBOARD;
				}else dodgeMove = PORT;	
			}else dodgeMove = FASTER;
		}else dodgeMove = SLOWER;	
	}	
	return moveChanged;
  }
  void print()
  {
    switch(owner)
    {
      case 1: cerr << "myShip ";  break;
      case 0: cerr << "opShip "; break;
    }
    cerr << id << " Coord:" << x << "," << y << " Direction: " << rotation << " Speed: " << speed << endl;
  }
  
  void reset()
  {
  }

  int getClosestBarrel()
  {
    int bestDist = 100;
    int bestId = -1;
    for(int bi = 0; bi < barrelCount; ++bi)
    {
      if(dist(*barrels[bi]) < bestDist && barrels[bi]->amount > 0)
      {
        bestDist = dist(*barrels[bi]);
        bestId = barrels[bi]->id;
      }
    }
    return bestId;
  }
  int getClosestShip(int owner = 1)
  {
    int bestDist = 100;
    int bestId = -1;
	if(owner == 1)
	{
		for(int si = 0; si < myShipCount; ++si)
		{
		  if(dist(*myShips[si]) < bestDist)
		  {
			bestDist = dist(*myShips[si]);
			bestId = myShips[si]->id;
		  }
		}
	}
	else
	{
		for(int si = 0; si < opShipCount; ++si)
		{
		  if(dist(*opShips[si]) < bestDist)
		  {
			bestDist = dist(*opShips[si]);
			bestId = opShips[si]->id;
		  }
		}
	}
    return bestId;
  }
  
	Point forwardPoint(int pspeed = 0, int protation = -1)
	{
		if(protation == -1) protation = rotation;
		Point p(x,y);
		while(pspeed > 0)
		{
			p = p.neighbor(rotation);
			
			if(p.x > 22) p.x = 22;
			if(p.x < 0) p.x = 0;
			if(p.y > 20) p.y = 20;
			if(p.y < 0) p.y = 0;
			--pspeed;			
		}

		return p;
	}
	Point forwardPoint2(int speed = 2, int protation = -1)
	{
		if(protation == -1) protation = rotation;
		
		Point currentPosition(this->front.x, this->front.y);
		Point nextP = currentPosition.neighbor(rotation);
		
		if(nextP.x > 22){ nextP.x = 22; ++nextP.y; }
		if(nextP.x < 0) { nextP.x = 0;  ++nextP.y; }
		if(nextP.y > 20){ nextP.y = 20; ++nextP.x; }
		if(nextP.y < 0) { nextP.y = 0;  ++nextP.x; }
		
		return nextP;
	}
	  
	bool intoMine()
	{
		bool intoMine = false;
		print();
		cerr << "front " << front.x << "," << front.y << " back  " << back.x << "," << back.y << endl;
		if(isInsideMap() && map[x][y]->isMined) intoMine = true;
		if(front.isInsideMap() && map[front.x][front.y]->isMined) intoMine = true;
		if(back.isInsideMap() && map[back.x][back.y]->isMined) intoMine = true;
		
		//cerr << "intoMine " << intoMine << endl;
		//Point frontNextMove = front.neighbor(rotation);
		//cerr << "frontNextMove" << frontNextMove.x << "," << frontNextMove.y << endl;
		//if(frontNextMove.isInsideMap() && map[frontNextMove.x][frontNextMove.y]->isMined) intoMine = true;		
		return intoMine;
	}
	bool intoCannon(bool onlyCenter = false)
	{
		bool intoCannon = false;
		if(isInsideMap() && map[x][y]->isTargeted && map[x][y]->cannonTurns == 1) intoCannon = true;
		if(!onlyCenter)
		{
			if(front.isInsideMap() && map[front.x][front.y]->isTargeted && map[front.x][front.y]->cannonTurns == 1) intoCannon = true;
			if(back.isInsideMap() && map[back.x][back.y]->isTargeted && map[back.x][back.y]->cannonTurns == 1) intoCannon = true;
		}
		return intoCannon;
	}
	  
	  
	MoveType nextMoveTo(Point targetPosition) 
	{
		Point currentPosition(this->x, this->y);

		MoveType nextMove;
		if (currentPosition == targetPosition) {
			return SLOWER;
		}

		double targetAngle, angleStraight, anglePort, angleStarboard, centerAngle, anglePortCenter, angleStarboardCenter;

		switch (speed) {
		case 2:
			//nextMove = SLOWER;
			//break;
		case 1:
		{
			// Suppose we've moved first
			currentPosition = currentPosition.neighbor(rotation);
			// We move out of the map
			if (!currentPosition.isInsideMap()) {
				nextMove = SLOWER;
				break;
			}

			// Target reached at next turn
			if (currentPosition == targetPosition) {
				nextMove = NONE;
				break;
			}

			// For each neighbor cell, find the closest to target
			targetAngle = currentPosition.angle(targetPosition);
			angleStraight = min(abs(rotation - targetAngle), 6 - abs(rotation - targetAngle));
			anglePort = min(abs((rotation + 1) - targetAngle), abs((rotation - 5) - targetAngle));
			angleStarboard = min(abs((rotation + 5) - targetAngle), abs((rotation - 1) - targetAngle));

			centerAngle = currentPosition.angle(Point(MAP_WIDTH / 2, MAP_HEIGHT / 2));
			anglePortCenter = min(abs((rotation + 1) - centerAngle), abs((rotation - 5) - centerAngle));
			angleStarboardCenter = min(abs((rotation + 5) - centerAngle), abs((rotation - 1) - centerAngle));

			// Next to target with bad angle, slow down then rotate (avoid to turn around the target!)
			if (currentPosition.dist(targetPosition) == 1 && angleStraight > 1.5) {
				nextMove = SLOWER;
				break;
			}

			int distanceMin = 0;

			Point nextPosition = currentPosition.neighbor(rotation);
			Point nextPositionR = nextPosition.neighbor(mod(rotation-1));
			Point nextPositionL = nextPosition.neighbor(mod(rotation+1));
			Point nextPositionFront = nextPosition.neighbor(rotation);
			Point nextPositionFront2 = nextPositionFront.neighbor(rotation);
			Point nextPositionFront3 = nextPositionFront2.neighbor(rotation);
					cerr << "currentPosition" << currentPosition.x << "," << currentPosition.y << endl;	
					cerr << "nextPosition" << nextPosition.x << "," << nextPosition.y << endl;	
					cerr << "nextPositionFront" << nextPositionFront.x << "," << nextPositionFront.y << endl;	
					cerr << "nextPositionFront2" << nextPositionFront2.x << "," << nextPositionFront2.y << endl;	
					cerr << "nextPositionFront3" << nextPositionFront3.x << "," << nextPositionFront3.y << endl;	
			bool fastCheck = true;
			if(!nextPositionFront2.isInsideMap()) fastCheck = false;
			if(nextPositionFront2.isInsideMap() && map[nextPositionFront2.x][nextPositionFront2.y]->isMined) fastCheck = false;			
			if(!nextPositionFront3.isInsideMap()) fastCheck = false;
			if(nextPositionFront3.isInsideMap() && map[nextPositionFront3.x][nextPositionFront3.y]->isMined) fastCheck = false;				

			// Test forward faster		
			if (nextPositionR.isInsideMap() && !map[nextPositionR.x][nextPositionR.y]->isMined && 
				nextPositionL.isInsideMap() && !map[nextPositionL.x][nextPositionL.y]->isMined && 
				nextPositionFront.isInsideMap() && !map[nextPositionFront.x][nextPositionFront.y]->isMined && 
				fastCheck && 
				currentPosition.dist(targetPosition) > 3 && angleStraight <= 1.5) {
				nextMove = FASTER;
				break;
			}
			if(speed == 2) // if we go faster, just check further
			{	
				if(!nextPositionFront.isInsideMap()) fastCheck = false;
				if(nextPositionFront.isInsideMap() && map[nextPositionFront.x][nextPositionFront.y]->isMined) fastCheck = false;	
				
			}
			// Test forward

			if (nextPosition.isInsideMap() && !map[nextPosition.x][nextPosition.y]->isMined && 
				fastCheck) {
				distanceMin = nextPosition.dist(targetPosition);
				nextMove = NONE;
			}
			//if(front.dist(*barrels[closestBarrelInd]) < back.dist(*barrels[closestBarrelInd]))


			// Test port
			nextPosition = currentPosition.neighbor((rotation + 1) % 6);
			if (nextPosition.isInsideMap()) {
				int distance = nextPosition.dist(targetPosition);
				if (distanceMin == NONE || distance < distanceMin || distance == distanceMin && anglePort < angleStraight - 0.5) {
					distanceMin = distance;
					nextMove = PORT;
				}
			}

			// Test starboard
			nextPosition = currentPosition.neighbor((rotation + 5) % 6);
			if (nextPosition.isInsideMap()) {
				int distance = nextPosition.dist(targetPosition);
				if (distanceMin == NONE || distance < distanceMin
						|| (distance == distanceMin && angleStarboard < anglePort - 0.5 && nextMove == PORT)
						|| (distance == distanceMin && angleStarboard < angleStraight - 0.5 && nextMove == NONE)
						|| (distance == distanceMin && nextMove == PORT && angleStarboard == anglePort
								&& angleStarboardCenter < anglePortCenter)
						|| (distance == distanceMin && nextMove == PORT && angleStarboard == anglePort
								&& angleStarboardCenter == anglePortCenter && (rotation == 1 || rotation == 4))) {
					distanceMin = distance;
					nextMove = STARBOARD;
				}
			}
			break;
		}
		case 0:
			// Rotate ship towards target
			targetAngle = currentPosition.angle(targetPosition);
			angleStraight = min(abs(rotation - targetAngle), 6 - abs(rotation - targetAngle));
			anglePort = min(abs((rotation + 1) - targetAngle), abs((rotation - 5) - targetAngle));
			angleStarboard = min(abs((rotation + 5) - targetAngle), abs((rotation - 1) - targetAngle));

			centerAngle = currentPosition.angle(Point(MAP_WIDTH / 2, MAP_HEIGHT / 2));
			anglePortCenter = min(abs((rotation + 1) - centerAngle), abs((rotation - 5) - centerAngle));
			angleStarboardCenter = min(abs((rotation + 5) - centerAngle), abs((rotation - 1) - centerAngle));

			Point forwardPosition = currentPosition.neighbor(rotation);

			nextMove = NONE;

			if (anglePort <= angleStarboard) {
				nextMove = PORT;
			}

			if (angleStarboard < anglePort || angleStarboard == anglePort && angleStarboardCenter < anglePortCenter
					|| angleStarboard == anglePort && angleStarboardCenter == anglePortCenter && (rotation == 1 || rotation == 4)) {
				nextMove = STARBOARD;
			}

			if (forwardPosition.isInsideMap() && angleStraight <= anglePort && angleStraight <= angleStarboard) {
				nextMove = FASTER;
			}
			break;
		}
		return nextMove;
	}
		
	Point nextPoint(MoveType m) 
	{
		Point currentPosition(this->x, this->y);
		Point nextP;
		switch(m)
		{
			case PORT: nextP = currentPosition.neighbor(mod(rotation+1));
			break;
			case STARBOARD: nextP = currentPosition.neighbor(mod(rotation-1));
			break;
			case SLOWER: nextP = currentPosition;
			break;
			case FASTER: nextP = currentPosition.neighbor(rotation);
						 nextP = nextP.neighbor(rotation);
			break;			
		}
		return nextP;
	}
	
	/*void simulate(Move &simuMove)
	{
		
		updatePosition(simuMove.type);
		if(!isInsideMap()) return;
		if(front.intoCannon(onlyCenter) || back.intoCannon(onlyCenter)) stock -= 25;
		if(center.intoCannon(onlyCenter)) stock -= 50;
		
		updateRotation(simuMove.type);	
		
	}*/
	
  Point front;
  Point back;
  Point center;
  int owner;
  int rotation;
  int speed;
  int stock;
  bool canFire;
};



/**
 * GENERAL FUNCTIONS 
**/
void initDefault()
{
  for(int i=0; i < 3; ++i)
  {
    myShips[i] = new Ship(i,0,0,0,0,0,0);
    myShipsSimu[i] = new Ship(i,0,0,0,0,0,0);
    opShips[i] = new Ship(i,0,0,0,0,0,0);
  }
  for(int i=0; i < 6; ++i)
  {
    ships[i] = new Ship(i,0,0,0,0,0,0);
  }
  for(int i=0; i<26; ++i)
  {
    barrels[i] = new Barrel(i,0,0,0);
  }
  for(int i=0; i<100; ++i)
  {
	cannonballs[i] = new Cannonball(i,0,0,0,0);
    mines[i] = new Mine(i,0,0);
  }  
  
  for(int i=0; i<23; ++i)
  {
	  for(int k=0; k<21; ++k)
	  {
		map[i][k] = new Hexagon(0,0);
	  }  
  }  
  solution = new Solution();  
  for(int i=0; i<3; ++i)
  {
	solution->moves[i] = new Move(NONE,0,0);
  }  
  solutionFight = new Solution();  
  for(int i=0; i<3; ++i)
  {
	solutionFight->moves[i] = new Move(NONE,0,0);
  }  
  
  //pool = new Solution();
  //poolSimu = new Solution();
  //for(int i=0; i<MAX_MOVESETS; ++i)
  //{
  //  for(int j=0; j<15; ++j)
  //  {
  //    solution->moveSets[i].moves[j].set(MOVE, 0);
  //    pool->moveSets[i].moves[j].set(MOVE, 0);
  //    poolSimu->moveSets[i].moves[j].set(MOVE, 0);
  //  }
  //}
}
void resetBeforeInputs()
{
  for(int i=0; i < 3; ++i)
  {
    myShips[i]->reset();
    myShipsSimu[i]->reset();
    opShips[i]->reset();
  }
  for(int i=0; i < 6; ++i)
  {
    ships[i]->reset();
  }
  for(int i=0; i < 26; ++i)
  {
    barrels[i]->reset();
  }
  for(int i=0; i<100; ++i)
  {
	cannonballs[i]->reset();
    mines[i]->reset();
  }  

  for(int i=0; i<23; ++i)
  {
	  for(int k=0; k<21; ++k)
	  {
		map[i][k]->isBarrel = false;
		map[i][k]->isMined = false;
		map[i][k]->isTargeted = false;
		map[i][k]->currentShip = NULL;
		map[i][k]->currentBarrel = NULL;
		map[i][k]->currentMine = NULL;
		map[i][k]->cannonTurns = 0;
	  }  
  }  
  //pool->reset();
  //poolSimu->reset();
  //solution->reset();
  //solC = 0;
  //finalSolC = 0;
  //debugC = 0;
}
void getInputEntities()
{
	int entityCount; // the number of entities (e.g. ships, mines or cannonballs)
	cin >> myShipCount; cin.ignore();
	cin >> entityCount; cin.ignore();
	int entityId, x, y, arg1, arg2, stockArg, ownerArg;
	string entityType;
	EntityType entityTypeEnum;

	myShipCount = 0; opShipCount = 0; barrelCount = 0; totalShipCount = 0;
	mineCount = 0; cannonballCount = 0;
	for (int i = 0; i < entityCount; i++)
	{
		cin >> entityId >> entityType >> x >> y >> arg1 >> arg2 >> stockArg >> ownerArg; cin.ignore();
		if(entityType == "SHIP") entityTypeEnum = SHIP;
		else if(entityType == "BARREL") entityTypeEnum = BARREL;
		else if(entityType == "CANNONBALL") entityTypeEnum = CANNONBALL;
		else if(entityType == "MINE") entityTypeEnum = MINE;

		switch(entityTypeEnum)
		{
		    case SHIP: ships[totalShipCount++]->updateWithInputs(entityId, x, y, arg1, arg2, stockArg, ownerArg);
				switch(ownerArg)
				{
					case 1: myShips[myShipCount]->updateWithInputs(entityId, x, y, arg1, arg2, stockArg, ownerArg);
							myShipsSimu[myShipCount++]->updateWithInputs(entityId, x, y, arg1, arg2, stockArg, ownerArg);
					  break;
					case 0: opShips[opShipCount++]->updateWithInputs(entityId, x, y, arg1, arg2, stockArg, ownerArg);
					  break;
				}
					break;
            
		    case BARREL: barrels[barrelCount++]->updateWithInputs(entityId, x, y, arg1);
				break;
		    case CANNONBALL: cannonballs[cannonballCount++]->updateWithInputs(entityId, x, y, arg1, arg2);
				break;
		    case MINE: mines[mineCount++]->updateWithInputs(entityId, x, y);
				break;
		}
	}
}
void populateMap()
{
  for(int i=0; i < 3; ++i)
  {
	map[myShips[i]->x][myShips[i]->y]->currentShip = myShips[i];
	map[opShips[i]->x][opShips[i]->y]->currentShip = opShips[i];
  }
  /*for(int i=0; i < 6; ++i)
  {
	map[ships[i]->x][ships[i]->y]->currentShip = ships[i];
  }*/
  for(int i=0; i < barrelCount; ++i)
  {
	map[barrels[i]->x][barrels[i]->y]->isBarrel = true;
	map[barrels[i]->x][barrels[i]->y]->currentBarrel = barrels[i];
  }
  for(int i=0; i<cannonballCount; ++i)
  {
	map[cannonballs[i]->x][cannonballs[i]->y]->isTargeted = true;
	map[cannonballs[i]->x][cannonballs[i]->y]->cannonTurns = cannonballs[i]->turns;
  }  
  for(int i=0; i<mineCount; ++i)
  {
	map[mines[i]->x][mines[i]->y]->isMined = true;
	map[mines[i]->x][mines[i]->y]->currentMine = mines[i];
  }  
}

int getIndFromId(int id, bool generalInd)
{
  if(generalInd)
  {
    for(int i = 0; i < totalShipCount; ++i)
    {
      if(ships[i]->id == id) return i;
    }
  }
  else
  {
    for(int i = 0; i < myShipCount; ++i)
    {
      if(myShips[i]->id == id) return i;
    }
    for(int i = 0; i < opShipCount; ++i)
    {
      if(opShips[i]->id == id) return i;
    }
    for(int i = 0; i < barrelCount; ++i)
    {
      if(barrels[i]->id == id) return i;
    }
  }
}
int main()
{
	initDefault();
    // game loop
    while (1) {
		resetBeforeInputs();
		getInputEntities();
		populateMap();
		
        for (int i = 0; i < myShipCount; i++) { myShips[i]->print();}
        //for (int i = 0; i < barrelCount; i++) { barrels[i]->print();}
		
		
		/**
		*	TODO:		*
		* 	A. DODGE MOVES: Check si on est cible des cannons:
		*		1. Parcourir tous les cannonballs
		*		2. Regarder si on va s'en prendre en faisant notre MOVE
		*			Si oui, Chercher un MOVE pour equiver
		*				Il existe un chemin pour esquiver, jouer ce move en priorité
		*				Sinon, chercher si on peux ne pas se le prendre en plein milieu
		*					Si c'est possible, jouer ce move en priorité
		*					Sinon on se prend le cannon en plein milieu
		*						Si on meurt => tirer un dernier coup ou poser une mine
		*						Si on ne meurt pas, rien a faire
		*			Sinon, rien a faire
		*	Si aucun DODGE MOVE a faire, passer a B
		*	B. FIRE MOVES:
		*		Regarder les FIRE possible et donner un score
		*		1. Barrel Target:
		*			1a Cibler les bariles les plus proches des ship adverse
		*			1b Verifier qu'on ne peux pas les atteindre avant
		*			1c Tirer au moment où le ship est censé le récup ( 1 tour avant car il récup avec le front)
		*			   Score => 
		*				- perte de rhum occasionée par le tir
		*				- distance du tir
		*		2. Ship Target:	
		*			2a Sur son chemin vers barrel pour le retarder/l'empecher
		*			2b La ou il peut le moins esquiver
		*			   Score => 
		*				- Chances de toucher ?
		*				- Temps perdu a esquiver le tir ?
		*				- A voir...
		*		3. Mine Target: 
		*			3a Cibler une mine pour que l'explosion fasse perdre du rhum
		*				Score => 
		*				- Chances de toucher ?
		*				- Temps perdu a esquiver l'explosion ?
		*				- A voir...
		*			3b Cibler une mine pour se frayer un chemin	
		*				Score => Temps gagné
		*	Si aucun FIRE MOVE avec un score suffisant, passer a C
		*	C. BARRELS MOVES: Se diriger vers le barrel le plus proche:
		*		1. Calculer le nombre de tours necessaire pour aller jusqu'au barrel
		*		2. Pareil pour les bateaux adverse
		*			2a Si on est le plus proche, y aller, passer a D						
		*			2b Sinon reevaluer B Ship target si ca peut nous faire arriver avant		
		*				Si oui, faire le FIRE move
		*				Sinon, FIRE le baril
		*	Si il n'y a plus de baril, passer a E
		*	D. MOVES MOVES: 
		*		1. Chercher le plus court chemin en esquivant les mines
		*		2. Si plusieurs choix possible, prendre les chemins avec:
		*			- le plus d'espace libre autour
		*			- le moins de mines a longer
		*			- la plus grande distance entre mes ships, pour eviter de les grouper et/ou se restreindre/bloquer soi meme
		*			- le plus grande distance avec les ship adverse si on a moins de rhum
		*	E. END GAME MODE: Fight!!
		*		1. Si on a + de rhum que l'adversaire sur tous nos ships: fuir
		*		2. Sinon se servir de ceux qui ont le moins de rhum comme ligne de front
		*
		**/
		
		
		/**
		*	TODO:		*
		* 	A. DODGE MOVES: Check si on est cible des cannons:
		*		1. Parcourir tous les cannonballs
		*		2. Regarder si on va s'en prendre en faisant notre MOVE
		*			Si oui, Chercher un MOVE pour equiver
		*				Il existe un chemin pour esquiver, jouer ce move en priorité
		*				Sinon, chercher si on peux ne pas se le prendre en plein milieu
		*					Si c'est possible, jouer ce move en priorité
		*					Sinon on se prend le cannon en plein milieu
		*						Si on meurt => tirer un dernier coup ou poser une mine
		*						Si on ne meurt pas, rien a faire
		*			Sinon, rien a faire
		*/
        for (int i = 0; i < myShipCount; i++)
		{
			int closestBarrelId = myShips[i]->getClosestBarrel();	

			
		}
		
		
        for (int i = 0; i < myShipCount; i++)
		{
		
			cerr << endl << "Processing myShip " << i << "..." << endl;
			
			Point dest;		
			int closestBarrelId = myShips[i]->getClosestBarrel();	
			int closestOpId = myShips[i]->getClosestShip(0);			
			
			int closestBarrelInd = getIndFromId(closestBarrelId);
			int distToClosestBarrel = myShips[i]->dist(*barrels[closestBarrelInd]);
			
			int closestOpInd = getIndFromId(closestOpId);
			int distToClosestOp = myShips[i]->dist(*opShips[closestOpInd]);
			
			int closestOpBarrelId = opShips[closestOpInd]->getClosestBarrel();
			int closestOpBarrelInd = getIndFromId(closestOpBarrelId);
			int distToClosestOpBarrel = myShips[i]->dist(*barrels[closestOpBarrelInd]);
			int opdistToClosestOpBarrel = opShips[closestOpInd]->dist(*barrels[closestOpBarrelInd]);
			int cannonDistToClosestOpBarrel = 1 + distToClosestOpBarrel / 3;
			
			// Get next auto move to closest barrel
			MoveType nextMoveType;
			bool nextMoveChanged = false;
			if(closestBarrelInd != -1)
			{
				nextMoveType = myShipsSimu[i]->nextMoveTo(*barrels[closestBarrelInd]);
				cerr << "nextMoveType        " << nextMoveType << endl;
				
				nextMoveChanged = myShipsSimu[i]->dodgeMineAndCannon(nextMoveType);				
				if(!nextMoveChanged) nextMoveChanged = myShipsSimu[i]->dodgeMineAndCannon(nextMoveType, true);
				cerr << "nextMoveType dodged " << nextMoveType << endl;
			}
			//Point nextPoint = myShipsSimu[i]->nextPoint(nextMoveType);
			//if(nextMoveType == NONE) nextMoveType = (turn%2) ? PORT : STARBOARD;
			// Change auto move to dodge mines
			
			
			// Change auto move to dodge cannons
			/*if(!nextMoveChanged)
			{
			*myShipsSimu[i] = *myShips[i];
				nextMoveChanged = myShipsSimu[i]->dodgeCannon(nextMoveType);
			}
			cerr << "nextMoveType cannon " << nextMoveType << endl;
			*/
			solution->moves[i]->type = nextMoveType;			
			
			// Get next auto move to closest op
			Point fightDest;
			fightDest.set(*opShips[closestOpInd]);
			fightDest.x += 5; if(fightDest.x > 22) fightDest.x = 21;
			fightDest.y += 5; if(fightDest.y > 20) fightDest.y = 19;
			*myShipsSimu[i] = *myShips[i];
			MoveType nextMoveTypeFight = myShipsSimu[i]->nextMoveTo(fightDest);			
			cerr << "nextMoveTypeFight        " << nextMoveTypeFight << endl;
			
			//if(nextMoveTypeFight == NONE) nextMoveTypeFight = (turn%2) ? FASTER : ((turn%2) ? PORT : STARBOARD);
			// Change auto move to dodge mines
			bool nextMoveFightChanged = false;
			myShipsSimu[i]->dodgeBorders(nextMoveTypeFight);
			nextMoveFightChanged = myShipsSimu[i]->dodgeMineAndCannon(nextMoveTypeFight);
			if(!nextMoveFightChanged) nextMoveFightChanged = myShipsSimu[i]->dodgeMineAndCannon(nextMoveType, true);
			cerr << "nextMoveTypeFight dodged " << nextMoveTypeFight << endl;
			
			// Change auto move to dodge cannons
			/*if(!nextMoveFightChanged)
			{
			*myShipsSimu[i] = *myShips[i];
				nextMoveFightChanged = myShipsSimu[i]->dodgeCannon(nextMoveTypeFight);
			}
			cerr << "nextMoveTypeFight cannon " << nextMoveTypeFight << endl;
			*/
			solutionFight->moves[i]->type = nextMoveTypeFight;			
			
			myShips[i]->print();
			bool escape = false;
			for (int ci = 0; ci < cannonballCount; ci++)
			{
				if( (cannonballs[ci]->x == myShips[i]->x && cannonballs[ci]->y == myShips[i]->y && cannonballs[ci]->turns < 4) ||
					(cannonballs[ci]->x == myShips[i]->back.x && cannonballs[ci]->y == myShips[i]->back.y && cannonballs[ci]->turns < 4) )
				{
					escape = true;
					cerr << "Escape " << cannonballs[ci]->x << "," << cannonballs[ci]->y << endl; 
				}				
			}
			
				//cerr << "distToClosestOpBarrel " << distToClosestOpBarrel << endl;
				//cerr << "opdistToClosestOpBarrel " << opdistToClosestOpBarrel << endl;
				//cerr << "cannonDistToClosestOpBarrel " << cannonDistToClosestOpBarrel << endl;
			if(myShips[i]->speed == 0 && escape)// && !myShips[i]->canFire)
			{
				cerr << "escapeMove "<< endl;
				dest.set(myShips[i]->forwardPoint2());
				//dest.x += 5; if(dest.x > 22) dest.x = 0;
				//dest.y += 5; if(dest.y > 20) dest.x = 0;
				//dest.set(barrels[closestBarrelInd]->x, barrels[closestBarrelInd]->y);		
			
				Point forwardP = myShips[i]->forwardPoint(3);
					cerr << "forwardP" << forwardP.x << "," << forwardP.y << endl;		
				if(map[forwardP.x][forwardP.y]->isMined || 
					(map[forwardP.x][forwardP.y]->isTargeted && map[forwardP.x][forwardP.y]->cannonTurns == 2) )
				{
					Point rightP = myShips[i]->forwardPoint(myShips[i]->speed, mod(myShips[i]->rotation-1));
					Point leftP = myShips[i]->forwardPoint(myShips[i]->speed, mod(myShips[i]->rotation+1));
					cerr << "rightP" << rightP.x << "," << rightP.y << endl;	
					if(map[rightP.x][rightP.y]->isMined || 
					(map[forwardP.x][forwardP.y]->isTargeted && map[forwardP.x][forwardP.y]->cannonTurns == 2) ) solutionFight->moves[i]->type = PORT;
					else solutionFight->moves[i]->type = STARBOARD;	
					cerr << "Dodge Mine" << endl;
					solutionFight->moves[i]->output();
					myShips[i]->canFire = true;
				}
				else
				{
					cout << "MOVE " << dest.x << " " <<  dest.y << endl; //" escape" << endl; // Any valid action, such as "WAIT" or "MOVE x y"	
					myShips[i]->canFire = true;		
				}					
			}
			else 
				if(turn != 1 && myShips[i]->canFire && distToClosestOp < 10 &&  myShips[i]->speed == 0 && opShips[closestOpInd]->speed == 0)
			{			
				cerr << "stuck Move "<< endl;
				//dest.set(opShips[closestOpInd]->forwardPoint());
				//cout << "FIRE " << dest.x << " " <<  dest.y << endl;					
				/*Point forwardP = (myShips[i]->speed == 0) ? myShips[i]->forwardPoint(2) : myShips[i]->forwardPoint(3);
				if(map[forwardP.x][forwardP.y]->isMined || 
					(map[forwardP.x][forwardP.y]->isTargeted && map[forwardP.x][forwardP.y]->cannonTurns == 2) )
				{
					Point rightP = myShips[i]->forwardPoint(3, mod(myShips[i]->rotation-1));
					Point leftP = myShips[i]->forwardPoint(3, mod(myShips[i]->rotation+1));
					if(map[rightP.x][rightP.y]->isMined || 
					(map[forwardP.x][forwardP.y]->isTargeted && map[forwardP.x][forwardP.y]->cannonTurns == 2) ) solutionFight->moves[i]->type = PORT;
					else solutionFight->moves[i]->type = STARBOARD;	
					cerr << "Dodge Mine" << endl;
					solutionFight->moves[i]->output();
					myShips[i]->canFire = true;
				}
				else
				{*/
					cout << "FIRE " << opShips[closestOpInd]->x << " " <<  opShips[closestOpInd]->y << endl;	
					myShips[i]->canFire = false;		
				//}					
			}
			/*else if(barrelCount == 1)
			{
				cout << "FIRE " << barrels[0]->x << " " <<  barrels[0]->y << endl;				
			}*/	
			else if( closestOpBarrelId != -1 && distToClosestOpBarrel<9 && myShips[i]->canFire &&
				cannonDistToClosestOpBarrel == opdistToClosestOpBarrel && distToClosestOpBarrel > opdistToClosestOpBarrel+1)
			{
				cerr << "Destroy his Barrel Move "<< endl;
					Point forwardP = myShips[i]->forwardPoint(3);									
					if(nextMoveChanged)
					{
						solutionFight->moves[i]->output();
						myShips[i]->canFire = true;
					}
					else if(map[forwardP.x][forwardP.y]->isMined || 
				(map[forwardP.x][forwardP.y]->isTargeted && map[forwardP.x][forwardP.y]->cannonTurns == 2) )
					{
						Point rightP = myShips[i]->forwardPoint(myShips[i]->speed, mod(myShips[i]->rotation-1));
						Point leftP = myShips[i]->forwardPoint(myShips[i]->speed, mod(myShips[i]->rotation+1));
						if(map[rightP.x][rightP.y]->isMined || map[rightP.x][rightP.y]->isTargeted ) solutionFight->moves[i]->type = PORT;
						else solutionFight->moves[i]->type = STARBOARD;	
						cerr << "Dodge Mine" << endl;
						solutionFight->moves[i]->output();
						myShips[i]->canFire = true;
					}
					else
					{
						cout << "FIRE " << barrels[closestOpBarrelInd]->x << " " <<  barrels[closestOpBarrelInd]->y << endl;		
						myShips[i]->canFire = false;	
					}
			}
			else if( //(nextMoveChanged && distToClosestOp <= 10) ||
				 (closestBarrelId != -1 && (!myShips[i]->canFire || distToClosestOp > 9))
			)
			{				
				cerr << "Go To Closest Barrel Move "<< endl;
				cerr << "closestBarrelId " << closestBarrelId << endl;
				dest.set(barrels[closestBarrelInd]->x, barrels[closestBarrelInd]->y);
				
				// Si on va tout droit, vérifier qu'il y a pas une mine 2 cases en face
				if(solution->moves[i]->type != PORT && solution->moves[i]->type != STARBOARD)
				{
					Point forwardP = myShips[i]->forwardPoint(3);
					if(map[forwardP.x][forwardP.y]->isMined)
					{
						Point rightP = myShips[i]->forwardPoint(myShips[i]->speed, mod(myShips[i]->rotation-1));
						Point leftP = myShips[i]->forwardPoint(myShips[i]->speed, mod(myShips[i]->rotation+1));
						if(map[rightP.x][rightP.y]->isMined) solution->moves[i]->type = PORT;
						else solution->moves[i]->type = STARBOARD;	
						cerr << "Dodge Mine" << endl;
					}
				}
				
				//if(changeOrder) 
				//else cout << "MOVE " << dest.x << " " <<  dest.y << " Auto" << endl;
					myShips[i]->canFire = true;
				solution->moves[i]->output();
			}
			else
			{				
				dest.set(*opShips[closestOpInd]);
				//dest.set(opShips[closestOpInd]->x, opShips[closestOpInd]->y);		
				if(1 + myShips[i]->dist(dest)/3 > myShips[i]->dist(dest)+1)		
					dest.set(opShips[closestOpInd]->forwardPoint(opShips[closestOpInd]->speed+4));			
				else if(1 + myShips[i]->dist(dest)/3 > myShips[i]->dist(dest))		
					dest.set(opShips[closestOpInd]->forwardPoint(opShips[closestOpInd]->speed+3));		
				else dest.set(opShips[closestOpInd]->forwardPoint(opShips[closestOpInd]->speed+2));
				
				if(opShips[closestOpInd]->speed == 0)
				{
					dest.x = opShips[closestOpInd]->x;
					dest.y = opShips[closestOpInd]->y;
				}
				
				if(distToClosestOp >= 10 || !myShips[i]->canFire)// && myShips[i]->speed != 0)
				{
					cerr << "Go To Closest Op Move "<< endl;					
					// Si on va tout droit, vérifier qu'il y a pas une mine/un cannon 2 cases en face
					if(solutionFight->moves[i]->type != PORT && solutionFight->moves[i]->type != STARBOARD)
					{
						Point forwardP = myShips[i]->forwardPoint(3);	
						if(map[forwardP.x][forwardP.y]->isMined || 
					(map[forwardP.x][forwardP.y]->isTargeted && map[forwardP.x][forwardP.y]->cannonTurns == 2) )
						{
							Point rightP = myShips[i]->forwardPoint(myShips[i]->speed, mod(myShips[i]->rotation-1));
							Point leftP = myShips[i]->forwardPoint(myShips[i]->speed, mod(myShips[i]->rotation+1));
							if(map[rightP.x][rightP.y]->isMined || map[rightP.x][rightP.y]->isTargeted ) solutionFight->moves[i]->type = PORT;
							else solutionFight->moves[i]->type = STARBOARD;	
							cerr << "Dodge Mine" << endl;
						}
					}
					solutionFight->moves[i]->output();
					myShips[i]->canFire = true;
					//if(changeOrderFight) solutionFight->moves[i]->output();
					//else cout << "MOVE " << dest.x << " " <<  dest.y  << " Fight" << endl;
				}
				else
				{	
			
					cerr << "Fire his forward point or speed 0 Move "<< endl;
					Point forwardP = myShips[i]->forwardPoint(3);									
					if(nextMoveFightChanged)
					{
						solutionFight->moves[i]->output();
						myShips[i]->canFire = true;
					}
					else if(map[forwardP.x][forwardP.y]->isMined || 
					(map[forwardP.x][forwardP.y]->isTargeted && map[forwardP.x][forwardP.y]->cannonTurns == 2))
					{
						Point rightP = myShips[i]->forwardPoint(myShips[i]->speed, mod(myShips[i]->rotation-1));
						Point leftP = myShips[i]->forwardPoint(myShips[i]->speed, mod(myShips[i]->rotation+1));
						if(map[rightP.x][rightP.y]->isMined || map[rightP.x][rightP.y]->isTargeted ) solutionFight->moves[i]->type = PORT;
						else solutionFight->moves[i]->type = STARBOARD;	
						cerr << "Dodge Mine" << endl;	
						solutionFight->moves[i]->output();
						myShips[i]->canFire = true;					
					}
					else
					{
						if( dest == myShips[i]->back ||
							dest == myShips[i]->center || 
							dest == myShips[i]->front ||
							dest == myShips[i]->forwardPoint(2) ||
							dest == myShips[i]->forwardPoint(3)||
							dest == myShips[i]->forwardPoint(4)||
							dest == myShips[i]->forwardPoint(5)||
							dest == myShips[i]->forwardPoint(6)||
							dest == myShips[i]->forwardPoint(7)||
							dest == myShips[i]->forwardPoint(8)||
							dest == myShips[i]->forwardPoint(9)) dest.set(opShips[closestOpInd]->front);
						cout << "FIRE " << dest.x << " " <<  dest.y << endl;//" Fire " << dest.x << "," <<  dest.y << endl;
						myShips[i]->canFire = false;
					}
				}
			}
        }
		++turn;
    }// end game loop
}
