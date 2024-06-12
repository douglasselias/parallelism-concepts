#include <stdio.h>

int main() {
  FILE* file = fopen("build/gradient.pgm", "wb");
  int width = 1000, height = 1000;

  // Write the PGM header
  fprintf(file, "P5\n"); // P5 indicates binary encoding
  fprintf(file, "%d %d\n", width, height);
  fprintf(file, "255\n"); // Maximum pixel value

  // Write pixel data
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // Generate a gradient from black to white
      unsigned char value = (255 * x) / (width - 1);
      fwrite(&value, sizeof(unsigned char), 1, file);
    }
  }

  fclose(file);

  printf("PGM file 'build/gradient.pgm' created\n");

  return 0;
}
