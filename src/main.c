#include <stdio.h>
#include <SDL/SDL.h>

#include "mandelbrot.h"
#include "lodepng.h"

#define WIDTH  2560
#define HEIGHT 1440
#define BPP    4
#define DEPTH  32

void setpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    int xPos, yPos;

    xPos = x;
    // Moving this into the setpixel method to simplify
    // Should be moved back out to optimise if necessary
    yPos = (y * screen->pitch) / BPP;

    Uint32 *pixmem32;
    Uint32 colour;

    colour = SDL_MapRGB(screen->format, r, g, b );

    pixmem32 = (Uint32*) screen->pixels + yPos + xPos;
    *pixmem32 = colour;
}

void draw_screen(Mandelbrot brot, SDL_Surface* screen)
{
    int xPos, yPos, colour;

    printf("drawn: %f,%f   to   %f,%f\n", brot->x1, brot->y1, brot->x2, brot->y2);


    if(SDL_MUSTLOCK(screen)) {
        if(SDL_LockSurface(screen) < 0) {
            return;
        }
    }

    for (yPos = 0; yPos < screen->h; yPos++) {
        for (xPos = 0; xPos < screen->w; xPos++) {
            colour = brot->pixels[xPos][yPos];
            setpixel(screen, xPos, yPos, colour, colour, colour);
        }
    }

    if(SDL_MUSTLOCK(screen)) {
        SDL_UnlockSurface(screen);
    }

    SDL_Flip(screen);

}

/* Given pixel coordinates, this will render a selected section of
 * the screen after zooming in
 */
void draw_mandelbrot_section(Mandelbrot brot, int x1, int y1, int x2, int y2)
{
    int temp;

    if (x1 > x2) {
        temp = x2;
        x2 = x1;
        x1 = temp;
    }

    if (y1 < y2) {
        temp = y2;
        y2 = y1;
        y1 = temp;
    }

    printf("pixel: %i,%i   to   %i,%i\n", x1, y1, x2, y2);

    float x1Brot, y1Brot, x2Brot, y2Brot;

    float plotX = (brot->x2 - brot->x1);
    float plotY = (brot->y2 - brot->y1);

    x1Brot = (plotX * ((float)x1 / brot->pixelWidth)) + brot->x1;
    y1Brot = brot->y2 - (plotY * ((float)y1 / brot->pixelHeight));

    x2Brot = (plotX * ((float)x2 / brot->pixelWidth)) + brot->x1;
    y2Brot = brot->y2 - (plotY * ((float)y2 / brot->pixelHeight));

    printf("calced: %f,%f   to   %f,%f\n", x1Brot, y1Brot, x2Brot, y2Brot);

    brot_recreate(brot, x1Brot, y1Brot, x2Brot, y2Brot);

    brot_calculate(brot);

}

void render_png(Mandelbrot brot)
{
    int y, x;
    int width = brot->pixelWidth;
    int height = brot->pixelHeight;

    Uint8 colour;

    unsigned char* image = malloc(width * height * 4);

    for (y = 0; y < brot->pixelHeight; y++) {
        for (x = 0; x < brot->pixelWidth; x++) {
            colour = brot->pixels[x][y];
            image[4 * width * y + 4 * x + 0] = colour;
            image[4 * width * y + 4 * x + 1] = colour;
            image[4 * width * y + 4 * x + 2] = colour;
            image[4 * width * y + 4 * x + 3] = 255;
        }
    }

    unsigned error = lodepng_encode32_file("brotout.png", image, brot->pixelWidth, brot->pixelHeight);

    /*if there's an error, display it*/
    if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
}


int main(int argc, char* argv[])
{
    SDL_Surface *screen;
    SDL_Event event;

    int running = 1;

    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;

    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_FULLSCREEN|SDL_HWSURFACE))) {
        SDL_Quit();
        return 1;
    }

    int x1, x2, y1, y2;

    Mandelbrot brot = brot_create(WIDTH, HEIGHT, 255, -2.5, -1.0, 1.0, 1.0);

    brot_calculate(brot);

    draw_screen(brot, screen);

    while(running) {
        while(SDL_PollEvent(&event)) {

            switch (event.type) {

            case SDL_QUIT:
                running = 0;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    // Escape key
                    running = 0;
                    break;
                case SDLK_p:
                    // Write out png
                    render_png(brot);
                    break;
                case SDLK_r:
                    // Reset image
                    // This needs to be Non height/width specific
                    printf("Resetting view\n");
                    brot = brot_create(WIDTH, HEIGHT, 255, -2.5, -1.0, 1.0, 1.0);
                    brot_calculate(brot);
                    draw_screen(brot, screen);
                    break;
                default:
                    break;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:           //mouse button down
                x1 = event.button.x;
                y1 = event.button.y;
                break; 
            case SDL_MOUSEBUTTONUP:           //mouse button up
                x2 = event.button.x;
                y2 = event.button.y;
                draw_mandelbrot_section(brot, x1, y1, x2, y2);
                draw_screen(brot, screen);
                break; 

            }
        }
    }

    SDL_Quit();

    brot_cleanup(brot);

    return 0;
}

