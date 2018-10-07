#include <iostream>
#include <fstream>
#include "SDL_Draw.h"
using namespace std;

//Button constants
const int BUTTON_WIDTH = SCREEN_WIDTH/3;
const int BUTTON_HEIGHT = SCREEN_HEIGHT/6;
int choice = 0;
int numScore[5] = {0, 0, 0, 0, 0};
int speed = 200;

int chartoint(char c[]){
	int index = 0;
	int num = 0;
	int mul = 10;
	while(c[index] != '\0'){
		num = num * mul + c[index] - 48;
		index++;
	}
	return num;
}

char *inttochar(int num){
	char rcscore[10];
	char cscore[10];
	int index = 0;
	int sodu;
	if(num == 0){
		cscore[0] = 48;
		cscore[1] = 0;
	}
	else{
		while(num>0){
			sodu = num % 10;
			num = num / 10;
			rcscore[index] = sodu + 48;
			index++;
		}
		for(int i=0; i < index; i++)
			cscore[i] = rcscore[index-i-1];
		cscore[index] = '\0';
	}
	return cscore;
}

enum LButtonSprite
{
	BUTTON_SPRITE_MOUSE_OUT = 0,
	BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
	BUTTON_SPRITE_MOUSE_DOWN = 2,
	BUTTON_SPRITE_MOUSE_UP = 3,
	BUTTON_SPRITE_TOTAL = 4
};

//The mouse button
class LButton
{
	public:
		//Initializes internal variables
		LButton();

		//Sets top left position
		void setPosition( int x, int y );

		//Handles mouse event
		void handleEvent( SDL_Event* e );
	
		//Shows button sprite
		void render(LTexture &gButton);

		//Get mCurrentSprite
		LButtonSprite getCurrent();

		//Set mCurrent
		void setCurrent(LButtonSprite sprite);

	private:
		//Top left position
		SDL_Point mPosition;

		//Currently used global sprite
		LButtonSprite mCurrentSprite;
};

//Mouse button sprites
SDL_Rect gSpriteClips[ BUTTON_SPRITE_TOTAL ];


//Buttons objects
LButton gButtons[ TOTAL_BUTTONS ]; 
LButton gButtonBack;

LButton::LButton()
{
	mPosition.x = 0;
	mPosition.y = 0;

	mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition( int x, int y )
{
	mPosition.x = x;
	mPosition.y = y;
}

void LButton::handleEvent( SDL_Event* e )
{
	//If mouse event happened
	if( e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP )
	{
		//Get mouse position
		int x, y;
		SDL_GetMouseState( &x, &y );

		//Check if mouse is in button
		bool inside = true;

		//Mouse is left of the button
		if( x < mPosition.x )
		{
			inside = false;
		}
		//Mouse is right of the button
		else if( x > mPosition.x + BUTTON_WIDTH )
		{
			inside = false;
		}
		//Mouse above the button
		else if( y < mPosition.y )
		{
			inside = false;
		}
		//Mouse below the button
		else if( y > mPosition.y + BUTTON_HEIGHT )
		{
			inside = false;
		}

		//Mouse is outside button
		if( !inside )
		{
			mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
		}
		//Mouse is inside button
		else
		{
			//Set mouse over sprite
			switch( e->type )
			{
				case SDL_MOUSEMOTION:
				mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
				break;
			
				case SDL_MOUSEBUTTONDOWN:
				mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
				break;
				
				case SDL_MOUSEBUTTONUP:
				mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
				break;
			}
		}
	}
}
	
void LButton::render(LTexture &gButton)
{
	//Show current button sprite
	gButton.setWidth(BUTTON_WIDTH);
	gButton.setHeight(BUTTON_HEIGHT);
	gButton.renderPicture( mPosition.x, mPosition.y);
}

LButtonSprite LButton::getCurrent()
{
	return this->mCurrentSprite;
}

void LButton::setCurrent(LButtonSprite sprite){
	this->mCurrentSprite = sprite;
}

FILE *fp;
void readFile(){
	fp = fopen("data.txt", "r");
	char cscore[100];
	char c;
	int index;
	int i = 0;
	while((c=fgetc(fp)) != -1 && i < 5){
		index = 0;
		cscore[index] = c;
		index++;
		while((c = fgetc(fp)) != '#'){
			cscore[index] = c;
			index++;
		}
		cscore[index] = '\0';
		cout<<cscore<<endl;
		numScore[i] = chartoint(cscore);
		cout<<"numScore: "<<numScore[i]<<endl;
		i++;
		}
	fclose(fp);
}

void writeFile(int score[]){
	char cscore[10];
	fp = fopen("data.txt", "w+");
	for(int i = 0; i < 5; i++){
		int index = 0;
		while(inttochar(score[i])[index]!= 0){
			cscore[index] = inttochar(score[i])[index];
			index++;
		}
		cscore[index] = 0;
		cout<<cscore<<endl;
		fputs(cscore, fp);
		fputc('#', fp);
	}
	fclose(fp);
}

void DrawMenu(){
	gBackGround.setHeight(SCREEN_HEIGHTPLUS);
	gBackGround.setWidth(SCREEN_WIDTH);
	gBackGround.render(0, 0);

	gSnake.setHeight(FACTOR*5);
	gSnake.setWidth( FACTOR*3*5);
	gSnake.renderPicture(SCREEN_WIDTH/3+FACTOR/2,  SCREEN_HEIGHT/3 - FACTOR*5 - FACTOR);
	
	for(int i = 0; i < 3; i++){ 
	gButtons[i].setPosition( SCREEN_WIDTH/3, SCREEN_HEIGHT/3 + BUTTON_HEIGHT*i);
	gButtons[i].render(gTextureButton[i]);
	}

	gSelect.setWidth(BUTTON_WIDTH);
	gSelect.setHeight(BUTTON_HEIGHT);
	gSelect.renderPicture(SCREEN_WIDTH/3, SCREEN_HEIGHT/3 + BUTTON_HEIGHT*choice);
	SDL_RenderPresent( gRenderer );
}

//In memory text stream
std::stringstream customText;

//Set text color as black
SDL_Color textColor = { 0, 0, 0, 255 };
void highScore(){
	gBackGround.render(0, 0);
	gBbutton.setHeight(BUTTON_HEIGHT);
	gBbutton.setWidth(BUTTON_WIDTH);
	int i;
	for(i = 0; i < 5; i++){
		gBbutton.renderPicture(SCREEN_WIDTH/3, SCREEN_HEIGHT/12 + BUTTON_HEIGHT * i);
		//Set text to be rendered
		customText.str( "" );
		customText << numScore[i];
		if( !gScore.loadFromRenderedText( customText.str().c_str(), textColor ))
			{
				printf( "Unable to render score texture!\n" );
			}
		int heso = 0;
		if(numScore[i]>=1000)
			heso = 4;
		else if(numScore[i]>=100)
			heso = 3;
		else if(numScore[i]>=10)
			heso = 2;
		else heso = 1;
		gScore.setWidth(SCREEN_WIDTH/14 * heso);
		gScore.setHeight(SCREEN_HEIGHT/8);
		gScore.renderPicture(SCREEN_WIDTH/2 - SCREEN_WIDTH/28 * heso, SCREEN_HEIGHT/12 + BUTTON_HEIGHT * i + SCREEN_HEIGHT/48);
	}
	gBbutton.renderPicture(SCREEN_WIDTH/3, SCREEN_HEIGHT/12 + BUTTON_HEIGHT * i);
	customText.str( "" );
	customText<<"BACK";
	if( !gScore.loadFromRenderedText( customText.str().c_str(), textColor ))
			{
				printf( "Unable to render score texture!\n" );
			}
	gScore.setWidth(SCREEN_WIDTH/14 * 4);
	gScore.setHeight(SCREEN_HEIGHT/8);
	gScore.renderPicture(SCREEN_WIDTH/2 - SCREEN_WIDTH/28 * 4, SCREEN_HEIGHT/12 + BUTTON_HEIGHT * i + SCREEN_HEIGHT/48);
	//SDL_RenderPresent( gRenderer );
}

void BubbleSort(int arr[], int n){
	for(int i = n-1; i > 0 ; i--)
		for(int j = 0; j < i; j++)
			if(arr[j] < arr[j+1]){
				int temp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = temp;
			}
}

int main( int argc, char* args[] )
{
	readFile();
	BubbleSort(numScore, 5);
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;
			choice = 0;

			//Event handler
			SDL_Event e;

			int value;
			if( Mix_PlayingMusic() == 0 )
				{
					//Play the music
					Mix_PlayMusic( gMusicMenu, -1 );
				}
			while( !quit )
			{
				int Score = 0;
				int FlameValue = 0;
				snake.initSnake();
				apple.initApple();
				endGame = true;
				value = 1;
				DrawMenu();
				while( SDL_PollEvent( &e ) != 0 ){
					//User requests quit
					if( e.type == SDL_QUIT )
						{
							quit = true;
						}
					else if(e.type == SDL_KEYDOWN)
					{
						switch( e.key.keysym.sym )
						{
							case SDLK_UP:
									choice--;
									if(choice < 0)
										choice = 0;
								break;
							case SDLK_DOWN:
									choice++;
									if(choice > 2)
										choice = 2;
								break;
							case SDLK_KP_ENTER:
							case SDLK_RETURN:
								if(choice == 0){
										endGame = false;
										Mix_HaltMusic();
										if( Mix_PlayingMusic() == 0 )
											{
												//Play the music
												Mix_PlayMusic( gMusic, -1 );
											}
									}
								else if(choice == 1){
									bool back = false;
									gButtonBack.setPosition(SCREEN_WIDTH/3, SCREEN_HEIGHT/12 + BUTTON_HEIGHT * 5);
									while(!back){
										highScore();
										gButtonBack.handleEvent(&e);
										if(gButtonBack.getCurrent() == BUTTON_SPRITE_MOUSE_OVER_MOTION)
											{
												gSelect.renderPicture(SCREEN_WIDTH/3, SCREEN_HEIGHT/12 + BUTTON_HEIGHT * 5);
											}
										while( SDL_PollEvent( &e ) != 0 ){
											if( e.type == SDL_QUIT )
												{
													quit = true;
													back = true;
													break;
												}
											else if(e.type == SDL_KEYDOWN)
											{
												switch( e.key.keysym.sym )
													{
														case SDLK_KP_ENTER:
														case SDLK_RETURN:
															if(choice == 1){
																back = true;
																gButtons[1].setCurrent(BUTTON_SPRITE_MOUSE_OUT);
																gButtonBack.setCurrent(BUTTON_SPRITE_MOUSE_OUT);
																choice = 0;
															}
															break;
														case SDLK_UP:
														case SDLK_DOWN:
															gButtonBack.setCurrent(BUTTON_SPRITE_MOUSE_OVER_MOTION);
															break;
													}
											}
											gButtonBack.handleEvent(&e);
											if(gButtonBack.getCurrent() == BUTTON_SPRITE_MOUSE_DOWN)
												{
														back = true;
														gButtons[1].setCurrent(BUTTON_SPRITE_MOUSE_OUT);
														choice = 0;
														gButtonBack.setCurrent(BUTTON_SPRITE_MOUSE_OUT);
														break;
												}
										}
										SDL_RenderPresent( gRenderer );
									}	
								}
								else if(choice == 2)
									return 0;
								break;
							case SDLK_ESCAPE:
								return 0;
								break;
							default:
								break;
						}
					}
					//Handle button events
					for( int i = 0; i < TOTAL_BUTTONS; i++ )
					{
						gButtons[i].handleEvent( &e );
						//select button
						if(gButtons[i].getCurrent() == BUTTON_SPRITE_MOUSE_OVER_MOTION){
							choice = i;
						}

						if(gButtons[0].getCurrent() == BUTTON_SPRITE_MOUSE_DOWN)
						{
							endGame = false;
							Mix_HaltMusic();
							if( Mix_PlayingMusic() == 0 )
								{
									//Play the music
									Mix_PlayMusic( gMusic, -1 );
								}
						}

						
						else if(gButtons[1].getCurrent() == BUTTON_SPRITE_MOUSE_DOWN)
						{
							bool back = false;
							gButtonBack.setPosition(SCREEN_WIDTH/3, SCREEN_HEIGHT/12 + BUTTON_HEIGHT * 5);
							while(!back){
								highScore();
								gButtonBack.handleEvent(&e);
								if(gButtonBack.getCurrent() == BUTTON_SPRITE_MOUSE_OVER_MOTION){
									gSelect.renderPicture(SCREEN_WIDTH/3, SCREEN_HEIGHT/12 + BUTTON_HEIGHT * 5);
								}
								while( SDL_PollEvent( &e ) != 0 ){
								if( e.type == SDL_QUIT )
									{
										quit = true;
										back = true;
										break;
									}
								else if(e.type == SDL_KEYDOWN){
									switch( e.key.keysym.sym )
										{
											case SDLK_KP_ENTER:
											case SDLK_RETURN:
												back = true;
												gButtons[1].setCurrent(BUTTON_SPRITE_MOUSE_OUT);
												gButtonBack.setCurrent(BUTTON_SPRITE_MOUSE_OUT);
												choice = 0;
												break;
											case SDLK_UP:
											case SDLK_DOWN:
												gButtonBack.setCurrent(BUTTON_SPRITE_MOUSE_OVER_MOTION);
												break;
										}
									}
								gButtonBack.handleEvent(&e);
								if(gButtonBack.getCurrent() == BUTTON_SPRITE_MOUSE_DOWN)
									{
										back = true;
										gButtons[1].setCurrent(BUTTON_SPRITE_MOUSE_OUT);
										choice = 0;
										gButtonBack.setCurrent(BUTTON_SPRITE_MOUSE_OUT);
										break;
									}
								}
								SDL_RenderPresent( gRenderer );
							}
						}
						else if(gButtons[2].getCurrent() == BUTTON_SPRITE_MOUSE_DOWN)
						{
							return 0;
						}

					}

				}
				
				while( !endGame && !quit ){
					//Draw Map
					FlameValue = FlameValue % 2;
					DrawMap(FlameValue);
					//DrawApple();
					DrawSnake();
					//Render Apple to the screen
					gApple.render(apple.getAppleX() * FACTOR, apple.getAppleY() * FACTOR);

					//Score
					//Set text to be rendered
					customText.str( "" );

					SDL_Color scoreColor = { 255, 0, 0, 255 };
					customText << "Your Score: " << Score;
					//Render text
					if( !gScore.loadFromRenderedText( customText.str().c_str(), scoreColor ))
					{
						printf( "Unable to render score texture!\n" );
					}
					//Render textures
					gScore.setHeight(FACTOR * 2);
					gScore.setWidth(FACTOR * 8);
					gScore.renderPicture(FACTOR, SCREEN_HEIGHT + FACTOR);

					//Update screen
					SDL_RenderPresent( gRenderer );
					FlameValue++;
					//Handle events on queue
					Sleep(speed);
					while( SDL_PollEvent( &e ) != 0 )
					{
						//User requests quit
					if( e.type == SDL_QUIT )
						{
							quit = true;
						}
						//User presses a key
						else if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
						{
							//Select surfaces based on key press
							switch( e.key.keysym.sym )
							{
								case SDLK_UP:
									if(snake.getDirectionSnake()[snake.getLengthSnake()-1] != ddown)
										snake.setDirectionSnake(dup);
									break;

								case SDLK_DOWN:
									if(snake.getDirectionSnake()[snake.getLengthSnake()-1] != dup)
									snake.setDirectionSnake(ddown);
									break;

								case SDLK_LEFT:
									if(snake.getDirectionSnake()[snake.getLengthSnake()-1] != dright)
									snake.setDirectionSnake(dleft);
									break;

								case SDLK_RIGHT:
									if(snake.getDirectionSnake()[snake.getLengthSnake()-1] != dleft)
									snake.setDirectionSnake(dright);
									break;

								case SDLK_ESCAPE:
									return 0;
									break;
								default:
								break;
							}
						}
					}

					value = snake.moveSnake(apple);
					if(value == -1){
						for(int i = 0; i < 5; i++)
							if(Score > numScore[i]){
								numScore[i] = Score;
								break;
							}
						writeFile(numScore);
						endGame = true;
						Mix_PlayChannel( -1, gHigh, 0 );
						Mix_HaltMusic();
						//Render textures
						SDL_SetRenderDrawColor( gRenderer, 0x97, 0xFF, 0xFF, 0xFF );
						SDL_RenderClear( gRenderer);
						gScore.setHeight(SCREEN_HEIGHT/3);
						gScore.setWidth(SCREEN_WIDTH/2);
						gScore.renderPicture(SCREEN_WIDTH/4, SCREEN_HEIGHT/3);
						SDL_RenderPresent( gRenderer );
						Sleep(1000);
						if( Mix_PlayingMusic() == 0 )
							{
								//Play the music
								Mix_PlayMusic( gMusicMenu, -1 );
							}
						speed = 200;
					}
					else if(value == 0){
						randomApple();
						Score++;
						speed -= 20;
						if(speed < 20)
							speed = 20;
						Mix_PlayChannel( -1, gScratch, 0 );
					}
					else if(value == 1){
					}
				}

			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}