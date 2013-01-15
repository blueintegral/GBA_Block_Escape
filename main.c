/////////////////////////////////////////////////////////// 
//  Name         : Hunter Scott
// T-Square login ID : hscott7
// Homework     : milestone 2 
// Collaboration: 
//                 "I worked on the homework assignment alone, using stuff from gbadev.org, the krawall SDK, lots of random old forum posts, 
//					and referenced some bits of source code from games on gbadev.org." 
//                 


/* to fix:
DONE-change to mode 3
DONE-Integrate linked list, and structure
DONE-design each level with bigger blocks
DONE-figure how to detect when you hit a block. try checking if any of the pixels in a 1 pixel border are set to the color that is in videobuffer. 
-getting the cheat code function working
DONE-clean up code to get typedefs and stuff in mylib.h
*/

/* questions
DONE-why the blinking, how to break from while loops in waitForVBlank
DONE-how to set up the linked list ball thing
DONE-krawall audio?
*/

/*known bugs
FIXED-(It was VBA's fault, not the game)-after the first level, sometimes the top third of the screen is blank. It eventually goes back to normal. waitForVBlank problem?
FIXED-KINDA-fix the random sound bugs by calling kramWorker() all over the place. try making an empty song and playing it before the level change 
screens and then restarting actual song after the new level starts.
FIXED-problem with hitting enter after the win screen
*/

/*A note about the sound:
The sound works mostly. The problem comes when we enter that while loop that waits for the start button to be pressed. I tried disabling the sound interrupt
there, but there is still stuff in the buffer, so it crackles for a little while after. I also tried playing a blank song. Same problem. But the 
real reason it is not on by default in this version is because I used so much freaking DMA for drawing stuff that by the 5th level, everything is running
half speed. I think this is also because I am using up all of the RAM. So until that works flawlessly, it is disabled, but supported. The fist level
works fine with sound, and if I had time then I would make only the first level play sound. Oh well. To try it out anyway, set the variable 'sound' to 1.

By the way, the sound is all handled by Krawall, none of its code did I write. The documentation on it is in the SDK. This is the free version.
*/



#include <string.h>
#include "temp.h"
#include "myLib.h"
#include "view.h"
#include "PressEnter.h"
#include "GameOver.h"
#include "death1.h"
#include "death2.h"
#include "ball.h"
#include "level2.h"
#include "level3.h"
#include "level4.h"
#include "level5.h"
#include "win.h"
#include "nuclear_blast.h"
#include <stdlib.h>
#include "LinkedList.h"
#include <stdio.h>
#include "modules.h"
#include "samples.h"
#include "krawall.h"
#include "mtypes.h"

void vbl() {kramWorker();}  //sound thing


#define ARRAY_LENGTH 20
#define OFFSET(r, c) ((r)*240 + (c))



STATE gameState = START;
int t = 0;

//start sound crap
	//IntrTable for crt0
void (*IntrTable[])() = {
	0, // v-blank
	0, // h-blank
	0, // vcount
	0, // timer0
	kradInterrupt, // timer1
	0, // timer2
	0, // timer3
	0, // serial
	0, // dma0
	0, // dma1
	0, // dma2
	0, // dma3
	0, // key
	0  // cart
};

#define MEM_REGISTERS 0x4000000
#define DISPCNT		*( volatile unsigned short* )( MEM_REGISTERS )
#define VCOUNT		*( volatile unsigned short* )( MEM_REGISTERS + 0x6 )
#define INT_IE		*( volatile unsigned short* )( MEM_REGISTERS + 0x200 )
#define INT_ME		*( volatile unsigned short* )( MEM_REGISTERS + 0x208 )
#define WSCNT		*( volatile unsigned short* )( MEM_REGISTERS + 0x204 )
#define P1			*( volatile unsigned short* )( MEM_REGISTERS + 0x130 )

char __EH_FRAME_BEGIN__[] = {};
// to resolve link-errors w/o crtbegin.o (-nostartfiles), which we don't really need in C

//end sound crap



int main(void) {
	
	int r = 5; //player positions
	int c = 4;
	int r2 = 80; //first ball positions
	int c2 = 120;
	int r3 = 90; //second ball positions
	int c3 = 12;
	int r4 = 23;// you get the idea
	int c4 = 90;
	int r5 = 65;
	int c5 = 72;
	int r6 = 110;
	int c6 = 123;
	int r7 = 111;
	int c7 = 108;
	int r8 = 41;
	int c8 = 60;
	int r9 = 100;
	int c9 = 147;
	int c10 = 40;
	int r10 = 110;
	int width = 2;
	int size = 10;
	int dr = 3; //speed of player
	int dc = 3;
	int dr2 = 2; //speed of first ball
	int dc2 = 2;
	int dr3 = 1; //speed of second ball
	int dc3 = 1;
	int dr4 = 1.5; //again, you get the idea
	int dc4 = 1.5;
	int dr5 = 1;
	int dc5 = 1;
	int dr6 = 2;
	int dc6 = 2;
	int dr7 = 1.25;
	int dc7 = 1.25;
	int dr8 = 2.5;
	int dc8 = 2.5;
	int dr9 = 2.3;
	int dc9 = 2.3;
	int count = 0;
	int sound = 0; //this decides whether or not you play music. 
	
	Junk first; //my structure use :)

		//Here is the Pallate that goes with the all of the images in mode 4:
				//0 - black
				//1 - red
				//2 - green
				//3 - yellow
				//4 - blue

	//my linked list use :)
	LinkedList intList = {NULL, NULL};
	addToHead(&intList, makeIntADT(1));
	


//sound interrupts 
	if(sound == 1){
	INT_ME = 1;					// enable interrupts
	INT_IE |= ( 1 << 13 ); 		// cart-remove-interrupt (handled by crt0)
	WSCNT = ( 5 << 2 ) | ( 1 << 14 );	// set rom-timing
	kragInit(KRAG_INIT_MONO); //fire up krawall
	krapPlay(&mod_secondpm,KRAP_MODE_LOOP,0); //start playing a loop in mono
	}
	//end sound


	while(keepGoing()) {
		if(sound == 1){
		kramWorker(); // for krawall. this actually does the work, so it needs to be called every frame.
		}
		waitForVBlank(ON);
		switch (gameState) {
			case START:
				//start mode 4
				FlipPage(ON);
				initializeGBA(MODE_4);
				paintBackground(PressEnterBitmap);
				first.cheat = 0; //heres where you toggle the cheat function. This works, you just have to do it manually.
				first.drawn = false;

				while(!keyHit(BUTTON_START)){
				//	first.cheat = cheatCheck(); //this would check to see if the cheat code is entered, but it doesn't work right now. Maybe in a future version.
					while(keyHit(BUTTON_START)){
						break;}
				}
				
					gameState = LEVEL1;
				
				
				break;
			case LEVEL1:

				//start mode 3
				if(sound == 1){
				kramWorker();
				}
				FlipPage(OFF);
				GBASetup();

				if(!(first.drawn))
				{
				drawRectDMA(0, 0, 160, 240, COLOR(0,0,0)); //the black background
				//frame
				drawRectDMA(1, 1, 240, 1, COLOR(0,0,31)); //left bar
				drawRectDMA(159, 1, 1, 240, COLOR(0,0,31));//bottom bar
				drawRectDMA(1, 1, 240, 1, COLOR(0,0,31));//bottom bar
				drawRectDMA(159, 1, 1, 240, COLOR(0,0,31));//bottom bar
				first.drawn = true;
				}
				
				waitForVBlank(ON);

			
				



				//player control
				
				drawRectDMA( r, c, width, width, COLOR(0,0,0));
				
				if(keyHit(BUTTON_UP)) {
					r--;
					if(r <= 1) r++;
				}
				if(keyHit(BUTTON_DOWN)) {
					r++;
					if(r >= 160) r--;
				}
				if(keyHit(BUTTON_LEFT)) {
					c--;
					if(c <= 1) c++;
				}
				if(keyHit(BUTTON_RIGHT)) {
					c++;
					if(c >= 240) c--;
				}
			
						
				//start ball code (makes ball move)
				drawRectDMA( r2, c2, size, size, COLOR(0,0,0) );
				drawRectDMA( r3, c3, size, size, COLOR(0,0,0) );
				drawRectDMA( r4, c4, size, size, COLOR(0,0,0) );
				drawRectDMA( r5, c5, size, size, COLOR(0,0,0) );

				r2 += dr;
				c2 += dc;
				r3 += dr2;
				c3 += dc2;
				r4 += dr3;
				c4 += dc3;
				r5 += dr4;
				c5 += dc4;
				
				drawRectDMA( r2, c2, size, size, COLOR(0,0,31) );
				drawRectDMA( r3, c3, size, size, COLOR(0,0,31) );
				drawRectDMA( r4, c4, size, size, COLOR(0,0,31) );
				drawRectDMA( r5, c5, size, size, COLOR(0,0,31) );//this apparently does absolutley nothing, but I am afraid to get rid of it
				
				//collision detection
				//walls (make ball bounce)
			
				//first block
			if((r2+size) > 156) dr = -dr; //top
			if((c2+size) > 236) dc = -dc; //bottom and right
			if((c2+size) < 15) dc = -dc; //left
			if(((r2) <= 3) || ((r2) >= 158)) dr = -dr; //top and bottom
			

				//2nd block
			if((r3+size) > 156) dr2 = -dr2; //top
			if((c3+size) > 236) dc2 = -dc2; //bottom and right
			if((c3+size) < 15) dc2 = -dc2; //left
			if(((r3) <= 3) || ((r3) >= 158)) dr2 = -dr2; //top and bottom
			
			
				//3rd block
			if((r4+size) > 156) dr3 = -dr3; //top
			if((c4+size) > 236) dc3 = -dc3; //bottom and right
			if((c4+size) < 15) dc3 = -dc3; //left
			if(((r4) <= 3) || ((r4) >= 158)) dr3 = -dr3; //top and bottom
			
			
				//4th block
			if((r5+size) > 156) dr4 = -dr4; //top
			if((c5+size) > 236) dc4 = -dc4; //bottom and right
			if((c5+size) < 15) dc4 = -dc4; //left
			if(((r5) <= 3) || ((r5) >= 158)) dr4 = -dr4; //top and bottom
			

				
				//draw player square
				
				drawRectDMA( r, c, width, width, COLOR(0,0,0));
				drawRectDMA( r, c, width, width, COLOR(0,0,31) ); //the actual square

				
				//draw bouncing ball
				drawRectDMA( r2, c2, size, size, COLOR(31,0,0));
				drawRectDMA( r3, c3, size, size, COLOR(31,0,0) );
				drawRectDMA( r4, c4, size, size, COLOR(31,0,0) );
				drawRectDMA( r5, c5, size, size, COLOR(31,0,0) );

				//right wall
				drawRectDMA( 1, 238, 72 , width - 1, COLOR(0,0,31) ); 
				drawRectDMA( 87, 238, 73,  width - 1, COLOR(0,0,31) ); 
				
				//keep player in boundries
				if(r < 2) {
					r = 2;
				}
				if(r > 157) {
					r = 157;
				}
				if((c > 236) && ((r > 87) || (r < 73)) ) {
					c = 236;
				}

				//check to see if the color in the videobuffer is red, if so then die
				if((first.cheat) == 0 ){ //but first see if they are cheating. If they are, ignore collision detection (invincibility)
				if((*(videobuffer + OFFSET(r,c)) == COLOR(31,0,0))){
			
				gameState = DIE;
				}
				}

				//check to see if they make it to the safe spot and send them to the next level
				
				if((c+size > 239)) {
				
					t = 0;
					
					
					drawBackground(3); 
					while(!keyHit(BUTTON_START)){ //i'm pretty sure this is where the sound problem is
					while(keyHit(BUTTON_START)){
						break;}
							}
				

					r = 4;
					c = 5;
				
					gameState = LEVEL2;
					
				
				
				}
				
				
			insertInOrder(&intList, makeIntADT(2), cmpInts); //add a node to the linked list
				break;

			case LEVEL2:
				//krapPlay(NULL,KRAP_MODE_LOOP,0); //this was my attempt to play nothing so the sound would stop. Didn't work.
				if(sound == 1){
				kramWorker();
				}
			waitForVBlank(ON);
				
				first.drawn = false;
				GBASetup();

				if(sound == 1){
				kramWorker();
				}

				if(!(first.drawn))
				{
				waitForVBlank(ON);
				drawRectDMA(0, 0, 160, 240, COLOR(0,0,0)); //the black background
				//frame
				drawRectDMA(1, 1, 240, 1, COLOR(0,0,31)); //left bar
				drawRectDMA(159, 1, 1, 240, COLOR(0,0,31));//bottom bar
				drawRectDMA(1, 1, 240, 1, COLOR(0,0,31));//bottom bar
				drawRectDMA(159, 1, 1, 240, COLOR(0,0,31));//bottom bar
				first.drawn = true;
			
				}
			
				
				//player control
				
				drawRectDMA( r, c, width, width, COLOR(0,0,0));
				
				if(keyHit(BUTTON_UP)) {
					r--;
					if(r <= 1) r++;
				}
				if(keyHit(BUTTON_DOWN)) {
					r++;
					if(r >= 160) r--;
				}
				if(keyHit(BUTTON_LEFT)) {
					c--;
					if(c <= 1) c++;
				}
				if(keyHit(BUTTON_RIGHT)) {
					c++;
					if(c >= 240) c--;
				}
			
						
				//start ball code (makes ball move)
				drawRectDMA( r2, c2, size, size, COLOR(0,0,0) );
				drawRectDMA( r3, c3, size, size, COLOR(0,0,0) );
				drawRectDMA( r4, c4, size, size, COLOR(0,0,0) );
				drawRectDMA( r5, c5, size, size, COLOR(0,0,0) );
				drawRectDMA( r6, c6, size, size, COLOR(0,0,0) );
				drawRectDMA( r7, c7, size, size, COLOR(0,0,0) );

				r2 += dr;
				c2 += dc;
				r3 += dr2;
				c3 += dc2;
				r4 += dr3;
				c4 += dc3;
				r5 += dr4;
				c5 += dc4;
				r6 += dr5;
				c6 += dc5;
				r7 += dr6;
				c7 += dc6;
				
				drawRectDMA( r2, c2, size, size, COLOR(0,0,31) );
				drawRectDMA( r3, c3, size, size, COLOR(0,0,31) );
				drawRectDMA( r4, c4, size, size, COLOR(0,0,31) );
				drawRectDMA( r5, c5, size, size, COLOR(0,0,31) );
				drawRectDMA( r6, c6, size, size, COLOR(0,0,31) );
				drawRectDMA( r7, c7, size, size, COLOR(0,0,31) );//this apparently does absolutley nothing, but I am afraid to get rid of it
				//collision detection
				
				//walls (make ball bounce)
			
			//first block
			if((r2+size) > 156) dr = -dr; //top
			if((c2+size) > 236) dc = -dc; //bottom and right
			if((c2+size) < 15) dc = -dc; //left
			if(((r2) <= 3) || ((r2) >= 158)) dr = -dr; //top and bottom
			

				//2nd block
			if((r3+size) > 156) dr2 = -dr2; //top
			if((c3+size) > 236) dc2 = -dc2; //bottom and right
			if((c3+size) < 15) dc2 = -dc2; //left
			if(((r3) <= 3) || ((r3) >= 158)) dr2 = -dr2; //top and bottom
			
			
				//3rd block
			if((r4+size) > 156) dr3 = -dr3; //top
			if((c4+size) > 236) dc3 = -dc3; //bottom and right
			if((c4+size) < 15) dc3 = -dc3; //left
			if(((r4) <= 3) || ((r4) >= 158)) dr3 = -dr3; //top and bottom
			
			
				//4th block
			if((r5+size) > 156) dr4 = -dr4; //top
			if((c5+size) > 236) dc4 = -dc4; //bottom and right
			if((c5+size) < 15) dc4 = -dc4; //left
			if(((r5) <= 3) || ((r5) >= 158)) dr4 = -dr4; //top and bottom

			//5th block
			if((r6+size) > 156) dr5 = -dr5; //top
			if((c6+size) > 236) dc5 = -dc5; //bottom and right
			if((c6+size) < 15) dc5 = -dc5; //left
			if(((r6) <= 3) || ((r6+size) >= 158)) dr5 = -dr5; //top and bottom
			
			
				//6th block
			if((r7+size) > 156) dr6 = -dr6; //top
			if((c7+size) > 236) dc6 = -dc6; //bottom and right
			if((c7+size) < 15) dc6 = -dc6; //left
			if(((r7) <= 3) || ((r7) >= 158)) dr6 = -dr6; //top and bottom
			

				
				//paddle
				
				drawRectDMA( r, c, width, width, COLOR(0,0,0));
				drawRectDMA( r, c, width, width, COLOR(0,0,31) );

				//bouncing ball
				drawRectDMA( r2, c2, size, size, COLOR(31,0,0));
				drawRectDMA( r3, c3, size, size, COLOR(31,0,0) );
				drawRectDMA( r4, c4, size, size, COLOR(31,0,0) );
				drawRectDMA( r5, c5, size, size, COLOR(31,0,0) );
				drawRectDMA( r6, c6, size, size, COLOR(31,0,0) );
				drawRectDMA( r7, c7, size, size, COLOR(31,0,0) );

				

				//right wall
				drawRectDMA( 1, 238, 72 , width - 1, COLOR(0,0,31) ); 
				drawRectDMA( 87, 238, 73,  width - 1, COLOR(0,0,31) ); 
				//keep player in square
				
				if(r < 2) {
					r = 2;
				}
				if(r > 157) {
					r = 157;
				}
				if((c > 236) && ((r > 87) || (r < 73)) ) {
					c = 236;
				}
				
				//check to see if the color in the videobuffer is red, if so then die
				
				if((first.cheat) == 0){
				if((*(videobuffer + OFFSET(r,c)) == COLOR(31,0,0))){		//change
				gameState = DIE;
				}
				}
				if((c+size > 239)) {
					
					t = 0;
					
				
					drawBackground(4);
					
				while(!keyHit(BUTTON_START)){
					while(keyHit(BUTTON_START)){
						break;}
				}
					
					r = 4;
					c = 5;
					gameState = LEVEL3;
				
				
				}
				
				insertInOrder(&intList, makeIntADT(3), 
		cmpInts);
				break;


				
			case LEVEL3:
				
				if(sound == 1){
				kramWorker();
				}
				waitForVBlank(ON);
			
				first.drawn = false;
				
				GBASetup();
				
				if(sound == 1){
				kramWorker();
				}

				if(!(first.drawn))
				{
				waitForVBlank(ON);
				drawRectDMA(0, 0, 160, 240, COLOR(0,0,0)); //the black background
				//frame
				drawRectDMA(1, 1, 240, 1, COLOR(0,0,31)); //left bar
				drawRectDMA(159, 1, 1, 240, COLOR(0,0,31));//bottom bar
				drawRectDMA(1, 1, 240, 1, COLOR(0,0,31));//bottom bar
				drawRectDMA(159, 1, 1, 240, COLOR(0,0,31));//bottom bar
				
				first.drawn = true;
				
				
				}
			
				
				//player control
				
				drawRectDMA( r, c, width, width, COLOR(0,0,0));
				
				if(keyHit(BUTTON_UP)) {
					r--;
					if(r <= 1) r++;
				}
				if(keyHit(BUTTON_DOWN)) {
					r++;
					if(r >= 160) r--;
				}
				if(keyHit(BUTTON_LEFT)) {
					c--;
					if(c <= 1) c++;
				}
				if(keyHit(BUTTON_RIGHT)) {
					c++;
					if(c >= 240) c--;
				}
			
						
				//start ball code (makes ball move)
				drawRectDMA( r2, c2, size, size, COLOR(0,0,0) );
				drawRectDMA( r3, c3, size, size, COLOR(0,0,0) );
				drawRectDMA( r4, c4, size, size, COLOR(0,0,0) );
				drawRectDMA( r5, c5, size, size, COLOR(0,0,0) );
				drawRectDMA( r6, c6, size, size, COLOR(0,0,0) );
				drawRectDMA( r7, c7, size, size, COLOR(0,0,0) );
				drawRectDMA( r8, c8, size, size, COLOR(0,0,0) );


				r2 += dr;
				c2 += dc;
				r3 += dr2;
				c3 += dc2;
				r4 += dr3;
				c4 += dc3;
				r5 += dr4;
				c5 += dc4;
				r6 += dr5;
				c6 += dc5;
				r7 += dr6;
				c7 += dc6;
				r8 += dr7;
				c8 += dc7;
				
				drawRectDMA( r2, c2, size, size, COLOR(0,0,31) );
				drawRectDMA( r3, c3, size, size, COLOR(0,0,31) );
				drawRectDMA( r4, c4, size, size, COLOR(0,0,31) );
				drawRectDMA( r5, c5, size, size, COLOR(0,0,31) );
				drawRectDMA( r6, c6, size, size, COLOR(0,0,31) );
				drawRectDMA( r7, c7, size, size, COLOR(0,0,31) );
				drawRectDMA( r8, c8, size, size, COLOR(0,0,31) );//this apparently does absolutley nothing, but I am afraid to get rid of it
				//collision detection
				
				//walls (make ball bounce)
			
			//first block
			if((r2+size) > 156) dr = -dr; //top
			if((c2+size) > 236) dc = -dc; //bottom and right
			if((c2+size) < 15) dc = -dc; //left
			if(((r2) <= 3) || ((r2) >= 158)) dr = -dr; //top and bottom
			

				//2nd block
			if((r3+size) > 156) dr2 = -dr2; //top
			if((c3+size) > 236) dc2 = -dc2; //bottom and right
			if((c3+size) < 15) dc2 = -dc2; //left
			if(((r3) <= 3) || ((r3) >= 158)) dr2 = -dr2; //top and bottom
			
			
				//3rd block
			if((r4+size) > 156) dr3 = -dr3; //top
			if((c4+size) > 236) dc3 = -dc3; //bottom and right
			if((c4+size) < 15) dc3 = -dc3; //left
			if(((r4) <= 3) || ((r4) >= 158)) dr3 = -dr3; //top and bottom
			
			
				//4th block
			if((r5+size) > 156) dr4 = -dr4; //top
			if((c5+size) > 236) dc4 = -dc4; //bottom and right
			if((c5+size) < 15) dc4 = -dc4; //left
			if(((r5) <= 3) || ((r5) >= 158)) dr4 = -dr4; //top and bottom

			//5th block
			if((r6+size) > 156) dr5 = -dr5; //top
			if((c6+size) > 236) dc5 = -dc5; //bottom and right
			if((c6+size) < 15) dc5 = -dc5; //left
			if(((r6) <= 3) || ((r6+size) >= 158)) dr5 = -dr5; //top and bottom
			
			
				//6th block
			if((r7+size) > 156) dr6 = -dr6; //top
			if((c7+size) > 236) dc6 = -dc6; //bottom and right
			if((c7+size) < 15) dc6 = -dc6; //left
			if(((r7) <= 3) || ((r7) >= 158)) dr6 = -dr6; //top and bottom

				//7th block
			if((r8+size) > 156) dr7 = -dr7; //top
			if((c8+size) > 236) dc7 = -dc7; //bottom and right
			if((c8+size) < 15) dc7 = -dc7; //left
			if(((r8) <= 3) || ((r8) >= 158)) dr7 = -dr7; //top and bottom


				//paddle
				
				drawRectDMA( r, c, width, width, COLOR(0,0,0));
				drawRectDMA( r, c, width, width, COLOR(0,0,31) );

				//bouncing ball
				drawRectDMA( r2, c2, size, size, COLOR(31,0,0));
				drawRectDMA( r3, c3, size, size, COLOR(31,0,0) );
				drawRectDMA( r4, c4, size, size, COLOR(31,0,0) );
				drawRectDMA( r5, c5, size, size, COLOR(31,0,0) );
				drawRectDMA( r6, c6, size, size, COLOR(31,0,0) );
				drawRectDMA( r7, c7, size, size, COLOR(31,0,0) );
				drawRectDMA( r8, c8, size, size, COLOR(31,0,0) );


				

				//right wall
				drawRectDMA( 1, 238, 72 , width - 1, COLOR(0,0,31) ); 
				drawRectDMA( 87, 238, 73,  width - 1, COLOR(0,0,31) ); 
				//keep player in square
				
				if(r < 2) {
					r = 2;
				}
				if(r > 157) {
					r = 157;
				}
				if((c > 236) && ((r > 87) || (r < 73)) ) {
					c = 236;
				}
				
				//check to see if the color in the videobuffer is red, if so then die
				
				if((first.cheat) == 0){
				if((*(videobuffer + OFFSET(r,c)) == COLOR(31,0,0))){	//change	
				gameState = DIE;
				}
				}
				if((c+size > 239)) {
					
					t = 0;
					
				
					drawBackground(5);
				while(!keyHit(BUTTON_START)){
					
					while(keyHit(BUTTON_START)){
						
						break;}
				}
					
					r = 4;
					c = 5;
					gameState = LEVEL4;
					
				
				
				}
				
				insertInOrder(&intList, makeIntADT(4), 
		cmpInts);
				break;

			case LEVEL4:

				
			
				
				waitForVBlank(ON);
			
				first.drawn = false;
				GBASetup();
				
				
				if(!(first.drawn))
				{
				waitForVBlank(ON);
				drawRectDMA(0, 0, 160, 240, COLOR(0,0,0)); //the black background
				//frame
				drawRectDMA(1, 1, 240, 1, COLOR(0,0,31)); //left bar
				drawRectDMA(159, 1, 1, 240, COLOR(0,0,31));//bottom bar
				drawRectDMA(1, 1, 240, 1, COLOR(0,0,31));//bottom bar
				drawRectDMA(159, 1, 1, 240, COLOR(0,0,31));//bottom bar
				first.drawn = true;
				
				
				}
			
				//player control
				
				drawRectDMA( r, c, width, width, COLOR(0,0,0));
				
				if(keyHit(BUTTON_UP)) {
					r--;
					if(r <= 1) r++;
				}
				if(keyHit(BUTTON_DOWN)) {
					r++;
					if(r >= 160) r--;
				}
				if(keyHit(BUTTON_LEFT)) {
					c--;
					if(c <= 1) c++;
				}
				if(keyHit(BUTTON_RIGHT)) {
					c++;
					if(c >= 240) c--;
				}
			
						
				//start ball code (makes ball move)
				drawRectDMA( r2, c2, size, size, COLOR(0,0,0) );
				drawRectDMA( r3, c3, size, size, COLOR(0,0,0) );
				drawRectDMA( r4, c4, size, size, COLOR(0,0,0) );
				drawRectDMA( r5, c5, size, size, COLOR(0,0,0) );
				drawRectDMA( r6, c6, size, size, COLOR(0,0,0) );
				drawRectDMA( r7, c7, size, size, COLOR(0,0,0) );
				drawRectDMA( r8, c8, size, size, COLOR(0,0,0) );
				drawRectDMA( r9, c9, size, size, COLOR(0,0,0) );
				drawRectDMA( r10, c10, size, size, COLOR(0,0,0) );


				r2 += dr;
				c2 += dc;
				r3 += dr2;
				c3 += dc2;
				r4 += dr3;
				c4 += dc3;
				r5 += dr4;
				c5 += dc4;
				r6 += dr5;
				c6 += dc5;
				r7 += dr6;
				c7 += dc6;
				r8 += dr7;
				c8 += dc7;
				r9 += dr8;
				c9 += dc8;
				r10 += dr9;
				c10 += dc9;
				
				drawRectDMA( r2, c2, size, size, COLOR(0,0,31) );
				drawRectDMA( r3, c3, size, size, COLOR(0,0,31) );
				drawRectDMA( r4, c4, size, size, COLOR(0,0,31) );
				drawRectDMA( r5, c5, size, size, COLOR(0,0,31) );
				drawRectDMA( r6, c6, size, size, COLOR(0,0,31) );
				drawRectDMA( r7, c7, size, size, COLOR(0,0,31) );
				drawRectDMA( r8, c8, size, size, COLOR(0,0,31) );
				drawRectDMA( r9, c9, size, size, COLOR(0,0,31) );
				drawRectDMA( r10, c10, size, size, COLOR(0,0,31) );//this apparently does absolutley nothing, but I am afraid to get rid of it
				//collision detection
				
				//walls (make ball bounce)
			//first block
			if((r2+size) > 156) dr = -dr; //top
			if((c2+size) > 236) dc = -dc; //bottom and right
			if((c2+size) < 15) dc = -dc; //left
			if(((r2) <= 3) || ((r2) >= 158)) dr = -dr; //top and bottom
			

				//2nd block
			if((r3+size) > 156) dr2 = -dr2; //top
			if((c3+size) > 236) dc2 = -dc2; //bottom and right
			if((c3+size) < 15) dc2 = -dc2; //left
			if(((r3) <= 3) || ((r3) >= 158)) dr2 = -dr2; //top and bottom
			
			
				//3rd block
			if((r4+size) > 156) dr3 = -dr3; //top
			if((c4+size) > 236) dc3 = -dc3; //bottom and right
			if((c4+size) < 15) dc3 = -dc3; //left
			if(((r4) <= 3) || ((r4) >= 158)) dr3 = -dr3; //top and bottom
			
			
				//4th block
			if((r5+size) > 156) dr4 = -dr4; //top
			if((c5+size) > 236) dc4 = -dc4; //bottom and right
			if((c5+size) < 15) dc4 = -dc4; //left
			if(((r5) <= 3) || ((r5) >= 158)) dr4 = -dr4; //top and bottom

			//5th block
			if((r6+size) > 156) dr5 = -dr5; //top
			if((c6+size) > 236) dc5 = -dc5; //bottom and right
			if((c6+size) < 15) dc5 = -dc5; //left
			if(((r6) <= 3) || ((r6+size) >= 158)) dr5 = -dr5; //top and bottom
			
			
				//6th block
			if((r7+size) > 156) dr6 = -dr6; //top
			if((c7+size) > 236) dc6 = -dc6; //bottom and right
			if((c7+size) < 15) dc6 = -dc6; //left
			if(((r7) <= 3) || ((r7) >= 158)) dr6 = -dr6; //top and bottom

				//7th block
			if((r8+size) > 156) dr7 = -dr7; //top
			if((c8+size) > 236) dc7 = -dc7; //bottom and right
			if((c8+size) < 15) dc7 = -dc7; //left
			if(((r8) <= 3) || ((r8) >= 158)) dr7 = -dr7; //top and bottom

			//8th block
			if((r9+size) > 156) dr8 = -dr8; //top
			if((c9+size) > 236) dc8 = -dc8; //bottom and right
			if((c9+size) < 15) dc8 = -dc8; //left
			if(((r9) <= 3) || ((r9) >= 158)) dr8 = -dr8; //top and bottom

			//9th block
			if((r10+size) > 156) dr9 = -dr9; //top
			if((c10+size) > 236) dc9 = -dc9; //bottom and right
			if((c10+size) < 15) dc9 = -dc9; //left
			if(((r10) <= 3) || ((r10) >= 158)) dr9 = -dr9; //top and bottom
				
			
			//paddle
				
				drawRectDMA( r, c, width, width, COLOR(0,0,0));
				drawRectDMA( r, c, width, width, COLOR(0,0,31) );

				//bouncing ball
				drawRectDMA( r2, c2, size, size, COLOR(31,0,0));
				drawRectDMA( r3, c3, size, size, COLOR(31,0,0) );
				drawRectDMA( r4, c4, size, size, COLOR(31,0,0) );
				drawRectDMA( r5, c5, size, size, COLOR(31,0,0) );
				drawRectDMA( r6, c6, size, size, COLOR(31,0,0) );
				drawRectDMA( r7, c7, size, size, COLOR(31,0,0) );
				drawRectDMA( r8, c8, size, size, COLOR(31,0,0) );
				drawRectDMA( r9, c9, size, size, COLOR(31,0,0) );
				drawRectDMA( r10, c10, size, size, COLOR(31,0,0) );


				

				//right wall
				drawRectDMA( 1, 238, 72 , width - 1, COLOR(0,0,31) ); 
				drawRectDMA( 87, 238, 73,  width - 1, COLOR(0,0,31) ); 
				//keep player in square
				
				if(r < 2) {
					r = 2;
				}
				if(r > 157) {
					r = 157;
				}
				if((c > 236) && ((r > 87) || (r < 73)) ) {
					c = 236;
				}
				
				//check to see if the color in the videobuffer is red, if so then die
				
				if((first.cheat) == 0){
				if((*(videobuffer + OFFSET(r,c)) == COLOR(31,0,0))){//change
				//if((r == COLOR(31,0,0)) || (c == COLOR(31,0,0))){				
				gameState = DIE;
				}
				}
				if((c+size > 239)) {
					
					t = 0;
					
				
					drawBackground(6);
					
				while(!keyHit(BUTTON_START)){
					
					while(keyHit(BUTTON_START)){
						
						break;}
				}
					
						r = 4;
					c = 5;
					gameState = LEVEL5;
					
				
				
				}
				
				insertInOrder(&intList, makeIntADT(5), 
		cmpInts);
				break;

			case LEVEL5:
				if(sound == 1){
				kramWorker();
				}

				waitForVBlank(ON);
			
				first.drawn = false;
				GBASetup();

				if(sound == 1){
				kramWorker();
				}

				if(!(first.drawn))
				{
				waitForVBlank(ON);
				drawRectDMA(0, 0, 160, 240, COLOR(0,0,0)); //the black background
				//frame
				drawRectDMA(1, 1, 240, 1, COLOR(0,0,31)); //left bar
				drawRectDMA(159, 1, 1, 240, COLOR(0,0,31));//bottom bar
				drawRectDMA(1, 1, 240, 1, COLOR(0,0,31));//bottom bar
				drawRectDMA(159, 1, 1, 240, COLOR(0,0,31));//bottom bar
				first.drawn = true;
				
				}
			
				
				//player control
				
				drawRectDMA( r, c, width, width, COLOR(0,0,0));
				
				if(keyHit(BUTTON_UP)) {
					r--;
					if(r <= 1) r++;
				}
				if(keyHit(BUTTON_DOWN)) {
					r++;
					if(r >= 160) r--;
				}
				if(keyHit(BUTTON_LEFT)) {
					c--;
					if(c <= 1) c++;
				}
				if(keyHit(BUTTON_RIGHT)) {
					c++;
					if(c >= 240) c--;
				}
			
						
				//start ball code (makes ball move)
				drawRectDMA( r2, c2, size, size, COLOR(0,0,0) );
				drawRectDMA( r3, c3, size, size, COLOR(0,0,0) );
				drawRectDMA( r4, c4, size, size, COLOR(0,0,0) );
				drawRectDMA( r5, c5, size, size, COLOR(0,0,0) );
				drawRectDMA( r6, c6, size, size, COLOR(0,0,0) );
				drawRectDMA( r7, c7, size, size, COLOR(0,0,0) );
				drawRectDMA( r8, c8, size, size, COLOR(0,0,0) );
				drawRectDMA( r9, c9, size, size, COLOR(0,0,0) );
				drawRectDMA( r10, c10, size, size, COLOR(0,0,0) );


				r2 += dr;
				c2 += dc;
				r3 += dr2;
				c3 += dc2;
				r4 += dr3;
				c4 += dc3;
				r5 += dr4;
				c5 += dc4;
				r6 += dr5;
				c6 += dc5;
				r7 += dr6;
				c7 += dc6;
				r8 += dr7;
				c8 += dc7;
				r9 += dr8;
				c9 += dc8;
				r10 += dr9;
				c10 += dc9;
				
				drawRectDMA( r2, c2, size, size, COLOR(0,0,31) );
				drawRectDMA( r3, c3, size, size, COLOR(0,0,31) );
				drawRectDMA( r4, c4, size, size, COLOR(0,0,31) );
				drawRectDMA( r5, c5, size, size, COLOR(0,0,31) );
				drawRectDMA( r6, c6, size, size, COLOR(0,0,31) );
				drawRectDMA( r7, c7, size, size, COLOR(0,0,31) );
				drawRectDMA( r8, c8, size, size, COLOR(0,0,31) );
				drawRectDMA( r9, c9, size, size, COLOR(0,0,31) );
				drawRectDMA( r10, c10, size, size, COLOR(0,0,31) );//this apparently does absolutley nothing, but I am afraid to get rid of it
				//collision detection
				
				//walls (make ball bounce)
			//first block
			if((r2+size) > 156) dr = -dr; //top
			if((c2+size) > 236) dc = -dc; //bottom and right
			if((c2+size) < 15) dc = -dc; //left
			if(((r2) <= 3) || ((r2) >= 158)) dr = -dr; //top and bottom
			

				//2nd block
			if((r3+size) > 156) dr2 = -dr2; //top
			if((c3+size) > 236) dc2 = -dc2; //bottom and right
			if((c3+size) < 15) dc2 = -dc2; //left
			if(((r3) <= 3) || ((r3) >= 158)) dr2 = -dr2; //top and bottom
			
			
				//3rd block
			if((r4+size) > 156) dr3 = -dr3; //top
			if((c4+size) > 236) dc3 = -dc3; //bottom and right
			if((c4+size) < 15) dc3 = -dc3; //left
			if(((r4) <= 3) || ((r4) >= 158)) dr3 = -dr3; //top and bottom
			
			
				//4th block
			if((r5+size) > 156) dr4 = -dr4; //top
			if((c5+size) > 236) dc4 = -dc4; //bottom and right
			if((c5+size) < 15) dc4 = -dc4; //left
			if(((r5) <= 3) || ((r5) >= 158)) dr4 = -dr4; //top and bottom

			//5th block
			if((r6+size) > 156) dr5 = -dr5; //top
			if((c6+size) > 236) dc5 = -dc5; //bottom and right
			if((c6+size) < 15) dc5 = -dc5; //left
			if(((r6) <= 3) || ((r6+size) >= 158)) dr5 = -dr5; //top and bottom
			
			
				//6th block
			if((r7+size) > 156) dr6 = -dr6; //top
			if((c7+size) > 236) dc6 = -dc6; //bottom and right
			if((c7+size) < 15) dc6 = -dc6; //left
			if(((r7) <= 3) || ((r7) >= 158)) dr6 = -dr6; //top and bottom

				//7th block
			if((r8+size) > 156) dr7 = -dr7; //top
			if((c8+size) > 236) dc7 = -dc7; //bottom and right
			if((c8+size) < 15) dc7 = -dc7; //left
			if(((r8) <= 3) || ((r8) >= 158)) dr7 = -dr7; //top and bottom

			//8th block
			if((r9+size) > 156) dr8 = -dr8; //top
			if((c9+size) > 236) dc8 = -dc8; //bottom and right
			if((c9+size) < 15) dc8 = -dc8; //left
			if(((r9) <= 3) || ((r9) >= 158)) dr8 = -dr8; //top and bottom

			//9th block
			if((r10+size) > 156) dr9 = -dr9; //top
			if((c10+size) > 236) dc9 = -dc9; //bottom and right
			if((c10+size) < 15) dc9 = -dc9; //left
			if(((r10) <= 3) || ((r10) >= 158)) dr9 = -dr9; //top and bottom
				
				
				//paddle
				
				drawRectDMA( r, c, size, size, COLOR(0,0,0));
				drawRectDMA( r, c, size, size, COLOR(0,0,31) );

				//bouncing ball
				drawRectDMA( r2, c2, size, size, COLOR(31,0,0));
				drawRectDMA( r3, c3, size, size, COLOR(31,0,0) );
				drawRectDMA( r4, c4, size, size, COLOR(31,0,0) );
				drawRectDMA( r5, c5, size, size, COLOR(31,0,0) );
				drawRectDMA( r6, c6, size, size, COLOR(31,0,0) );
				drawRectDMA( r7, c7, size, size, COLOR(31,0,0) );
				drawRectDMA( r8, c8, size, size, COLOR(31,0,0) );
				drawRectDMA( r9, c9, size, size, COLOR(31,0,0) );
				drawRectDMA( r10, c10, size, size, COLOR(31,0,0) );

				//right wall
				drawRectDMA( 1, 238, 72 , width - 1, COLOR(0,0,31) ); 
				drawRectDMA( 87, 238, 73,  width - 1, COLOR(0,0,31) ); 
				//keep player in square
				
				if(r < 2) {
					r = 2;
				}
				if(r > 157) {
					r = 157;
				}
				if((c > 236) && ((r > 87) || (r < 73)) ) {
					c = 236;
				}
				
				//check to see if the color in the videobuffer is red, if so then die
				
				if((first.cheat) == 0){
				if((*(videobuffer + OFFSET(r,c)) == COLOR(31,0,0))){//change
				//if((r == COLOR(31,0,0)) || (c == COLOR(31,0,0))){				
				gameState = DIE;
				}
				}
	
				if((c+size > 239)) {
					gameState = WIN;
				}
				insertInOrder(&intList, makeIntADT(6), 
		cmpInts);
				break;


			case WIN: //if they make it past level 5, display a win screen and reset some variables
				if(sound == 1){
				kramWorker();
				}

				drawBackground(7);
				if(keyHit(BUTTON_START)) {
					gameState = CREDITS; //next stop is the credits
					r = 5;
					c = 4;
					count = 0;
				
					if(sound == 1){	
				kramWorker();
				}

					first.drawn = 0;
				}
				while(keyHit(BUTTON_START)) {
					while(!keyHit(BUTTON_START)) {
					break; }
				}
				break;


			case CREDITS: //display the Pony Shrapnel logo
				
				if(sound == 1){
				kramWorker();
				}

				drawBackground(8); 
				
				if(keyHit(BUTTON_START)) {
					gameState = START;
					r = 5;
					c = 4;
					count = 0;

					if(sound == 1){
					kramWorker();
					}

					first.drawn = 0;
				}
				while(keyHit(BUTTON_START)) {
					while(!keyHit(BUTTON_START)) {
					break; }
				}
				break;


			

		
			case DIE: //if they die, then display a death screen, reset some variables, and go back to the start screen
				
				if(sound == 1){
				kramWorker();
				}
				
				drawBackground(2);
				
				if(keyHit(BUTTON_START)) {
					gameState = START;
					r = 5;
					c = 4;
					count = 0;
					
					if(sound == 1){
					kramWorker();
					}

					first.drawn = 0;
				}
				while(keyHit(BUTTON_START)) {
					while(!keyHit(BUTTON_START)) {
					break; }
				}
				break;

		
		}

	}
	return 0;
}
//hooray, over 1200 lines!
