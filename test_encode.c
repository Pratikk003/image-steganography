#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>

int main(int argc, char *argv[])
{
    EncodeInfo E1;
    DecodeInfo D1;
    // Validate command line argument (CLA)
    int res = check_operation_type(argv);

    if (res == e_encode)
    {
        printf("Encoding is selected\n");
       if(read_and_validate_encode_args(argv, &E1 ) == e_success)
       {
            printf("Info : Read and validate encode args is success\n");
            if (open_files(&E1) == e_success)
            {
                printf("Info : Files are opened successfully\n");
                if(do_encoding(&E1) == e_success)
                {
                    printf("INFO : Encoding is success\n");
                }
                else
                {
                    printf("Failed to encode\n");
                }
            }
            else
            {
                printf("Open Files is failure");
                return 0;
            }
            
        }
       else
       {
            printf("Read and validate encode args is failure\n");
            return 0;
       }
    }
    else if (res == e_decode)
    {
        printf("Decoding is selected\n");
        if (read_and_validate_decode_args(argv, &D1) == e_success)
        {
            printf("Info : Read and validate decode args is success\n");
            if (open_files_decode(&D1) == e_success)
            {
                printf("Info : Files are opened successfully\n");
                if (do_decoding(&D1) == e_success)
                {
                    printf("INFO : Decoding is success\n");
                }
                else
                {
                    printf("Failed to decode\n");
                }
            }
            else
            {
                printf("Open Files is failure");
                return 0;
            }
        }
        else
        {
            printf("Read and validate decode args is failure\n");
            return 0;
        }
    }
    else
    {
        printf("Invalid option\n");
        printf("For Encoding: ./a.out -e beautiful.bmp secret.txt [stego.bmp]\n");
        printf("For Decoding: ./a.out -d beautiful.bmp secret.txt [stego.bmp]\n");
    }

    return 0;
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1] , "-e") == 0 )
        return e_encode;
    else if ( strcmp(argv[1] , "-d") == 0)
        return e_decode;
    else
		return e_unsupported;
}

