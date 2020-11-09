#include <stdio.h> //remove
#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
	// This plugin accepts 1 command line arguments;
	// factor to multiply each pixel by.
        int dummy;
};

const char *get_plugin_name(void) {
	return "mirrorv";
}

const char *get_plugin_desc(void) {
	return "mirror image vertically";
}

void *parse_arguments(int num_args, char *args[]) {
	(void) args; // this is just to avoid a warning about an unused parameter
    
	if (num_args != 0) {
	    return NULL;
	}
	
	return calloc(1, sizeof(struct Arguments));
}

struct Image *transform_image(struct Image *source, void *arg_data) {
	struct Arguments *args = arg_data;

	// Allocate a result Image
	struct Image *out = img_create(source->width, source->height);
	if (!out) {
		free(args);
		return NULL;
	}

	//Rotate 90 Degrees
	unsigned num_pixels = source->width * source->height;
	for (unsigned i = 0; i < num_pixels / 2; i++) {
	  out->data[i] = source->data[num_pixels - i - 1];
	  out->data[num_pixels - i - 1] = source->data[i];
	}

	//Make 2D Array for easier use
	uint32_t twoD[source->width][source->height];
	unsigned pixel = 0;
	for(unsigned i = 0; i < source->height; i++) {
	  for(unsigned j = 0; j < source->width; j++) {
	    twoD[j][i] = out->data[pixel];
	    pixel++;
	  }
	}

	//Change the pixels in the 2D Array (Reflect across y)
	for(unsigned i = 0; i < source->height; i++) {
	  for(unsigned j = 0; j < source->width / 2; j++) {
	    uint32_t temp = twoD[j][i];
	    twoD[j][i] = twoD[source->width - j - 1][i];
	    twoD[source->width - j - 1][i] = temp;
	  }
	}

	//Put pixels back into out
	pixel = 0;
	for(unsigned i = 0; i < source->height; i++) {
	  for(unsigned j = 0; j < source->width; j++) {
	    out->data[pixel] = twoD[j][i];
	    pixel++;
	  }
	}

	

	free(args);

	return out;
}
