/*
 * PROJ1-1: YOUR TASK A CODE HERE
 *
 * Feel free to define additional helper functions.
 */

#include "calc_depth.h"
#include "utils.h"
#include <math.h>
#include <limits.h>
#include <stdio.h>


/* Implements the normalized displacement function */
unsigned char normalized_displacement(int dx, int dy,
        int maximum_displacement) {

    double squared_displacement = dx * dx + dy * dy;
    double normalized_displacement = round(255 * sqrt(squared_displacement) / sqrt(2 * maximum_displacement * maximum_displacement));
    return (unsigned char) normalized_displacement;

}

int bitmap(int x, int y, int image_width){
	return y * image_width + x;
}

void distance(unsigned int *distance, unsigned char *ndisplacement,
			  unsigned char *left, unsigned int x, unsigned int y, int image_width, 
			  unsigned char *right, unsigned int i, unsigned int j,
			  int feature_width, int feature_height, int max_displacement) {
	
	unsigned int l_x = 0, l_y = 0;
	unsigned int r_i = 0, r_j = 0;
	unsigned int sum = 0;
	for (l_x = x - feature_width, r_i = i - feature_width; (l_x <= x + feature_width) && (r_i <= i + feature_width); l_x++, r_i++){
		for (l_y = y - feature_height, r_j = j - feature_height; (l_y <= y + feature_height) && (r_j <= j + feature_height); l_y++, r_j++){
			sum += pow((left[bitmap(l_x,l_y,image_width)] - right[bitmap(r_i,r_j,image_width)]),2) ;
		}
	}
	if (sum < *distance){
		*distance = sum;
		*ndisplacement = normalized_displacement(x-i, y-j, max_displacement);
	} else if (sum == *distance){
		if (*ndisplacement >= normalized_displacement(x-i, y-j, max_displacement)){
			*distance = sum;
			*ndisplacement = normalized_displacement(x-i, y-j, max_displacement);
		}
	}
}


void depth_map_similar(unsigned int x,unsigned int y, unsigned char *depth_map,
				  	   unsigned char *left, unsigned char *right, int image_width,
				       int image_height, int feature_width, int feature_height, int maximum_displacement){
	
	int search_width_left = x - maximum_displacement;
	int search_width_right = x + maximum_displacement;
	int search_height_top = y - maximum_displacement;
	int search_height_bottom = y + maximum_displacement;
	unsigned int i = 0, j = 0;
	unsigned int temp_distance = UINT_MAX;
	unsigned char ndisplacement = 255;

	if (search_width_left < 0){
		search_width_left = 0;
	}
	if (search_width_right >= image_width){
		search_width_right = image_width-1;
	}
	if (search_height_top < 0){
		search_height_top = 0;
	} 
	if (search_height_bottom >= image_height){
		search_height_bottom = image_height-1;
	}
	for (j = search_height_top; j <= search_height_bottom; j++){
		for (i = search_width_left; i <= search_width_right; i++){
			if (((j >= feature_height) && (j < (image_height-feature_height)) && (i >= feature_width) && (i < (image_width-feature_width)))){
				distance(&temp_distance, &ndisplacement, left, x, y, image_width, right, i, j, feature_width, feature_height, maximum_displacement);		
			}
		}
	}
	depth_map[bitmap(x, y, image_width)] = ndisplacement;

}

void calc_depth(unsigned char *depth_map, unsigned char *left,
        unsigned char *right, int image_width, int image_height,
        int feature_width, int feature_height, int maximum_displacement) {

	unsigned int x, y = 0;
	for (y = 0; y < image_height; y++){
		for (x = 0; x < image_width; x++){
			if (y < feature_height || y >= (image_height-feature_height) || x < feature_width || x >= (image_width-feature_width)){
				depth_map[bitmap(x, y, image_width)] = 0;
			} else {
				depth_map_similar(x, y, depth_map, left, right, image_width, image_height, feature_width, feature_height, maximum_displacement);
			}
		}
	}	
}

