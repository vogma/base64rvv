#include <base64.h>

int main(int argc, char *argv[])
{
    struct arguments args = {0};
    parse_arguments(argc, argv, &args);

    if (args.printHelp)
    {
        print_help();
        return 0;
    }

    readInputFile(&args);

    if (args.input_data == NULL)
    {
        return 0;
    }

    size_t encoded_length = base64_encoded_length(args.file_size);

    char *base64_data = malloc(encoded_length);

    base64_encode_rvv_m1(args.input_data, base64_data, args.file_size);

    if (base64_data != NULL)
    {
        for (int i = 0; i < encoded_length; i++)
        {
            printf("%c", base64_data[i]);
        }
    }
    printf("\n");

    printf("%ld\n", encoded_length);

    free(args.input_data);
    free(base64_data);
    return 0;
}
