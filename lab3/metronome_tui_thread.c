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
#include <pthread.h>

#include <termios.h>
#include <unistd.h>		// read()
#include <sys/select.h>

#include "userLEDmmap.h"

char * TimeStr[4] = {"2/4", "3/4", "4/4", "6/8"};

int main(void)
{
  char c;
  int success;
  run = False;
  keepgoing = 1;

  success = init_LED ();
  if (success == -1) 
      return -1;
  init_KEY ();

  status = 3;
  location = 0;
  tempo = 90;
  run = False;

  printf ("   Default: TimeSig %s, Tempo %d, Run %d\n", TimeStr[status], tempo, run); 
  
  // Test loop
  while (keepgoing) {

	  c = getch();
      pthread_mutex_lock (&mutex_lock);
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
      location = 0;
      if (c == 'q') 
      {
          pthread_mutex_unlock (&mutex_lock);
          break;
      }
      printf ("   Key %c: TimeSig %s, Tempo %d, Run %d\n", c, TimeStr[status], tempo, run); 
	  fflush(stdout);
      pthread_mutex_unlock (&mutex_lock);
  }
  fflush(stdout);

  exit_KEY ();
  exit_LED ();
  return 0;
}

