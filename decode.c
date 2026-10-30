#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"
/* Function Definitions */

/*
 * Read and validate Decode args from argv
 * Input: argv from main
 * Output: DecodeInfo populated with file names
 * Return Value: e_success or e_failure
 */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // stego image (input)
    if (strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    // output file name is optional, decided after decoding extn if not given
    if (argv[3] != NULL)
    {
        decInfo->output_fname = argv[3];
    }
    else
    {
        decInfo->output_fname = NULL;
    }

    return e_success;
}

/*
 * Get File pointer for stego image (i/p)
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files_decode(DecodeInfo *decInfo)
{
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/* Decode a byte from LSB of 8 image bytes */
char decode_byte_from_lsb(char *image_buffer)
{
    char data = 0;

    for (int i = 0; i < 8; i++)
    {
        data = data | ((image_buffer[i] & 1) << i); // 7...43210
    }

    return data;
}

/* Read size (8*size) bytes of rgb data and decode into data[0..size-1] */
Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo)
{
    for (int i = 0; i < size; i++)
    {
        // read 8 bytes of rgb data
        fread(decInfo->image_data, 8, 1, decInfo->fptr_stego_image);
        // lsb to byte
        data[i] = decode_byte_from_lsb(decInfo->image_data);
    }

    return e_success;
}

Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
    int len = strlen(magic_string);
    char str[len + 1];

    decode_data_from_image(str, len, decInfo);
    str[len] = '\0';

    if (strcmp(str, magic_string) == 0)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/* Read 32 bytes of rgb data, decode lsb of each to rebuild an int */
int decode_size_from_lsb(DecodeInfo *decInfo)
{
    char str[32];
    int size = 0;

    fread(str, 32, 1, decInfo->fptr_stego_image);

    for (int i = 0; i < 32; i++)
    {
        size = size | ((str[i] & 1) << i); // 7...43210
    }

    return size;
}

Status decode_secret_file_extn_size(int *size, DecodeInfo *decInfo)
{
    *size = decode_size_from_lsb(decInfo);
    return e_success;
}

Status decode_secret_file_extn(int size, DecodeInfo *decInfo)
{
    decode_data_from_image(decInfo->extn_secret_file, size, decInfo);
    decInfo->extn_secret_file[size] = '\0';
    return e_success;
}

Status decode_secret_file_size(long *file_size, DecodeInfo *decInfo)
{
    *file_size = decode_size_from_lsb(decInfo);
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char data[decInfo->size_secret_file];

    decode_data_from_image(data, decInfo->size_secret_file, decInfo);
    fwrite(data, decInfo->size_secret_file, 1, decInfo->fptr_output);

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    // Skip 54 byte bmp header
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    if (decode_magic_string(MAGIC_STRING, decInfo) == e_success)
    {
        printf("Info : Decoded magic string successfully\n");

        if (decode_secret_file_extn_size(&(decInfo->extn_size), decInfo) == e_success)
        {
            printf("Info : Decoded secret file extn size successfully\n");

            if (decode_secret_file_extn(decInfo->extn_size, decInfo) == e_success)
            {
                printf("Info : Decoded secret file extn successfully : %s\n", decInfo->extn_secret_file);

                // If user did not pass an output file name, build default one
                if (decInfo->output_fname == NULL)
                {
                    char name[100];
                    strcpy(name, "output");
                    strcat(name, decInfo->extn_secret_file);
                    decInfo->output_fname = strdup(name);
                }

                decInfo->fptr_output = fopen(decInfo->output_fname, "w");
                if (decInfo->fptr_output == NULL)
                {
                    perror("fopen");
                    fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_fname);
                    return e_failure;
                }

                if (decode_secret_file_size(&(decInfo->size_secret_file), decInfo) == e_success)
                {
                    printf("Info : Decoded secret file size successfully\n");

                    if (decode_secret_file_data(decInfo) == e_success)
                    {
                        printf("Info : Decoded secret file data successfully\n");
                        printf("Info : Decoded data written to %s\n", decInfo->output_fname);
                    }
                    else
                    {
                        printf("Failed to decode secret file data\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Failed to decode secret file size\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Failed to decode secret file extn\n");
                return e_failure;
            }
        }
        else
        {
            printf("Failed to decode secret file extn size\n");
            return e_failure;
        }
    }
    else
    {
        printf("ERROR: Magic string does not match. Not a valid stego image\n");
        return e_failure;
    }

    return e_success;
}
