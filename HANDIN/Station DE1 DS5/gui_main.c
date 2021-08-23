#include "graphics.h"
#include "touch.h"
#include "wifi.h"

int main()
{
    struct Point p;
    int i = 0;
    int j = 0;
    int length;
    int ctr = 0;
    char myList[1000];
    int state = 0;

   
    Init_Touch();

    clearScreen();

    setup_WiFi();

    displayMainScreen();

    p.x = 1000;
    p.y = 1000;

    char *myFood[1000];
    length = pull_database(myFood);
    
    //For logging purposes
    for (j = 0; j < length; j++)
    {

        printf(myFood[j]);
    }

    if (length >= 3)
    {
        drawShoppingItem(myFood[0], 0);
        drawShoppingItem(myFood[1], 1);
        drawShoppingItem(myFood[2], 2);
    }
    else if (length == 2)
    {
        drawShoppingItem(myFood[0], 0);
        drawShoppingItem(myFood[1], 1);
    }
    else if (length == 1)
    {
        drawShoppingItem(myFood[0], 0);
    }
    else if (length == 0)
    {
        drawShoppingItem("Your list is empty", 1);
    }

    for (i = 0; i < length; i++)
        printf(myFood[i]);

    i = 0;

    while (1)
    {

        p.x = 1000;
        p.y = 1000;

        if (state == 0) //Main Screen State 
        {
            //Getting the touch coordinates  
            while (p.x >= 500 || p.y >= 800)
            {
                p = GetPress();
            }

            printf("Pressed at: %d, %d\n", p.x, p.y);

            //Increment i accordingly depending if user presses the up or down button 
            if (360 <= p.x && p.x <= 420 && 420 <= p.y && p.y <= 490 && i < length - 3)
                i++;
            else if (90 <= p.x && p.x <= 150 && 420 <= p.y && p.y <= 490 && i > 0)
                i--;

            //Sending list
            else if (360 <= p.x && p.x <= 440 && 520 <= p.y && p.y <= 620) //Checks for send button coordinates
            {
                if(length == 0) {
                    displayLoadingDialog("SHOPAL LIST EMPTY");
                    delay(3000);
                }

                else {
                    displayLoadingDialog("SENDING...");
                    send_shopal_list(); 
                }
                

                displayMainScreen();
            }

        //Clearing list
        else if (350 <= p.x && p.x <= 420 && 640 <= p.y && p.y <= 750) //Checks for clear button coordinates
        {
            ;
            displayLoadingDialog("CLEARING...");

            delete_database();
            length = 0;
            displayMainScreen();
        }

        //Updating list
        else if (170 <= p.x && p.x <= 330 && 540 <= p.y && p.y <= 710) //Checks for refresh button coordinates
        {
            displayLoadingDialog("UPDATING...");
            length = pull_database(myFood);
            i = 0;

            if (length > 0)
            {
                strcpy(myList, "\\nThis is your shpping list:\\n");
                for (ctr = 0; ctr < length; ctr++)
                {
                    strcat(myList, myFood[ctr]);
                    strcat(myList, ", ");
                }
                strcat(myList, "Have fun shopping!\\n");
            }
            
            displayMainScreen();
        }
        //Display List depending on the length of shopping list
        if (length >= 3)
        {
            drawShoppingItem(myFood[i], 0);
            drawShoppingItem(myFood[i + 1], 1);
            drawShoppingItem(myFood[i + 2], 2);
        }
        else if (length == 2)
        {
            drawShoppingItem(myFood[0], 0);
            drawShoppingItem(myFood[1], 1);
        }
        else if (length == 1)
        {
            drawShoppingItem(myFood[0], 0);
        }
        else if (length == 0)
        {
            drawShoppingItem("Your list is empty", 1);
        }
        //If help button pressed, go to help screen
        if (80 <= p.x && p.x <= 160 && 670 <= p.y && p.y <= 750)
        {
            state = 1;
            displayHelpScreen();
        }

        WaitForRelease();
    }
    else if (state == 1) //Help Screen State 
    {
        while (p.x >= 500 || p.y >= 800)
        {
            p = GetPress();
        }
        //Go to main screen if back button is pressed
        if (330 <= p.x && p.x <= 390 && 560 <= p.y && p.y <= 670)
        {
            state = 0;
            displayMainScreen();
            if (length >= 3)
            {
                drawShoppingItem(myFood[0], 0);
                drawShoppingItem(myFood[1], 1);
                drawShoppingItem(myFood[2], 2);
            }
            else if (length == 2)
            {
                drawShoppingItem(myFood[0], 0);
                drawShoppingItem(myFood[1], 1);
            }
            else if (length == 1)
            {
                drawShoppingItem(myFood[0], 0);
            }
            else if (length == 0)
            {
                drawShoppingItem("Your list is empty", 1);
            }
        }

        WaitForRelease();
    }
}

return 0;
}