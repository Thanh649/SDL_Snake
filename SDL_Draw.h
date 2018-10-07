#include <sstream>
#include <ctime>
#include <Windows.h>
#include "Snake.h"
#include "Utility.h"
#include <conio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <string>

#define LEFT	VK_LEFT
#define RIGHT	VK_RIGHT
#define UP		VK_UP
#define	DOWN	VK_DOWN
#define ESC		VK_ESCAPE
#define SCREEN_WIDTH	MAX_X
#define SCREEN_HEIGHT	MAX_Y
#define SCREEN_HEIGHTPLUS SCREEN_HEIGHT + FACTOR*4
const int TOTAL_BUTTONS = 3;
Snake snake;
Apple apple;

bool endGame = false;
int ranX;
int ranY;

//Điều kiện xuất hiện của Táo (khác tường, khác rắn)
bool conditionApple(int x, int y){
	if((x >= (MAX_X/FACTOR)/4 && x <= ((MAX_X/FACTOR) - (MAX_X/FACTOR)/4) && y == (MAX_Y/FACTOR)/4) || (y >= (MAX_Y/FACTOR)/4 && y <= ((MAX_Y/FACTOR)-(MAX_Y/FACTOR)/4) && x == (MAX_X/FACTOR)/2) || x == 0 || x == (MAX_X/FACTOR)-1 || y == 0 || y == (MAX_Y/FACTOR)-1)
		return true;
	for(int i = 0; i < snake.getLengthSnake(); i++)
		if(x == snake.getSnakeX()[i] && y == snake.getSnakeY()[i])
			return true;
	return false;
}

//chọn random vị trí Táo tiếp theo
void randomApple(){
	while(1){
	srand(time(NULL));
	ranX = rand() % (MAX_X/FACTOR-1) + 1;
	ranY = rand() % (MAX_Y/FACTOR-1) + 1;
	if(!conditionApple(ranX, ranY))
		break;
	}
	apple.setAppleX(ranX);
	apple.setAppleY(ranY);
}

//Texture wrapper class - Class Load các đối tượng Texture SDL (hình ảnh, font chữ)
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );

		//Deallocates texture
		void free();

		//Renders texture at given point
		void render( int x, int y);

		//Renders picture texture
		void renderPicture( int x, int y);

		//Gets image dimensions
		int getWidth();
		int getHeight();

		void setWidth(int w);
		void setHeight(int y);

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image as texture
SDL_Texture* loadTexture( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures - các texture lưu hình ảnh
LTexture gSnake;
LTexture gHeadSnake_right;
LTexture gHeadSnake_left;
LTexture gHeadSnake_down;
LTexture gHeadSnake_up;
LTexture gBodySnake_ver;
LTexture gBodySnake_hor;
LTexture gTailSnake_right;
LTexture gTailSnake_left;
LTexture gTailSnake_down;
LTexture gTailSnake_up;
LTexture gMidSnake_leftup;
LTexture gMidSnake_rightup;
LTexture gMidSnake_upleft;
LTexture gMidSnake_upright;
LTexture gApple;
LTexture gBackGround;
LTexture gSelect;
LTexture gFlame[2];
LTexture gEnter;
LTexture gTextureButton[TOTAL_BUTTONS];
LTexture gBbutton;
LTexture gBlock;
LTexture gBackground[2];
//biến lưu file âm thanh
//The music that will be played 
Mix_Music *gMusic = NULL;
Mix_Music *gMusicMenu = NULL;

//The sound effects that will be used
Mix_Chunk *gScratch = NULL;
Mix_Chunk *gHigh = NULL;

//Globally used font - font chữ
TTF_Font *gFont = NULL;

//Scene textures - Điểm số
LTexture gScore;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth	= FACTOR;
	mHeight = FACTOR;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

//load hình ảnh 
bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0xFF, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = FACTOR;
			mHeight = FACTOR;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

//load font chữ 
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	//Return success
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

// dựng hình texture ra cửa sổ 
void LTexture::render( int x, int y )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	SDL_SetRenderDrawColor( gRenderer, 199, 149, 122, 0xFF );
	SDL_RenderFillRect( gRenderer, &renderQuad);
	SDL_RenderCopy( gRenderer, mTexture, NULL, &renderQuad );
}

void LTexture::renderPicture(int x, int y)
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	SDL_RenderCopy( gRenderer, mTexture, NULL, &renderQuad );
}

// lấy giá trị chiều ngang texture
int LTexture::getWidth()
{
	return mWidth;
}

// lấy chiều dọc
int LTexture::getHeight()
{
	return mHeight;
}

// cài đặt giá trị chiều ngang
void LTexture::setWidth(int w){
	this->mWidth = w;
}
// cài đặt giá trị chiều dọc
void LTexture::setHeight(int h){
	this->mHeight = h;
}

// khởi tạo SDL
bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHTPLUS, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				 //Initialize SDL_mixer
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
					success = false;
				}

				//Initialize SDL_ttf
				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

//load dữ liệu
bool loadMedia()
{
	//Loading success flag
	bool success = true;

	if( !gBackground[0].loadFromFile( "picture/snake_background.png" )){
		printf( "Failed to load snake_background' texture image!\n" );
		success = false;
	}
	if( !gBackground[1].loadFromFile( "picture/background1.png" )){
		printf( "Failed to load background1' texture image!\n" );
		success = false;
	}
	if( !gBlock.loadFromFile( "picture/Gold_Block.png" )){
		printf( "Failed to load gBlock' texture image!\n" );
		success = false;
	}

	//load gBbutton
	if( !gBbutton.loadFromFile( "picture/button.png" )){
		printf( "Failed to load gBbutton' texture image!\n" );
		success = false;
	}

	//Load Apple' texture
	if( !gApple.loadFromFile( "picture/apple.png" )){
		printf( "Failed to load Apple' texture image!\n" );
		success = false;
	}

	//Load HeadSnake texture
	if( !gSnake.loadFromFile( "picture/snake.png" )){
		printf( "Failed to load Snake' texture image!\n" );
		success = false;
	}

	if( !gHeadSnake_right.loadFromFile( "picture/headSnake_right.png" )){
		printf( "Failed to load HeadSnake' texture image!\n" );
		success = false;
	}

	if( !gHeadSnake_left.loadFromFile( "picture/headSnake_left.png" )){
		printf( "Failed to load HeadSnake' texture image!\n" );
		success = false;
	}

	if( !gHeadSnake_down.loadFromFile( "picture/headSnake_down.png" )){
		printf( "Failed to load HeadSnake' texture image!\n" );
		success = false;
	}

	if( !gHeadSnake_up.loadFromFile( "picture/headSnake_up.png" )){
		printf( "Failed to load HeadSnake' texture image!\n" );
		success = false;
	}

	//Load BodySnake texture
	if( !gBodySnake_ver.loadFromFile( "picture/bodySnake_ver.png" )){
		printf( "Failed to load BodySnake' texture image!\n" );
		success = false;
	}

	if( !gBodySnake_hor.loadFromFile( "picture/bodySnake_hor.png" )){
		printf( "Failed to load BodySnake' texture image!\n" );
		success = false;
	}

	//Load TailSnake texture
	if( !gTailSnake_right.loadFromFile( "picture/tailSnake_right.png" )){
		printf( "Failed to load TailSnake' texture image!\n" );
		success = false;
	}

	if( !gTailSnake_left.loadFromFile( "picture/tailSnake_left.png" )){
		printf( "Failed to load TailSnake' texture image!\n" );
		success = false;
	}

	if( !gTailSnake_down.loadFromFile( "picture/tailSnake_down.png" )){
		printf( "Failed to load TailSnake' texture image!\n" );
		success = false;
	}

	if( !gTailSnake_up.loadFromFile( "picture/tailSnake_up.png" )){
		printf( "Failed to load TailSnake' texture image!\n" );
		success = false;
	}

	//Load midSnake texture
	if( !gMidSnake_leftup.loadFromFile( "picture/mid_leftup_downright.png" )){
		printf( "Failed to load MidSnake' texture image!\n" );
		success = false;
	}
	if( !gMidSnake_rightup.loadFromFile( "picture/mid_rightup_downleft.png" )){
		printf( "Failed to load MidSnake' texture image!\n" );
		success = false;
	}
	if( !gMidSnake_upleft.loadFromFile( "picture/mid_upleft_rightdown.png" )){
		printf( "Failed to load MidSnake' texture image!\n" );
		success = false;
	}
	if( !gMidSnake_upright.loadFromFile( "picture/mid_upright_leftdown.png" )){
		printf( "Failed to load MidSnake' texture image!\n" );
		success = false;
	}

	if(!gEnter.loadFromFile("picture/EnterPlay.png")){
		printf( "Failed to load Enter' texture image!\n" );
		success = false;
	}

	if(!gTextureButton[0].loadFromFile("picture/button_start.png")){
		printf( "Failed to load ButtonStart' texture image!\n" );
		success = false;
	}

	//Buttons
	if(!gTextureButton[1].loadFromFile("picture/button_score.png")){
		printf( "Failed to load ButtonScore' texture image!\n" );
		success = false;
	}

	if(!gTextureButton[2].loadFromFile("picture/button_exit.png")){
		printf( "Failed to load ButtonExit' texture image!\n" );
		success = false;
	}

	if(!gSelect.loadFromFile("picture/button_select.png")){
		printf( "Failed to load Select' texture image!\n" );
		success = false;
	}

	//Flame
	if(!gFlame[0].loadFromFile("picture/flame.png")){
		printf( "Failed to load Flame' texture image!\n" );
		success = false;
	}

	
	if(!gFlame[1].loadFromFile("picture/flame1.png")){
		printf( "Failed to load Flame1' texture image!\n" );
		success = false;
	}


	//back ground menu
	if(!gBackGround.loadFromFile("picture/backGroundMenu.png")){
		printf( "Failed to load BackGround' texture image!\n" );
		success = false;
	}

	//Load sound effects
	gScratch = Mix_LoadWAV( "sound/scratch.wav" );
	if( gScratch == NULL )
	{
		printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gHigh = Mix_LoadWAV( "sound/high.wav" );
	if( gHigh == NULL )
	{
		printf( "Failed to load high sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	//Load music
	gMusic = Mix_LoadMUS( "sound/03ptalit.mid" );
	if( gMusic == NULL )
	{
		printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gMusicMenu = Mix_LoadMUS( "sound/1_rosann.mid" );
	if( gMusic == NULL )
	{
		printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gFont = TTF_OpenFont( "font/comic.ttf", 100 );
	if( gFont == NULL )
	{
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	//Nothing to load
	return success;
}

//kết thúc SDL
void close()
{
	gBackground[0].free();
	gBackground[1].free();
	gBlock.free();
	gBbutton.free();

	//Free loaded Apple images
	gApple.free();
	
	//Free loaded Snake images
	gHeadSnake_right.free();
	gHeadSnake_left.free();
	gHeadSnake_up.free();
	gHeadSnake_down.free();

	gBodySnake_ver.free();
	gBodySnake_hor.free();

	gTailSnake_right.free();
	gTailSnake_left.free();
	gTailSnake_up.free();
	gTailSnake_down.free();

	gMidSnake_leftup.free();
	gMidSnake_rightup.free();
	gMidSnake_upleft.free();
	gMidSnake_upright.free();
	
	for(int i=0; i < TOTAL_BUTTONS; i++)
		gTextureButton[i].free();
	gFlame[0].free();
	gFlame[1].free();

	gBackGround.free();
	gSelect.free();
	//Free the sound effects
	Mix_FreeChunk( gScratch );
	Mix_FreeChunk( gHigh );
	gScratch = NULL;
	gHigh = NULL;

	//Free the music
	Mix_FreeMusic( gMusic );
	gMusic = NULL;

	Mix_FreeMusic( gMusicMenu );
	gMusicMenu = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

// vẽ map
void DrawMap(int num){
	SDL_SetRenderDrawColor( gRenderer, 0x97, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear(gRenderer);

	for(int x = 0; x < SCREEN_WIDTH; x += FACTOR){
		gBlock.renderPicture(x, 0);
		gBlock.renderPicture(x, SCREEN_HEIGHT - FACTOR);
	}
	for(int y = 0; y < SCREEN_HEIGHT; y += FACTOR){
		gBlock.renderPicture(0, y);
		gBlock.renderPicture(SCREEN_WIDTH - FACTOR, y);
	}
	SDL_Rect wallAround = { FACTOR, FACTOR, SCREEN_WIDTH-FACTOR*2, SCREEN_HEIGHT-FACTOR*2 };
	SDL_SetRenderDrawColor( gRenderer, 199, 149, 122, 0xFF );
	SDL_RenderFillRect( gRenderer, &wallAround);

	gBackground[1].setWidth(SCREEN_WIDTH);
	gBackground[1].setHeight(SCREEN_HEIGHTPLUS - SCREEN_HEIGHT);
	gBackground[1].render(0, SCREEN_HEIGHT);

	gBackground[0].setWidth(SCREEN_WIDTH/2);
	gBackground[0].setHeight(SCREEN_HEIGHTPLUS - SCREEN_HEIGHT);
	gBackground[0].renderPicture(SCREEN_WIDTH/4, SCREEN_HEIGHT);
	for(int x =  SCREEN_WIDTH/4; x <= SCREEN_WIDTH-(SCREEN_WIDTH/4); x += FACTOR)
		gFlame[num].render(x, SCREEN_HEIGHT/4);
	for(int y = SCREEN_HEIGHT/4; y <= SCREEN_HEIGHT-(SCREEN_HEIGHT/4); y += FACTOR)
		gFlame[num].render( SCREEN_WIDTH/2, y);
}

// vẽ rắn
void DrawSnake(){
	
	//Draw snake
	for(int i = 0; i < snake.getLengthSnake(); i++){
		if((i != snake.getLengthSnake() - 1) && (snake.getSnakeX()[i-1] != snake.getSnakeX()[i+1]) && (snake.getSnakeY()[i-1] != snake.getSnakeY()[i+1]) && ((snake.getDirectionSnake()[i-1] == dright && snake.getDirectionSnake()[i] == dup) || (snake.getDirectionSnake()[i-1] == ddown && snake.getDirectionSnake()[i] == dleft))){
			gMidSnake_leftup.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
		}
		else if((i != snake.getLengthSnake() - 1) && (snake.getSnakeX()[i-1] != snake.getSnakeX()[i+1]) && (snake.getSnakeY()[i-1] != snake.getSnakeY()[i+1]) && ((snake.getDirectionSnake()[i-1] == ddown && snake.getDirectionSnake()[i] == dright) || (snake.getDirectionSnake()[i-1] == dleft && snake.getDirectionSnake()[i] == dup))){
			gMidSnake_rightup.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
		}
		else if((i != snake.getLengthSnake() - 1) && (snake.getSnakeX()[i-1] != snake.getSnakeX()[i+1]) && (snake.getSnakeY()[i-1] != snake.getSnakeY()[i+1]) && ((snake.getDirectionSnake()[i-1] == dright && snake.getDirectionSnake()[i] == ddown) || (snake.getDirectionSnake()[i-1] == dup && snake.getDirectionSnake()[i] == dleft))){
			gMidSnake_upleft.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
		}
		else if((i != snake.getLengthSnake() - 1) && (snake.getSnakeX()[i-1] != snake.getSnakeX()[i+1]) && (snake.getSnakeY()[i-1] != snake.getSnakeY()[i+1]) && ((snake.getDirectionSnake()[i-1] == dup && snake.getDirectionSnake()[i] == dright) || (snake.getDirectionSnake()[i-1] == dleft && snake.getDirectionSnake()[i] == ddown))){
			gMidSnake_upright.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
		}
		else if(snake.getDirectionSnake()[i] == dright){
			if(i == 0){
				gTailSnake_right.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
			}
			else if( i == snake.getLengthSnake() - 1){
				gHeadSnake_right.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
			}
			else{
				gBodySnake_hor.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
			}
		}
		else if(snake.getDirectionSnake()[i] == dleft){
			if(i == 0){
				gTailSnake_left.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
			}
			else if( i == snake.getLengthSnake() - 1){
				gHeadSnake_left.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
			}
			else{
				gBodySnake_hor.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
			}
		}
		else if(snake.getDirectionSnake()[i] == dup){
			if(i == 0){
				gTailSnake_up.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
			}
			else if( i == snake.getLengthSnake() - 1){
				gHeadSnake_up.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
			}
			else{
				gBodySnake_ver.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
			}
		}
		else if(snake.getDirectionSnake()[i] == ddown){
			if(i == 0){
				gTailSnake_down.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
			}
			else if( i == snake.getLengthSnake() - 1){
				gHeadSnake_down.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
			}
			else{
				gBodySnake_ver.render(snake.getSnakeX()[i] * FACTOR, snake.getSnakeY()[i] * FACTOR);
			}
		}
	}

	//delete old tail
	SDL_Rect Snake;
	if(snake.getSnakeXo()[0] != 0 && snake.getSnakeY()[0] != 0){
	Snake.x = snake.getSnakeXo()[0]*FACTOR;
	Snake.y = snake.getSnakeYo()[0]*FACTOR;
	Snake.w = FACTOR;
	Snake.h = FACTOR;
	SDL_SetRenderDrawColor( gRenderer, 199, 149, 122, 0xFF );
	SDL_RenderFillRect( gRenderer, &Snake);
	}
}

// vẽ táo
void DrawApple(){
	SDL_Rect Apple;
	Apple.x = apple.getAppleX()*FACTOR;
	Apple.y = apple.getAppleY()*FACTOR;
	Apple.w = FACTOR;
	Apple.h = FACTOR;
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
	SDL_RenderFillRect( gRenderer, &Apple);
}

