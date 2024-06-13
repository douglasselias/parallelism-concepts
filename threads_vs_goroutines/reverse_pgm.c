#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sysinfo.h>

typedef struct {
  unsigned char** pixels;
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
  fscanf(input_file, "P5\n%d %d\n255\n", &image.width, &image.height);

	image.pixels = (unsigned char**)malloc(image.height * sizeof(unsigned char*));
	for (int i = 0; i < image.height; i++) {
		image.pixels[i] = (unsigned char*)malloc(image.width * sizeof(unsigned char));
	}

	for (int i = 0; i < image.height; i++) {
		fread(image.pixels[i], sizeof(unsigned char), image.width, input_file);
	}

  fclose(input_file);

  int total_threads = get_nprocs() - 1; // Leave one core for current process
  pthread_t threads[total_threads];
  ThreadArgs thread_args[total_threads];
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

  fprintf(outputFile, "P5\n%d %d\n255\n", image.width, image.height);

  for (int i = 0; i < image.height; i++) {
    fwrite(image.pixels[i], sizeof(unsigned char), image.width, outputFile);
  }

  fclose(outputFile);

  printf("New image saved as inverted_gradient_c.pgm\n");

  return 0;
}
