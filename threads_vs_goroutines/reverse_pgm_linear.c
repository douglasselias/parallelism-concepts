#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sysinfo.h>

// Define the maximum width and height of the image
#define MAX_WIDTH  1000
#define MAX_HEIGHT 1000

typedef struct {
  unsigned char pixels[MAX_WIDTH][MAX_HEIGHT];
  int width;
  int height;
} Image;

int main() {
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

  for (int i = 0; i < image.height; i++) {
    for (int j = 0; j < image.width; j++) {
      image.pixels[i][j] = 255 - image.pixels[i][j];
    }
  }

  FILE* outputFile = fopen("build/inverted_gradient_c_linear.pgm", "wb");

  // Write image header to output file
  fprintf(outputFile, "P5\n%d %d\n255\n", image.width, image.height);

  // Write inverted pixel values to output file
  fwrite(image.pixels, sizeof(unsigned char), image.width * image.height, outputFile);
  fclose(outputFile);

  printf("New image saved as inverted_gradient_c_linear.pgm\n");

  return 0;
}
