#include <stdio.h> //remove
#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
	// This plugin accepts 1 command line arguments;
	// factor to multiply each pixel by.
        double factor;
};

const char *get_plugin_name(void) {
	return "expose";
}

const char *get_plugin_desc(void) {
	return "adjust the intensity of all pixels";
}

void *parse_arguments(int num_args, char *args[]) {
	(void) args; // this is just to avoid a warning about an unused parameter
	double as_doub = atof(args[0]);
	if (num_args != 1 || as_doub < 0) {
	  printf("Error: Invalid arguments\n");
	  exit(1);  
	  //return NULL;
	}

	if (as_doub> 255) {
	  as_doub = 255;
	}

	
	struct Arguments *a = malloc(sizeof(struct Arguments));
	a->factor = as_doub;
	return a;
}

// Helper function to swap the blue and green color component values.
static uint32_t expose(uint32_t pix, double factor) {
	uint8_t r, g, b, a;
	img_unpack_pixel(pix, &r, &g, &b, &a);
	if(r > 255 / factor) {
	  r = 255;
	} else {
	  r *= factor;;
	}

	if(g > 255 / factor) {
	  g = 255;
	} else {
	  g *= factor;
	}

	if(b >255 / factor) {
	  b = 255;
	} else {
	  b *= factor;
	}
	
	return img_pack_pixel(r, g, b, a);
}

struct Image *transform_image(struct Image *source, void *arg_data) {
	struct Arguments *args = arg_data;
      
	printf("Factor: %f",args->factor);

	// Allocate a result Image
	struct Image *out = img_create(source->width, source->height);
	if (!out) {
		free(args);
		return NULL;
	}

	unsigned num_pixels = source->width * source->height;
	for (unsigned i = 0; i < num_pixels; i++) {
	  out->data[i] = expose(source->data[i], args->factor);
	}

	free(args);

	return out;
}
