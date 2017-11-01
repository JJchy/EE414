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
#include <signal.h>

#include <termios.h>
#include <unistd.h>		// read()
#include <sys/select.h>

#include "userLEDmmap.h"

// GLobal termios structs
static struct termios old_tio;
static struct termios new_tio;

// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig) {
    printf( "\nCtrl-C pressed, cleaning up and exiting...\n" );
	keepgoing = 0;
}

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

void init_KEY ()
{
    signal(SIGINT, signal_handler);

    // Init termios: Disable buffered IO with arg 'echo'
    int echo = 0;
    init_termios(echo);

    printf ("    Algo_Metronome_TUI\n\n");
    printf ("  Menu for Metronome TUI:\n");
    printf ("'z': Time signature 2/4 > 3/4 > 4/4 > 6/8 > 2/4 ...\n");
    printf ("'c': Dec tempo            Dec tempo by 5\n");
    printf ("'b': Inc tempo            Inc tempo by 5\n");
    printf ("'m': Start/Stop           Toggles start and stop\n");
    printf ("'q': Quit this program\n");
}

// Read one character without Enter key: Blocking
char getch(void) 
{
  char ch = 0;

  if (read(0, &ch, 1) < 0)  perror ("read()");		// Read one character
 
  return ch;
}

void exit_KEY ()
{
    reset_termios ();
}
