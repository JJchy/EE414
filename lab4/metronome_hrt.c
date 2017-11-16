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
#include <time.h>
#include <signal.h>
#include <sys/types.h>

#include <termios.h>
#include <unistd.h>		// read()
#include <sys/select.h>

#include "userLEDmmap.h"

char * TimeStr[4] = {"2/4", "3/4", "4/4", "6/8"};

int TimeSig[4][6] = {{7, 1, 0, 0, 0, 0},\
                     {7, 1, 1, 0, 0, 0},\
                     {7, 1, 3, 1, 0, 0},\
                     {7, 1, 1, 3, 1, 1}};

static struct timespec prev = {.tv_sec = 0, .tv_nsec = 0};

int tempo;
int status;
int location;
int run;
int on;
int beat[4] = {2, 3, 4, 6};

void handler () 
{
    if (on == True)
    {
        play_LED (TimeSig[status][location]);
        on = False;
    }
    else
    {
        stop_LED (TimeSig[status][location]);
        on = True;
        if (++location >= beat[status])
            location = 0;
    }

}

int main(void)
{
  char c;
  timer_t t_id;
  long long time;

  init_LED ();
  
  struct sigaction act;
  sigset_t set;
  
  sigemptyset (&set);
  sigaddset (&set, SIGALRM);

  act.sa_flags = 0;
  act.sa_mask = set;
  act.sa_handler = &handler;

  sigaction (SIGALRM, &act, NULL);

  init_KEY ();

  status = 3;
  location = 0;
  tempo = 90;
  run = False;
  on = True;

  printf ("   Default: TimeSig %s, Tempo %d, Run %d\n", TimeStr[status], tempo, run); 
  
  // Test loop
  while (1) {
      while (!key_hit ()) usleep (10000);
      time = 60 * NSEC_PER_SEC / (2 * tempo); 
	  struct itimerspec tim_spec = \
      {.it_interval = {.tv_sec = time / NSEC_PER_SEC, .tv_nsec = (time % NSEC_PER_SEC) * 1000},\
          .it_value = {.tv_sec = time / NSEC_PER_SEC, .tv_nsec = (time % NSEC_PER_SEC) * 1000}};
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
        case 'm' : if (run == True)
                   {
                       timer_delete (t_id);
                       run = False;
                   }
                   else
                   {
                       timer_create (CLOCK_MONOTONIC, NULL, &t_id);
                       run = True;
                   }
      }
      if (c == 'q') 
          break;

      location = 0;
      on = True;
      timer_settime (t_id, 0, &tim_spec, NULL);
      printf ("   Key %c: TimeSig %s, Tempo %d, Run %d\n", c, TimeStr[status], tempo, run); 
	  fflush(stdout);
  }
  fflush(stdout);

  exit_KEY ();
  exit_LED ();
  return 0;
}

