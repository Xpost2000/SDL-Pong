// SDL_Pong.cpp : Defines the entry point for the console application.
// This is going to be a fairly C based application written with a few C++ features

#include "stdafx.h"
#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <SDL_image.h>
#include <cmath>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
// Has STDIO
// BUTTON INPUT EVENTS
const int event_start = 1; // Start Game
const int event_quit = 2; // Quit Game
const int event_concede = 3; // End Game
const int event_options = 4; // Options Menu
const int event_inc = 0x4AB; // Increment Counter
const int event_dec = 0x3AB; // Decrement Counter
const int event_menuRet = 5;
int winner = 0;
int lastHit = 0;
int flag = 0;
bool cancer = false;
bool ai = false;
int speed = 45;
int lScore = 0;
int rScore = 0;
Mix_Music *music = NULL;
Mix_Chunk *beep = NULL;
void checkWinner();
struct ball
{
	SDL_Rect rect;
	int x, y; // Obvious
	int w, h; // Obvious
	int vx = 15; // Velocity or speed in this case
	int vy = 0; // default
};
const int boxWidth = 600;
const int boxHeight = 300;
struct button
{
	SDL_Rect rect;
	SDL_Texture* tex;
	int w, h;
}startButton, quitButton, optionButton, incButton, decButton;
struct paddle
{
	SDL_Rect rect;
	int x, y;
	int w, h;
	int n; // id
	void collide(ball &a)
	{
		// check for only X axis collision
		if ((a.rect.x < rect.x + rect.w &&
			a.rect.x + a.rect.w > rect.x)& (a.rect.y < rect.y + rect.h &&
			a.rect.y + a.rect.h > rect.y))
		{
			a.vx = -a.vx;
			a.vy = -a.vy;
			Mix_PlayChannel(-1, beep, 0);
			int ad = 15;
			printf("Collision Detected. Reversing Direction\n");
			a.vy = (rand() % ad) % rect.y - 5;
			lastHit = n;
			if ( speed != 30)
			speed-=3;
		}
	}
};
// Variables
const int width = 1024;
const int height = 768;
// General Paddle Variables
const int startpX = width / 5; 
const int startpY = height / 2 - 150;
const int pW = 10;
const int pH = 100;
// General Ball Variables
// MIDDLE OF SCREEN
const int startbX = width / 2;
const int startbY = height / 2;
const int bSize = 30;
int menu = 1; // FLAG
void processInput();
void checkInput(button a, int type);
void cleanUp();
void floorCollide(ball &a, paddle &x, paddle &y);
SDL_Window* window;
SDL_Renderer* rend;
SDL_Event event;
ball b;
paddle pl, pr; // a left, b right
SDL_Texture* title[2];
SDL_Surface* loader;
SDL_Rect titleScr;

SDL_Texture* winLst[3]; // Texture
SDL_Rect lstWinner;
SDL_Texture* concede[3];
SDL_Surface* msg, num; // FOR TEXT
TTF_Font* font = NULL;
SDL_Color fontColor = { 0, 0, 0 };
SDL_Color fontColor1 = { 0, 0, 255 };
SDL_Color fontColor2 = { 0, 255, 0 };
int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	if (!IMG_Init(IMG_INIT_PNG)){
		printf("%s,", IMG_GetError());
	}
	Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2048);
	TTF_Init();
	music = Mix_LoadMUS("assests\\audio\\Partylikeits1980.wav");
	beep = Mix_LoadWAV("assests\\audio\\beep.wav");
	b.h = bSize/2;
	b.w = bSize/2;
	b.x = startbX;
	b.y = startbY;

	pl.x = startpX - 190;
	pl.y = startpY;
	pl.w = pW;
	pl.h = pH;
	pr.x = startpX + 800;
	pr.y = startpY;
	pr.w = pW;
	pr.h = pH;

	pr.n = 2;
	pl.n = 1;

	b.rect.h = b.h;
	b.rect.w = b.w;
	b.rect.x = b.x;
	b.rect.y = b.y;

	pl.rect.h = pl.h;
	pl.rect.w = pl.w;
	pl.rect.x = pl.x;
	pl.rect.y = pl.y;

	pr.rect.h = pr.h;
	pr.rect.w = pr.w;
	pr.rect.x = pr.x;
	pr.rect.y = pr.y;

	startButton.w = boxWidth/2;
	startButton.h = boxHeight/2;
	quitButton.w = boxWidth/2;
	quitButton.h = boxHeight/2;

	optionButton.w = boxWidth / 2;
	optionButton.h = boxHeight / 2;

	//SETUP ABOVE
	window = SDL_CreateWindow("Pong by Jerry Zhu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_BORDERLESS);
	rend = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC);
	loader = IMG_Load("assests\\menu\\title.png");
	title[0] = SDL_CreateTextureFromSurface(rend, loader);
	loader = IMG_Load("assests\\menu\\title2.png");
	title[1] = SDL_CreateTextureFromSurface(rend, loader);
	loader = IMG_Load("assests\\menu\\start.png");
	startButton.tex = SDL_CreateTextureFromSurface(rend, loader);
	loader = IMG_Load("assests\\menu\\quit.png");
	quitButton.tex = SDL_CreateTextureFromSurface(rend, loader);
	loader = IMG_Load("assests\\menu\\options.png");
	optionButton.tex = SDL_CreateTextureFromSurface(rend, loader);
	loader = IMG_Load("assests\\menu\\cond1.png");
	winLst[0] = SDL_CreateTextureFromSurface(rend, loader);
	loader = IMG_Load("assests\\menu\\cond2.png");
	winLst[1] = SDL_CreateTextureFromSurface(rend, loader);
	loader = IMG_Load("assests\\menu\\cond3.png");
	winLst[2] = SDL_CreateTextureFromSurface(rend, loader);
	loader = IMG_Load("assests\\menu\\tie.png");
	concede[0] = SDL_CreateTextureFromSurface(rend, loader);
	loader = IMG_Load("assests\\menu\\winL.png");
	concede[1] = SDL_CreateTextureFromSurface(rend, loader);
	loader = IMG_Load("assests\\menu\\winR.png");
	concede[2] = SDL_CreateTextureFromSurface(rend, loader);
	lstWinner.w = 350;
	lstWinner.h = 60;
	lstWinner.x = 380;
	lstWinner.y = 585;
	font = TTF_OpenFont("assests\\font\\8-BIT WONDER.TTF", 10);
	msg = TTF_RenderText_Solid(font, "This is the options menu", fontColor);
	SDL_Surface* msg2;
	SDL_Surface* diff[4];
	msg2 = TTF_RenderText_Solid(font, "Change the game speed ( arrow keys )", fontColor);
	diff[0] = TTF_RenderText_Solid(font, "Easy(speed is 15)", fontColor1);
	diff[1] = TTF_RenderText_Solid(font, "Normal(speed is 25)", fontColor1);
	diff[2] = TTF_RenderText_Solid(font, "Hard(speed is 35)", fontColor1);
	diff[3] = TTF_RenderText_Solid(font, "GIANTDAD(speed is 40)", fontColor1);
	SDL_Surface* gmp; // too messages
	SDL_Texture* gm;
	gmp = TTF_RenderText_Solid(font, "A**hole Pong has been enabled", fontColor2);
	SDL_Texture* fntTx, *fntTx1;
	SDL_Texture* dif[4];

	dif[0] = SDL_CreateTextureFromSurface(rend, diff[0]);
	dif[1] = SDL_CreateTextureFromSurface(rend, diff[1]);
	dif[2] = SDL_CreateTextureFromSurface(rend, diff[2]);
	dif[3] = SDL_CreateTextureFromSurface(rend, diff[3]);
	gm = SDL_CreateTextureFromSurface(rend, gmp);
	fntTx = SDL_CreateTextureFromSurface(rend, msg);
	fntTx1 = SDL_CreateTextureFromSurface(rend, msg2);
	SDL_Rect box;
	box.w = width;
	box.h = 100;
	box.x = 0;
	box.y = 0;
	SDL_Rect box1;
	box1.w = width;
	box1.h = 100;
	box1.x = 0;
	box1.y = 100;
	SDL_Rect box2;
	box2.w = width;
	box2.h = 100;
	box2.x = 0;
	box2.y = 260;
	// pretty redundant
	//MessageBoxA(NULL, "This is a pong game made by Jerry Zhu\n It gets faster every paddle hit so yeah...", "Info", MB_ICONINFORMATION);
	while (true)
	{
	//	if (Mix_PlayingMusic() == 0)
	//		Mix_PlayMusic(music, 5000);
		if (menu == 1)
		{
			Mix_PauseMusic();
			titleScr.w = boxWidth;
			titleScr.h = boxHeight;
			titleScr.x = 225;
			titleScr.y = 15;

			startButton.rect.w = startButton.w;
			startButton.rect.h = startButton.h;
			optionButton.rect.w = optionButton.w;
			optionButton.rect.h = optionButton.h;

			startButton.rect.x = (width / 2) - 150;
			quitButton.rect.x = (width / 2) - 150;
			optionButton.rect.x = (width / 2) - 470;
			startButton.rect.y = 400;
			quitButton.rect.y = 600;
			optionButton.rect.y = 500;
			quitButton.rect.w = quitButton.w;
			quitButton.rect.h = quitButton.h;

			// IMPLEMENT MENU
			SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
			SDL_RenderClear(rend);
			SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
			SDL_RenderCopy(rend, title[ai], NULL, &titleScr);

			SDL_RenderCopy(rend, startButton.tex, NULL, &startButton.rect);

			SDL_RenderCopy(rend, quitButton.tex, NULL, &quitButton.rect);
			SDL_RenderCopy(rend, optionButton.tex, NULL, &optionButton.rect);
			checkInput(startButton, event_start);
			checkInput(quitButton, event_quit);
			checkInput(optionButton, event_options);
			SDL_RenderPresent(rend);
			processInput();
			SDL_Delay(1);
		}
		else if (menu == 2)
		{
			box2.y = 260;
			SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
			SDL_RenderClear(rend);
			SDL_RenderCopy(rend, fntTx, NULL, &box);
			SDL_RenderCopy(rend, fntTx1, NULL, &box1);
			SDL_RenderCopy(rend, dif[flag], NULL, &box2);
			SDL_RenderCopy(rend, startButton.tex, NULL, &startButton.rect);
			checkInput(startButton, event_start);
			SDL_RenderPresent(rend);
			processInput();
			SDL_Delay(1);
		}
			else if ( menu == 0 ){
				box2.y = 130;
			quitButton.rect.h = boxHeight / 4;
			quitButton.rect.w = boxWidth / 4;
			quitButton.rect.y = 650;
			quitButton.rect.x = 475;
			titleScr.h = boxHeight / 3;
			titleScr.w = boxWidth / 3 + 35;
			titleScr.x = 400;

			srand(time(NULL));
			SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
			SDL_RenderClear(rend);
			SDL_RenderCopy(rend, title[ai], NULL, &titleScr);
			if (cancer == true)
			SDL_RenderCopy(rend, gm, NULL, &box2);

			SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
			SDL_RenderFillRect(rend, &pr.rect);
			SDL_SetRenderDrawColor(rend, 0, 0, 255, 255);
			SDL_RenderFillRect(rend, &pl.rect);
			SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
			SDL_RenderFillRect(rend, &b.rect);
			SDL_RenderCopy(rend, quitButton.tex, NULL, &quitButton.rect);
			SDL_RenderCopy(rend, winLst[winner], NULL, &lstWinner);
			SDL_RenderPresent(rend);
			b.rect.x += b.vx;
			printf("ball X = %d\n", b.rect.x);
			b.rect.y += b.vy;
			printf("ball Y = %d\n", b.rect.y);

			if (ai == true)
			{
				if (flag == 3 || 2){ // UNBEATABLE AI
					if (b.rect.x > 450)
						pr.rect.y = b.rect.y - 50; // Start The Move
				}
				if (flag == 1 || 0)
				{
					if (b.rect.x > 500)
						if (pr.rect.y != b.rect.y)
							pr.rect.y += 10;
				}
			}
			pr.collide(b);
			pl.collide(b);
			floorCollide(b, pl, pr);
			checkInput(quitButton, event_concede);
			processInput();
			SDL_Delay(speed);
		}
	}
	Mix_Quit();
	SDL_Quit();
	return 0;
}

void processInput()
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			cleanUp();
			exit(1);
			return;
		}
		if (menu == 1)
		{
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_x)
				{
					ai = true;
				}
				if (event.key.keysym.sym == SDLK_c)
				{
					ai = false;
				}
				if (event.key.keysym.sym == SDLK_0)
				{
					cancer = true;
				}
				if (event.key.keysym.sym == SDLK_1)
				{
					cancer = false;
				}
			}
		}
		if (menu == 2)
		{
			if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.sym == SDLK_RIGHT)
				{
					if (flag != 3){
						b.vx += 10;
						flag++;
					}
				}
				if (event.key.keysym.sym == SDLK_LEFT)
				{
					if (flag != 0){
						b.vx -= 10;
						if (b.vx >= 40)
							b.vx = 40;
						flag--;
					}
				}
			}
		}
	}
	const int speed = 35;
	if (state[SDL_SCANCODE_W])
	{
		if (!(pl.rect.y < 0))
		pl.rect.y -= speed;
	}
	if (state[SDL_SCANCODE_S])
	{
		if (!(pl.rect.y + pl.rect.h > height))
		pl.rect.y += speed;
	}
	if (cancer == true)
	{
		if (state[SDL_SCANCODE_A])
		{
				pl.rect.x -= speed;
		}
		if (state[SDL_SCANCODE_D])
		{
				pl.rect.x += speed;
		}
	}
	if (ai == false){
		if (state[SDL_SCANCODE_UP])
		{
			if (!(pr.rect.y < 0))
				pr.rect.y -= speed;
		}
		if (state[SDL_SCANCODE_DOWN])
		{
			if (!(pr.rect.y + pr.rect.h > height))
				pr.rect.y += speed;
		}
	}
}

void checkWinner()
{
	// Check the winner of last round
	if (lastHit == 1){
		winner = 1;
		lScore++;
	}
	if (lastHit == 2){
		winner = 2;
		rScore++;
	}
	if (lastHit == 0)
	{
		winner = 0;
	}
}

void floorCollide(ball &a, paddle &x, paddle &y) // pl pr
{
	if (a.rect.x > 1024 || a.rect.x < 0){
		printf("Hit the walls... Resetting\n");
		a.rect.x = width / 2;
		a.rect.y = height / 2;
		speed = 45;
		checkWinner();
		x.rect.y = startpY;
		y.rect.y = startpY;
		b.vy = 0;
		pl.rect.x = pl.x;
		pr.rect.x = pr.x;
		SDL_Delay(500);
		lastHit = 0;
	}
	if (a.rect.y > 768 || a.rect.y < 0){
		printf("Hit the ceiling\n");
		a.vy = -a.vy;
		Mix_PlayChannel(-1, beep, 0);
	}
}
void checkInput(button a, int type) // Button prototype
{
	if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
	{
		int x, y; // store
		bool inside = true;
		SDL_GetMouseState(&x, &y);
		if (x > a.rect.x + a.rect.w){
			printf("Mouse is Outside\n");
			inside = false;
		}
		else if (x < a.rect.x){
			printf("Mouse is Outside\n");
			inside = false;
		}
		if (y > a.rect.y + a.rect.h){
			printf("Mouse is Outside\n");
			inside = false;
		}
		else if (y < a.rect.y){
			printf("Mouse is Outside\n");
			inside = false;
		}
		if (inside == true){
			printf("Mouse is inside\n");
			SDL_SetTextureColorMod(a.tex, 220, 0, 155);
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (type == event_quit){
					cleanUp();
					exit(0);
				}
				if (type == event_start)
					menu = 0;
				if (type == event_options)
					menu = 2; // OPTIONS MENU
				if (type == event_inc)
					b.vx++;
				if (type == event_dec)
					b.vx--;
				if (type == event_menuRet)
					menu = 1;
				if (type == event_concede)
				{
					menu = 1337;
					Mix_PlayMusic(music, 1);
					if (lScore == rScore){
						SDL_RenderCopy(rend, concede[0], NULL, NULL);
						SDL_RenderPresent(rend);
						SDL_Delay(1550);
						menu = 1;
						ai = 0;
						rScore = 0;
						lScore = 0;
						lastHit = 0;
						winner = 0;
					}
					if (lScore > rScore)
					{
						SDL_RenderCopy(rend, concede[1], NULL, NULL);
						SDL_RenderPresent(rend);
						SDL_Delay(1550);
						menu = 1;
						ai = 0;
						rScore = 0;
						lScore = 0;
						lastHit = 0;
						winner = 0;
					}
					if (rScore > lScore)
					{
						SDL_RenderCopy(rend, concede[2], NULL, NULL);
						SDL_RenderPresent(rend);
						SDL_Delay(1550);
						menu = 1;
						ai = 0;
						rScore = 0;
						lScore = 0;
						lastHit = 0;
						winner = 0;
					}
				}
			}
		}
		else if ( inside == false )
		SDL_SetTextureColorMod(a.tex, 255, 255, 255);
	}
}

void cleanUp()
{
	// Hardly proper cleanup but eh.
	SDL_FreeSurface(loader);
	TTF_CloseFont(font);
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
	TTF_Quit();
}