#ifndef BROT_H
#define BROT_H

#include <stdint.h>

typedef struct mandelbrot_fractal *Mandelbrot;
typedef struct mandelbrot_fractal {

    // The coordinates of the bottom left corner
    double x1;
    double y1;

    // The coordinates of the top right corner
    double x2;
    double y2;

    // These variables store the initial coordinates that
    // the mandelbrot was created with
    // Used for resetting the zoom
    double startX1;
    double startY1;

    double startX2;
    double startY2;

    // The size of the area in pixels
    // Allows us to calculate the complex
    // number value for each pixel
    int pixelWidth;
    int pixelHeight;

    // A 2D array of the pixels in the image
    uint32_t **canvas;

    // A 2D array of the raw escape values for the Mandelbrot set
    // Will store the values detailing how many
    // iterations the calculation took to escape
    int **raw_values;

    // A 2D array of the smoothed Mandelbrot values
    double **smooth_values;

    // Maximum number of iterations we'll go through
    // to see if the pixel escapes the bounds
    int repeats;

} Mandelbrot_Data;

// Create the Mandelbrot Data struct and populate it with data
Mandelbrot brot_create(int pixWidth, int pixHeight, int repeats, double x1, double y1, double x2, double y2);

Mandelbrot brot_zoom(Mandelbrot brot, double x1, double y1, double x2, double y2);

Mandelbrot brot_reset_zoom(Mandelbrot brot);

Mandelbrot brot_smooth_calculate(Mandelbrot brot);

double brot_scale_value(double value, double high, double low);

double brot_calc_smooth_value(Mandelbrot brot, int xPos, int yPos);

uint32_t colour_from_hue(double value);

// Cleanup the Mandelbrot data struct and free all the assigned memory
void brot_cleanup(Mandelbrot brot);

#endif
