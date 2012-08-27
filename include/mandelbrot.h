#ifndef BROT_H
#define BROT_H

typedef struct mandelbrot_fractal *Mandelbrot;
typedef struct mandelbrot_fractal {

    // The coordinates of the bottom left corner
    float x1;
    float y1;

    // The coordinates of the top right corner
    float x2;
    float y2;

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
Mandelbrot brot_create(int pixWidth, int pixHeight, int repeats, float x1, float y1, float x2, float y2);

Mandelbrot brot_recreate(Mandelbrot brot, float x1, float y1, float x2, float y2);

Mandelbrot brot_calculate(Mandelbrot brot);

int brot_pixel_coords(Mandelbrot brot, int xPos, int yPos);

int brot_calc_escape(float xPos, float yPos, float limit, int depth);

// Cleanup the Mandelbrot data struct and free all the assigned memory
void brot_cleanup(Mandelbrot brot);

#endif
