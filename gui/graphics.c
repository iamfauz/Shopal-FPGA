#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "fonts.h"
#include "Colours.h"
#include "touch.h"
#include "graphics.h"

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

/* 
 * Method to draw a simple rectangle
*/
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

/**
 * Method to draw a filled bordered rectangle 
 * */
void FilledBorderedRectangle(int x1, int y1, int x2, int y2, int FillColour, int BorderColour, int BorderThickness)
{

    //      1       3
    //      + ----- +
    //      |       |
    //      |       |
    //      +-------+
    //      4       2

    int i = 0;

    //Draw a the border
    for (i = 0; i < BorderThickness; ++i)
        Rectangle(x1 + i, y1 + i, x2 - i, y2 - i, BorderColour, 1);

    //Filling the rectangle
    for (i = y1 + i; i < y2 - BorderThickness + 1; ++i)
        HLine(x1 + BorderThickness, i, x2 - x1 - 2 * BorderThickness + 1, FillColour);
}

/**
 * Method to draw a filled rectangle 
 * */
void FilledRectangle(int x1, int y1, int x2, int y2, int Colour)
{

    //      1       3
    //      + ----- +
    //      |       |
    //      |       |
    //      +-------+
    //      4       2

    for (int i = y1; i < y2; ++i)
        HLine(x1, i, x2 - x1, Colour);
}

/**
 * Method to draw a simple circle using the Bresenheim algorithm
 * */
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
            //Draws border Arc points
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

/**
 * Method to draw a filled  circle using the Bresenheim algorithm
 * */
void FilledCircle(int x1, int y1, int radius, int Colour)
{

    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int error = dx - (radius << 1);

    while (x >= y)
    {
        //Draws line across circle
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

/**
 * Method to draw a filled circle with border using the Bresenheim algorithm
 * */
void FilledCircleWithBorder(int x1, int y1, int radius, int fillColour, int borderColour, int borderThickness)
{

    bool isFirst = 1;

    for (int i = 0; i < borderThickness; ++i)
    {

        int x = radius - 1 + i;
        int y = 0;
        int dx = 1;
        int dy = 1;
        int error = dx - (radius << 1);

        while (x >= y)
        {

            //Draws border arc points
            WriteAPixel(x1 + x, y1 + y, borderColour);
            WriteAPixel(x1 + y, y1 + x, borderColour);
            WriteAPixel(x1 - y, y1 + x, borderColour);
            WriteAPixel(x1 - x, y1 + y, borderColour);
            WriteAPixel(x1 - x, y1 - y, borderColour);
            WriteAPixel(x1 - y, y1 - x, borderColour);
            WriteAPixel(x1 + y, y1 - x, borderColour);
            WriteAPixel(x1 + x, y1 - y, borderColour);

            if (isFirst == 1)
            {

                //Draws line across circle
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

        isFirst = 0;
    }
}

/**
 * Method to draw random lines of different colors
 * */
void RandLines(int numLines)
{

    int colour, x1, y1, x2, y2, length;

    for (int i = 0; i < numLines; ++i)
    {
        //Normal Line
        x1 = rand() % 800;
        y1 = rand() % 480;
        x2 = rand() % 800;
        y2 = rand() % 480;
        length = rand() % 400;
        colour = rand() % 64;

        Line(x1, y1, x2, y2, colour);

        //Vertical Lin
        x1 = rand() % 800;
        y1 = rand() % 480;
        colour = rand() % 64;

        VLine(x1, y1, length, colour);

        //Horizantal line
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

/**
 *Method that prints the text[] array starting at the specified coordinates and also goes to the new line when
 newline character is reached
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

                i++; //skip the newline character

                //move to new line pos
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

                i++; //skip the newline character

                //move to new line pos
                y += 14;
                xpos = x - i * 12;
            }

            OutGraphicsCharFont2(xpos + i * 12, y, textColour, backgroundcolour, ((int)text[i]), 0);
        }

        break;
    }
}

/**
 * Method that draws a filled and bordered rectangular button and overlays the text[] in the center
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

/**
 * Method that draws a filled and bordered circular button and overlays the text[] in the center
*/
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

void displayRefreshButton()
{

    ButtonCircle("REFRESH", DIV_SHOPPING_LIST + DIV_VOICE / 2, YRES / 2, 75, WHITE, YELLOW_GREEN, 5, BLACK, 2);
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

    Button(text, x1, y1, x2, y2, YELLOW_GREEN, 2, WHITE, BLACK, 2);
}

/**
 * Method that clears screen by painting the whole screen white
 * */
void clearScreen()
{

    FilledRectangle(0, 0, XRES, YRES, WHITE);
}

/**
 * Method to display title
 * */
void displayTitle()
{

    print("Shopal", 25, 25, DARK_OLIVE_GREEN, WHITE, 2);
    HLine(25, 50, XRES - MARGIN_BUTTON_50, YELLOW_GREEN);
}

/**
 * Method that displays the scroll elemenst, send and help button
 * */
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

    Button("^", x1Up, y1Up, x2Up, y2Up, OLIVE_DRAB, 3, YELLOW_GREEN, WHITE, 2);

    scrollLineLength = (MAX_LIST_SIZE * Y_LIST_OFFSET + (MAX_LIST_SIZE - 1) * MARGIN_SHOPPING_LIST) - 2 * SCROLL_BUTTON_SIZE;

    VLine(x1Up + SCROLL_BUTTON_SIZE / 2, y2Up, scrollLineLength, YELLOW_GREEN);

    //Cordinates for DOWN Button
    x1Down = x1Up;
    y1Down = y2Up + scrollLineLength;
    x2Down = x1Down + SCROLL_BUTTON_SIZE;
    y2Down = y1Down + SCROLL_BUTTON_SIZE;

    Button("v", x1Down, y1Down, x2Down, y2Down, OLIVE_DRAB, 3, YELLOW_GREEN, WHITE, 2);

    //Send Button
    int x1Send = x2Down + MARGIN_BUTTON_50;
    int y1Send = y1Down;
    int x2Send = x1Send + BUTTON_SIZE_WIDTH;
    int y2Send = y2Down;
    Button("SEND", x1Send, y1Send, x2Send, y2Send, OLIVE_DRAB, 3, YELLOW_GREEN, WHITE, 2);

    //Clear Button
    int x1Clear = x2Send + MARGIN_BUTTON_25;
    int y1Clear = y1Down;
    int x2Clear = x1Clear + BUTTON_SIZE_WIDTH;
    int y2Clear = y2Down;
    Button("CLEAR", x1Clear, y1Clear, x2Clear, y2Clear, GOLDEN_ROD, 3, GOLD, WHITE, 2);

    //Help Button
    int x1Help = x2Clear - SCROLL_BUTTON_SIZE;
    int y1Help = y1Up;
    int x2Help = x1Help + SCROLL_BUTTON_SIZE;
    int y2Help = y1Help + SCROLL_BUTTON_SIZE;
    Button("?", x1Help, y1Help, x2Help, y2Help, GOLDEN_ROD, 3, GOLD, WHITE, 2);
}

/**
 * Method that displays the main screen
 * */
void displayMainScreen()
{
    clearScreen();
    displayTitle();
    displayScrollElementAndSendAndHelpButton();
    displayRefreshButton();
}

/**
 * Method that displays teh help screen
 * */
void displayHelpScreen()
{

    int x1Up;
    int y1Up;
    int x2Up;
    int y2Up;
    int x1Down;
    int y1Down;
    int x2Down;
    int y2Down;

    clearScreen();

    //Title
    print("Help", 25, 25, DARK_OLIVE_GREEN, WHITE, 2);
    HLine(25, 50, XRES - MARGIN_BUTTON_50, YELLOW_GREEN);

    //UP Button
    x1Up = MARGIN_BUTTON_25;
    y1Up = Y_LIST_START;
    x2Up = x1Up + SCROLL_BUTTON_SIZE;
    y2Up = y1Up + SCROLL_BUTTON_SIZE;

    //Displaying instructions for voice commands
    print("1. Activate Voice - Say 'Hello Shopal'", x1Up, y1Up, BLACK, WHITE, 2);
    print("2. Deactivate Device - Say 'Goodbye'", x1Up, y1Up + MARGIN_BUTTON_25, BLACK, WHITE, 2);
    print("3. Clear List - Say 'Clear 'Shopping List'", x1Up, y1Up + 2 * MARGIN_BUTTON_25, BLACK, WHITE, 2);
    print("4. Add Items - 'Add' + 'Item1' + 'and' + 'Item2' ..", x1Up, y1Up + 3 * MARGIN_BUTTON_25, BLACK, WHITE, 2);
    print("5. Remove Items - 'Remove' + 'Item1' + 'and' + 'Item2' ..", x1Up, y1Up + 4 * MARGIN_BUTTON_25, BLACK, WHITE, 2);

    x1Down = x1Up;
    y1Down = y2Up + MARGIN_BUTTON_25;
    x2Down = x1Down + SCROLL_BUTTON_SIZE;
    y2Down = y1Down + SCROLL_BUTTON_SIZE;

    //Refresh buton
    int x = x1Down + MARGIN_BUTTON_50;
    int y = y2Down + MARGIN_BUTTON_25 + MARGIN_BUTTON_50;
    int radius = 50;
    ButtonCircle("REFRESH", x, y, radius, WHITE, YELLOW_GREEN, 5, BLACK, 2);
    print("Press to refresh shopping list", x + radius + MARGIN_BUTTON_25, y - radius, BLACK, WHITE, 1);

    //Send Button
    int x1Send = x1Up;
    int y1Send = y + radius + MARGIN_BUTTON_25;
    int x2Send = x1Send + BUTTON_SIZE_WIDTH;
    int y2Send = y1Send + BUTTON_SIZE_HEIGHT;
    Button("SEND", x1Send, y1Send, x2Send, y2Send, OLIVE_DRAB, 3, YELLOW_GREEN, WHITE, 2);
    print("Press to send shopping list instantly to phone", x1Send + BUTTON_SIZE_WIDTH + MARGIN_BUTTON_25, y1Send, BLACK, WHITE, 1);

    //Clear Button
    int x1Clear = x1Up;
    int y1Clear = y2Send + MARGIN_BUTTON_25;
    int x2Clear = x1Clear + BUTTON_SIZE_WIDTH;
    int y2Clear = y1Clear + BUTTON_SIZE_HEIGHT;
    Button("CLEAR", x1Clear, y1Clear, x2Clear, y2Clear, GOLDEN_ROD, 3, GOLD, WHITE, 2);
    print("Press to clear shopping list", x1Clear + BUTTON_SIZE_WIDTH + MARGIN_BUTTON_25, y1Clear, BLACK, WHITE, 1);

    //Back Button
    int x1Back = XRES / 2 + 200;
    int y1Back = y1Clear - MARGIN_BUTTON_50;
    int x2Back = x1Back + BUTTON_SIZE_WIDTH;
    int y2Back = y1Back + BUTTON_SIZE_HEIGHT;
    Button("BACK", x1Back, y1Back, x2Back, y2Back, GOLDEN_ROD, 3, GOLD, WHITE, 2);
}

/**
 * Method that displays loading dialog
 * */
void displayLoadingDialog(char *action)
{

    int x1 = XRES / 2 - X_LOADING / 2;
    int y1 = YRES / 2 - Y_LOADING / 2;
    int x2 = x1 + X_LOADING;
    int y2 = y1 + Y_LOADING;
    Button(action, x1, y1, x2, y2, BLACK, 1, WHITE, BLACK, 2);
}
