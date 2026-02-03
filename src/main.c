#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/image.h"

static void print_menu()
{
    printf("\n=== Image Utility ===\n");
    printf("Commands:\n");
    printf("  load <filename>         - Load a BMP file\n");
    printf("  save <filename>         - Save current image\n");
    printf("  fill <R> <G> <B>        - Fill image with color\n");
    printf("  rotate <angle>          - Rotate by angle (degrees)\n");
    printf("  scale <factor>          - Scale by factor (e.g. 0.5, 2.0)\n");
    printf("  resize <w> <h>          - Resize to width/height\n");
    printf("  crop <x> <y> <w> <h>    - Crop region\n");
    printf("  embed <message>         - Hide text inside image\n");
    printf("  extract                 - Recover hidden text from image\n");
    printf("  exit                    - Quit program\n");
    printf("======================\n");
}

int main()
{
    //Initializing space for image
    BMPImage *img = NULL;
    char line[512];

    //Printing menu
    printf("Welcome to the Image Utility (BMP only).\n");
    print_menu();

    //while true in C
    while (1)
    {   
        //get the command name
        printf("\n> ");
        if (!fgets(line, sizeof(line), stdin))
            break;

        // strip newline
        line[strcspn(line, "\r\n")] = 0;

        //If the user did not enter anything
        if (strlen(line) == 0)
            continue;

        //Now, what is the command that her entered
        char *cmd = strtok(line, " ");
        if (!cmd)
            continue;

        //If he entered exit, just exit
        if (strcmp(cmd, "exit") == 0)
        {
            break;
        }

        //For help we print the menu 
        else if (strcmp(cmd, "help") == 0)
        {
            print_menu();
        }

        //If the user wants to load the image =
        else if (strcmp(cmd, "load") == 0)
        {  
            //we get the file name
            char *fname = strtok(NULL, " ");
            if (!fname)
            {
                printf("Usage: load <filename>\n");
                continue;
            }

            //We load the image 
            BMPImage *tmp = load_bmp(fname);

            if (!tmp)
            {
                printf("Failed to load %s\n", fname);
            }

            else
            {
                //We delete the previous image
                free_bmp(img);

                //Copy the temporary one
                img = tmp;

                //Print for a success
                printf("Loaded %s (%dx%d, %d bpp)\n",
                       fname, img->dib.biWidth, img->dib.biHeight, img->dib.biBitCount);
            }
        }

        //If the user wants to save the image 
        else if (strcmp(cmd, "save") == 0)
        {
            if (!img)
            {
                printf("No image loaded.\n");
                continue;
            }
            char *fname = strtok(NULL, " ");
            if (!fname)
            {
                printf("Usage: save <filename>\n");
                continue;
            }
            if (save_bmp(fname, img) == 0)
            {
                printf("Saved image to %s\n", fname);
            }
            else
            {
                printf("Failed to save image.\n");
            }
        }

        //Filling the picture with color described by the user
        else if (strcmp(cmd, "fill") == 0)
        {
            if (!img)
            {
                printf("No image loaded.\n");
                continue;
            }
            char *r = strtok(NULL, " ");
            char *g = strtok(NULL, " ");
            char *b = strtok(NULL, " ");
            if (!r || !g || !b)
            {
                printf("Usage: fill <R> <G> <B>\n");
                continue;
            }
            unsigned char color[3];
            color[0] = (unsigned char)atoi(r);
            color[1] = (unsigned char)atoi(g);
            color[2] = (unsigned char)atoi(b);
            fill_bmp(img, color);
            printf("Image filled with color (%d, %d, %d).\n", color[0], color[1], color[2]);
        }

        //If the user prompted to rotate 
        else if (strcmp(cmd, "rotate") == 0)
        {
            if (!img)
            {
                printf("No image loaded.\n");
                continue;
            }
            char *ang = strtok(NULL, " ");
            if (!ang)
            {
                printf("Usage: rotate <angle>\n");
                continue;
            }
            double angle = atof(ang);
            BMPImage *rot = rotate_bmp(img, angle);
            if (rot)
            {
                free_bmp(img);
                img = rot;
                printf("Rotated image by %.2f degrees.\n", angle);
            }
            else
            {
                printf("Rotation failed.\n");
            }
        }

        //Scaling the image 
        else if (strcmp(cmd, "scale") == 0)
        {
            if (!img)
            {
                printf("No image loaded.\n");
                continue;
            }
            char *fac = strtok(NULL, " ");
            if (!fac)
            {
                printf("Usage: scale <factor>\n");
                continue;
            }
            double factor = atof(fac);
            if (factor <= 0)
            {
                printf("Scale factor must be > 0.\n");
                continue;
            }
            BMPImage *sc = scale_bmp(img, factor);
            if (sc)
            {
                free_bmp(img);
                img = sc;
                printf("Scaled image by %.2fx.\n", factor);
            }
            else
            {
                printf("Scaling failed.\n");
            }
        }
        else if (strcmp(cmd, "resize") == 0)
        {
            if (!img)
            {
                printf("No image loaded.\n");
                continue;
            }
            char *w = strtok(NULL, " ");
            char *h = strtok(NULL, " ");
            if (!w || !h)
            {
                printf("Usage: resize <w> <h>\n");
                continue;
            }
            int new_w = atoi(w), new_h = atoi(h);
            BMPImage *rsz = resize_bmp(img, new_w, new_h);
            if (rsz)
            {
                free_bmp(img);
                img = rsz;
                printf("Resized image to %dx%d.\n", new_w, new_h);
            }
            else
            {
                printf("Resize failed.\n");
            }
        }
        else if (strcmp(cmd, "crop") == 0)
        {
            if (!img)
            {
                printf("No image loaded.\n");
                continue;
            }
            char *sx = strtok(NULL, " ");
            char *sy = strtok(NULL, " ");
            char *sw = strtok(NULL, " ");
            char *sh = strtok(NULL, " ");
            if (!sx || !sy || !sw || !sh)
            {
                printf("Usage: crop <x> <y> <w> <h>\n");
                continue;
            }
            int x = atoi(sx), y = atoi(sy), w = atoi(sw), h = atoi(sh);
            BMPImage *cr = crop_bmp(img, x, y, w, h);
            if (cr)
            {
                free_bmp(img);
                img = cr;
                printf("Cropped to region (%d,%d,%d,%d).\n", x, y, w, h);
            }
            else
            {
                printf("Crop failed.\n");
            }
        }
        else if (strcmp(cmd, "embed") == 0)
        {
            if (!img)
            {
                printf("No image loaded.\n");
                continue;
            }
            char *msg = strtok(NULL, "");
            if (!msg)
            {
                printf("Usage: embed <message>\n");
                continue;
            }
            if (embed_message(img, msg, 0) == 0)
            {
                printf("Message embedded.\n");
            }
            else
            {
                printf("Failed to embed message.\n");
            }
        }
        else if (strcmp(cmd, "extract") == 0)
        {
            if (!img)
            {
                printf("No image loaded.\n");
                continue;
            }
            char *msg = extract_message(img, 0);
            if (msg)
            {
                printf("Extracted message: \"%s\"\n", msg);
                free_message(msg);
            }
            else
            {
                printf("No message found.\n");
            }
        }
        else
        {
            printf("Unknown command: %s\n", cmd);
            print_menu();
        }
    }

    free_bmp(img);
    printf("Goodbye!\n");
    return 0;
}
