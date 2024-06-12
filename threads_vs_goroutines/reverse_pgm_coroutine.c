#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sysinfo.h>

#include "neco/neco.h"

// Define the maximum width and height of the image
#define MAX_WIDTH  1000
#define MAX_HEIGHT 1000

typedef struct {
  unsigned char pixels[MAX_WIDTH][MAX_HEIGHT];
  int width;
  int height;
} Image;

void invert_chunk(int argc, void *argv[])  {
  neco_waitgroup* wg = argv[0];
  unsigned char* pixels = argv[1];
  int i = *(int*)argv[2];
  pixels[i] = 255 - pixels[i];
  neco_waitgroup_done(wg);
}

int neco_main(int argc, char* argv[]) {
  FILE* input_file = fopen("build/gradient.pgm", "rb");
  Image image;
  // Read image width and height from input file
  fscanf(input_file, "P5\n%d %d\n255\n", &image.width, &image.height);

  if (image.width > MAX_WIDTH || image.height > MAX_HEIGHT) {
    printf("Error: Image size exceeds maximum dimensions.\n");
    fclose(input_file);
    return 1;
  }

  // Read pixel values from input file
  fread(image.pixels, sizeof(unsigned char), image.width * image.height, input_file);
  fclose(input_file);

  neco_waitgroup wg;
  neco_waitgroup_init(&wg);
	long total_pixels = image.height * image.width;
  for (int i = 0; i < total_pixels; i++) {
    neco_waitgroup_add(&wg, 1);
    neco_start(invert_chunk, 3, &wg, &image.pixels, &i);
  }

  neco_waitgroup_wait(&wg);

  FILE* outputFile = fopen("build/inverted_gradient_c_coroutine.pgm", "wb");

  // Write image header to output file
  fprintf(outputFile, "P5\n%d %d\n255\n", image.width, image.height);

  // Write inverted pixel values to output file
  fwrite(image.pixels, sizeof(unsigned char), image.width * image.height, outputFile);
  fclose(outputFile);

  printf("New image saved as inverted_gradient_c_coroutine.pgm\n");

  return 0;
}
