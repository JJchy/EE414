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
#include <sys/socket.h>
#include <pthread.h>

#include "metro_client.h"

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

int sockfd;

void * recv_LED_signal (void * data)
{
  char LED_signal [2];
  while (1)
  {
    recv (sockfd, LED_signal, 2, 0);
    printf ("%s", LED_signal);
    fflush (stdout);
  }
}

int main(int argc, char *argv[])
{
  char c;
  timer_t t_id;
  long long time;
  int thread_id;
  pthread_t pthread;
  char packet [50];
  int success;

  init_CLIENT (argc, argv, &sockfd);
  
  thread_id = pthread_create (&pthread, NULL, recv_LED_signal, NULL);
  
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
      c = getch();
      switch (c)
      {
        case 'q' : send (sockfd, "Quit", 5, 0);
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
                       send (sockfd, "Stop", 5, 0);
                       run = False;
                   }
                   else
                   {
                       sprintf (packet, "TimeSig %s , Tempo %d , Start", TimeStr[status], tempo);
                       send (sockfd, packet, 50, 0);
                       run = True;
                   }
      }
      if (c == 'q') 
      {
          printf ("Quit\n");
          break;
      }

      location = 0;
      on = True;
      printf ("   Key %c: TimeSig %s, Tempo %d, Run %d\n", c, TimeStr[status], tempo, run); 
      fflush(stdout);
  }
  fflush(stdout);

  close (sockfd);
  exit_KEY ();
  return 0;
}

