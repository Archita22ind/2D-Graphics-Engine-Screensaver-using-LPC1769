/*
 ===============================================================================
 Name        : DrawSquare.c
 Author      : Archita Chakraborty
 Version     : 1.0
 Copyright   : $(copyright)
 Description : function and main definition
 ===============================================================================
 */

#include <cr_section_macros.h>
#include <NXP/crp.h>
#include "LPC17xx.h"                        /* LPC17xx definitions */
#include "ssp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>



/* Be careful with the port number and location number, because

 some of the location may not exist in that port. */

#define PORT_NUM            0


uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];


#define ST7735_TFTWIDTH 127
#define ST7735_TFTHEIGHT 159

#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_SLPOUT 0x11
#define ST7735_DISPON 0x29


#define swap(x, y) {x = x + y; y = x - y; x = x - y ;}

// defining color values
#define GREEN 0x00FF00
#define DARKBLUE 0x000055
#define BLACK 0x000000
#define BLUE 0x0007FF
#define RED 0xFF0000
#define MAGENTA 0x00F81F
#define WHITE 0xFFFFFF
#define PURPLE 0xCC33FF
#define BROWN 0x783F04
#define SKY 0XCFE2F3
#define ORANGE 0XCC5500
#define PINK 0xFF033E
#define YELLOW 0xFFE135
#define LEAVES 0x737000
#define LIGHTBLUE 0x00FFE0


int _height = ST7735_TFTHEIGHT;
int _width = ST7735_TFTWIDTH;
float lambda = 0.8;
struct Point{

	int16_t x;
	int16_t y;

};

void spiwrite(uint8_t c)
{

    int pnum = 0;
    src_addr[0] = c;
    SSP_SSELToggle( pnum, 0 );
    SSPSend( pnum, (uint8_t *)src_addr, 1 );
    SSP_SSELToggle( pnum, 1 );
}



void writecommand(uint8_t c)
{
    LPC_GPIO0->FIOCLR |= (0x1<<21);
    spiwrite(c);
}



void writedata(uint8_t c)
{
    LPC_GPIO0->FIOSET |= (0x1<<21);
    spiwrite(c);
}


void writeword(uint16_t c)
{
    uint8_t d;
    d = c >> 8;
    writedata(d);
    d = c & 0xFF;
    writedata(d);
}



void write888(uint32_t color, uint32_t repeat)
{
    uint8_t red, green, blue;
    int i;
    red = (color >> 16);
    green = (color >> 8) & 0xFF;
    blue = color & 0xFF;
    for (i = 0; i< repeat; i++) {
        writedata(red);
        writedata(green);
        writedata(blue);
    }
}



void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    writecommand(ST7735_CASET);
    writeword(x0);
    writeword(x1);
    writecommand(ST7735_RASET);
    writeword(y0);
    writeword(y1);
}


void fillrect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)
{

    int16_t width, height;
    width = x1-x0+1;
    height = y1-y0+1;
    setAddrWindow(x0,y0,x1,y1);
    writecommand(ST7735_RAMWR);
    write888(color,width*height);
}



void lcddelay(int ms)
{

    int count = 24000;
    int i;
    for( i = count*ms; i--; i > 0);
}



void lcd_init()
{

 int i;
 printf("LCD Demo Begins!!!\n");
 // Set pins P0.16, P0.21, P0.22 as output
 LPC_GPIO0->FIODIR |= (0x1<<16);

 LPC_GPIO0->FIODIR |= (0x1<<21);

 LPC_GPIO0->FIODIR |= (0x1<<22);

 // Hardware Reset Sequence
 LPC_GPIO0->FIOSET |= (0x1<<22);
 lcddelay(500);

 LPC_GPIO0->FIOCLR |= (0x1<<22);
 lcddelay(500);

 LPC_GPIO0->FIOSET |= (0x1<<22);
 lcddelay(500);

 // initialize buffers
 for ( i = 0; i < SSP_BUFSIZE; i++ )
 {

   src_addr[i] = 0;
   dest_addr[i] = 0;
 }

 // Take LCD display out of sleep mode
 writecommand(ST7735_SLPOUT);
 lcddelay(500);

 // Turn LCD display on
 writecommand(ST7735_DISPON);
 lcddelay(500);
}


void drawPixel(int16_t x, int16_t y, uint32_t color)
{

    if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
        return;

    setAddrWindow(x, y, x + 1, y + 1);
    writecommand(ST7735_RAMWR);
    write888(color, 1);
}


void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)
{

    int16_t slope = abs(y1 - y0) > abs(x1 - x0);
    if (slope) {
        swap(x0, y0);
        swap(x1, y1);
    }

    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);
    int16_t err = dx / 2;
    int16_t ystep;
    if (y0 < y1) {
        ystep = 1;
    }

    else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (slope) {
            drawPixel(y0, x0, color);
        }

        else {
            drawPixel(x0, y0, color);
        }

        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

//draw square function

void drawSquare(int point0_x, int point0_y, int sideLength, int16_t noOfLayers, uint32_t color, float UserInputLambda) {

    //setting 4 points of the square
    int16_t point1_x = point0_x + sideLength;
    int16_t point1_y = point0_y;
    int16_t point2_x = point1_x;
    int16_t point2_y = point1_y + sideLength;
    int16_t point3_x = point0_x;
    int16_t point3_y = point0_y + sideLength;

    //draw
    int16_t pTemp_x = point0_x;
    int16_t pTemp_y = point0_y;
    float lambda = 0;
    for (int i = 0; i < noOfLayers; i++) {
        pTemp_x = point0_x;
        pTemp_y = point0_y;

        point0_x = point0_x + lambda * (point1_x - point0_x);
        point0_y = point0_y + lambda * (point1_y - point0_y);

        point1_x = point1_x + lambda * (point2_x - point1_x);
        point1_y = point1_y + lambda * (point2_y - point1_y);

        point2_x = point2_x + lambda * (point3_x - point2_x);
        point2_y = point2_y + lambda * (point3_y - point2_y);

        point3_x = point3_x + lambda * (pTemp_x - point3_x);
        point3_y = point3_y + lambda * (pTemp_y - point3_y);

        //connecting Points P0 and P1
        lcddelay(20);
        drawLine(point0_x, point0_y, point1_x, point1_y, color);

        //connecting Points P1 and P2
        lcddelay(20);
        drawLine(point1_x, point1_y, point2_x, point2_y, color);

        //connecting Points P2 and P3
        lcddelay(20);
        drawLine(point2_x, point2_y, point3_x, point3_y, color);

        //connecting Points P3 and P0
        lcddelay(20);
        drawLine(point3_x, point3_y, point0_x, point0_y, color);

        lambda = UserInputLambda;
    }
}

//Draw Tree Screen saver

//Rotation of Branches using Rotation Matrix along with Translation MAtrix and it's Inverse

struct Point RotateBranches(struct Point p1, struct Point p2, int radian) {

	float angle = radian * 3.14 / 180;

	float delta_x = -1 * p1.x;

	float delta_y = -1 * p1.y;

	float translationMatrix[3][3] = {{1, 0 , delta_x}, {0, 1, delta_y}, {0, 0, 1}};

	float rotationMatrix[3][3] ={{cos(angle), -sin(angle), 0}, {sin(angle), cos(angle), 0}, {0, 0, 1}};

	float inverseTraslationMatrix[3][3] = {{1, 0, -delta_x}, {0, 1, -delta_y}, {0, 0, 1}};

	float tempMatrix[3][3] = {{0, 0 , 0}, {0, 0, 0}, {0, 0, 0}};

	// Rotation Matrix * Translation Matrix

	for (int i = 0; i < 3; i++) {

		for (int j = 0; j < 3; j++) {

			for (int k = 0; k < 3; k++){

				tempMatrix[i][j] += rotationMatrix[i][k] * translationMatrix[k][j];

			}

		}

	}

	float compositionMatrix[3][3] = {{0, 0 , 0}, {0, 0, 0}, {0, 0, 0}};

	for (int i = 0; i < 3; i++) {

		for (int j = 0; j < 3; j++) {

			for (int k = 0; k < 3; k++){

				compositionMatrix[i][j] += inverseTraslationMatrix[i][k] * tempMatrix[k][j];

			}

		}

	}


	struct Point newPoint = {0, 0};

	newPoint.x = compositionMatrix[0][0] * p2.x + compositionMatrix[0][1] * p2.y + compositionMatrix[0][2];

	newPoint.y = compositionMatrix[1][0] * p2.x + compositionMatrix[1][1] * p2.y + compositionMatrix[1][2];

	return newPoint;

}



 // finding Tip point of the branch

struct Point branchTip(struct Point point1, struct Point point2, int angle) {

	srand(time(0));

	struct Point point_temp = {0,0};

	point_temp.x = point2.x + lambda * (point2.x - point1.x);

	point_temp.y = point2.y + lambda * (point2.y - point1.y);

	struct Point newPoint = {0,0};

	newPoint = RotateBranches(point2, point_temp, angle);

	return newPoint;

}


//main draw tree function
void drawTree (struct Point trunkBottom, struct Point trunkTop, int layers) {

	int arrayLen = 500;

	struct Point arr[arrayLen][2];

	arr[0][0] = trunkBottom;

	arr[0][1] = trunkTop;

	int fastMovingIndex = 1;

	int slowMovingIndex = 0;

	uint32_t color;

	while (fastMovingIndex < arrayLen) {

		if (fastMovingIndex < 50) {

			color = BROWN;

		} else {

			color = LEAVES;

		}

		float radian =rand()%4 + 28; //randomizing the degree of rotation of the branches


			//drawing left tilted branches

			arr[fastMovingIndex][0] = arr[slowMovingIndex][1];

			arr[fastMovingIndex][1] = branchTip(arr[slowMovingIndex][0], arr[slowMovingIndex][1], radian);


			drawLine(arr[fastMovingIndex][0].x, arr[fastMovingIndex][0].y, arr[fastMovingIndex][1].x, arr[fastMovingIndex][1].y, color);

			fastMovingIndex++;

			//drawing straight branches

			arr[fastMovingIndex][0] = arr[slowMovingIndex][1];

			arr[fastMovingIndex][1] = branchTip(arr[slowMovingIndex][0], arr[slowMovingIndex][1], 0);

			drawLine(arr[fastMovingIndex][0].x, arr[fastMovingIndex][0].y, arr[fastMovingIndex][1].x, arr[fastMovingIndex][1].y, color);

			fastMovingIndex++;

			//draw right branches

			arr[fastMovingIndex][0] = arr[slowMovingIndex][1];

			arr[fastMovingIndex][1] = branchTip(arr[slowMovingIndex][0], arr[slowMovingIndex][1], -radian);


			drawLine(arr[fastMovingIndex][0].x, arr[fastMovingIndex][0].y, arr[fastMovingIndex][1].x, arr[fastMovingIndex][1].y, color);

			fastMovingIndex++;



		slowMovingIndex++;

	}

}





// Main Function

int main (void)
{
	float userInputL;

    uint32_t pnum = PORT_NUM;
    pnum = 0 ;
    if ( pnum == 0 )
        SSP0Init();

    else
        puts("Port number is not correct");

    lcd_init();

    fillrect(0, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, WHITE);

    printf("specify Value of Lambda\n");

    scanf("%f",&userInputL);

    int xPosition;
    int yPosition ;
    int sideLength = 0;
    int noOfLevels = 0;
    int ArrayOfColors[13] = {BLACK, GREEN, RED, PURPLE,DARKBLUE,PINK,YELLOW, BROWN,MAGENTA,ORANGE,WHITE};
    uint32_t color = 0;

    //Draw 10 squares with random position using randomize function rand()
    for(int i=1; i<=10; i++){
       	xPosition = 0.05 * ST7735_TFTWIDTH + rand() % 100 + 5;//xPos: approx range between 11 to 110
       	yPosition = 0.10 * ST7735_TFTHEIGHT + rand() % 100+ 10;//yPos: approx range between 20  to 130
       	sideLength = rand() % 5 + 30;//side length: range between 30 to 34
       	noOfLevels = rand() % 2 +12 ;//level: range between 10 to 13
       	drawSquare(xPosition,yPosition,sideLength,noOfLevels,ArrayOfColors[color++],userInputL);
    }

    printf("\nEnd of Square Screen saver Demo!");

    lcddelay(500);

    lcd_init();

    	//Creating the background of display as Sky and GRASS

    	fillrect(0, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, LIGHTBLUE);

    	fillrect(0, 0, ST7735_TFTWIDTH, 0.25 * ST7735_TFTHEIGHT, GREEN);

    	//Creation of 5 trees in the forest for more clarity

    for(int j=0;j<5;j++)
    {
    	int k = rand()%60 +12;  //randomizing the tree trunks for different regions print
    	int l= rand()%14+15;

    	struct Point trunkBottom = {0.20 * ST7735_TFTWIDTH + k, 0.08 * ST7735_TFTHEIGHT+l};

    	struct Point trunkTop = {0.20 * ST7735_TFTWIDTH + k, 0.25 * ST7735_TFTHEIGHT+l};

    	//drawing trunk width
    	for (int i = -2; i < 3; i++) {

    		drawLine(trunkBottom.x + i, trunkBottom.y, trunkTop.x + i, trunkTop.y, BROWN);

    	}

    	//draw tree call function

    	drawTree (trunkBottom, trunkTop, 9);  //more than 8 levels of branches drawn


    }

    printf("\nEnd of Forest Screen Saver Demo!");


    return 0;

}
