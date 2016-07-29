#include "global.h"
#include "CGame.h"

#ifndef _WIN32_
#include <termios.h>
#include <unistd.h>
#endif

/*
int getch (void) {
    int ch;
    struct termios oldt, newt;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ECHO|ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}
*/

HANDLE hConsole;	//output stream handle

CGame * g_pGame;

int main()
{	
#ifdef _WIN32_
	hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
#else
    hConsole = 0;
#endif

#ifndef _WIN32_
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO|ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif
	g_pGame = new CGame( hConsole );
	g_pGame->RunGame();	
	delete g_pGame;

#ifndef _WIN32_
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    return 0;
}




