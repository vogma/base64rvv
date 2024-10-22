#include <base64.h>

#define RUN_SIZE 1

#define TEST_DATA "VGhlIGZpcnN0IG1lcmdlIG9mIGFkamFjZW50IGJ5dGVzIGlzIHBlcmZvcm1lZCBieSBpbnN0cnVjdGlvbiBwbWFkZHVic3cuIFRoZSBpbnN0cnVjdGlvbiB2ZXJ0aWNhbGx5IG11bHRpcGxpZXMgc2lnbmVkIGJ5dGVzIHlpZWxkaW5nIDE2LWJpdCBzaWduZWQgaW50ZXJtZWRpYXRlIHZhbHVlcyBhbmQgdGhlbiB0aGUgdmFsdWVzIGFyZSBhZGRlZC4gSW5wdXQgdmFsdWVzIGFyZSA2LWJpdCwgc28gb2J2aW91c2x5IGFyZSBub24tbmVnYXRpdmUgYW5kIHRoZSBpbnN0cnVjdGlvbiBjb3VsZCBiZSBzYWZlbHkgdXNlZC4KClRoZSBpbnN0cnVjdGlvbiBwbWFkZHVzYncgaXMgdXNlZCB0byBwZXJmb3JtIHNoaWZ0ICYgYml0LW9yIGluIGEgc2luZ2xlIHN0ZXAuCgpUaGVuIG1lcmdpbmcgMTItYml0IGZpZWxkcyBpcyBkb25lIGJ5IGFub3RoZXIgbXVsdGlwbHktYWRkIGluc3RydWN0aW9uIHBtYWRkd2QuIFRoZSBpbnN0cnVjdGlvbiBhbHNvIG9wZXJhdGVzIG9uIHNpZ25lZCB2YWx1ZXMsIGJ1dCBhZ2FpbiDigJQgdGhlIGlucHV0cyBhcmUgbmV2ZXIgbmVnYXRpdmUu"
#define TEST_LENGTH 728

#define N 1048576

int main(int argc, char *argv[])
{
    struct arguments args = {0};
    parse_arguments(argc, argv, &args);

    if (args.printHelp)
    {
        print_help();
        return 0;
    }

    printf("input path: %s\n", args.input_path);
    printf("output path: %s\n", args.output_path);

    readInputFile(&args);

    size_t encoded_length = base64_encoded_length(args.file_size);

    char *base64_data = malloc(encoded_length);

    base64_encode_rvv_m1(args.input_data, (uint8_t *)base64_data, args.file_size);

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
