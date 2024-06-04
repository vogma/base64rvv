#include <libb64rvv.h>
#include <base64.h>

#define RUN_SIZE 1

void run_base64_decode()
{
    struct timespec start, end;
    uint64_t timeElapsed_scalar;

    // const char *base64_data = "QUJDREVGR2FiY2RlZmcxMjM0NTY3";
    // const char *base64_data = "MTIzNDU2NysvQUJDREVGR0hhYmNkZWZnaGlqa2w=";

    // const int a[run_size] = {2048, 4096, 8192, 1000000, 2000000, 4000000, 8000000, 16000000, 32000000, 64000000};
    const int a[RUN_SIZE] = {900};

    for (int i = 0; i < RUN_SIZE; i++)
    {

        int N = a[i];

        printf("running with %d bytes\n", N);

        char *base64_data = setupDecodingData(N);

        // input
        for (int i = 0; i < 70; i++)
        {
            printf("%c ", base64_data[i]);
        }
        printf("\n\n");

        size_t output_length = 0;
        size_t output_length_rvv = 0;

        int8_t *output_scalar = (int8_t *)malloc(N * sizeof(uint8_t));
        int8_t *output_rvv = (int8_t *)malloc(N * sizeof(uint8_t));

        build_decoding_table();

        output_scalar = (int8_t *)base64_decode((const unsigned char *)base64_data, N, &output_length);
        // unsigned char *decoded = base64_decode(base64_data, 28, &output_length);

        // measure scalar code
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        output_scalar = (int8_t *)base64_decode((const unsigned char *)base64_data, N, &output_length);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        timeElapsed_scalar = timespecDiff(&end, &start);
        printf("base64_scalar time (qs): %ld\n", timeElapsed_scalar / 1);

        base64_decode_rvv(base64_data, output_rvv, N, &output_length_rvv);

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        base64_decode_rvv(base64_data, output_rvv, N, &output_length_rvv);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        timeElapsed_scalar = timespecDiff(&end, &start);
        printf("base64_rvv time (qs): %ld\n", timeElapsed_scalar / 1);

        printf("--------------------\n");

        checkResults((uint8_t *)output_scalar, (uint8_t *)output_rvv, (N / 4) * 3);

        free(base64_data);
        free(output_rvv);
        free(output_scalar);
    }

    base64_cleanup();
}

#define N 900

void run_base64_encode(void)
{
    struct timespec start, end;
    uint64_t timeElapsed_scalar, timeElapsed_vector;

    char *inputData = setupEncodingData(N);

    int encode_length = Base64encode_len(N * sizeof(char));

    uint8_t *output_scalar = (uint8_t *)malloc(sizeof(uint8_t) * encode_length);
    uint8_t *output_vector = (uint8_t *)malloc(sizeof(uint8_t) * encode_length);

    Base64encode((char *)output_scalar, inputData, N);

    // measure scalar code
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    Base64encode((char *)output_scalar, inputData, N);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    timeElapsed_scalar = timespecDiff(&end, &start);
    printf("base64_scalar time: %ld\n", timeElapsed_scalar / 1);

    base64_encode_rvv((uint8_t *)inputData, output_vector, N);
    // base64_encode_asm((uint8_t *)inputData, (char *)output_vector, offsets, gather_index_lmul4, &length);

    // measure vector code
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    base64_encode_rvv((uint8_t *)inputData, output_vector, N);
    // base64_encode_asm((uint8_t *)inputData, (char *)output_vector, offsets, gather_index_lmul4, &length);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    timeElapsed_vector = timespecDiff(&end, &start);
    printf("base64_vector time: %ld\n", timeElapsed_vector / 1);

    // float speedup = ((float)timeElapsed_scalar / (float)timeElapsed_vector);
    // printf("speedup %.02f %%\n", speedup * 100);

    for (int i = 0; i < 70; i++)
    {
        printf("%c", output_scalar[i]);
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
