// myLib.h
#ifdef _DEBUG
#include <stdio.h>
#endif

typedef enum state_t {
	START, LEVEL1, LEVEL2, LEVEL3, LEVEL4, LEVEL5, WIN, DIE, CREDITS
} STATE;


typedef unsigned short u16;
typedef unsigned int u32;
#define bool int
#define true 1
#define false 0

#ifdef _DEBUG
extern FILE *debug;
extern int t;
#else
#define REG_DISPCNT *(u16 *) 0X4000000
#define COLOR(r, g, b) ((r) | (g) << 5 | (b) << 10)
#define OFFSET(r, c) ((r)*240 + (c))
#define BG2_ENABLE (1 << 10)
#define DRAW_BACK_BUFFER 0X10

#define REG_DMA3_SAD *(volatile u32 *) 0X40000D4
#define REG_DMA3_DAD *(volatile u32 *) 0X40000D8
#define REG_DMA3_CNT *(volatile u32 *) 0X40000DC

#define DMA_ON (1 << 31)
#define DMA_SRC_FIXED (2 << 23)

#define DMA_MEMCOPY3_SHRT(_dst,_src,_count)        \
    REG_DMA3_CNT = 0;                              \
    REG_DMA3_SAD = (int)(_src);                    \
    REG_DMA3_DAD = (int)(_dst);                    \
    REG_DMA3_CNT = (_count)| DMA_ON; 

#define DMA_MEMFILL3_SHRT(_dst,_src,_count)        \
    REG_DMA3_CNT = 0;                              \
    REG_DMA3_SAD = (int)(_src);                    \
    REG_DMA3_DAD = (int)(_dst);                    \
    REG_DMA3_CNT = (_count)| DMA_ON|DMA_SRC_FIXED; 
extern u16 *videobuffer;
#endif

#define MODE_3 3
#define MODE_4 4
#define BLACK_INDEX 0
#define BLUE_INDEX 1
#define RED_INDEX 2
#define ON 1
#define OFF 0

#define BUTTON_START   1 << 3
#define BUTTON_SELECT   1 << 2
#define BUTTON_UP   1 << 6
#define BUTTON_DOWN 1 << 7
#define BUTTON_A			0x0001      // A Button
#define BUTTON_B			0x0002      // B Button
#define BUTTON_RIGHT        0x0010      // Right key
#define BUTTON_LEFT         0x0020      // Left key
#define TRIGGER_RIGHT	    0x0100      // R shoulder Button
#define TRIGGER_LEFT         0x0200		// L shoulder Button







void drawLine(int, int, int, u16);
void drawRect(int row, int col, 
              int height, int width, u16 color );
void drawRectDMA(int row, int col, 
              int height, int width, u16 color );
void waitForVBlank();
void GBASetup();
bool keyHit(int theKey);
void drawBackground(int code);
void FlipPage();
int cheatCheck();
void setPixel4(int r, int c, int value);
void drawLineMode4(int row, int col, int width, 
				   int index);
void drawRectMode4( int row, int col, 
				   int height, int width, int index );
void drawRectDMAMode4( int row, int col, 
				   int height, int width, int index );
void initializeGBA(u16 mode);
void paintBackground(const u16 map[]);
bool keepGoing();


//my structure use
	typedef struct {
	int drawn; //whether or not the game has been drawn
	int cheat; //whether or not the cheat is active
	} Junk;
