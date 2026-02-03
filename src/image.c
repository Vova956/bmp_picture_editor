#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../include/image.h"

#define M_PI 3.14159265358979323846

// Function that loads the BMP file and returns the
// BMPImage pointer
BMPImage *load_bmp(const char *filename)
{

    // Trying to load the file
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        perror("Error opening file");
        return NULL;
    }

    // Allocating space for the BMP file
    BMPImage *img = (BMPImage *)malloc(sizeof(BMPImage));
    if (!img)
    {
        fclose(file);
        return NULL;
    }

    // Read headers
    fread(&img->header, sizeof(BMPHeader), 1, file);
    fread(&img->dib, sizeof(DIBHeader), 1, file);

    // Validate BMP
    if (img->header.bfType != 0x4D42)
    { // 'BM'
        fprintf(stderr, "Not a BMP file!\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Allocate memory for pixel data
    img->data = (unsigned char *)malloc(img->dib.biSizeImage);
    if (!img->data)
    {
        free(img);
        fclose(file);
        return NULL;
    }

    // Move to pixel data offset
    fseek(file, img->header.bfOffBits, SEEK_SET);
    fread(img->data, 1, img->dib.biSizeImage, file);

    fclose(file);
    return img;
}

// Function that frees the memory of the BMP object
void free_bmp(BMPImage *image)
{
    if (image)
    {
        free(image->data);
        free(image);
    }
}

// Function to fill the image with a given color
// color should be an array of 3 bytes: [R, G, B]
// Supports only 24 bit filling 
void fill_bmp(BMPImage *image, unsigned char color[3])
{

    // Check if the umage is correct
    if (!image || !image->data)
        return;

    // Only support 24-bit BMPs
    if (image->dib.biBitCount != 24)
        return;

    // Getting info about the image
    int width = image->dib.biWidth;
    int height = image->dib.biHeight;
    // BMP rows are padded to 4 bytes
    int row_padded = (width * 3 + 3) & (~3);

    // For each pixel fill it with color
    for (int y = 0; y < height; y++)
    {
        unsigned char *row = image->data + y * row_padded;
        for (int x = 0; x < width; x++)
        {
            // BMP stores pixels as BGR
            row[x * 3 + 0] = color[2];
            row[x * 3 + 1] = color[1];
            row[x * 3 + 2] = color[0];
        }
    }
}


// Function that saves the BMP object to a file
// Returns 0 if successful 1 otherwise
int save_bmp(const char *filename, BMPImage *image)
{

    // Trying to open a file for reading
    FILE *f = fopen(filename, "wb");
    if (!f)
        return 1;

    // Writing a header
    fwrite(&image->header, sizeof(BMPHeader), 1, f);
    fwrite(&image->dib, sizeof(DIBHeader), 1, f);

    // Writing the pixels
    fwrite(image->data, 1, image->dib.biSizeImage, f);

    fclose(f);
    return 0;
}

// Function that rotates BMP for a angles in degrees
// Returns the pointer to the new rotated image
BMPImage *rotate_bmp(const BMPImage *src, double angle_degrees)
{
    if (!src || !src->data)
        return NULL;

    
    if (src->dib.biBitCount != 24 || src->dib.biCompression != 0)
    {
        printf("Unssuported BMP format for rotation!\n");
        return NULL;
    }

    // Getting info about the image
    int width = src->dib.biWidth;
    int height = src->dib.biHeight;
    int row_padded = (width * 3 + 3) & (~3);

    // Create a new image for result
    BMPImage *dst = (BMPImage *)malloc(sizeof(BMPImage));
    if (!dst)
        return NULL;

    dst->header = src->header;
    dst->dib = src->dib;

    dst->data = (unsigned char *)malloc(src->dib.biSizeImage);
    if (!dst->data)
    {
        free(dst);
        return NULL;
    }

    // Rotation setup
    double angle = angle_degrees * M_PI / 180.0;
    double cosA = cos(angle);
    double sinA = sin(angle);

    double cx = width / 2.0;
    double cy = height / 2.0;

    int dst_row_padded = row_padded;

    // Fill with black initially
    memset(dst->data, 0, dst->dib.biSizeImage);

    // For each pixel in destination
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Map back to source
            double srcX = cosA * (x - cx) + sinA * (y - cy) + cx;
            double srcY = -sinA * (x - cx) + cosA * (y - cy) + cy;

            int srcXi = (int)round(srcX);
            int srcYi = (int)round(srcY);

            if (srcXi >= 0 && srcXi < width && srcYi >= 0 && srcYi < height)
            {
                unsigned char *dstPixel = dst->data + y * dst_row_padded + x * 3;
                unsigned char *srcPixel = src->data + srcYi * row_padded + srcXi * 3;

                dstPixel[0] = srcPixel[0]; // B
                dstPixel[1] = srcPixel[1]; // G
                dstPixel[2] = srcPixel[2]; // R
            }
        }
    }

    return dst;
}


// Function that scales the bmp image
// Returns the poiter to a scaled bmp image
BMPImage *scale_bmp(const BMPImage *src, double factor)
{
    if (!src || !src->data || factor <= 0)
        return NULL;

    // Getting info about the image
    int oldW = src->dib.biWidth;
    int oldH = src->dib.biHeight;
    int oldRowPadded = (oldW * 3 + 3) & (~3);

    // What should the result size be?
    int newW = (int)(oldW * factor);
    int newH = (int)(oldH * factor);
    int newRowPadded = (newW * 3 + 3) & (~3);

    BMPImage *dst = (BMPImage *)malloc(sizeof(BMPImage));
    if (!dst)
        return NULL;

    // Copy headers and update
    dst->header = src->header;
    dst->dib = src->dib;

    // Creating and setting the new img header
    dst->dib.biWidth = newW;
    dst->dib.biHeight = newH;
    dst->dib.biSizeImage = newRowPadded * newH;
    dst->header.bfSize = dst->header.bfOffBits + dst->dib.biSizeImage;

    dst->data = (unsigned char *)malloc(dst->dib.biSizeImage);
    if (!dst->data)
    {
        free(dst);
        return NULL;
    }

    // Fill with black (in case padding bytes exist)
    memset(dst->data, 0, dst->dib.biSizeImage);

    // Nearest-neighbor scaling
    for (int y = 0; y < newH; y++)
    {
        for (int x = 0; x < newW; x++)
        {
            int srcX = (int)(x / factor);
            int srcY = (int)(y / factor);

            unsigned char *dstPixel = dst->data + y * newRowPadded + x * 3;
            unsigned char *srcPixel = src->data + srcY * oldRowPadded + srcX * 3;

            dstPixel[0] = srcPixel[0]; // B
            dstPixel[1] = srcPixel[1]; // G
            dstPixel[2] = srcPixel[2]; // R
        }
    }

    return dst;
}

// Helper: compute row size (padded to 4 bytes) for 24-bit BMP.
// bytesPerPixel = biBitCount / 8
// rowSize = ((width * bytesPerPixel + 3) / 4) * 4
static size_t row_size(const DIBHeader *dib)
{
    int bytesPerPixel = dib->biBitCount / 8;
    int w = dib->biWidth;
    size_t row = ((size_t)w * bytesPerPixel + 3) & ~3u; // align to 4
    return row;
}

// Set or get a single bit inside a byte depending on mode
static void set_bit_in_byte(unsigned char *byte, int bit_val, int use_msb)
{
    if (use_msb)
    {
        /* set most-significant bit (bit 7) to bit_val (0 or 1) */
        if (bit_val)
            *byte |= 0x80;
        else
            *byte &= 0x7F;
    }
    else
    {
        /* set least-significant bit (bit 0) */
        if (bit_val)
            *byte |= 0x01;
        else
            *byte &= 0xFE;
    }
}

// Functions that gets us a most significant or a least significant bit
static int get_bit_from_byte(unsigned char byte, int use_msb)
{
    if (use_msb)
    {
        return (byte & 0x80) ? 1 : 0;
    }
    else
    {
        return (byte & 0x01) ? 1 : 0;
    }
}

// Embed a message in image pixel bytes.
// Returns 0 on success, -1 on error (e.g., unsupported format or not enough capacity).
// We store a 32-bit unsigned length first (little-endian), then message bytes.
// Each bit of that data is stored in one image byte (one color channel byte).
int embed_message(BMPImage *img, const char *message, int use_msb)
{
    if (!img || !message)
        return -1;

    DIBHeader *dib = &img->dib;

    if (dib->biBitCount != 24 && dib->biBitCount != 32)
    {
        fprintf(stderr, "embed_message: only 24-bit or 32-bit BMP supported\n");
        return -1;
    }

    int bytesPerPixel = dib->biBitCount / 8;
    int width = dib->biWidth;
    int height = (dib->biHeight > 0) ? dib->biHeight : -dib->biHeight; // handle top-down or bottom-up
    size_t rowBytes = row_size(dib);

    // size_t totalColorBytes = rowBytes * (size_t)height;

    // We will only embed in the actual color bytes; avoid embedding into padding (optional)
    // Strategy: iterate rows, for each pixel use bytesPerPixel bytes, skip any padding at row end.

    uint32_t msg_len = (uint32_t)strlen(message);
    // number of bits to embed = 32 (length) + msg_len * 8
    uint64_t bits_needed = 32ULL + (uint64_t)msg_len * 8ULL;

    // compute available bytes for embedding: width * height * bytesPerPixel
    uint64_t usable_bytes = (uint64_t)width * (uint64_t)height * (uint64_t)bytesPerPixel;
    if (bits_needed > usable_bytes)
    {
        fprintf(stderr, "embed_message: not enough capacity. need %llu bytes, have %llu bytes\n",
                (unsigned long long)bits_needed, (unsigned long long)usable_bytes);
        return -1;
    }

    // Prepare buffer: 4 bytes length (little endian) then message bytes
    uint8_t *payload = (uint8_t *)malloc(4 + msg_len);
    if (!payload)
        return -1;
    payload[0] = (uint8_t)(msg_len & 0xFF);
    payload[1] = (uint8_t)((msg_len >> 8) & 0xFF);
    payload[2] = (uint8_t)((msg_len >> 16) & 0xFF);
    payload[3] = (uint8_t)((msg_len >> 24) & 0xFF);
    memcpy(payload + 4, message, msg_len);
    uint64_t total_bits = bits_needed;

    // iterate image bytes in pixel order. BMP stores rows; if biHeight positive -> bottom-up rows.
    unsigned char *data = img->data;
    uint64_t bit_index = 0; // index into payload bits (0..total_bits-1)

    for (int row = 0; row < height && bit_index < total_bits; ++row)
    {
        // destination row index depends on bottom-up vs top-down
        int src_row = (dib->biHeight > 0) ? (height - 1 - row) : row; // convert to data row index
        unsigned char *row_ptr = data + src_row * rowBytes;

        // iterate pixels in this row
        for (int col = 0; col < width && bit_index < total_bits; ++col)
        {
            unsigned char *px = row_ptr + col * bytesPerPixel; // B, G, R (, A) order
            for (int c = 0; c < bytesPerPixel && bit_index < total_bits; ++c)
            {
                uint64_t byte_idx = bit_index / 8;
                int bit_in_byte = 7 - (bit_index % 8); // we'll take bits MSB-first inside each payload byte
                int bit_val = (payload[byte_idx] >> bit_in_byte) & 0x1;
                set_bit_in_byte(&px[c], bit_val, use_msb);
                bit_index++;
            }
        }
        // padding bytes at row end (rowBytes - width*bytesPerPixel) are left untouched
    }

    free(payload);

    if (bit_index != total_bits)
    {
        fprintf(stderr, "embed_message: internal error: embedded %llu bits, expected %llu\n",
                (unsigned long long)bit_index, (unsigned long long)total_bits);
        return -1;
    }

    return 0;
}

// Extract message previously embedded with embed_message.
// Returns a newly-allocated C-string (null-terminated) on success (caller must free with free_message),
// or NULL on error.
char *extract_message(BMPImage *img, int use_msb)
{
    if (!img)
        return NULL;

    // We can only do it with 24 or 32 bit BMP
    DIBHeader *dib = &img->dib;
    if (dib->biBitCount != 24 && dib->biBitCount != 32)
    {
        fprintf(stderr, "extract_message: only 24-bit or 32-bit BMP supported\n");
        return NULL;
    }

    // Calculating nessesary information
    int bytesPerPixel = dib->biBitCount / 8;
    int width = dib->biWidth;
    int height = (dib->biHeight > 0) ? dib->biHeight : -dib->biHeight;
    size_t rowBytes = row_size(dib);
    uint64_t usable_bytes = (uint64_t)width * (uint64_t)height * (uint64_t)bytesPerPixel;

    unsigned char *data = img->data;
    uint64_t bit_index = 0;

    // First extract 32 bits for length
    uint8_t len_bytes[4] = {0, 0, 0, 0};
    for (int row = 0; row < height && bit_index < 32; ++row)
    {
        // We will extract the length of the message first
        int src_row = (dib->biHeight > 0) ? (height - 1 - row) : row;
        unsigned char *row_ptr = data + src_row * rowBytes;
        for (int col = 0; col < width && bit_index < 32; ++col)
        {
            unsigned char *px = row_ptr + col * bytesPerPixel;
            for (int c = 0; c < bytesPerPixel && bit_index < 32; ++c)
            {
                int out_byte = (int)(bit_index / 8);
                int out_bitpos = 7 - (bit_index % 8);
                int bit_val = get_bit_from_byte(px[c], use_msb);
                len_bytes[out_byte] |= (bit_val << out_bitpos);
                bit_index++;
            }
        }
    }

    if (bit_index < 32)
    {
        fprintf(stderr, "extract_message: image too small to contain length header\n");
        return NULL;
    }

    // Calculating the length of the message =
    uint32_t msg_len = (uint32_t)len_bytes[0] | ((uint32_t)len_bytes[1] << 8) |
                       ((uint32_t)len_bytes[2] << 16) | ((uint32_t)len_bytes[3] << 24);

    printf("Extracted length = %u\n", msg_len);

    // guard against absurd lengths
    uint64_t bits_needed = (uint64_t)msg_len * 8ULL;
    if (bits_needed > usable_bytes - 32ULL)
    {
        fprintf(stderr, "extract_message: declared message length %u exceeds capacity\n", msg_len);
        return NULL;
    }

    // Allocating space for msg
    char *msg = (char *)malloc((size_t)msg_len + 1);
    if (!msg)
        return NULL;
    memset(msg, 0, msg_len + 1);

    // uint64_t total_bits_to_read = bits_needed;
    uint64_t read_bits = 0;

    // Continue reading from where we left off (bit_index == 32)
    // Reset counter to read message bits only
    read_bits = 0;
    uint64_t payload_bits = (uint64_t)msg_len * 8ULL;

    // Start reading AFTER first 32 bits
    uint64_t skip_bits = 32;
    uint64_t total_bits_seen = 0;

    // Extracting the msg itself
    for (int row = 0; row < height && read_bits < payload_bits; ++row)
    {
        int src_row = (dib->biHeight > 0) ? (height - 1 - row) : row;
        unsigned char *row_ptr = data + src_row * rowBytes;
        for (int col = 0; col < width && read_bits < payload_bits; ++col)
        {
            unsigned char *px = row_ptr + col * bytesPerPixel;
            for (int c = 0; c < bytesPerPixel && read_bits < payload_bits; ++c)
            {
                if (total_bits_seen < skip_bits)
                {
                    total_bits_seen++;
                    continue;
                }
                int out_byte = (int)(read_bits / 8);
                int out_bitpos = 7 - (read_bits % 8);
                int bit_val = get_bit_from_byte(px[c], use_msb);
                if (bit_val)
                    msg[out_byte] |= (1 << out_bitpos);
                read_bits++;
                total_bits_seen++;
            }
        }
    }

    // null terminate
    msg[msg_len] = '\0';

    return msg;
}

// Function that deletes message from the heap
void free_message(char *msg)
{
    free(msg);
}

// Function that resizes the BMP image
BMPImage *resize_bmp(const BMPImage *src, int new_width, int new_height)
{
    if (!src || new_width <= 0 || new_height <= 0)
        return NULL;

    // Getting info about the image
    int src_width = src->dib.biWidth;
    int src_height = (src->dib.biHeight > 0) ? src->dib.biHeight : -src->dib.biHeight;
    int bpp = src->dib.biBitCount / 8;
    if (bpp < 3)
    {
        fprintf(stderr, "resize_bmp: only supports 24/32-bit images\n");
        return NULL;
    }

    // Allocate new BMP
    BMPImage *dst = (BMPImage *)malloc(sizeof(BMPImage));
    if (!dst)
        return NULL;
    memcpy(&dst->header, &src->header, sizeof(BMPHeader));
    memcpy(&dst->dib, &src->dib, sizeof(DIBHeader));

    // Fill in the header
    dst->dib.biWidth = new_width;
    dst->dib.biHeight = (src->dib.biHeight > 0) ? new_height : -new_height;

    int row_size_src = ((src_width * bpp + 3) / 4) * 4;
    int row_size_dst = ((new_width * bpp + 3) / 4) * 4;
    dst->dib.biSizeImage = row_size_dst * new_height;
    dst->header.bfSize = dst->dib.biSizeImage + dst->header.bfOffBits;

    dst->data = (unsigned char *)malloc(dst->dib.biSizeImage);
    if (!dst->data)
    {
        free(dst);
        return NULL;
    }

    // Simple nearest-neighbor scaling
    for (int y = 0; y < new_height; y++)
    {
        int src_y = (y * src_height) / new_height;
        unsigned char *dst_row = dst->data + y * row_size_dst;
        unsigned char *src_row = src->data + src_y * row_size_src;

        for (int x = 0; x < new_width; x++)
        {
            int src_x = (x * src_width) / new_width;
            unsigned char *dst_px = dst_row + x * bpp;
            unsigned char *src_px = src_row + src_x * bpp;
            memcpy(dst_px, src_px, bpp);
        }
    }

    return dst;
}

// Function that crops the BMP image
BMPImage *crop_bmp(const BMPImage *src, int x, int y, int crop_width, int crop_height)
{
    if (!src || crop_width <= 0 || crop_height <= 0)
        return NULL;

    int src_width = src->dib.biWidth;
    int src_height = (src->dib.biHeight > 0) ? src->dib.biHeight : -src->dib.biHeight;
    int bpp = src->dib.biBitCount / 8;
    if (bpp < 3)
    {
        fprintf(stderr, "crop_bmp: only supports 24/32-bit images\n");
        return NULL;
    }

    // Bounds check
    if (x < 0 || y < 0 || x + crop_width > src_width || y + crop_height > src_height)
    {
        fprintf(stderr, "crop_bmp: crop rectangle out of bounds\n");
        return NULL;
    }

    // Allocate new BMP
    BMPImage *dst = (BMPImage *)malloc(sizeof(BMPImage));
    if (!dst)
        return NULL;
    memcpy(&dst->header, &src->header, sizeof(BMPHeader));
    memcpy(&dst->dib, &src->dib, sizeof(DIBHeader));

    dst->dib.biWidth = crop_width;
    dst->dib.biHeight = (src->dib.biHeight > 0) ? crop_height : -crop_height;

    // Filling in the header
    int row_size_src = ((src_width * bpp + 3) / 4) * 4;
    int row_size_dst = ((crop_width * bpp + 3) / 4) * 4;
    dst->dib.biSizeImage = row_size_dst * crop_height;
    dst->header.bfSize = dst->dib.biSizeImage + dst->header.bfOffBits;

    dst->data = (unsigned char *)malloc(dst->dib.biSizeImage);
    if (!dst->data)
    {
        free(dst);
        return NULL;
    }

    // Copy pixels row by row
    for (int row = 0; row < crop_height; row++)
    {
        unsigned char *dst_row = dst->data + row * row_size_dst;
        unsigned char *src_row = src->data + (y + row) * row_size_src + x * bpp;
        memcpy(dst_row, src_row, crop_width * bpp);
    }

    return dst;
}