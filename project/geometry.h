#include <stdbool.h>
# include "bmp.h"
# define pi 3.14159265358979323846 // defining pi

//create a formula structure
typedef struct
{
    float m;
    float c;
    // this struct stores the m and c values in the formula for a line y = mx + c
}
formula;

//create a point structure, which gives us the x and y coordinates of the given point
typedef struct
{
    float x;
    float y;

    // this struct stores the m and c values in the formula for a line y = mx + c
}
point;

formula lineforml(point point, float angle);

point endpoint(int len, point p, formula formula);

point drawline(formula formula, point point, int len, int height, int width, RGBTRIPLE image[height][width]);

float dist(point p, point q);

point rotate(point pt, float angle, point center);

void six(point pt, int height, int width, RGBTRIPLE image[height][width]);

void fill(point pt, int height, int width, RGBTRIPLE image[height][width]);

void stylize(int height, int width, RGBTRIPLE image[height][width]);
