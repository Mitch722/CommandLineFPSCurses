/*
	OneLoneCoder.com - Command Line First Person Shooter (FPS) Engine
	"Why were games not done like this is 1990?" - @Javidx9

	License
	~~~~~~~
	Copyright (C) 2018  Javidx9
	This program comes with ABSOLUTELY NO WARRANTY.
	This is free software, and you are welcome to redistribute it
	under certain conditions; See license for details. 
	Original works located at:
	https://www.github.com/onelonecoder
	https://www.onelonecoder.com
	https://www.youtube.com/javidx9

	GNU GPLv3
	https://github.com/OneLoneCoder/videos/blob/master/LICENSE

	From Javidx9 :)
	~~~~~~~~~~~~~~~
	Hello! Ultimately I don't care what you use this for. It's intended to be 
	educational, and perhaps to the oddly minded - a little bit of fun. 
	Please hack this, change it and use it in any way you see fit. You acknowledge 
	that I am not responsible for anything bad that happens as a result of 
	your actions. However this code is protected by GNU GPLv3, see the license in the
	github repo. This means you must attribute me if you use it. You can view this
	license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
	Cheers!

	Background
	~~~~~~~~~~
	Whilst waiting for TheMexicanRunner to start the finale of his NesMania project,
	his Twitch stream had a counter counting down for a couple of hours until it started.
	With some time on my hands, I thought it might be fun to see what the graphical
	capabilities of the console are. Turns out, not very much, but hey, it's nice to think
	Wolfenstein could have existed a few years earlier, and in just ~200 lines of code.

	IMPORTANT!!!!
	~~~~~~~~~~~~~
	READ ME BEFORE RUNNING!!! This program expects the console dimensions to be set to 
	120 Columns by 40 Rows. I recommend a small font "Consolas" at size 16. You can do this
	by running the program, and right clicking on the console title bar, and specifying 
	the properties. You can also choose to default to them in the future.
	
	Controls: A = Turn Left, D = Turn Right, W = Walk Forwards, S = Walk Backwards

	Future Modifications
	~~~~~~~~~~~~~~~~~~~~
	1) Shade block segments based on angle from player, i.e. less light reflected off
	walls at side of player. Walls straight on are brightest.
	2) Find an interesting and optimised ray-tracing method. I'm sure one must exist
	to more optimally search the map space
	3) Add bullets!
	4) Add bad guys!

	Author
	~~~~~~
	Twitter: @javidx9
	Blog: www.onelonecoder.com

	Video:
	~~~~~~	
	https://youtu.be/xW8skO7MFYw

	Last Updated: 27/02/2017
*/

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include <chrono>
using namespace std;

#include <stdio.h>
// #include <Windows.h>
#include<curses.h>

int nScreenWidth = 120;			// Console Screen Size X (columns)
int nScreenHeight = 40;			// Console Screen Size Y (rows)
int nMapWidth = 16;				// World Dimensions
int nMapHeight = 16;

float fPlayerX = 14.7f;			// Player Start Position
float fPlayerY = 5.09f;
float fPlayerA = 0.0f;			// Player Start Rotation
float fFOV = 3.14159f / 4.0f;	// Field of View
float fDepth = 16.0f;			// Maximum rendering distance
float fSpeed = 2.0f;			// Walking Speed


// render screen using curses
void renderConsole(wchar_t* screen, const int screenRows, const int screenCols) {

	// index to extract character
	int index = 0;

	// iterate col by col
	for (int row=0; row<screenRows; row++) {
		// iterate each row
		 for(int col=0; col<screenCols; col++) {
			// extract char
			wchar_t individualChar = screen[index];
			// add char to screen
			mvaddch(row, col, individualChar);
						
			// update index
			index++;
		}
	}
	// refresh screen
	refresh();
}


void toScreen(char*& screen, const int index, const string& text) {
  for (int i = 0; i < text.size(); i++)
    screen[index + i] = text.at(i);
};

int main()
{	

	/* initialize curses */
    initscr();
    cbreak();
    noecho();
    clear();
    /* get terminal size from curses */
    int maxlines = LINES - 1;
    int maxcols = COLS - 1;

	wchar_t *screenVec = new wchar_t[nScreenWidth*nScreenHeight];

	// Create Map of world space # = wall block, . = space
	wstring map;
	map += L"#########.......";
	map += L"#...............";
	map += L"#.......########";
	map += L"#..............#";
	map += L"#......##......#";
	map += L"#......##......#";
	map += L"#..............#";
	map += L"###............#";
	map += L"##.............#";
	map += L"#......####..###";
	map += L"#......#.......#";
	map += L"#......#.......#";
	map += L"#..............#";
	map += L"#......#########";
	map += L"#..............#";
	map += L"################";
	// map += L"################";
	// map += L"#..............#";
	// map += L"#..............#";
	// map += L"#..............#";
	// map += L"#......##......#";
	// map += L"#......##......#";
	// map += L"#..............#";
	// map += L"#..............#";
	// map += L"#..............#";
	// map += L"#......####....#";
	// map += L"#..............#";
	// map += L"#..............#";
	// map += L"#..............#";
	// map += L"#..............#";
	// map += L"#..............#";
	// map += L"################";

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();
	
	while (1)
	{
		// We'll need time differential per frame to calculate modification
		// to movement speeds, to ensure consistant movement, as ray-tracing
		// is non-deterministic
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();

		for (int x = 0; x < nScreenWidth; x++)
		{
			// For each column, calculate the projected ray angle into world space
			float fRayAngle = (fPlayerA - fFOV/2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

			// Find distance to wall
			float fStepSize = 0.1f;		  // Increment size for ray casting, decrease to increase										
			float fDistanceToWall = 0.0f; //                                      resolution

			bool bHitWall = false;		// Set when ray hits wall block
			bool bBoundary = false;		// Set when ray hits boundary between two wall blocks

			float fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
			float fEyeY = cosf(fRayAngle);

			// Incrementally cast ray from player, along ray angle, testing for 
			// intersection with a block
			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += fStepSize;
				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);
				
				// Test if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true;			// Just set distance to maximum depth
					fDistanceToWall = fDepth;
				}
				else
				{
					// Ray is inbounds so test to see if the ray cell is a wall block
					if (map.c_str()[nTestX * nMapWidth + nTestY] == '#')
					{
						// Ray has hit wall
						bHitWall = true;

						// To highlight tile boundaries, cast a ray from each corner
						// of the tile, to the player. The more coincident this ray
						// is to the rendering ray, the closer we are to a tile 
						// boundary, which we'll shade to add detail to the walls
						vector<pair<float, float>> p;

						// Test each corner of hit tile, storing the distance from
						// the player, and the calculated dot product of the two rays
						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++)
							{
								// Angle of corner to eye
								float vy = (float)nTestY + ty - fPlayerY;
								float vx = (float)nTestX + tx - fPlayerX;
								float d = sqrt(vx*vx + vy*vy); 
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(make_pair(d, dot));
							}

						// Sort Pairs from closest to farthest
						sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right) {return left.first < right.first; });
						
						// First two/three are closest (we will never see all four)
						float fBound = 0.01;
						if (acos(p.at(0).second) < fBound) bBoundary = true;
						if (acos(p.at(1).second) < fBound) bBoundary = true;
						if (acos(p.at(2).second) < fBound) bBoundary = true;
					}
				}
			}
		
			// Calculate distance to ceiling and floor
			int nCeiling = (float)(nScreenHeight/2.0) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			// Shader walls based on distance
			short nShade = ' ';
			if (fDistanceToWall <= fDepth / 4.0f)			nShade = '#';	// Very close	
			else if (fDistanceToWall < fDepth / 3.0f)		nShade = '+';
			else if (fDistanceToWall < fDepth / 2.0f)		nShade = '*';
			else if (fDistanceToWall < fDepth)				nShade = '.';
			else											nShade = ' ';	// Too far away

			if (bBoundary)		nShade = ' '; // Black it out
			
			for (int y = 0; y < nScreenHeight; y++)
			{
				// Each Row
				if(y <= nCeiling)
					screenVec[y*nScreenWidth + x] = ' ';
				else if(y > nCeiling && y <= nFloor)
					screenVec[y*nScreenWidth + x] = nShade;
				else // Floor
				{				
					// Shade floor based on distance
					float b = 1.0f - (((float)y -nScreenHeight/2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25)		nShade = 'X';
					else if (b < 0.5)	nShade = ':';
					else if (b < 0.75)	nShade = '~';
					else if (b < 0.9)	nShade = '-';
					else				nShade = ' ';
					screenVec[y*nScreenWidth + x] = nShade;
				}
			}
		}
		
		// // Display Stats
		// swprintf_s(screenVec, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", fPlayerX, fPlayerY, fPlayerA, 1.0f/fElapsedTime);
		
		
		// Display Map
		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapWidth; ny++)
			{
				screenVec[(ny+1)*nScreenWidth + nx] = map[ny * nMapWidth + nx];
			}
		screenVec[((int)fPlayerX+1) * nScreenWidth + (int)fPlayerY] = 'P';

		// Display Frame
		screenVec[nScreenWidth * nScreenHeight - 1] = '\0';
		renderConsole(screenVec, nScreenHeight, nScreenWidth);
		
		// add frame rate
		string frameRate = to_string(1.0f/fElapsedTime);
		char* frameRate_ptr = &frameRate[0];
		mvaddstr(0., 0., frameRate_ptr);
		refresh();

		// add player inputs
		int playerInput = getch();

		// Handle CCW Rotation ascii a(97)
		if (playerInput == 97) {
			fPlayerA -= (fSpeed * 0.5f) * fElapsedTime;
		}

		// Handle CCW Rotation ascii d(100)
		if (playerInput == 100) {
			fPlayerA += (fSpeed * 0.5f) * fElapsedTime;
		}
		
		// Handle Forwards movement & collision ascii w(119)
		if (playerInput == 119)
		{
			fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;;
			fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;;
				fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
			}			
		}

		// Handle backwards movement & collision ascii s(115)
		if (playerInput == 115)
		{
			fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;;
			fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;;
				fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;;
			}
		}


		// WriteConsoleOutputCharacter(hConsole, screenVec, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	return 0;
}

// That's It!! - Jx9
