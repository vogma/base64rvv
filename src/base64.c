#include <base64.h>

#define RUN_SIZE 16

#define TEST_DATA "TYjMvW8y9nUjzpYDqXTGR7XYplyPjQUNbaepdGflZbRuMXHdZGdThzbpit9eUCYTKMxvu"
#define TEST_LENGTH 69

void run_base64_decode()
{
    struct timespec start, end;
    uint64_t timeElapsed_scalar;

    // const char *base64_data = "MTIzNDU2NysvQUJDREVGR0hhYmNkZWZnaGlqa2w=";

    const int a[RUN_SIZE] = {32,
                             64,
                             128,
                             256,
                             512,
                             1024,
                             2048,
                             4096,
                             8192,
                             16384,
                             32768,
                             65536,
                             131072,
                             262144,
                             524288,
                             1048576};
    //                          2097152,
    //                          4194304,
    //                          8388608,
    //                          16777216,
    //                          33554432,
    //                          67108864};
    // const int a[RUN_SIZE] = {TEST_LENGTH};

    for (int i = 0; i < RUN_SIZE; i++)
    {

        int N = a[i];

        printf("running with %d bytes\n", N);

        char *base64_data = setupDecodingData(N);
        // char *base64_data = TEST_DATA;

        size_t output_length = 0;
        size_t output_length_rvv = 0;

        int8_t *output_rvv;
        posix_memalign((void *)&output_rvv, 16, N);

        int8_t *output_scalar = (int8_t *)malloc(N * sizeof(uint8_t));
        // int8_t *output_rvv = (int8_t *)malloc(N * sizeof(uint8_t));

        // output_scalar = (int8_t *)base64_decode((const unsigned char *)base64_data, N, output_scalar, &output_length);
        // unsigned int scalar_length = base64_decode_tail((const char *)base64_data, (unsigned int)N, (unsigned char *)output_scalar);

        // size_t scalar_length = DecodeChunk((const char *)base64_data, N, (uint8_t *)output_scalar);

        // measure scalar code
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        // unsigned int scalar_length = base64_decode_tail((const char *)base64_data, (unsigned int)N, (unsigned char *)output_scalar);
        size_t scalar_length = DecodeChunk((const char *)base64_data, N, (uint8_t *)output_scalar);

        // output_scalar = (int8_t *)base64_decode((const unsigned char *)base64_data, N, output_scalar, &output_length);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        timeElapsed_scalar = timespecDiff(&end, &start);
        printf("%ld base64_scalar time (qs): %f\n", scalar_length, ((double)timeElapsed_scalar / 1000));

        // base64_decode_rvv(base64_data, output_rvv, N);

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        size_t rvv_length = base64_decode_rvv(base64_data, output_rvv, N);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        timeElapsed_scalar = timespecDiff(&end, &start);
        printf("%ld base64_rvv time (qs): %f\n",rvv_length, ((double)timeElapsed_scalar / 1000));

        // printf("--------------------\n");

        // for (int i = 0; i < (N / 4) * 3; i++)
        // {
        //     printf("%c", output_rvv[i]);
        // }
        // printf("   %d\n\n", scalar_length);

        // for (int i = 0; i < 20; i++)
        // {
        //     printf("%c", output_scalar[i]);
        // }
        // printf("\n\n");

        checkResults((uint8_t *)output_scalar, (uint8_t *)output_rvv, (N / 4) * 3);

        free(base64_data);
        free(output_rvv);
        free(output_scalar);
    }
}

#define N 65536

void run_base64_encode(void)
{
    struct timespec start, end;
    uint64_t timeElapsed_scalar, timeElapsed_vector;

    uint8_t *inputData = setupEncodingData(N);

    int encode_length = Base64encode_len(N * sizeof(char));

    uint8_t *output_scalar = (uint8_t *)malloc(sizeof(uint8_t) * encode_length);
    uint8_t *output_vector = (uint8_t *)malloc(sizeof(uint8_t) * encode_length);

    Base64encode((char *)output_scalar, (uint8_t *)inputData, N);

    // measure scalar code
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    Base64encode((char *)output_scalar, (uint8_t *)inputData, N);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    timeElapsed_scalar = timespecDiff(&end, &start);
    printf("base64_scalar time: %f\n", ((double)timeElapsed_scalar / 1000));

    base64_encode_rvv(inputData, output_vector, N);
    // base64_encode_asm((uint8_t *)inputData, (char *)output_vector, offsets, gather_index_lmul4, &length);

    // measure vector code
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    base64_encode_rvv(inputData, output_vector, N);
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
