#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sysinfo.h>

typedef struct {
  unsigned char** pixels;
  int width;
  int height;
} Image;

int main() {
  FILE* input_file = fopen("build/gradient.pgm", "rb");
  Image image;
  fscanf(input_file, "P5\n%d %d\n255\n", &image.width, &image.height);

	image.pixels = (unsigned char**)malloc(image.height * sizeof(unsigned char*));
	for (int i = 0; i < image.height; i++) {
		image.pixels[i] = (unsigned char*)malloc(image.width * sizeof(unsigned char));
	}

	for (int i = 0; i < image.height; i++) {
		fread(image.pixels[i], sizeof(unsigned char), image.width, input_file);
	}
  fclose(input_file);

  for (int i = 0; i < image.height; i++) {
    for (int j = 0; j < image.width; j++) {
      image.pixels[i][j] = 255 - image.pixels[i][j];
    }
  }

  FILE* outputFile = fopen("build/inverted_gradient_c_linear.pgm", "wb");

  fprintf(outputFile, "P5\n%d %d\n255\n", image.width, image.height);

  for (int i = 0; i < image.height; i++) {
    fwrite(image.pixels[i], sizeof(unsigned char), image.width, outputFile);
  }

  fclose(outputFile);

  printf("New image saved as inverted_gradient_c_linear.pgm\n");

  return 0;
}
