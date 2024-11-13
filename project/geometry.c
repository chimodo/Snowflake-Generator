# include <math.h>
# include "geometry.h"
# include <stdio.h>
# include <stdbool.h>
# include <stdlib.h>
//# include "bmp.h"

int sqr(int number);

// TODO generate equation
formula lineforml(point point, float angle)
{
    // the coordinate point (a,b) that we want our line to pass through
    int a = point.x;
    int b = point.y;

    // the formula for a line passing through point P(a,b) at angle A is y = tan(A)x - atan(A) + b
    formula formula;

    // convert angle from degrees to radians
    angle = angle * (pi / 180);

    formula.m = tan(angle);
    formula.c = -1 * (a * tan(angle)) + b; // substituted the coordinates of the point this line passes throug and rearranged to get c
    return formula;
}

// function for drawing lines on a plane
point drawline(formula formula, point pt, int len, int height, int width, RGBTRIPLE image[height][width])
{
    int x = pt.x;

    point current; // keep track of current point
    current.x = pt.x;
    current.y = pt.y;
    float distance = dist(pt, current); // this distance should be zero

    while (distance < len)
    {
        float m = formula.m;
        float c = formula.c;
        int y = round((m * x) + c);

        if(y < height && y >= 0 && x < width && x >= 0)
        {
            //arrplane[x][y] = true;

            // change pixels
            image[x][y].rgbtBlue = 255; // swapping x and y to see what happens
            image[x][y].rgbtGreen = 255;
            image[x][y].rgbtRed = 255;

            current.x = x;
            current.y = y;
            fill(current, width, height, image);
            six(current, height, width, image);
        }
        distance = dist(pt, current); // getting the new distance
        x++;
    }

    return current;
}

point endpoint(int len, point p, formula formula)
{
    // use trig to find the endpoint given the length
    point end;

    float angle = atan(formula.m); // using the tan inverse to get the angle of the line
    end.x = len * cos(angle) + p.x; // this is the formula to find the x coordinate (refer back to notebook)
    end.y = len * sin(angle) + p.y; // point p is the start point of the line
    return(end);
}

float dist(point p, point q)
{
    // checks the distance from the starting point to the end using distance formula
    float dist = sqrt(pow(p.x - q.x, 2) + pow(p.y - q.y, 2));
    return dist;
}

point rotate(point pt, float angle, point center)
{
    // convert angle to radians
    angle = angle * (pi/180);

    point new;
    // Below is the standard formula for rotating a point a certain angle (in radians) about a certain point.
    new.x = center.x + (pt.x - center.x) * cos(angle) - (pt.y - center.y) * sin(angle);
    new.y = center.y + (pt.x - center.x) * sin(angle) + (pt.y - center.y) * cos(angle);

    return new;
}

void six(point pt, int height, int width, RGBTRIPLE image[height][width])
{
    // rotate 60, 120, 180, and 240 degrees to create 6 symmetrical
    point new;
    point center;
    center.x = width / 2;
    center.y = height / 2;

    for (int i = 1; i <= 5; i++)
    {
        float angle = (float) (60 * i);
        new = rotate(pt, angle, center);
        int x = (int) new.x;
        int y = (int) new.y;
        fill(new, width, height, image); ////////


        if(x >= 0 && x < width && y >= 0 && y < height)
        {
            // change pixels
            image[x][y].rgbtBlue = 255;
            image[x][y].rgbtGreen = 255;
            image[x][y].rgbtRed = 255;
        }
    }

}

void fill(point pt, int height, int width, RGBTRIPLE image[height][width])
{

    int size = 15; // this will create 15 by 15 boxes around each poxel

    int x = round(pt.x) - (size/ 2);
    int y = round(pt.y) - (size / 2);


    // iterate through the box around the pixel and fill it
    for (int i = x; i <= x + size / 2; i++)
    {
        for (int j = y; j <= y + size / 2; j++)
        {
            // change pixels

            if(j < height && j >= 0 && i < width && i >= 0)
            {
                image[i][j].rgbtBlue = 255;
                image[i][j].rgbtGreen = 255;
                image[i][j].rgbtRed = 255;

            }

        }
    }
}

// stylize the crystal with a modified version of my filter code for edges
// this function will give the snowflake a crystal effect
void stylize(int height, int width, RGBTRIPLE image[height][width])
{
    // reused code i wrote for filter problem
    // create a copy array to store the original color values
    RGBTRIPLE copy[height][width];
    // populate the copy array with pixels
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // copy[i][j] = image[i][j];
            copy[i][j].rgbtRed = image[i][j].rgbtRed;
            copy[i][j].rgbtGreen = image[i][j].rgbtGreen;
            copy[i][j].rgbtBlue = image[i][j].rgbtBlue;
        }
    }
    // gx matrix
    int gxmatrix[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};

    // gy matrix
    int gymatrix[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};


    for (int i = 0; i < height; i++) // iterate through the whole image
    {
        for (int j = 0; j < width; j++) //
        {
            int gxred = 0;
            int gxgreen = 0;
            int gxblue = 0;

            int gyred = 0;
            int gygreen = 0;
            int gyblue = 0;

            int row = 0;
            int N = 0;
            int M = 0;

            for (int n = 0; n < 3; n++) // iterate through the surrounding 3 x 3 block
            {
                N = i + n - 1; // we will multiply gx[0][0] with gx[i-1][j-1] and so on
                for (int m = 0; m < 3; m++)
                {
                    M = j + m - 1;
                    if (N >= 0 && M >= 0 && N < height && M < width)
                    {
                        // gx
                        gxred += copy[N][M].rgbtRed * gxmatrix[n][m]; // multiply it by the appropriate number
                        gxgreen += copy[N][M].rgbtGreen * gxmatrix[n][m];
                        gxblue += copy[N][M].rgbtBlue * gxmatrix[n][m];

                        // gy
                        gyred += copy[N][M].rgbtRed * gymatrix[n][m]; // multiply it by the appropriate number
                        gygreen += copy[N][M].rgbtGreen * gymatrix[n][m];
                        gyblue += copy[N][M].rgbtBlue * gymatrix[n][m];
                    }
                }
            }

            if (round(sqrt(sqr(gxred) + sqr(gyred))) <= 255) // sqr squares the number
            {
                image[i][j].rgbtRed = round(sqrt(sqr(gxred) + sqr(gyred)));
            }
            else
                image[i][j].rgbtRed = 100 + rand() % 80; //180

            // green
            if (round(sqrt(sqr(gxgreen) + sqr(gygreen))) <= 255)
            {
                image[i][j].rgbtGreen = round(sqrt(sqr(gxgreen) + sqr(gygreen)));
            }
            else
                image[i][j].rgbtGreen = 120 + rand() % 80; // 200

            // blue
            if (round(sqrt(sqr(gxblue) + sqr(gyblue))) <= 230)
            {
                image[i][j].rgbtBlue = round(sqrt(sqr(gxblue) + sqr(gyblue)));
            }
            else
                image[i][j].rgbtBlue = 175 + rand() % 80;
        }
    }

    return;
}

int sqr(int number)
{
    return number * number;
}
