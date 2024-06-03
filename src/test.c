#include <libb64rvv.h>

void checkResults(int8_t *output_scalar, int8_t *output_vector, size_t length)
{
    size_t error = 0;
    for (int i = 0; i < length; i++)
    {
        if (output_scalar[i] != output_vector[i])
        {
            printf("Error at index %d! scalar is 0x%02X, vector is 0x%02X \n", i, output_scalar[i], output_vector[i]);
            error = 1;
            break;
        }
    }
    if (!error)
    {
        printf("No Errors\n");
    }
}

int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
    return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

char *setupInputData(int N)
{
    // char alphabet[26] = "MTIzNDU2NysvQUJDREVGR0g=";
    char alphabet[900] = "CgpIaSBldmVyeW9uZSwgaSByZWNlbnRseSBzdGFydGVkIHN0dWR5aW5nIHRoZSBSSVNDLVYgYXJjaGl0ZWN0dXJlLCBhbmQgbWFuYWdlZCB0byBtYWtlIG15IG93biAzMmJpdCB2ZXJzaW9uIGluIGEgZ2FtZSBjYWxsZWQgVHVyaW5nIGNvbXBsZXRlLiBUaGUgc3lzdGVtIGlzIGFibGUgdG8gZXhlY3V0ZSBldmVyeSBpbnN0cnVjdGlvbiBvZiB0aGUgYmFzZSBtb2R1bGVzLCBub3cgdGhhdCBpIHdhbnQgdG8gdHJ5IGFuZCBhZGQgc3VwcG9ydCBmb3IgZmxvYXRpbmcgcG9pbnQgbnVtYmVycywgaSdtIHN0dWNrIHdpdGggYSByZWFsbHkgc3R1cGlkIHF1ZXN0aW9uPwoKSSBhZGRlZCAzMiBzZXBhcmF0ZSByZWdpc3RlcnMgZm9yIHN0b3JpbmcgZmxvYXRzLCBhbmQgYW4gZW5jb2RlciBmb3IgdGhlIElFRUUtNzU0IGZvcm1hdC4gYnV0IGlmIGkgdXNlIHNvbWV0aGluZyBsaWtlCgpsaSB0MCwgNjU0MzIxCgpmY3Z0LnMudyBmdDAsIHQwCgpmdDAgd2lsbCBiZSBzZXQgdG8gNjU0MzIxLjAgKElFRUUgZW5jb2RlZCkKCkhlcmUgY29tZXMgdGhlIHN0dXBpZCBxdWVzdGlvbi4uLiBob3cgZG8gaSBwdXQgc3R1ZmYgYWZ0ZXIgdGhlIGRvdD8gZXZlcnkgbnVtYmVyIGkgY29udmVydCB3aWxsIGJlIGp1c3Qgbi4wCgpob3cgY2FuIGkgc2V0IGZ0MCB0byBzb21ldGhpbmcgbGlrZSAwLjYyIG9yIDEuND8=";

    char *inputData = (char *)malloc(sizeof(char) * N);

    for (int i = 0; i < N; i++)
    {
        inputData[i] = alphabet[i % 900];
    }

    return inputData;
}

#define RUN_SIZE 1

int main(void)
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

        char *base64_data = setupInputData(N);

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
        printf("base64_scalar time: %ld\n", timeElapsed_scalar / 1000);

        base64_decode_rvv(base64_data, output_rvv, N, &output_length_rvv);

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        base64_decode_rvv(base64_data, output_rvv, N, &output_length_rvv);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        timeElapsed_scalar = timespecDiff(&end, &start);
        printf("base64_rvv time: %ld\n", timeElapsed_scalar / 1000);

        printf("--------------------\n");

        // for (int i = 0; i < 70; i++)
        // {
        //     printf("%c", output_scalar[i]);
        // }
        // printf("\n");

        // for (int i = 0; i < 900; i++)
        // {

        //     printf("%c", output_rvv[i]);
        // }
        // printf("\n");

        // printf("--------------------\n");

        // hex output
        // for (int i = 0; i < 70; i++)
        // {
        //     printf("0x%02X ", output_scalar[i]);
        // }
        // printf("\n");

        // for (int i = 0; i < 70; i++)
        // {
        //     printf("0x%02X ", output_rvv[i]);
        // }
        // printf("\n");

        checkResults(output_scalar, output_rvv, (N / 4) * 3);

        free(base64_data);
        free(output_rvv);
        free(output_scalar);
    }

    base64_cleanup();
    return 0;
}
