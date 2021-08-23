#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "fonts.h"
#include "Colours.h"

#define XRES 800
#define YRES 480

// graphics register addresses
#define GraphicsCommandReg (*(volatile unsigned short int *)(0xFF210000))
#define GraphicsStatusReg (*(volatile unsigned short int *)(0xFF210000))
#define GraphicsX1Reg (*(volatile unsigned short int *)(0xFF210002))
#define GraphicsY1Reg (*(volatile unsigned short int *)(0xFF210004))
#define GraphicsX2Reg (*(volatile unsigned short int *)(0xFF210006))
#define GraphicsY2Reg (*(volatile unsigned short int *)(0xFF210008))
#define GraphicsColourReg (*(volatile unsigned short int *)(0xFF21000E))
#define GraphicsBackGroundColourReg (*(volatile unsigned short int *)(0xFF210010))

/************************************************************************************************
** This macro pauses until the graphics chip status register indicates that it is idle
***********************************************************************************************/

#define WAIT_FOR_GRAPHICS                          \
    while ((GraphicsStatusReg & 0x0001) != 0x0001) \
        ;

// #defined constants representing values we write to the graphics 'command' register to get
// it to draw something. You will add more values as you add hardware to the graphics chip
// Note DrawHLine, DrawVLine and DrawLine at the moment do nothing - you will modify these

#define DrawHLine 1
#define DrawVLine 2
#define DrawLine 3
#define PutAPixel 0xA
#define GetAPixel 0xB
#define ProgramPaletteColour 0x10

// defined constants representing colours pre-programmed into colour palette
// there are 256 colours but only 8 are shown below, we write these to the colour registers
//
// the header files "Colours.h" contains constants for all 256 colours
// while the course file "ColourPaletteData.c" contains the 24 bit RGB data
// that is pre-programmed into the palette

#define BLACK 0
#define WHITE 1
#define RED 2
#define LIME 3
#define BLUE 4
#define YELLOW 5
#define CYAN 6
#define MAGENTA 7

//Division
#define DIV_SHOPPING_LIST 500
#define DIV_VOICE 300

//Shopping List Constants
#define X_LIST_START 25
#define Y_LIST_START 75
#define MARGIN_SHOPPING_LIST 25
#define X_LIST_OFFSET 375
#define Y_LIST_OFFSET 100
#define MAX_LIST_SIZE 3

//Scroll Bar Constants
#define SCROLL_BUTTON_SIZE 50

//Normal Button
#define MARGIN_BUTTON_25 25
#define MARGIN_BUTTON_50 50

#define BUTTON_SIZE_HEIGHT 50
#define BUTTON_SIZE_WIDTH 100

/*******************************************************************************************
* This function writes a single pixel to the x,y coords specified using the specified colour
* Note colour is a byte and represents a palette number (0-255) not a 24 bit RGB value
********************************************************************************************/
void WriteAPixel(int x, int y, int Colour)
{
    WAIT_FOR_GRAPHICS; // is graphics ready for new command

    GraphicsX1Reg = x; // write coords to x1, y1
    GraphicsY1Reg = y;
    GraphicsColourReg = Colour;     // set pixel colour
    GraphicsCommandReg = PutAPixel; // give graphics "write pixel" command
}

/*********************************************************************************************
* This function read a single pixel from the x,y coords specified and returns its colour
* Note returned colour is a byte and represents a palette number (0-255) not a 24 bit RGB value
*********************************************************************************************/

int ReadAPixel(int x, int y)
{
    WAIT_FOR_GRAPHICS; // is graphics ready for new command

    GraphicsX1Reg = x; // write coords to x1, y1
    GraphicsY1Reg = y;
    GraphicsCommandReg = GetAPixel; // give graphics a "get pixel" command

    WAIT_FOR_GRAPHICS;               // is graphics done reading pixel
    return (int)(GraphicsColourReg); // return the palette number (colour)
}

/**********************************************************************************
** subroutine to program a hardware (graphics chip) palette number with an RGB value
** e.g. ProgramPalette(RED, 0x00FF0000) ;
**
************************************************************************************/

void ProgramPalette(int PaletteNumber, int RGB)
{
    WAIT_FOR_GRAPHICS;
    GraphicsColourReg = PaletteNumber;
    GraphicsX1Reg = RGB >> 16;                 // program red value in ls.8 bit of X1 reg
    GraphicsY1Reg = RGB;                       // program green and blue into ls 16 bit of Y1 reg
    GraphicsCommandReg = ProgramPaletteColour; // issue command
}

/********************************************************************************************* This function draw a horizontal line, 1 pixel at a time starting at the x,y coords specified
*********************************************************************************************/

void HLine(int x1, int y1, int length, int Colour)
{

    WAIT_FOR_GRAPHICS; // is graphics ready for new command

    GraphicsColourReg = Colour; // set pixel colour

    GraphicsX1Reg = x1;
    GraphicsY1Reg = y1;
    GraphicsX2Reg = x1 + length - 1;
    GraphicsY2Reg = y1;

    GraphicsCommandReg = DrawHLine;
}

/********************************************************************************************* This function draw a vertical line, 1 pixel at a time starting at the x,y coords specified
*********************************************************************************************/

void VLine(int x1, int y1, int length, int Colour)
{

    WAIT_FOR_GRAPHICS; // is graphics ready for new command

    GraphicsColourReg = Colour; // set pixel colour

    GraphicsX1Reg = x1;
    GraphicsY1Reg = y1;
    GraphicsX2Reg = x1;
    GraphicsY2Reg = y1 + length - 1;

    GraphicsCommandReg = DrawVLine;
}

/*******************************************************************************
** Implementation of Bresenhams line drawing algorithm
*******************************************************************************/
int abs(int a)
{
    if (a < 0)
        return -a;
    else
        return a;
}

int sign(int a)
{
    if (a < 0)
        return -1;
    else if (a == 0)
        return 0;
    else
        return 1;
}

void Line(int x1, int y1, int x2, int y2, int Colour)
{

    WAIT_FOR_GRAPHICS; // is graphics ready for new command

    GraphicsColourReg = Colour; // set pixel colour

    GraphicsX1Reg = x1;
    GraphicsY1Reg = y1;
    GraphicsX2Reg = x2;
    GraphicsY2Reg = y2;

    GraphicsCommandReg = DrawLine;
}

void Rectangle(int x1, int y1, int x2, int y2, int Colour, int thickness)
{

    //      1       3
    //      + ----- +
    //      |       |
    //      |       |
    //      +-------+
    //      4       2

    for (int i = 0; i < thickness; ++i)
    {

        //Line from 1 to 3
        HLine(x1 + i, y1 + i, x2 - x1, Colour);

        //Line from 4 to 2
        HLine(x1 + i, y2 - i, x2 - x1, Colour);

        //Line from 1 to 4
        VLine(x1 + i, y1 + i, y2 - y1, Colour);

        //Line from 3 to 2
        VLine(x2 - i, y1 + i, y2 - y1 + 1, Colour);
    }
}

void FilledBorderedRectangle(int x1, int y1, int x2, int y2, int FillColour, int BorderColour, int BorderThickness)
{

    //      1       3
    //      + ----- +
    //      |       |
    //      |       |
    //      +-------+
    //      4       2

    int i = 0;

    for (i = 0; i < BorderThickness; ++i)
    {

        Rectangle(x1 + i, y1 + i, x2 - i, y2 - i, BorderColour, 1);
    }

    for (i = y1 + i; i < y2 - BorderThickness + 1; ++i)
    {

        HLine(x1 + BorderThickness, i, x2 - x1 - 2 * BorderThickness + 1, FillColour);
    }
}

void FilledRectangle(int x1, int y1, int x2, int y2, int Colour)
{

    //      1       3
    //      + ----- +
    //      |       |
    //      |       |
    //      +-------+
    //      4       2

    for (int i = y1; i < y2; ++i)
    {

        HLine(x1, i, x2 - x1, Colour);
    }
}

void Circle(int x1, int y1, int radius, int Colour, int thickness)
{

    for (int i = 0; i < thickness; ++i)
    {

        int x = radius - 1 + i;
        int y = 0;
        int dx = 1;
        int dy = 1;
        int error = dx - (radius << 1);

        while (x >= y)
        {

            //draws border arc points
            WriteAPixel(x1 + x, y1 + y, Colour);
            WriteAPixel(x1 + y, y1 + x, Colour);
            WriteAPixel(x1 - y, y1 + x, Colour);
            WriteAPixel(x1 - x, y1 + y, Colour);
            WriteAPixel(x1 - x, y1 - y, Colour);
            WriteAPixel(x1 - y, y1 - x, Colour);
            WriteAPixel(x1 + y, y1 - x, Colour);
            WriteAPixel(x1 + x, y1 - y, Colour);

            if (error <= 0)
            {

                y++;
                error += dy;
                dy += 2;
            }

            if (error > 0)
            {

                x--;
                dx += 2;
                error += dx - (radius << 1);
            }
        }
    }
}

void FilledCircle(int x1, int y1, int radius, int Colour)
{

    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int error = dx - (radius << 1);

    while (x >= y)
    {

        //draws line across circle
        HLine(x1 - y, y1 + x, 2 * y, Colour);
        HLine(x1 - x, y1 + y, 2 * x, Colour);
        HLine(x1 - x, y1 - y, 2 * x, Colour);
        HLine(x1 - y, y1 - x, 2 * y, Colour);

        if (error <= 0)
        {

            y++;
            error += dy;
            dy += 2;
        }

        if (error > 0)
        {

            x--;
            dx += 2;
            error += dx - (radius << 1);
        }
    }
}

void FilledCircleWithBorder(int x1, int y1, int radius, int fillColour, int borderColour, int borderThickness)
{

    bool first = 1;

    for (int i = 0; i < borderThickness; ++i)
    {

        int x = radius - 1 + i;
        int y = 0;
        int dx = 1;
        int dy = 1;
        int error = dx - (radius << 1);

        while (x >= y)
        {

            //draws border arc points
            WriteAPixel(x1 + x, y1 + y, borderColour);
            WriteAPixel(x1 + y, y1 + x, borderColour);
            WriteAPixel(x1 - y, y1 + x, borderColour);
            WriteAPixel(x1 - x, y1 + y, borderColour);
            WriteAPixel(x1 - x, y1 - y, borderColour);
            WriteAPixel(x1 - y, y1 - x, borderColour);
            WriteAPixel(x1 + y, y1 - x, borderColour);
            WriteAPixel(x1 + x, y1 - y, borderColour);

            if (first == 1)
            {

                //draws line across circle
                HLine(x1 - y + 1, y1 + x, 2 * (y - 1) + 1, fillColour);
                HLine(x1 - x + 1, y1 + y, 2 * (x - 1) + 1, fillColour);
                HLine(x1 - x + 1, y1 - y, 2 * (x - 1) + 1, fillColour);
                HLine(x1 - y + 1, y1 - x, 2 * (y - 1) + 1, fillColour);
            }

            if (error <= 0)
            {

                y++;
                error += dy;
                dy += 2;
            }

            if (error > 0)
            {

                x--;
                dx += 2;
                error += dx - (radius << 1);
            }
        }

        first = 0;
    }
}

void RandLines(int numLines)
{

    int colour, x1, y1, x2, y2, length;

    for (int i = 0; i < numLines; ++i)
    {

        x1 = rand() % 800;
        y1 = rand() % 480;
        x2 = rand() % 800;
        y2 = rand() % 480;

        length = rand() % 400;

        colour = rand() % 64;

        Line(x1, y1, x2, y2, colour);

        x1 = rand() % 800;
        y1 = rand() % 480;

        colour = rand() % 64;

        VLine(x1, y1, length, colour);

        x1 = rand() % 800;
        y1 = rand() % 480;

        length = rand() % 400;

        colour = rand() % 64;

        HLine(x1, y1, length, colour);
    }
}

/*************************************************************************************************
** This function draws a single ASCII character at the coord and colour specified
** it optionally ERASES the background colour pixels to the background colour
** This means you can use this to erase characters
**
** e.g. writing a space character with Erase set to true will set all pixels in the
** character to the background colour
**
*************************************************************************************************/
void OutGraphicsCharFont1(int x, int y, int fontcolour, int backgroundcolour, int c, int Erase)
{

    // using register variables (as opposed to stack based ones) may make execution faster
    // depends on compiler and CPU
    register int row, column, theX = x, theY = y;
    register int pixels;
    register char theColour = fontcolour;
    register int BitMask, theC = c;

    // if x,y coord off edge of screen don't bother
    // XRES and YRES are #defined to be 800 and 480 respectively

    if (((short)(x) > (short)(XRES - 1)) || ((short)(y) > (short)(YRES - 1)))
        return;

    // if printable character subtract hex 20
    if (((short)(theC) >= (short)(' ')) && ((short)(theC) <= (short)('~')))
    {

        theC = theC - 0x20;

        for (row = 0; (char)(row) < (char)(7); row++)
        {
            // get the bit pattern for row 0 of the character from the software font
            pixels = Font5x7[theC][row];
            BitMask = 16;

            for (column = 0; (char)(column) < (char)(5); column++)
            {

                // if a pixel in the character display it
                if ((pixels & BitMask))
                    WriteAPixel(theX + column, theY + row, theColour);

                else
                {

                    if (Erase == true)
                        // if pixel is part of background (not part of character)
                        // erase the background to value of variable BackGroundColour
                        WriteAPixel(theX + column, theY + row, backgroundcolour);
                }

                BitMask = BitMask >> 1;
            }
        }
    }
}

/*************************************************************************************************
** This function draws a single ASCII character at the coord and colour specified
** it optionally ERASES the background colour pixels to the background colour
** This means you can use this to erase characters
**
** e.g. writing a space character with Erase set to true will set all pixels in the
** character to the background colour
**
*************************************************************************************************/
void OutGraphicsCharFont2(int x, int y, int fontcolour, int backgroundcolour, int c, int Erase)
{

    // using register variables (as opposed to stack based ones) may make execution faster
    // depends on compiler and CPU
    register int row, column, theX = x, theY = y;
    register int pixels;
    register char theColour = fontcolour;
    register int BitMask, theC = c;

    // if x,y coord off edge of screen don't bother
    // XRES and YRES are #defined to be 800 and 480 respectively

    if (((short)(x) > (short)(XRES - 1)) || ((short)(y) > (short)(YRES - 1)))
        return;

    // if printable character subtract hex 20
    if (((short)(theC) >= (short)(' ')) && ((short)(theC) <= (short)('~')))
    {

        theC = theC - 0x20;

        for (row = 0; (char)(row) < (char)(14); row += 2)
        {

            for (int i = 0; i < 2; ++i)
            {

                // get the bit pattern for row 0 of the character from the software font
                pixels = Font10x14[theC][row + i];
                BitMask = 512;

                for (column = 0; (char)(column) < (char)(10); column++)
                {

                    // if a pixel in the character display it
                    if ((pixels & BitMask))
                        WriteAPixel(theX + column, theY + (row >> 1), theColour);

                    else
                    {

                        if (Erase == true)
                            // if pixel is part of background (not part of character)
                            // erase the background to value of variable BackGroundColour
                            WriteAPixel(theX + column, theY + (row >> 1), backgroundcolour);
                    }

                    BitMask = BitMask >> 1;
                }
            }
        }
    }
}

/*
// Given a character array, this function prints the text in the array starting at position (x,y) and will
// print on the same line untill a '\n' character is seen, in which case the function will start printing on a new line
//
// Fontnum selects the font to be printed
*/
void print(char text[], int x, int y, int textColour, int backgroundcolour, int fontNum)
{

    int xpos = x;
    int numChars = strlen(text);

    switch (fontNum)
    {

    case 1: // 5X7 Font Size

        for (int i = 0; i < numChars; i++)
        {

            if (text[i] == '\n')
            {

                i++; //skip the newline character for printing

                //move to new line position
                y += 7;
                xpos = x - i * 5;
            }

            OutGraphicsCharFont1(xpos + i * 5, y, textColour, backgroundcolour, ((int)text[i]), 0);
        }

        break;

    case 2: // 10X14 Font Size

        for (int i = 0; i < numChars; i++)
        {

            if (text[i] == '\n')
            {

                i++; //skip the newline character for printing

                //move to new line position
                y += 14;
                xpos = x - i * 12;
            }

            OutGraphicsCharFont2(xpos + i * 12, y, textColour, backgroundcolour, ((int)text[i]), 0);
        }

        break;
    }
}

/*
 * Method to draw a rectangular button with a coloured border and fill, and overlays a given message as centered text.
 *
*/
void Button(char text[], int x1, int y1, int x2, int y2, int borderColour, int borderThickness, int fillColour, int textColour, int fontNum)
{

    int numChars = strlen(text);
    int buttonCenterX = (x2 - x1) / 2 + x1;
    int buttonCenterY = (y2 - y1) / 2 + y1;

    int textStart = x1;

    FilledBorderedRectangle(x1, y1, x2, y2, fillColour, borderColour, borderThickness);

    if (fontNum == 1)
    {

        textStart = buttonCenterX - (numChars * 5) / 2;
        buttonCenterY -= 3;
    }
    else
    {

        textStart = buttonCenterX - (numChars * 10) / 2;
        buttonCenterY -= 5;
    }

    print(text, textStart, buttonCenterY, textColour, fillColour, fontNum);
}

void ButtonCircle(char text[], int x1, int y1, int radius, int fillColour, int borderColour, int borderThickness, int textColour, int fontNum)
{
    int textStart = x1;
    int numChars = strlen(text);
    int buttonCenterY = y1;

    FilledCircleWithBorder(x1, y1, radius, fillColour, borderColour, borderThickness);

    if (fontNum == 1)
    {

        textStart = x1 - (numChars * 5) / 2;
        buttonCenterY -= 3;
    }
    else
    {

        textStart = x1 - (numChars * 10) / 2;
        buttonCenterY -= 5;
    }

    print(text, textStart, buttonCenterY, textColour, fillColour, fontNum);
}

void displayVoiceButton()
{

    ButtonCircle("TALK", DIV_SHOPPING_LIST + DIV_VOICE / 2, YRES / 2 - 50, 75, WHITE, BLUE, 10, BLACK, 2);
}

/**
 * Method that draws the shopping item at the the specified position
 * */
void drawShoppingItem(char text[], int pos)
{

    int x1 = X_LIST_START;
    int y1 = Y_LIST_START + (Y_LIST_OFFSET + MARGIN_SHOPPING_LIST) * pos;
    int x2 = X_LIST_START + X_LIST_OFFSET;
    int y2 = y1 + Y_LIST_OFFSET;

    Button(text, x1, y1, x2, y2, BLUE, 5, WHITE, BLACK, 2);
}

/**
 * Method that clears screen by painting the whole screen white
 * */
void clearScreen()
{

    FilledRectangle(0, 0, XRES, YRES, WHITE);
}

void displayTitle()
{

    print("Shopal", 25, 25, INDIGO, WHITE, 2);
    HLine(25, 50, XRES - MARGIN_BUTTON_50, BLUE);
}

void displayScrollElementAndSendAndHelpButton()
{

    int x1Up;
    int y1Up;
    int x2Up;
    int y2Up;
    int x1Down;
    int y1Down;
    int x2Down;
    int y2Down;
    int scrollLineLength;

    //Cordinates for UP Button
    x1Up = X_LIST_START + X_LIST_OFFSET + MARGIN_BUTTON_50;
    y1Up = Y_LIST_START;
    x2Up = x1Up + SCROLL_BUTTON_SIZE;
    y2Up = y1Up + SCROLL_BUTTON_SIZE;

    Button("^", x1Up, y1Up, x2Up, y2Up, BLUE, 5, WHITE, BLACK, 2);

    scrollLineLength = (MAX_LIST_SIZE * Y_LIST_OFFSET + (MAX_LIST_SIZE - 1) * MARGIN_SHOPPING_LIST) - 2 * SCROLL_BUTTON_SIZE;

    VLine(x1Up + SCROLL_BUTTON_SIZE / 2, y2Up, scrollLineLength, BLACK);

    //Cordinates for DOWN Button
    x1Down = x1Up;
    y1Down = y2Up + scrollLineLength;
    x2Down = x1Down + SCROLL_BUTTON_SIZE;
    y2Down = y1Down + SCROLL_BUTTON_SIZE;

    Button("v", x1Down, y1Down, x2Down, y2Down, BLUE, 5, WHITE, BLACK, 2);

    //Send Button
    int x1Send = x2Down + MARGIN_BUTTON_50;
    int y1Send = y1Down;
    int x2Send = x1Send + BUTTON_SIZE_WIDTH;
    int y2Send = y2Down;
    Button("SEND", x1Send, y1Send, x2Send, y2Send, BLUE, 5, BLUE_VIOLET, WHITE, 2);

    //Help Button
    int x1Help = x2Send + MARGIN_BUTTON_25;
    int y1Help = y1Down;
    int x2Help = x1Help + BUTTON_SIZE_WIDTH;
    int y2Help = y2Down;
    Button("HELP", x1Help, y1Help, x2Help, y2Help, BLUE, 5, BLUE_VIOLET, WHITE, 2);
}

void displayMainScreen()
{

    displayTitle();
    displayScrollElementAndSendAndHelpButton();
    displayVoiceButton();
}

int main(void)
{

    clearScreen();

    displayMainScreen();

    drawShoppingItem("Apples", 0);
    drawShoppingItem("Oranges", 1);
    drawShoppingItem("Bananas", 2);

    return 0;
}
