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

typedef struct {
  Image* image;
  int start_row;
  int end_row;
} ThreadArgs;

void* invert_chunk(void* arg) {
  ThreadArgs* args = (ThreadArgs*)arg;
  Image* image = args->image;

  for (int i = args->start_row; i < args->end_row; i++) {
    for (int j = 0; j < image->width; j++) {
      image->pixels[i][j] = 255 - image->pixels[i][j];
    }
  }

  return NULL;
}

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

  // Determine the number of threads based on the number of CPU cores
  int total_threads = get_nprocs() - 1; // Leave one core for current process
  pthread_t threads[total_threads];
  ThreadArgs thread_args[total_threads];
  // Divide the image rows among the threads
  int rows_per_thread = image.height / total_threads;
  int extra_rows = image.height % total_threads;
  int start_row = 0;

  for (int i = 0; i < total_threads; i++) {
    thread_args[i].image = &image;
    thread_args[i].start_row = start_row;
    thread_args[i].end_row = start_row + rows_per_thread + (i < extra_rows ? 1 : 0);
    start_row = thread_args[i].end_row;

    pthread_create(&threads[i], NULL, invert_chunk, &thread_args[i]);
  }

  for (int i = 0; i < total_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  FILE* outputFile = fopen("build/inverted_gradient_c.pgm", "wb");

  // Write image header to output file
  fprintf(outputFile, "P5\n%d %d\n255\n", image.width, image.height);

  // Write inverted pixel values to output file
  fwrite(image.pixels, sizeof(unsigned char), image.width * image.height, outputFile);
  fclose(outputFile);

  printf("New image saved as inverted_gradient_c.pgm\n");

  return 0;
}
