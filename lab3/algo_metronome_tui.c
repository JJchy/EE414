/*
	Program test_single_key_nb.c

	Test checking single key without enter
	with non-blocking mode
	using termios

    Modified from
http://stackoverflow.com/questions/7469139/what-is-equivalent-to-getch-getche-in-linux
    Global old_termios and new_termios for efficient key inputs.

	Tested by	Byung Kook Kim,	Aug. 10, 2016.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <termios.h>
#include <unistd.h>		// read()
#include <sys/select.h>

#define True 1
#define False 0

// GLobal termios structs
static struct termios old_tio;
static struct termios new_tio;


// Initialize new terminal i/o settings 
void init_termios(int echo) 
{
  tcgetattr(0, &old_tio); 		// Grab old_tio terminal i/o setting 
  new_tio = old_tio; 			// Copy old_tio to new_tio
  new_tio.c_lflag &= ~ICANON; 	// disable buffered i/o 
  new_tio.c_lflag &= echo? ECHO : ~ECHO; 	// Set echo mode 
  if (tcsetattr(0, TCSANOW, &new_tio) < 0)  perror("tcsetattr ~ICANON");
								// Set new_tio terminal i/o setting
}


// Restore old terminal i/o settings 
void reset_termios(void) 
{
  tcsetattr(0, TCSANOW, &old_tio);
}

char getch (void)
{
    char ch = 0;
    if (read (0, &ch, 1) < 0) perror ("read()");
    return ch;
}


int main(void)
{
  char c;
  int echo, status, tempo, stop, run;
 
  int TimeSig[4][6] = {{7, 1, 0, 0, 0, 0},\
                       {7, 1, 1, 0, 0, 0},\
                       {7, 1, 3, 1, 0, 0},\
                       {7, 1, 1, 3, 1, 1}};

  char * TimeStr[4] = {"2/4", "3/4", "4/4", "6/8"};
  
  // Init termios: Disable buffered IO with arg 'echo'
  echo = 0;				// Disable echo
  init_termios(echo);

  // Test loop
  printf ("    Algo_Metronome_TUI\n\n");
  printf ("  Menu for Metronome TUI:\n");
  printf ("'z': Time signature 2/4 > 3/4 > 4/4 > 6/8 > 2/4 ...\n");
  printf ("'c': Dec tempo            Dec tempo by 5\n");
  printf ("'b': Inc tempo            Inc tempo by 5\n");
  printf ("'m': Start/Stop           Toggles start and stop\n");
  printf ("'q': Quit this program\n");

  status = 3;
  tempo = 90;
  run = False;

  while (1) {
	c = getch();
    switch (c)
    {
        case 'q' : printf ("   q: Quit!\n");
                   break;
        case 'z' : status = (status + 1) % 4;
                   break;
        case 'c' : if (tempo == 30) tempo = 30;
                   else tempo -= 5;
                   break;
        case 'b' : if (tempo == 200) tempo = 200;
                   else tempo += 5;
                   break;
        case 'm' : if (run == True) run = False;
                   else run = True;
    }
    if (c == 'q') break;
    printf ("   Key %c: TimeSig %s, Tempo %d, Run %d\n", c, TimeStr[status], tempo, run); 
	fflush(stdout);
  }
  fflush(stdout);

  // Reset termios
  reset_termios();

  return 0;
}
