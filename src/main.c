#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL/SDL.h>

#include "main.h"
#include "mandelbrot.h"
#include "lodepng.h"

#define BPP    4
#define DEPTH  32


void usage(int exitval) {
    printf("Mandelbrot usage:\n");
    printf("mandelbrot width height outputfile\n");
    exit(exitval);
}

Args parse_args(int argc, char *argv[]) {

    Args args = {0, 0, ""};

    int c;
    while ( (c = getopt(argc, argv, "")) != -1) {
        switch (c)
        {
            default:
                usage(0);
                break;
        }
    }


    args.width       = atoi(argv[1]);
    args.height      = atoi(argv[2]);
    args.output_file =  argv[3];

    if (*args.output_file == '\0') {
        printf("Need to specify an output file\n");
        usage(1);
    } else {
        printf("Writing images to %s\n", args.output_file);
    }

    if (args.width < 1) {
        printf("Need to specify a width\n");
        usage(1);
    }

    if (args.height < 1) {
        printf("Need to specify a height\n");
        usage(1);
    }

    return args;
}

void setpixel(SDL_Surface *screen, int x, int y, Uint32 colour)
{
    Uint32 *pixmem32;
    Uint32 pColour;

    pColour = SDL_MapRGB(screen->format,
                         (colour >> 16) & 255,
                         (colour >> 8)  & 255,
                         (colour )      & 255
                         );

    pixmem32 = (Uint32*) screen->pixels + y + x;
    *pixmem32 = pColour;
}

void draw_screen(Mandelbrot brot, SDL_Surface* screen)
{
    if(SDL_MUSTLOCK(screen)) {
        if(SDL_LockSurface(screen) < 0) {
            return;
        }
    }

    int yPos;

    for (int y = 0; y < screen->h; y++) {
        yPos = (y * screen->pitch) / BPP;
        for (int x = 0; x < screen->w; x++) {
            setpixel(screen, x, yPos, brot->canvas[x][y]);
        }
    }

    if(SDL_MUSTLOCK(screen)) {
        SDL_UnlockSurface(screen);
    }

    SDL_Flip(screen);
}

void render_png(Mandelbrot brot, char* output_file)
{
    int width  = brot->pixelWidth;
    int height = brot->pixelHeight;

    Uint32 colour;

    unsigned err;

    unsigned char* image = malloc(width * height * 4);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            colour = brot->canvas[x][y];
            image[4 * width * y + 4 * x + 0] = (colour >> 16) & 255;
            image[4 * width * y + 4 * x + 1] = (colour >> 8)  & 255;
            image[4 * width * y + 4 * x + 2] = (colour)       & 255;
            image[4 * width * y + 4 * x + 3] = 255;
        }
    }

    err = lodepng_encode32_file(output_file, image, width, height);

    if (err) {
        printf("error %u: %s\n", err, lodepng_error_text(err));
    }
}


int main(int argc, char* argv[])
{

    Args args = parse_args(argc, argv);

    SDL_Surface *screen;
    SDL_Event event;

    int running = 1;
    double x1, x2, y1, y2;
    double temp;

    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        return 1;
    }

    if (!(screen = SDL_SetVideoMode(args.width, args.height, DEPTH, SDL_FULLSCREEN|SDL_HWSURFACE))) {
        SDL_Quit();
        return 1;
    }

    Mandelbrot brot = brot_create(args.width, args.height, 255, -2.5, -1.0, 1.0, 1.0);

    brot_smooth_calculate(brot);

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
                    render_png(brot, args.output_file);
                    break;
                case SDLK_r:
                    // Reset image
                    brot_reset_zoom(brot);
                    draw_screen(brot, screen);
                    break;
                default:
                    break;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                x1 = (double)event.button.x/args.width;
                y1 = (double)event.button.y/args.height;
                break; 
            case SDL_MOUSEBUTTONUP:
                x2 = (double)event.button.x/args.width;
                y2 = (double)event.button.y/args.height;

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

                // Make sure the zoomed area always maintains the correct ratio.
                // Because the values have already been scaled between 0 and 1
                // we just need to make sure the area is a square
                y2 = (x2 - x1) + y1;

                brot_zoom(brot, x1, y1, x2, y2);
                draw_screen(brot, screen);
                break; 

            }
        }
    }

    SDL_Quit();

    brot_cleanup(brot);

    return 0;
}

