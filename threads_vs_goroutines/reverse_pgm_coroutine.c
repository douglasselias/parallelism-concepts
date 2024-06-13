#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sysinfo.h>

#include "neco/neco.h"

typedef struct {
  unsigned char* pixels;
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
  fscanf(input_file, "P5\n%d %d\n255\n", &image.width, &image.height);

	long total_pixels = image.height * image.width;

	image.pixels = malloc(total_pixels * sizeof(unsigned char));

	for (int i = 0; i < total_pixels; i++) {
		fread(&image.pixels[i], sizeof(unsigned char), 1, input_file);
	}
  fclose(input_file);

  neco_waitgroup wg;
  neco_waitgroup_init(&wg);
  for (int i = 0; i < total_pixels; i++) {
    neco_waitgroup_add(&wg, 1);
    neco_start(invert_chunk, 3, &wg, image.pixels, &i);
  }

  neco_waitgroup_wait(&wg);

  FILE* output_file = fopen("build/inverted_gradient_c_coroutine.pgm", "wb");

  fprintf(output_file, "P5\n%d %d\n255\n", image.width, image.height);

  for (int i = 0; i < total_pixels; i++) {
    fwrite(&image.pixels[i], sizeof(unsigned char), 1, output_file);
  }

  fclose(output_file);

  printf("New image saved as inverted_gradient_c_coroutine.pgm\n");

  return 0;
}
