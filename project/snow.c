# include <math.h>
# include <stdbool.h>
# include <stdlib.h>
# include <stdio.h>
//# include "bmp.h"
# include "geometry.h"
# include <time.h>

// clang -o snow snow.c geometry.c -lm

int width;
int height;
point flip(float len, point start, float angle);
void makeflake(float angle, int iterations, float len, point pt, int height, int width, RGBTRIPLE image[height][width]);

int main(void)
{
    srand(time(NULL));
    // create a pointer for the blank bmp
    FILE *ptrblank = fopen("black.bmp", "r"); // this sucks. change this.
    if (ptrblank == NULL)
    {
        printf("File not found.\n");
        return 1;
    }
    // i will read the metadata from this black image to get the exact dimensions I want

    // read bitmapfileheader of the image into bmf
    BITMAPFILEHEADER bmf;
    fread(&bmf, sizeof(BITMAPFILEHEADER), 1, ptrblank);
    //printf("read file header\n"); ///////////////////////

    // read bitmapinfoheader into bmi
    BITMAPINFOHEADER bmi;
    fread(&bmi, sizeof(BITMAPINFOHEADER), 1, ptrblank);
    //printf("read info header\n"); ///////////////////////

    // randomised initial conditions for the first branch
    int iterations = 2 + rand() % 3;
    float len = (float) (40 + rand() % 20); // rand() % (91 + 60)
    int branches = 4 + rand() % 6;

    // generate a unique name for the output file using date and time
    struct tm *timeinfo;
    char buffer[80];
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // use sprintf to create the filename
    sprintf(buffer, "snow%d%d%d%d%d%d.bmp", timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);


    // open a file for the new image
    FILE *ptrout = fopen(buffer, "wb"); // replaced snow.bmp with buffer
    if (ptrout == NULL)
    {
        fclose(ptrblank);
        printf("Could not create bitmap image");
        return 3;
    }

    // allocate memory for pixel array
    // Allocate memory for image
    width = bmi.biWidth;
    height = abs(bmi.biHeight);
    // The line of code below is from the given code in the Filter problem written by the cs50 team
    RGBTRIPLE(*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (image == NULL)
    {
        printf("Not enough memory.\n");
        fclose(ptrblank);
        fclose(ptrout);
        return 2;
    }

    point pt;
    pt.x = width / 2;
    pt.y = height / 2;

    int r = rand() % 2;
    if(r == 0)
    {
        makeflake(0, iterations, len/2, pt, height, width, image);
        //printf("makeflake len/2 successful \n");
    }// nothing like reading old commented out debugging print statements to laugh at your past self's suffering

    for(int i = 0; i < branches / 2; i ++)
    {
        makeflake(0, iterations, len, pt, height, width, image);
        pt.x = pt.x + 10 + rand() % 20;
        // random length and iterations
         iterations = 2 + rand() % 3;
         len = (float) ((30 * i) + rand() % 20);
    }

    // randomly choose whether the crystal will have a taper
    int taper = rand() % 2;
    if(taper == 0)
    {
        for(int i = branches / 2; i > 0; i --)
        {
            makeflake(0, iterations, len, pt, height, width, image);
            pt.x = pt.x + 10 + rand() % 20;
            // random len and iterations
            iterations = 2 + rand() % 3;
            len = (float) ((30 * i) + rand() % (2 * i)); // rand() % (91 + 60)
        }
    }

    stylize(height, width, image);




    // write metadata
    fwrite(&bmf, sizeof(BITMAPFILEHEADER), 1, ptrout);
    fwrite(&bmi, sizeof(BITMAPINFOHEADER), 1, ptrout);

    // padding to keep rows a multiple of 4
    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;

    // Write new pixels to outfile
    for (int i = 0; i < height; i++)
    {
        // this will add a pixel ROW
        fwrite(image[i], sizeof(RGBTRIPLE), width, ptrout);

        // add padding
        for (int n = 0; n < padding; n++)
        {
            fputc(0x00, ptrout);
        }
    }


    // FREE MEMORY

    // ptrblank
    fclose(ptrblank);
    // image
    free(image);
    // ptrout
    fclose(ptrout);

    printf("* Congradulations, you now have a snowflake! *\n");
}

void makeflake(float angle, int iterations, float len, point pt, int height, int width, RGBTRIPLE image[height][width])
{// this code creates the snowflake
    float theta = angle + 60;
    float beta = angle - 60;

    if (fabs(angle) == 120.0 || fabs(angle) == 240.0 || fabs(angle) == 180.0) //
    {
        pt = flip(len, pt, angle); //try rotating 180 degrees instead of this

    }

    // // generate the equations
    formula line = lineforml(pt, angle);

    // generate random number
    int branches = 1 + rand() % 4;
    // straight line
    if (iterations > 0 && len >= 0)
    {
        for(int i = 0; i <= branches; i++)
        {
            //srand(time(NULL));
            float n = 1.5 + 1.9 * (float)rand() / RAND_MAX; // why didnt I use python?

            //formula horizontal = lineforml(pt, angle);
            point new = drawline(line, pt, len, height, width, image); //point new will be the end of the main line // change len to gap

            // in case new is out of bounds
            if(!(new.y < height && new.y >= 0 && new.x < width && new.x >= 0))
            {
                return;
            }

            if(len > 0)
            {
                makeflake(theta, iterations -1, len/ n, new, height, width, image);
                makeflake(beta, iterations -1, len/ n, new, height, width, image);
            }

            pt.x = new.x;
            pt.y = new.y;

            len = len / n;

        }

    }
    else
    {
        return;
    }
}

// flip function will lazyly sort out the angle issue because i dont have the emotinal strength to redo drawline
point flip(float len, point start, float angle)
{
    // convert angles to radians
    angle = angle * (pi / 180);

    // given the atarting point and length, function will use distance formula (refer to notebook) to flip the starting point of lines
    // at angles like 120 degrees, because drawline drws from left to right only, causing lines at these angles to face the wrong
    // opp is just opposite side in sohcahtoa. the y is to remind me that this will be used to get the y value of the output point
    float oppy = (sin(angle) * len);//fabs
    float adjx = (cos(angle) * len);//fabs
    point point;
    point.x = start.x + adjx; // swap y and x?
    point.y = start.y + oppy;

    if(point.y > height)
    {
        point.y = height - 10;
    }
    else if(point.y < 0)
    {
        point.y = 0 + 10;
    }
    else if(point.x > width)
    {
        point.x = width - 10;
    }
    else if(point.x < 0)
    {
        point.x = 0 + 10;
    }


    return point;
}



