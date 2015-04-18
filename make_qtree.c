/*
 * PROJ1-1: YOUR TASK B CODE HERE
 *
 * Feel free to define additional helper functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include "quadtree.h"
#include "make_qtree.h"
#include "utils.h"

#define ABS(x) (((x) < 0) ? (-(x)) : (x))

int homogenous(unsigned char *depth_map, int map_width, int x, int y, int section_width) {
    depth_map+=(x+y*map_width);
    char color = *depth_map;
    char newColor = *depth_map;
    for (int j=0; j<section_width; j++){
        for(int i = 0; i<section_width; i++){
            newColor = *(depth_map+i+j*map_width);
            if(newColor != color){
                return 256;
            }
        }
    }
    return *depth_map;
}

qNode *depth_to_quad_helper(unsigned char *depth_map, int map_width, int x, int y, int width, unsigned char *map) {
    qNode *root = (qNode*)malloc(sizeof(qNode));
    if(root == NULL){
        allocation_failed();
    }
    int row = y, col = x;
    char color = *depth_map, newColor = *depth_map;
    int size = map_width;
    unsigned char *cur = map;
    if (size == 1){
        root->leaf = 1;
        root->size = size;
        root->x = col;
        root->y = row;
        root->gray_value = *depth_map;
    } else if (homogenous(depth_map, map_width, x, y, map_width) != 256) {
        root->leaf = 1;
        root->size = size;
        root->x = col;
        root->y = row;
        root->gray_value = homogenous(depth_map, map_width, x, y, map_width);
    } else{
        root->size = size; 
        root->x = col;
        root->y = row;
        while(row-root->y<size){
            while (col-root->x<size){
                color = *(cur+(col-col%size)+width*row);
                newColor = *(cur+col+width*row);
                if(newColor != color){
                    root->leaf = 0;
                    root->gray_value = homogenous(depth_map, map_width, x, y, size);     
                    size = size/2;
                    unsigned char* copy = depth_map;
                    unsigned char* nw = malloc(size*size*sizeof(char));
                    if(nw == NULL){
                        allocation_failed();
                    }
                    unsigned char* ne = malloc(size*size*sizeof(char));
                    if(ne == NULL){
                        allocation_failed();
                    }
                    unsigned char* se = malloc(size*size*sizeof(char));
                    if(se == NULL){
                        allocation_failed();
                    }
                    unsigned char* sw = malloc(size*size*sizeof(char));
                    if(sw == NULL){
                        allocation_failed();
                    }
                    unsigned char nwlist[size*size];
                    for (int j=0; j<size; j++){
                        for(int i = 0; i<size; i++){
                            nwlist[i+j*size] = *depth_map;
                            depth_map++;
                        }
                        depth_map+=size;
                    }
                    nw = &nwlist[0];
                    depth_map = copy;
                    unsigned char nelist[size*size];
                    depth_map+=size;
                    for (int j=0; j<size; j++){
                        for(int i = 0; i<size; i++){
                            nelist[i+j*size] = *depth_map;
                            depth_map++;
                        }
                        depth_map+=size;
                    }
                    ne = &nelist[0];
                    depth_map = copy;
                    unsigned char selist[size*size];
                    depth_map+=(size*size*2+size);
                    for (int j=0; j<size; j++){
                        for(int i = 0; i<size; i++){
                            selist[i+j*size] = *depth_map;
                            depth_map++;
                        }
                        depth_map+=size;
                    }
                    se = &selist[0];
                    depth_map = copy;      
                    unsigned char swlist[size*size];
                    depth_map+=(size*size*2);
                    for (int j=0; j<size; j++){
                        for(int i = 0; i<size; i++){
                            swlist[i+j*size] = *depth_map;
                            depth_map++;
                        }
                        depth_map+=size;
                    }
                    sw = &swlist[0];
                    depth_map = copy;
                    root->child_NW = depth_to_quad_helper(nw,size,x, y, width, map);
                    root->child_NE = depth_to_quad_helper(ne,size,x+size, y, width, map);
                    root->child_SE = depth_to_quad_helper(se,size,x+size, y+size, width, map);
                    root->child_SW = depth_to_quad_helper(sw,size,x, y+size, width, map);
                }
                col++;
            }
            row++;
        }
    }
    return root;

}

qNode *depth_to_quad(unsigned char *depth_map, int map_width){
    unsigned char *map = depth_map;
    int width = map_width;
    return depth_to_quad_helper(depth_map, map_width, 0, 0, width, map);
}

void free_qtree(qNode *qtree_node) {
    if(qtree_node) {
        if(!qtree_node->leaf){
            free_qtree(qtree_node->child_NW);
            free_qtree(qtree_node->child_NE);
            free_qtree(qtree_node->child_SE);
            free_qtree(qtree_node->child_SW);
        }
        free(qtree_node);
    }
}
