class Apple{
private:
	int appleX, appleY;
	char displayApple;
public: 
	friend class Snake;
	Apple(){
		appleX = 5;
		appleY = 10;
		displayApple = 'o';
	}
	void initApple(){
		appleX = 5;
		appleY = 10;
		displayApple = 'o';
	}

	void setAppleX(int x){
		this->appleX = x;
	}

	int getAppleX(){
		return this->appleX;
	}

	void setAppleY(int y){
		this->appleY = y;
	}

	int getAppleY(){
		return this->appleY;
	}

	void setDisplayApple(char c){
		this->displayApple = c;
	}

	char getDisplayApple(){
		return this->displayApple;
	}

	bool checkEatApple(int x, int y){
		if(x == this->appleX && y == this->appleY)
			return true;
		return false;
	}
};