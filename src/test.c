#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/image.h"

// Helper: check if file exists
int file_exists(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (f) { fclose(f); return 1; }
    return 0;
}

int main() {
    printf("=== Running Image Utility Tests ===\n");

    // 1. Load
    BMPImage *img = load_bmp("test/blackbuck.bmp");
    assert(img != NULL);
    printf("[PASS] Loaded test/input.bmp (%dx%d, %d bpp)\n",
           img->dib.biWidth, img->dib.biHeight, img->dib.biBitCount);

    // 2. Save
    assert(save_bmp("test/output_save.bmp", img) == 0);
    assert(file_exists("test/output_save.bmp"));
    printf("[PASS] Save and file exists\n");

    // 3. Fill
    unsigned char red[3] = {255, 0, 0};
    fill_bmp(img, red);
    assert(img->data[0] == 0 && img->data[1] == 0 && img->data[2] == 255);
    printf("[PASS] Fill set first pixel red\n");

    // 4. Rotate
    BMPImage *rot = rotate_bmp(img, 90);
    assert(rot != NULL);
    assert(save_bmp("test/rotated_save.bmp", rot) == 0);
    printf("[PASS] Rotate produced an image %dx%d\n",
           rot->dib.biWidth, rot->dib.biHeight);
    free_bmp(rot);

    // 5. Scale
    BMPImage *sc = scale_bmp(img, 0.5);
    assert(sc != NULL);
    assert(save_bmp("test/scaled_save.bmp", sc) == 0);
    printf("[PASS] Scale reduced to %dx%d\n", sc->dib.biWidth, sc->dib.biHeight);
    free_bmp(sc);

    // 6. Resize
    BMPImage *rsz = resize_bmp(img, 100, 100);
    assert(rsz != NULL);
    assert(rsz->dib.biWidth == 100 && abs(rsz->dib.biHeight) == 100);
    assert(save_bmp("test/resized_save.bmp", rsz) == 0);
    printf("[PASS] Resize produced 100x100 image\n");
    free_bmp(rsz);

    // 7. Crop
    BMPImage *cr = crop_bmp(img, 0, 0, 50, 50);
    assert(cr != NULL);
    assert(cr->dib.biWidth == 50 && abs(cr->dib.biHeight) == 50);
    assert(save_bmp("test/croped_save.bmp", rot) == 0);
    printf("[PASS] Crop produced 50x50 image\n");
    free_bmp(cr);

    // 8. Embed and Extract
    const char *secret = "Hello Stego";
    assert(embed_message(img, secret, 0) == 0);

    assert(save_bmp("test/embed_save.bmp", img) == 0);

    char *msg = extract_message(img, 0);
    assert(msg != NULL);
    assert(strcmp(msg, secret) == 0);
    
    printf("[PASS] Steganography embed/extract: \"%s\"\n", msg);
    free_message(msg);

    // 9. Clean up
    free_bmp(img);

    printf("=== All tests passed! ===\n");
    return 0;
}