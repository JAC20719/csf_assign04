#include <stdio.h> //remove
#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
	// This plugin accepts 1 command line arguments;
	// factor to multiply each pixel by.
        int n;
};

const char *get_plugin_name(void) {
	return "tile";
}

const char *get_plugin_desc(void) {
	return "tile source image in an NxN arrangement";
}

void *parse_arguments(int num_args, char *args[]) {
	(void) args; // this is just to avoid a warning about an unused parameter
    
	if (num_args == 0 || atoi(args[0]) < 1) {
	  printf("Error: Invalid argument\n");
	  exit(1);  
	  //return NULL;
	}

	struct Arguments *a = malloc(sizeof(struct Arguments));
	a->n = atoi(args[0]);
	return a;
}

struct Image *transform_image(struct Image *source, void *arg_data) {
	struct Arguments *args = arg_data;
	int n = args->n;

	if(n > source->width || n > source->height) {
		printf("Error: Invalid argument\n");
		exit(1);
	}

	// Allocate a result Image
	struct Image *out = img_create(source->width, source->height);
	struct Image *nbyn = img_create(source->width, source->height);
	if (!out || !nbyn) {
		free(args);
		return NULL;
	}

	unsigned num_pixels = source->width * source->height;
	unsigned nbyn_pixel = 0;
	for(int r = 0; r < n; r++) {
	  for(unsigned i = 0; i < num_pixels; i += n) {
	    /*uint64_t avg_r = 0;
	    uint64_t avg_g = 0;
	    uint64_t avg_b = 0;
	    uint64_t avg_a = 0;
	    for(int j = 0; j < n; j++) {
	      uint8_t r, g, b, a;
	      img_unpack_pixel(source->data[i + j], &r, &g, &b, &a);
	      avg_r += r;
	      avg_g += g;
	      avg_b += b;
	      avg_a += a;
	    }
	    avg_r = avg_r / (uint64_t)n;
	    avg_g = avg_g / (uint64_t)n;
	    avg_b = avg_b / (uint64_t)n;
	    avg_a = avg_a / (uint64_t)n;*/
	    nbyn->data[nbyn_pixel] = source->data[i];//img_pack_pixel((uint8_t)avg_r, (uint8_t)avg_g, (uint8_t)avg_b, (uint8_t)avg_a);
	    nbyn_pixel++;
	  }
	}

	free(args);

	return nbyn;
}
