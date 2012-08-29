#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include "mandelbrot.h"

Mandelbrot brot_create(int pixelWidth, int pixelHeight, int repeats, double x1, double y1, double x2, double y2)
{
    Mandelbrot brot = (Mandelbrot) malloc(sizeof(Mandelbrot_Data));

    brot->x1 = x1;
    brot->y1 = y1;

    brot->x2 = x2;
    brot->y2 = y2;

    brot->pixelWidth = pixelWidth;
    brot->pixelHeight = pixelHeight;

    brot->pixels = (int**) malloc(sizeof(int*) * brot->pixelWidth);

    brot->repeats = repeats;

    // Assign the memory for the 2D array
    // Actually done as an array of pointers to arrays of ints
    for (int i = 0; i < brot->pixelWidth; i++) {
        brot->pixels[i] = (int*) malloc(sizeof(int*) * brot->pixelHeight);
    }

    return brot;
}

/** Takes coordinates for two points which define the upper left
  * and lower right corners of a rectangle.
  * These are doubles which are given as a 0 to 1 value of the distance
  * from the top left corner of the screen.
  */
Mandelbrot brot_zoom(Mandelbrot brot, double x1, double y1, double x2, double y2)
{
    double x1Brot, y1Brot, x2Brot, y2Brot;

    double plotX = (brot->x2 - brot->x1);
    double plotY = (brot->y1 - brot->y2);

    x1Brot = brot->x1 + (plotX * x1);
    y1Brot = brot->y1 - (plotY * y1);

    x2Brot = brot->x1 + (plotX * x2);
    y2Brot = brot->y1 - (plotY * y2);

    brot->x1 = x1Brot;
    brot->y1 = y1Brot;

    brot->x2 = x2Brot;
    brot->y2 = y2Brot;

    brot_calculate(brot);

    return brot;
}

Mandelbrot brot_calculate(Mandelbrot brot)
{
    for (int xPos = 0; xPos < brot->pixelWidth; xPos++) {
        for (int yPos = 0; yPos < brot->pixelHeight; yPos++) {
            brot->pixels[xPos][yPos] = brot_pixel_coords(brot, xPos, yPos);
        }
    }

    return brot;
}

int brot_pixel_coords(Mandelbrot brot, int xPos, int yPos)
{
    double x = (double)brot->x1 + ((brot->x2 - brot->x1) * ((double)xPos / brot->pixelWidth));

    // Subtracting from y1 because we have two coordinate systems
    // Mandelbrot is plotted on usual x/y axes with y increasing upwards
    // Pixels have origin at top left corner and y increases downwards
    double y = (double)brot->y1 - ((brot->y1 - brot->y2) * ((double)yPos / brot->pixelHeight));

    return brot_calc_escape(x, y, brot->repeats);
}

int brot_calc_escape(double xPos, double yPos, int depth)
{
    double x = 0;
    double y = 0;

    double temp = 0;

    int iteration = 0;

    while ( ((x*x + y*y) < 4) && (iteration < depth) ) {

        temp = x*x - y*y + xPos;

        y = 2*x*y + yPos;

        x = temp;

        iteration++;
    }

    return calc_colour(iteration);
}


uint32_t calc_colour(int value)
{
    uint32_t colour;

    if (value < 1) {
        colour = 0 | (255 << 16) | (255 << 8) | 255;
    } else if (value > 200) {
        colour = 0;
    } else {
        switch(value % 8) {
            case 0:
                colour = (0 << 16) | (0 << 8) | 100;
                break;
            case 1:
                colour = (0 << 16) | (0 << 8) | 150;
                break;
            case 2:
                colour = (0 << 16) | (0 << 8) | 200;
                break;
            case 3:
                colour = (50 << 16) | (50 << 8) | 250;
                break;
            case 4:
                colour = (100 << 16) | (100 << 8) | 250;
                break;
            case 5:
                colour = (150 << 16) | (150 << 8) | 250;
                break;
            case 6:
                colour = (200 << 16) | (200 << 8) | 250;
                break;
            case 7:
                colour = (250 << 16) | (250 << 8) | 250;
                break;
        }
    }

    return colour;
}

void brot_cleanup(Mandelbrot brot)
{
    for (int i = 0; i < brot->pixelWidth; i++) {
        free(brot->pixels[i]);
    }

    free(brot->pixels);

    free(brot);
}


