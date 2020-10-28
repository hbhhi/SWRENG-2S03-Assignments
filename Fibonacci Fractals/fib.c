#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL 2
#define EXTRA 1

typedef enum {
    up,
    right,
    down,
    left
	} direction;

int draw(int x, int y, int dir, int step, int w, int h, RGB *im, RGB c) {
#define I(i, j) im[(i)*w + j]
  int j;
  if (dir == up){
    /* out of bounds stuff */
    if (x + step > h) return -1;
    for (j = 0; j < step; j++) I(x + j, y) = c;
  }
  else if (dir == down){
    if (x - step < 0) return -1;
    for (j = 0; j < step; j++) I(x - j, y) = c;
  }
  else if (dir == right){
    if (y + step > w) return -1;
    for (j = 0; j < step; j++) I(x, y + j) = c;
  }
  else{ 
    if (y - step < 0) return -1;
    for (j = 0; j < step; j++) I(x, y - j) = c;
  }
#undef I
  return 0;
}

char *fibMaker(int n, char *a, char *b, int count) {

  if (count == n) return a;

  /* + 1 for termination character */
  char *tmp = malloc(strlen(a) + strlen(b) + EXTRA);

  strcpy(tmp, a);
  strcat(tmp, b);

  free(b);
  b = a;
  a = tmp;

  return fibMaker(n, a, b, count + EXTRA);
}

int fib(int n, int x, int y, int step, RGB bc, RGB fc, int w, int h, RGB *image) {

  int i;
  char *fib = malloc(sizeof(char *));
  char *first = malloc(sizeof(char *));
  char *second = malloc(sizeof(char *));
  strcpy(first, "0");
  strcpy(second, "1");
  fib = fibMaker(n, first, second, INITIAL);
  
  for (i = 0; i < w * h; i++) image[i] = bc;

  /* initial direction is up */
  int dir = 0;      

  for (i = 0; i < strlen(fib); i++) {

    if (draw(x, y, dir, step, w, h, image, fc)){
      printf("Coordinates outside defined width and heigth\n");
      return 0;
    }

    if (!(dir % 2)) 
      x += step - (dir * step);
    else
      y += ((right / dir) * step) - ((dir / left) * step);

    if (fib[i] == '0') {
      /* go CW if even */
      if (!(i % 2)) dir = (dir + 1) % 4;
      /* go CCW if odd */
      else dir = !dir ? left : dir - 1;
    }

  }
  free(fib);
  return i;
}
