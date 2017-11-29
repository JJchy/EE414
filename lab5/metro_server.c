/*
        Program test_single_key_nb.c

        Test checking single key without enter
        with non-blocking mode
        using termios

    Modified from
http://stackoverflow.com/questions/7469139/what-is-equivalent-to-getch-getche-in-linux
    Global old_termios and new_termios for efficient key inputs.

        Tested by       Byung Kook Kim, Aug. 10, 2016.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>

#include <termios.h>
#include <unistd.h>             // read()
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include "metro_server.h"

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
int new_fd;

void handler ()
{
    if (on == True)
    {
        play_LED (TimeSig[status][location]);
        on = False;
        switch (TimeSig[status][location])
        {
          case 7: send (new_fd, "#", 2, 0);
                  break;
          case 3: send (new_fd, "+", 2, 0);
                  break;
          case 1: send (new_fd, "!", 2, 0);
                  break;
        }
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
  int sockfd;
  struct sockaddr_storage their_addr;
  char packet [50], current_TimeSig [4];
  int recv_bytes;
  int sin_size;

  init_LED ();

  struct sigaction act;
  sigset_t set;

  sigemptyset (&set);
  sigaddset (&set, SIGALRM);

  act.sa_flags = 0;
  act.sa_mask = set;
  act.sa_handler = &handler;

  sigaction (SIGALRM, &act, NULL);

  init_SERVER (&sockfd);

  while (1)
  {
    sin_size = sizeof their_addr;
    new_fd = accept (sockfd, (struct sockaddr *) &their_addr, &sin_size);
    if (new_fd == -1)
    {
      perror ("accept");
      continue;
    }

    printf ("Connect\n");
    if (!fork())
    {
      close (sockfd);
      
      run = False;

      while (1)
      {
        memset (packet, 0, 50);
        recv_bytes = recv (new_fd, packet, 50, 0);
        if (recv_bytes > 10)
        {
          sscanf (packet, "TimeSig %s , Tempo %d , Start", current_TimeSig, &tempo);
          if (strcmp (current_TimeSig, "2/4") == 0) status = 0;
          else if (strcmp (current_TimeSig, "3/4") == 0) status = 1;
          else if (strcmp (current_TimeSig, "4/4") == 0) status = 2;
          else if (strcmp (current_TimeSig, "6/8") == 0) status = 3;
          time = 60 * NSEC_PER_SEC / (2 * tempo);
          struct itimerspec tim_spec =\
      {.it_interval = {.tv_sec = time / NSEC_PER_SEC, .tv_nsec = (time % NSEC_PER_SEC) * 1000},\
          .it_value = {.tv_sec = time / NSEC_PER_SEC, .tv_nsec = (time % NSEC_PER_SEC) * 1000}};

          location = 0;
          on = True;
          run = True;
          timer_create (CLOCK_MONOTONIC, NULL, &t_id);
          timer_settime (t_id, 0, &tim_spec, NULL);
          printf ("   Current: TimeSig %s, Tempo %d, Run %d\n", TimeStr[status], tempo, run);
        }

        else
        {
          if (strcmp (packet, "Stop") == 0)
          {
            timer_delete (t_id);
            stop_LED (7);
            run = False;
          }
          else if (strcmp (packet, "Quit") == 0) break;
        }
        fflush(stdout);
      }
      close (new_fd);
      fflush(stdout);
      exit (0);
    }
    else 
    {
      wait (NULL);
      close (new_fd);
    }
  }

  close (sockfd);
  exit_LED ();
}
                      

