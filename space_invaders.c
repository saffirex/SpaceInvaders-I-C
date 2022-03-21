#include <stdio.h>   //printf, scanf
#include <conio.h>   //kbhit,getch
#include <stdlib.h>  //exit, random
#include <stdbool.h> //bool
#include <unistd.h>  //sleep

#define ROW 24
#define COL 42
#define no_of_maps 7

void bullet_travel();         //travelling of the bullet
void bullet_remove();         //removal of bullet
void initboard();             //board mapping
void dispBoard();             //board display
bool bullet_hit();            // bullet collision
void bomb_spawn();            //bomb rendering
void startscreen();           //the initial screen
void endscreen();             //the end screen
int generateRandom(int, int); //random generator within a range

int pos_y = 22;
int pos_x = 19;
int score = 0;
int r, c;

char arrow = 030;
char board[ROW][COL];
char val_a = 176;
char val_hash = 254;
char val_percent = 219;
char val_ship = 206;

//Map print
void initboard()
{
    system("cls");
    FILE *fp;
    fp = fopen("levels.txt", "r");
    char ch;
    int chosen_map;

    printf("Choose a map from 1 to %d:", no_of_maps);
    scanf("%d", &chosen_map);
    fflush(stdin);
    //fseek(fp,0,SEEK_END);
    //int size= ftell(fp);
    //printf("size: %d\n",size);
    fseek(fp, ((ROW) * (COL) + ROW) * (chosen_map - 1), 0);

    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            ch = fgetc(fp);
            if (ch == EOF)
            {
                board[i][j] = '\0';
                break;
            }
            if (ch == '@') //2hits
            {
                board[i][j] = val_a;
            }
            else if (ch == '#') //1hit
            {
                board[i][j] = val_hash;
            }
            else if (ch == '%') //3hits
            {
                board[i][j] = val_percent;
            }
            else
            {
                board[i][j] = ch;
            }
        }
    }

    dispBoard();
    fclose(fp);
}

//Display board
void dispBoard()
{
    printf("Score = %d\n", score);
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            printf(" %c", board[i][j]);
        }
    }
}

//Function to generate random number
int generateRandom(int minimum_number, int max_number)
{
    return rand() % (max_number + 1 - minimum_number) + minimum_number;
}

int main()
{
    score = 0;
    startscreen();
    fflush(stdin);
    initboard();

    //Main Loop
    while (1)
    {
        system("cls");
        board[pos_y][pos_x] = val_ship;

        //Spaceship controls
        if (kbhit())
        {
            switch (getch())
            {
            case 'a':
                if (pos_x > 1)
                {
                    board[pos_y][pos_x] = ' ';
                    board[pos_y][pos_x - 1] = val_ship;
                    pos_x--;
                }
                break;

            case 'd':
                if (pos_x < COL - 3)
                {
                    board[pos_y][pos_x] = ' ';
                    board[pos_y][pos_x + 1] = val_ship;
                    pos_x++;
                }
                break;

            case 'j':
                board[pos_y - 1][pos_x] = arrow;
                break;
            }
        }
        dispBoard();
        bullet_travel();
        bullet_travel();
        bomb_spawn();
    }
    return 0;
}

void bullet_travel()
{
    for (r = 2; r < ROW; r++)
    {
        for (c = 1; c < COL; c++)
        {
            //same direction ma scan garda euta loop ma siddhai tala pugcha
            if (board[ROW - r][COL - c] == 'V')
            {
                if ((board[ROW - 3][COL - c] == 'V' || (board[ROW - 4][COL - c] == 'V' && board[ROW - 3][COL - c] == arrow)) && board[ROW - 2][COL - c] == val_ship)
                { //Spaceship Hit
                    fflush(stdin);
                    endscreen();
                }
                else if (board[ROW - 2][COL - c] == 'V')
                { //remove bomb
                    board[ROW - 2][COL - c] = ' ';
                }
                else if (board[ROW - r + 1][COL - c] != ' ' || board[ROW - r + 2][COL - c] == arrow)
                { //Skip the enemy
                    board[ROW - r + 2][COL - c] = 'V';
                }
                else
                { //bomb travel
                    board[ROW - r + 1][COL - c] = 'V';
                }
                board[ROW - r][COL - c] = ' ';
            }
            //bullet part
            if (board[r][c] == arrow)
            {
                if (bullet_hit() == 0)
                { //travel
                    board[r][c] = ' ';
                    board[r - 1][c] = arrow;
                }
                bullet_remove();
            }
        }
    }
}

void bullet_remove()
{
    if (board[1][c] == arrow)
    {
        board[1][c] = ' ';
    }
}

bool bullet_hit()
{
    if (board[r - 1][c] != ' ')
    {
        if (board[r - 1][c] != val_a && board[r - 1][c] != val_percent)
        { //single hit to anything except spaces, 178 (@, dotted), or 219(%,bigrect)
            board[r - 1][c] = ' ';
        }
        else if (board[r - 1][c] == val_a)
        { //2hits
            board[r - 1][c] = val_hash;
        }
        else if (board[r - 1][c] == val_percent)
        { //3hits, becomes 178, 178 becomes 254, 254 ultimately becomes ' '
            board[r - 1][c] = val_a;
        }
        board[r][c] = ' ';
        score += 10;
        return 1;
    }
    return 0;
}

void bomb_spawn()
{

    for (int i = 1; i < 25; i++)
    {
        //generaterandom(min, max) value
        int randomRow = generateRandom(1, ROW - 3);
        int randomColumn = generateRandom(1, COL - 1);

        //initialize bomb position
        char bombGuess = board[randomRow][randomColumn];
        if (bombGuess == ' ')
        {
            char bombGuessUp = board[randomRow - 1][randomColumn];
            char bombGuessDown = board[randomRow + 1][randomColumn];

            //bomb spawn condition
            if (bombGuessUp != ' ' && bombGuessUp != arrow && bombGuessUp != '*' && bombGuessDown == ' ')
            {
                board[randomRow][randomColumn] = 'V';
            }
        }
    }
}

void startscreen()
{

    char text1[20][135] = {
        "             /$$$$$$         /$$$$$$$          /$$$$$$          /$$$$$$         /$$$$$$$$                                             ",
        "            /$$__  $$       | $$__  $$        /$$__  $$        /$$__  $$       | $$_____/                                             ",
        "           | $$  \\__/       | $$  \\ $$       | $$  \\ $$       | $$  \\__/       | $$                                                   ",
        "           |  $$$$$$        | $$$$$$$/       | $$$$$$$$       | $$             | $$$$$                                                ",
        "            \\____  $$       | $$____/        | $$__  $$       | $$             | $$__/                                                ",
        "            /$$  \\ $$       | $$             | $$  | $$       | $$    $$       | $$                                                   ",
        "           |  $$$$$$/       | $$             | $$  | $$       |  $$$$$$/       | $$$$$$$$                                             ",
        "            \\______/        |__/             |__/  |__/        \\______/        |________/                                             ",
        "                                                                                                                                      ",
        "                                                                                                                                      ",
        "                                                                                                                                      ",
        "            /$$$$$$        /$$   /$$        /$$    /$$         /$$$$$$         /$$$$$$$         /$$$$$$$$        /$$$$$$$             ",
        "           |_  $$_/       | $$$ | $$       | $$   | $$        /$$__  $$       | $$__  $$       | $$_____/       | $$__  $$            ",
        "             | $$         | $$$$| $$       | $$   | $$       | $$  \\ $$       | $$  \\ $$       | $$             | $$  \\ $$            ",
        "             | $$         | $$ $$ $$       |  $$ / $$/       | $$$$$$$$       | $$  | $$       | $$$$$          | $$$$$$$/            ",
        "             | $$         | $$  $$$$        \\  $$ $$/        | $$__  $$       | $$  | $$       | $$__/          | $$__  $$            ",
        "             | $$         | $$\\  $$$         \\  $$$/         | $$  | $$       | $$  | $$       | $$             | $$  \\ $$            ",
        "            /$$$$$$       | $$ \\  $$          \\  $/          | $$  | $$       | $$$$$$$/       | $$$$$$$$       | $$  | $$            ",
        "           |______/       |__/  \\__/           \\_/           |__/  |__/       |_______/        |________/       |__/  |__/            "};

    char text2[20][135] = {
        "            $$$$$$\\         $$$$$$$\\          $$$$$$\\          $$$$$$\\         $$$$$$$$\\                                              ",
        "           $$  __$$\\        $$  __$$\\        $$  __$$\\        $$  __$$\\        $$  _____|                                             ",
        "           $$ /  \\__|       $$ |  $$ |       $$ /  $$ |       $$ /  \\__|       $$ |                                                   ",
        "           \\$$$$$$\\         $$$$$$$  |       $$$$$$$$ |       $$ |             $$$$$\\                                                 ",
        "            \\____$$\\        $$  ____/        $$  __$$ |       $$ |             $$  __|                                                ",
        "           $$\\   $$ |       $$ |             $$ |  $$ |       $$ |  $$\\        $$ |                                                   ",
        "           \\$$$$$$  |       $$ |             $$ |  $$ |       \\$$$$$$  |       $$$$$$$$\\                                              ",
        "            \\______/        \\__|             \\__|  \\__|        \\______/        \\________|                                             ",
        "                                                                                                                                      ",
        "                                                                                                                                      ",
        "                                                                                                                                      ",
        "           $$$$$$\\        $$\\   $$\\        $$\\    $$\\         $$$$$$\\         $$$$$$$\\         $$$$$$$$\\        $$$$$$$\\              ",
        "           \\_ $$  _|      $$$\\  $$ |       $$ |   $$ |       $$  __$$\\        $$  __$$\\        $$  _____|       $$  __$$\\             ",
        "             $$ |         $$$$\\ $$ |       $$ |   $$ |       $$ /  $$ |       $$ |  $$ |       $$ |             $$ |  $$ |            ",
        "             $$ |         $$ $$\\$$ |       \\$$\\  $$  |       $$$$$$$$ |       $$ |  $$ |       $$$$$\\           $$$$$$$  |            ",
        "             $$ |         $$ \\$$$$ |        \\$$\\$$  /        $$  __$$ |       $$ |  $$ |       $$  __|          $$  __$$<             ",
        "             $$ |         $$ |\\$$$ |         \\$$$  /         $$ |  $$ |       $$ |  $$ |       $$ |             $$ |  $$ |            ",
        "           $$$$$$\\        $$ | \\$$ |          \\$  /          $$ |  $$ |       $$$$$$$  |       $$$$$$$$\\        $$ |  $$ |            ",
        "           \\______|       \\__|  \\__|           \\_/           \\__|  \\__|       \\_______/        \\________|       \\__|  \\__|            "};

    int i;

    while (!kbhit())
    {
        printf("\n\n\n");
        for (i = 0; i < 20; i++)
        {

            printf(" %s ", text1[i]);
            printf("\n");
        }
        sleep(1);
        system("cls");
        printf("\nPress any key to start %c\n", 206);

        printf("\n\n\n");
        for (i = 0; i < 20; i++)
        {

            printf(" %s ", text2[i]);
            printf("\n");
        }
        sleep(1);
        system("cls");
        printf("\nPress any key to start %c\n", 197);
    }
    getch();
}

void endscreen()
{
    char text1[9][95] = {
        "  /$$$$$$                                           /$$$$$$                                   ",
        " /$$__  $$                                         /$$__  $$                                  ",
        "| $$  \\__/  /$$$$$$  /$$$$$$/$$$$   /$$$$$$       | $$  \\ $$ /$$    /$$ /$$$$$$   /$$$$$$   ",
        "| $$ /$$$$ |____  $$| $$_  $$_  $$ /$$__  $$      | $$  | $$|  $$  /$$/ $$__  $$ /$$__  $$    ",
        "| $$|_  $$  /$$$$$$$| $$ \\ $$ \\ $$| $$$$$$$$      | $$  | $$ \\  $$/$$/| $$$$$$$$| $$  \\__/",
        "| $$  \\ $$ /$$__  $$| $$ | $$ | $$| $$_____/      | $$  | $$  \\  $$$/ | $$_____/| $$        ",
        "|  $$$$$$/|  $$$$$$$| $$ | $$ | $$|  $$$$$$$      |  $$$$$$/   \\  $/  |  $$$$$$$| $$         ",
        " \\______/  \\_______/|__/ |__/ |__/ \\_______/       \\______/     \\_/    \\_______/|__/    "};

    char text2[9][100] = {
        " $$$$$$\\                                           $$$$$$\\                                       ",
        "$$  __$$\\                                         $$  __$$\\                                      ",
        "$$ /  \\__| $$$$$$\\  $$$$$$\\$$$$\\   $$$$$$\\        $$ /  $$ |$$\\    $$\\  $$$$$$\\   $$$$$$\\ ",
        "$$ |$$$$\\  \\____$$\\ $$  _$$  _$$\\ $$  __$$\\       $$ |  $$ |\\$$\\  $$  |$$  __$$\\ $$  __$$\\",
        "$$ |\\_$$ | $$$$$$$ |$$ / $$ / $$ |$$$$$$$$ |      $$ |  $$ | \\$$\\$$  / $$$$$$$$ |$$ |  \\__|    ",
        "$$ |  $$ |$$  __$$ |$$ | $$ | $$ |$$   ____|      $$ |  $$ |  \\$$$  /  $$   ____|$$ |             ",
        "\\$$$$$$  |\\$$$$$$$ |$$ | $$ | $$ |\\$$$$$$$\\        $$$$$$  |   \\$  /   \\$$$$$$$\\ $$ |       ",
        " \\______/  \\_______|\\__| \\__| \\__| \\_______|       \\______/     \\_/     \\_______|\\__|    "};

    int i;
    char ch;
    fflush(stdin);
    while (!kbhit())
    {
        system("cls");
        printf("Final Score %d\n", score);
        printf("\n***Press r to restart*** \n");
        printf("\n\n\n\n");
        for (i = 0; i < 9; i++)
        {
            printf("%s", text1[i]);
            printf("\n");
        }
        sleep(1);
        system("cls");
        printf("Final Score %d\n", score);
        printf("\n***Press r to restart*** \n");
        printf("\n\n\n\n");
        for (i = 0; i < 9; i++)
        {
            printf("%s", text2[i]);
            printf("\n");
        }
        sleep(1);
        system("cls");
    }
    fflush(stdin);
    ch = getch();
    if (ch == 'r')
    {
        main();
    }
    else
    {
        exit(0);
    }
}
