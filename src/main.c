#include <stdio.h>
#include <SDL/SDL.h>

#include "mandelbrot.h"
#include "lodepng.h"

#define WIDTH  2560
#define HEIGHT 1440
#define BPP    4
#define DEPTH  32




void setpixel(SDL_Surface *screen, int x, int y, Uint32 colour)
{
    int xPos, yPos;
    Uint8 r, g, b;

    r = (colour >> 16) & 255;
    g = (colour >> 8)  & 255;
    b = (colour )      & 255;

    xPos = x;
    // Moving this into the setpixel method to simplify
    // Should be moved back out to optimise if necessary
    yPos = (y * screen->pitch) / BPP;

    Uint32 *pixmem32;
    Uint32 pColour;

    pColour = SDL_MapRGB(screen->format, r, g, b );

    pixmem32 = (Uint32*) screen->pixels + yPos + xPos;
    *pixmem32 = pColour;
}

void draw_screen(Mandelbrot brot, SDL_Surface* screen)
{
    if(SDL_MUSTLOCK(screen)) {
        if(SDL_LockSurface(screen) < 0) {
            return;
        }
    }

    for (int y = 0; y < screen->h; y++) {
        for (int x = 0; x < screen->w; x++) {

            setpixel(screen, x, y, brot->pixels[x][y]);
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
Mandelbrot draw_mandelbrot_section(Mandelbrot brot, double x1, double y1, double x2, double y2)
{
    double x1Brot, y1Brot, x2Brot, y2Brot;

    double plotX = (brot->x2 - brot->x1);
    double plotY = (brot->y1 - brot->y2);

    x1Brot = brot->x1 + (plotX * x1);
    y1Brot = brot->y1 - (plotY * y1);

    x2Brot = brot->x1 + (plotX * x2);
    y2Brot = brot->y1 - (plotY * y2);

    brot_recreate(brot, x1Brot, y1Brot, x2Brot, y2Brot);

    brot_fix_ratio(brot, HEIGHT, WIDTH);

    brot_calculate(brot);

    return brot;
}

void render_png(Mandelbrot brot)
{
    int width  = brot->pixelWidth;
    int height = brot->pixelHeight;

    Uint32 colour;

    unsigned char* image = malloc(width * height * 4);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            colour = brot->pixels[x][y];
            image[4 * width * y + 4 * x + 0] = (colour >> 16) & 255;
            image[4 * width * y + 4 * x + 1] = (colour >> 8)  & 255;
            image[4 * width * y + 4 * x + 2] = (colour)       & 255;
            image[4 * width * y + 4 * x + 3] = 255;
        }
    }

    unsigned err = lodepng_encode32_file("brotout.png", image, width, height);

    /*if there's an error, display it*/
    if (err) {
        printf("error %u: %s\n", err, lodepng_error_text(err));
    }
}


int main(int argc, char* argv[])
{
    SDL_Surface *screen;
    SDL_Event event;

    int running = 1;

    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        return 1;
    }

    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_FULLSCREEN|SDL_HWSURFACE))) {
        SDL_Quit();
        return 1;
    }

    double x1, x2, y1, y2;
    double temp;

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
                    brot_cleanup(brot);
                    brot = brot_create(WIDTH, HEIGHT, 255, -2.5, -1.0, 1.0, 1.0);
                    brot_calculate(brot);
                    draw_screen(brot, screen);
                    break;
                default:
                    break;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:           //mouse button down
                x1 = (double)event.button.x/WIDTH;
                // Subtract from height so that negative y is downwards
                y1 = (double)event.button.y/HEIGHT;
                break; 
            case SDL_MOUSEBUTTONUP:           //mouse button up
                x2 = (double)event.button.x/WIDTH;
                // Subtract from height so that negative y is downwards
                y2 = (double)event.button.y/HEIGHT;

                if (x1 > x2) {
                    temp = x2;
                    x2 = x1;
                    x1 = temp;
                }

                if (y1 > y2) {
                    temp = y2;
                    y2 = y1;
                    y1 = temp;
                }

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

