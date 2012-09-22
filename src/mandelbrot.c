#include <stdio.h>
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

    brot->startX1 = x1;
    brot->startY1 = y1;

    brot->startX2 = x2;
    brot->startY2 = y2;

    brot->pixelWidth = pixelWidth;
    brot->pixelHeight = pixelHeight;

    brot->canvas = (uint32_t**) malloc(sizeof(uint32_t*) * brot->pixelWidth);

    brot->smooth_values = (double**) malloc(sizeof(double*) * brot->pixelWidth);

    brot->repeats = repeats;

    // Assign the memory for the canvas
    // Actually done as an array of pointers to arrays of ints
    for (int i = 0; i < brot->pixelWidth; i++) {
        brot->canvas[i] = (uint32_t*) malloc(sizeof(uint32_t*) * brot->pixelHeight);
    }

    // Assign the memory for the smooth value array
    for (int i = 0; i < brot->pixelWidth; i++) {
        brot->smooth_values[i] = (double*) malloc(sizeof(double*) * brot->pixelHeight);
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

    brot_smooth_calculate(brot);

    return brot;
}

Mandelbrot brot_reset_zoom(Mandelbrot brot)
{
    brot->x1 = brot->startX1;
    brot->y1 = brot->startY1;

    brot->x2 = brot->startX2;
    brot->y2 = brot->startY2;

    brot_smooth_calculate(brot);

    return brot;
}

Mandelbrot brot_smooth_calculate(Mandelbrot brot)
{
    double highest = 0.0;
    double lowest = 1000;
    double value;

    // Calculate mandelbrot values
    for (int xPos = 0; xPos < brot->pixelWidth; xPos++) {
        for (int yPos = 0; yPos < brot->pixelHeight; yPos++) {
            value = brot_calc_smooth_value(brot, xPos, yPos);
            if (value > highest) {
                highest = value;
            }
            if (value > 0 && value < lowest) {
                lowest = value;
            }
            brot->smooth_values[xPos][yPos] = value;
        }
    }

    // scaling from 0 to 360
    for (int xPos = 0; xPos < brot->pixelWidth; xPos++) {
        for (int yPos = 0; yPos < brot->pixelHeight; yPos++) {
            brot->smooth_values[xPos][yPos] = 360.0 * brot_scale_value(brot->smooth_values[xPos][yPos], highest, lowest);
        }
    }

    // calculate colours
    for (int xPos = 0; xPos < brot->pixelWidth; xPos++) {
        for (int yPos = 0; yPos < brot->pixelHeight; yPos++) {
            brot->canvas[xPos][yPos] = colour_from_hue(brot->smooth_values[xPos][yPos]);
        }
    }


    return brot;
}

double brot_scale_value(double value, double high, double low)
{
    return ( (value - low) / (high - low) );
}

double brot_calc_smooth_value(Mandelbrot brot, int xPos, int yPos)
{
    double xCoord = (double)brot->x1 + ((brot->x2 - brot->x1) * ((double)xPos / brot->pixelWidth));

    // Subtracting from y1 because we have two coordinate systems
    // Mandelbrot is plotted on usual x/y axes with y increasing upwards
    // Pixels have origin at top left corner and y increases downwards
    double yCoord = (double)brot->y1 - ((brot->y1 - brot->y2) * ((double)yPos / brot->pixelHeight));

    double x = 0;
    double y = 0;

    double temp = 0;

    int iteration = 0;

    while ( ((x*x + y*y) < 4) && (iteration < brot->repeats) ) {

        temp = x*x - y*y + xCoord;

        y = 2*x*y + yCoord;

        x = temp;

        iteration++;
    }

    if (iteration == brot->repeats) {
        return -1.0;
    } else {
        return (double)iteration + 1.0 - (log( log( sqrt(x*x + y*y) ) ) / log(2));
    }
}


uint32_t colour_from_hue(double value)
{
    if (value < 0) {
        return 0;
    }

    uint32_t colour;

    double hue = fmod(value, 360.0);

    double sat = 0.5;
    double val = 0.5;

    int h = hue / 60;

    double f = (double)hue/60-h;
    double p = val*(1.0-sat);
    double q = val*(1.0-sat*f);
    double t = val*(1.0-sat*(1-f));
    double v = val;

    int r,g,b;

    switch(h) {
        default:
        case 0:
        case 6:
            r = (int)(v*255); g = (int)(t*255); b = (int)(p*255);
            break;
        case 1:
            r = (int)(q*255); g = (int)(v*255); b = (int)(p*255);
            break;
        case 2:
            r = (int)(p*255); g = (int)(v*255); b = (int)(t*255);
            break;
        case 3:
            r = (int)(p*255); g = (int)(q*255); b = (int)(v*255);
            break;
        case 4:
            r = (int)(t*255); g = (int)(p*255); b = (int)(v*255);
            break;
        case 5:
            r = (int)(v*255); g = (int)(p*255); b = (int)(q*255);
            break;
    }

    colour = (r << 16) | (g << 8)   | b;

    return colour;
}

void brot_cleanup(Mandelbrot brot)
{
    for (int i = 0; i < brot->pixelWidth; i++) {
        free(brot->canvas[i]);
    }

    free(brot->canvas);

    free(brot);
}


