#include <base64.h>

#define RUN_SIZE 1

#define TEST_DATA "VGhlIGZpcnN0IG1lcmdlIG9mIGFkamFjZW50IGJ5dGVzIGlzIHBlcmZvcm1lZCBieSBpbnN0cnVjdGlvbiBwbWFkZHVic3cuIFRoZSBpbnN0cnVjdGlvbiB2ZXJ0aWNhbGx5IG11bHRpcGxpZXMgc2lnbmVkIGJ5dGVzIHlpZWxkaW5nIDE2LWJpdCBzaWduZWQgaW50ZXJtZWRpYXRlIHZhbHVlcyBhbmQgdGhlbiB0aGUgdmFsdWVzIGFyZSBhZGRlZC4gSW5wdXQgdmFsdWVzIGFyZSA2LWJpdCwgc28gb2J2aW91c2x5IGFyZSBub24tbmVnYXRpdmUgYW5kIHRoZSBpbnN0cnVjdGlvbiBjb3VsZCBiZSBzYWZlbHkgdXNlZC4KClRoZSBpbnN0cnVjdGlvbiBwbWFkZHVzYncgaXMgdXNlZCB0byBwZXJmb3JtIHNoaWZ0ICYgYml0LW9yIGluIGEgc2luZ2xlIHN0ZXAuCgpUaGVuIG1lcmdpbmcgMTItYml0IGZpZWxkcyBpcyBkb25lIGJ5IGFub3RoZXIgbXVsdGlwbHktYWRkIGluc3RydWN0aW9uIHBtYWRkd2QuIFRoZSBpbnN0cnVjdGlvbiBhbHNvIG9wZXJhdGVzIG9uIHNpZ25lZCB2YWx1ZXMsIGJ1dCBhZ2FpbiDigJQgdGhlIGlucHV0cyBhcmUgbmV2ZXIgbmVnYXRpdmUu"
#define TEST_LENGTH 728

void run_base64_decode()
{
    struct timespec start, end;
    uint64_t timeElapsed_scalar, timeElapsed_rvv;

    // const char *base64_data = "MTIzNDU2NysvQUJDREVGR0hhYmNkZWZnaGlqa2w=";

    // const int a[RUN_SIZE] = {32,
    //                          64,
    //                          128,
    //                          256,
    //                          512,
    //                          1024,
    //                          2048,
    //                          4096,
    //                          8192,
    //                          16384,
    //                          32768,
    //                          65536,
    //                          131072,
    //                          262144,
    //                          524288,
    //                          1048576};
    const int a[RUN_SIZE] = {TEST_LENGTH};

    for (int i = 0; i < RUN_SIZE; i++)
    {

        int N = a[i];

        printf("running with %d bytes\n", N);

        // char *base64_data = setupDecodingData(N);
        char *base64_data = TEST_DATA;
        // size_t output_length = 0;
        // size_t output_length_rvv = 0;

        int8_t *output_scalar = (int8_t *)malloc(N * sizeof(uint8_t));
        int8_t *output_rvv = (int8_t *)malloc(N * sizeof(uint8_t));

        // output_scalar = (int8_t *)base64_decode((const unsigned char *)base64_data, N, output_scalar, &output_length);

        base64_decode_tail((const char *)base64_data, N, (unsigned char *)output_scalar);
        // unsigned int base64_decode_tail(const char *in, unsigned int inlen, unsigned char *out);

        // measure scalar code
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        base64_decode_tail(base64_data, N, (unsigned char *)output_scalar);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        timeElapsed_scalar = timespecDiff(&end, &start);
        printf("base64_scalar time (qs): %f\n", ((double)timeElapsed_scalar / 1000));

        base64_decode_rvv_m2(base64_data, output_rvv, N);

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        base64_decode_rvv_m2(base64_data, output_rvv, N);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        timeElapsed_rvv = timespecDiff(&end, &start);
        printf("base64_rvv time (qs): %f\n", ((double)timeElapsed_rvv  / 1000));

        printf("--------------------\n");

        for (int i = 0; i < 50; i++)
        {
            printf("0x%02X ", output_rvv[i]);
        }
        printf("\n\n");

        //   for (int i = 674; i < 774; i++)
        // {
        //     printf("0x%02X ", output_scalar[i]);
        // }
        // printf("\n\n");

        checkResults((uint8_t *)output_scalar, (uint8_t *)output_rvv, (N / 4) * 3);

        // free(base64_data);
        free(output_rvv);
        free(output_scalar);
    }
}

#define N 1048576

void run_base64_encode(void)
{
    struct timespec start, end;
    uint64_t timeElapsed_scalar, timeElapsed_vector;

    uint8_t *inputData = setupEncodingData(N);

    int encode_length = Base64encode_len(N * sizeof(char));

    uint8_t *output_scalar = (uint8_t *)malloc(sizeof(uint8_t) * encode_length);
    uint8_t *output_vector = (uint8_t *)malloc(sizeof(uint8_t) * encode_length);

    Base64encode((char *)output_scalar, (const char *)inputData, N);

    // measure scalar code
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    Base64encode((char *)output_scalar, (const char *)inputData, N);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    timeElapsed_scalar = timespecDiff(&end, &start);
    printf("base64_scalar time: %f\n", ((double)timeElapsed_scalar / 1000));

    base64_encode_rvv_m4(inputData, output_vector, N);
    // base64_encode_asm((uint8_t *)inputData, (char *)output_vector, offsets, gather_index_lmul4, &length);

    // measure vector code
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    base64_encode_rvv_m4(inputData, output_vector, N);
    // base64_encode_asm((uint8_t *)inputData, (char *)output_vector, offsets, gather_index_lmul4, &length);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    timeElapsed_vector = timespecDiff(&end, &start);
    printf("base64_vector time: %f\n", ((double)timeElapsed_vector / 1000));

    // float speedup = ((float)timeElapsed_scalar / (float)timeElapsed_vector);
    // printf("speedup %.02f %%\n", speedup * 100);

    for (int i = 0; i < 100; i++)
    {
        printf("0x%02X, ", inputData[i]);
    }
    printf("\n");

    for (int i = 0; i < 70; i++)
    {
        printf("%c", output_vector[i]);
    }
    printf("\n");

    checkResults(output_scalar, output_vector, encode_length);

    free(inputData);
    free(output_scalar);
    free(output_vector);
}

int main(void)
{

    run_base64_decode();
    printf("\n\n----------------------\n\n");
    run_base64_encode();

    return 0;
}
