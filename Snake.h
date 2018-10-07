#define FACTOR	20
#define MAX_X	48 * FACTOR
#define MAX_Y	32 * FACTOR
#define MAX		MAX_X*MAX_Y
#define dleft	1
#define dright	2
#define dup		3
#define ddown	4
#include "Apple.h"

class Snake{
private: 
	int snakeX[MAX], snakeY[MAX];
	int snakeXo[MAX], snakeYo[MAX];
	int lengthSnake;
	char displaySnake;
	int directionSnake[MAX];
public:
	friend class Apple;
	Snake(){
		lengthSnake		= 3;
		for(int i = 0; i < lengthSnake; i++){
			snakeX[i]	= 3 + i;
			snakeY[i]	= 5;
			directionSnake[i]	= dright;
		}
		snakeXo[0]		= 0;
		snakeYo[0]		= 0;
		displaySnake	='*';
		directionSnake[lengthSnake-1]	= dright;
	}
	void initSnake(){
		lengthSnake		= 3;
		for(int i = 0; i < lengthSnake; i++){
			snakeX[i]	= 3+i;
			snakeY[i]	= 5;
			directionSnake[i]	= dright;
		}
		snakeXo[0]		= 0;
		snakeYo[0]		= 0;
		displaySnake	='*';

	}

	int *getSnakeX(){
		return this->snakeX;
	}

	int *getSnakeY(){
		return this->snakeY;
	}

	int *getSnakeXo(){
		return this->snakeXo;
	}

	int *getSnakeYo(){
		return this->snakeYo;
	}

	int getLengthSnake(){
		return this->lengthSnake;
	}

	void setDisplaySnake(char newDisplay){
		this->displaySnake = newDisplay;
	}

	char getDisplaySnake(){
		return this->displaySnake;
	}

	void setDirectionSnake(int newDirection){
		this->directionSnake[lengthSnake-1] = newDirection;
	}

	int *getDirectionSnake(){
		return this->directionSnake;
	}

	bool checkDieSnake(int nextX, int nextY){
		for(int i = 0; i < this->lengthSnake - 2; i++)
			if(nextX == snakeX[i]*FACTOR && nextY == snakeY[i]*FACTOR)
				return true;
		if(nextX == 0 || nextX == MAX_X-1*FACTOR || nextY == 0 || nextY == MAX_Y-1*FACTOR)
			return true;
		if((nextX >= MAX_X/4 && nextX <= (MAX_X - MAX_X/4) && nextY == MAX_Y/4) || (nextY >= MAX_Y/4 && nextY <= (MAX_Y-MAX_Y/4) && nextX == MAX_X/2))
			return true;
		return false;
	}

	void changeDirection(int nextX, int nextY){
		snakeXo[0] = snakeX[0];
		snakeYo[0] = snakeY[0];
		for(int i = 0; i < lengthSnake - 1 ; i++){
			snakeX[i]			= snakeX[i+1];
			snakeY[i]			= snakeY[i+1];
			directionSnake[i]	= directionSnake[i+1];
		}
		snakeX[lengthSnake-1] = nextX;
		snakeY[lengthSnake-1] = nextY;
		/*if(nextX < 1 && directionSnake[lengthSnake-1] == dleft)
			snakeX[lengthSnake-1] = MAX_X-2*FACTOR;
		if(nextX > MAX_X-2 && directionSnake[lengthSnake-1] == dright)
			snakeX[lengthSnake-1] = 1*FACTOR;
		if(nextY < 1 && directionSnake[lengthSnake-1] == dup)
			snakeY[lengthSnake-1] = MAX_Y-2*FACTOR;
		if(nextY > MAX_Y-2 && directionSnake[lengthSnake-1] == ddown)
			snakeY[lengthSnake-1] = 1*FACTOR;*/
	}

	void upgradeSnake(int addX, int addY){
		snakeX[lengthSnake]			= addX;
		snakeY[lengthSnake]			= addY;
		directionSnake[lengthSnake] = directionSnake[lengthSnake-1];
		lengthSnake++;
	}

	int moveSnake(Apple apple){
		int tempX = snakeX[lengthSnake-1];
		int tempY = snakeY[lengthSnake-1];
		switch(directionSnake[lengthSnake-1]){
		case dleft:
			tempX--;
			break;
		case dright:
			tempX++;
			break;
		case dup:
			tempY--;
			break;
		case ddown:
			tempY++;
			break;
		}
		if(checkDieSnake(tempX*FACTOR, tempY*FACTOR)){
			return -1;
		}
		else if(apple.checkEatApple(tempX, tempY)){
			upgradeSnake(tempX, tempY);
			return 0;
		}
		changeDirection(tempX, tempY);
		return 1;
	}
};