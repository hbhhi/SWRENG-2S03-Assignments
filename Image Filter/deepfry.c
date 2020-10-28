#include <stdlib.h>
#include <stdio.h>
#include "bmp.h"

#define RGBTAX 3
#define N 3

typedef struct{
    int width, height, max;
    RGB *image;
} PPM;

static int i, j, k, l;

PPM *readPPM(char *filename){

    FILE *file = fopen(filename, "r");
    if (!file){
        printf("Unable to open file: %s\n", filename);
        return NULL;
    }

    char filetype[2];
    fscanf(file, "%s", filetype);
    int width, height, maxRGB;
    fscanf(file, "%d%d%d", &width, &height, &maxRGB);

    RGB *image = malloc(width * height * sizeof(RGB));

    for (i = 0; i < width * height; i++)
        fscanf(file, "%hhd%hhd%hhd", &image[i].r, &image[i].g, &image[i].b);

    PPM *ppm = malloc(sizeof(PPM));

    ppm->width = width;
    ppm->height = height;
    ppm->max = maxRGB;
    ppm->image = image;

    fclose(file);

    return ppm;

}

void writePPM(char *filename, PPM *ppm){

    FILE *file = fopen(filename, "w");

    fprintf(file, "P3\n");
    fprintf(file, "%d %d\n%d\n", ppm->width, ppm->height, ppm->max);

    RGB *image = ppm->image;

    for (i = 0; i < ppm->height * ppm->width; i++){
        fprintf(file, "%d %d %d ", image[i].r, image[i].g, image[i].b);
        if (!((i + 1) % ppm->width)) fprintf(file, "\n");
    }

    fclose(file);

}

void convolution(PPM *ppm, int rows, int columns, int *mirror[columns], int kernel[][N]) {

    int count, track = 0;
    float convoluted;
    int *output[rows];

    for (i = 0; i < rows; i++)
        output[i] = (int *)malloc(columns * sizeof(int)); 

    for (i = 0; i < rows; i++){

        for (j = 0; j < columns; j++){

            convoluted = 0.0;

            for (k = 0; k < N; k++){
                for (l = 0; l < N; l++){
                    if ((i - N/2 + k) >= rows || (i - N/2 + k) < 0 || (j - N/2 + l) >= columns || (j - N/2 + l) < 0) continue;
                    convoluted += (float) mirror[i - N/2 + k][j - N/2 + l] * kernel[k][l];
                }

            }

            output[i][j] = (int) convoluted;
        }
       
    }

    for (i = 0; i < ppm->height; i++){
        count = 0;
        for (j = 0; j < ppm->width; j++){
            ppm->image[track].r = output[i][j + count++];
            ppm->image[track].g = output[i][j + count++];
            ppm->image[track].b = output[i][j + count];
            track++;
        }
    }

}

int main(int argc, char **argv){

    if (argc != 3){
        printf("USAGE: filter input.ppm output.ppm\n");
        return -1;
    }
    
    PPM *ppm = readPPM(argv[1]);
    
    int count, track = 0;
    int *mirror[ppm->height];

    for (i = 0; i < ppm->height; i++)
        mirror[i] = (int *)malloc(ppm->width * RGBTAX * sizeof(int)); 
        
    for (i = 0; i < ppm->height; i++){
        count = 0;
        for (j = 0; j < ppm->width; j++){
            mirror[i][j + count++] = ppm->image[track].r;
            mirror[i][j + count++] = ppm->image[track].g;
            mirror[i][j + count] = ppm->image[track].b;
            track++;
        }
    }

    int option;
    int k0[N][N] = {0, 0, 0, 0, 1, 0, 0, 0, 0};
    int k1[N][N] = {1, 0, -1, 0, 0, 0, -1, 0, 1};
    int k2[N][N] = {0, 1, 0, 1, -4, 1, 0, 1, 0};
    int k3[N][N] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
    int k4[N][N] = {0, -1, 0, -1, 5, -1, 0, -1, 0};

    printf("Which type of frying would you like to apply? -1 to exit.\n1) Cursed Deepfry\n2) Heavy Deepfry\n3) Premium Deepfry\n4) Light Fry\n");
    scanf("%d", &option);

    switch (option) { 
        case -1: printf("Goodbye.\n"); convolution(ppm, ppm->height, ppm->width * RGBTAX, mirror, k0); writePPM(argv[2], ppm);break;
        case 1: convolution(ppm, ppm->height, ppm->width * RGBTAX, mirror, k1); writePPM(argv[2], ppm); break; 
        case 2: convolution(ppm, ppm->height, ppm->width * RGBTAX, mirror, k2); writePPM(argv[2], ppm); break; 
        case 3: convolution(ppm, ppm->height, ppm->width * RGBTAX, mirror, k3); writePPM(argv[2], ppm); break;
        case 4: convolution(ppm, ppm->height, ppm->width * RGBTAX, mirror, k4); writePPM(argv[2], ppm); break;
        default: printf("Please select one of the above options.\n"); break;   
    }

    free(ppm->image);
    free(ppm);

    return 0;

}