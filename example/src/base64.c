#include <base64.h>

int main(int argc, char *argv[])
{
    char input[] = "Hallo Welt";
    char output[20];
    base64_encode_rvv_m1_asm((uint8_t*)input, output, 11);

    printf("0x%02X\n", input[0]);
    printf("%d\n", output[0]);

    // void base64_encode_rvv_m1_asm(uint8_t *input, char *output, size_t length);

    // struct arguments args = {0};
    // parse_arguments(argc, argv, &args);

    // if (args.printHelp)
    // {
    //     print_help();
    //     return 0;
    // }

    // readInputFile(&args);

    // if (args.input_data == NULL)
    // {
    //     return 0;
    // }

    // size_t encoded_length = base64_encoded_length(args.file_size);

    // char *base64_data = malloc(encoded_length);

    // base64_encode_rvv_m4(args.input_data, base64_data, args.file_size);

    // if (base64_data != NULL)
    // {
    //     for (int i = 0; i < encoded_length; i++)
    //     {
    //         printf("%c", base64_data[i]);
    //     }
    // }
    // printf("\n");

    // printf("%ld\n", encoded_length);

    // free(args.input_data);
    // free(base64_data);
    return 0;
}
