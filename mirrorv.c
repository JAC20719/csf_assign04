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

	unsigned num_pixels = source->width * source->height;
	for (unsigned i = 0; i < num_pixels / 2; i++) {
	  out->data[i] = source->data[num_pixels - i - 1];
	  out->data[num_pixels - i - 1] = source->data[i];
	}	  

	free(args);

	return out;
}
