  #include "bmp.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int width, height, max;
    RGB *image;
} PPM;

static int i, j;

PPM *readPPM(char *fileName) {

  FILE *file = fopen(fileName, "r");

  if (!file) {
    printf("ERROR: Unable to open input file: %s\n", fileName);
    fclose(file);
    return NULL;
  }

  char filetype[2];
  PPM *ppm = malloc(sizeof(PPM));

  fscanf(file, "%s %d %d %d", filetype, &ppm->width, &ppm->height, &ppm->max);

  RGB *image = malloc(ppm->width * ppm->height * sizeof(RGB));

  for (i = 0; i < ppm->width * ppm->height; i++)
    fscanf(file, "%hhd %hhd %hhd", &image[i].r, &image[i].g, &image[i].b);

  ppm->image = image;

  fclose(file);

  return ppm;
}

void writePPM(char *fileName, PPM *ppm) {

  FILE *file = fopen(fileName, "w");

  fprintf(file, "P3\n%d %d\n%d\n", ppm->width, ppm->height, ppm->max);

  RGB *image = ppm->image;

  for (i = 0; i < ppm->height * ppm->width; i++) {
    fprintf(file, "%d %d %d ", image[i].r, image[i].g, image[i].b);
    if (!((i + 1) % ppm->width)) fprintf(file, "\n");
  }

  fclose(file);
}

void convolution(PPM *ppm, int n, double kernel[][n]) {

  float conv[3];
  int R = 0, G = 1, B = 2, k, l;
  /* copy of original non-convoluted ppm->image values */
  RGB *mirror = malloc(ppm->height * ppm->width * sizeof(RGB));

  for (i = 0; i < ppm->height * ppm->width; i++) {
    mirror[i].r = ppm->image[i].r;
    mirror[i].g = ppm->image[i].g;
    mirror[i].b = ppm->image[i].b;
  }

  for (i = 0; i < ppm->height; i++) {

    for (j = 0; j < ppm->width; j++) {

      conv[R] = conv[G] = conv[B] = 0.0;

      for (k = 0; k < n; k++) {

        for (l = 0; l < n; l++) {
          /* if out of bounds, skip */
          if ((i - n/2 + k) >= ppm->height || (i - n/2 + k) < 0 || (j - n/2 + l) >= ppm->width || (j - n/2 + l) < 0) continue;
          // printf("r: %d\n", mirror[(i - n/2 + k) * ppm->width + j - n/2 + l].r);
          conv[R] += mirror[(i - n/2 + k) * ppm->width + j - n/2 + l].r * kernel[k][l];
          conv[G] += mirror[(i - n/2 + k) * ppm->width + j - n/2 + l].g * kernel[k][l];
          conv[B] += mirror[(i - n/2 + k) * ppm->width + j - n/2 + l].b * kernel[k][l];
        }

      }

      /* assuming if convR, convG, convB are < 0, values should just be 0
      assuming if convR, convG, convB are > ppm->max, values should be ppm->max
      otherwise, conv values are valid (within [0, ppm->max]) */
      // printf("R: %f, G: %f, B: %f\n", conv[R], conv[G], conv[B]);
      ppm->image[i * ppm->width + j].r = conv[R] < 0 ? 0 : conv[R] > ppm->max ? ppm->max : (int) conv[R];
      ppm->image[i * ppm->width + j].g = conv[G] < 0 ? 0 : conv[G] > ppm->max ? ppm->max : (int) conv[G];
      ppm->image[i * ppm->width + j].b = conv[B] < 0 ? 0 : conv[B] > ppm->max ? ppm->max : (int) conv[B];
      
    }

  }

  free(mirror);
}

void readKernel(PPM *ppm, char **argv){

  int n, scale;
  FILE *file = fopen(argv[2], "r");

  if (!file) {
    printf("ERROR: Unable to open kernel\n");
    fclose(file);
    return;
  }

  fscanf(file, "%d %d", &n, &scale);

  if (!(n % 2)) {
    printf("ERROR: Kernel must have odd n. Instead found n = %d\n", n);
    fclose(file);
    return;
  }

  double kernel[n][n];

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      fscanf(file, "%lf", &kernel[i][j]);
      kernel[i][j] /= scale;
    }
  }

  fclose(file);
  convolution(ppm, n, kernel);
  writePPM(argv[3], ppm);
  free(ppm->image);
  free(ppm);

}

int main(int argc, char **argv) {

  if (argc != 4) {
    printf("USAGE: filter input_file.ppm kernel output_file.ppm\n");
    return -1;
  }

  PPM *ppm = readPPM(argv[1]);

  if (ppm) readKernel(ppm, argv);

  return 0;
}