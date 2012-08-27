#ifndef BROT_H
#define BROT_H

typedef struct mandelbrot_fractal *Mandelbrot;
typedef struct mandelbrot_fractal {

    // The coordinates of the bottom left corner
    double x1;
    double y1;

    // The coordinates of the top right corner
    double x2;
    double y2;

    // The size of the area in pixels
    // Allows us to calculate the complex
    // number value for each pixel
    int pixelWidth;
    int pixelHeight;

    // A 2D array of the pixels in the canvas
    // Will store the values detailing how many
    // iterations the calculation took to escape
    int **pixels;

    // Maximum number of iterations we'll go through
    // to see if the pixel escapes the bounds
    int repeats;

} Mandelbrot_Data;

// Create the Mandelbrot Data struct and populate it with data
Mandelbrot brot_create(int pixWidth, int pixHeight, int repeats, double x1, double y1, double x2, double y2);

Mandelbrot brot_recreate(Mandelbrot brot, double x1, double y1, double x2, double y2);

Mandelbrot brot_calculate(Mandelbrot brot);

int brot_pixel_coords(Mandelbrot brot, int xPos, int yPos);

int brot_calc_escape(double xPos, double yPos, double limit, int depth);

// Cleanup the Mandelbrot data struct and free all the assigned memory
void brot_cleanup(Mandelbrot brot);

#endif
