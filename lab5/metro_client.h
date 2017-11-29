/*		File Beaglebone_GPIO_mmap.h

  Beaglebone GPIO memory map

*/

#define True 1
#define False 0

void init_KEY ();
int key_hit ();
char getch ();
void exit_KEY ();

void init_CLIENT (int, char **, int *);
