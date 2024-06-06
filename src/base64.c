#include <libb64rvv.h>
#include <base64.h>
#include "b64/cencode.h"

#define RUN_SIZE 22

void run_base64_decode()
{
    struct timespec start, end;
    uint64_t timeElapsed_scalar;

    // const char *base64_data = "QUJDREVGR2FiY2RlZmcxMjM0NTY3";
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
                             1048576,
                             2097152,
                             4194304,
                             8388608,
                             16777216,
                             33554432,
                             67108864};
    // const int a[RUN_SIZE] = {65536};

    for (int i = 0; i < RUN_SIZE; i++)
    {

        int N = a[i];

        printf("running with %d bytes\n", N);

        char *base64_data = setupDecodingData(N);
        // char *base64_data = "TXkgaHVzYmFuZCBoYWQgYmVlbiByZWFsbHkgYmFkbHkgaHVydCBieSBoaXMgZXguIFNoZSBjaGVhdGVkIG9uIGhpbSBhbmQgdHVybmVkIGhpcyB3b3JsZCB1cHNpZGUgZG93bi4gV2hlbiBoZSBzdGFydGVkIGRhdGluZyBhZ2FpbiwgaGUgdGhvdWdodCBoZSB3YXMgdG90YWxseSBvdmVyIGl0LCBidXQgZGlkbid0IHJlYWxpemUgaGUgd2FzIHJlc2VudGZ1bCB0b3dhcmQgd29tZW4uIEkgYWxzbyB3YXNuJ3QgaGlzIGV4YWN0ICJ0eXBlIiBiYXNlZCBvbiB0aGUgd29tZW4gaGUnZCBkYXRlZCBpbiB0aGUgcGFzdC4gRXZlcnl0aGluZyBmZWx0IGxpa2UgYSB0ZXN0LS1saWtlLCBpZiBJIHJlc3BvbmQgdGhpcyB3YXksIGhvdyB3aWxsIHNoZSByZWFjdD8gSXQgd2FzIGV4aGF1c3RpbmcgZm9yIG1lIGF0IHRpbWVzLiBZZXQsIHRoZXJlIHdhcyBzb21ldGhpbmcgYWJvdXQgaGltIHRoYXQga2VwdCBtZSBoYW5naW5nIGluIHRoZXJlOyBJJ2QgZ2V0IGEgZ2xpbXBzZSBub3cgYW5kIHRoZW4gb2YgdGhlIHN3ZWV0IGd1eSBoZSB3YXMgYmVmb3JlIGNyYXBweSBzdHVmZiBoYXBwZW5lZCB0byBoaW0gYW5kIGhlIGxvc3QgZmFpdGggaW4gcmVsYXRpb25zaGlwcy4gRXZlbiBzbywgSSBzdGFydGVkIHRvIGZlZWwgbGlrZSBoZSB3YXMgc2V0dGxpbmcgZm9yIG1lIGJlY2F1c2UgSSB3YXMgcGF0aWVudCwgZGlkbid0IGJhY2sgYXdheSB3aGVuIGhlIGxhc2hlZCBvdXQgZW1vdGlvbmFsbHksIHRvb2sgY2FyZSBvZiBoaW0gYW5kIHdhcyBhIGZhaXRoZnVsIGdpcmxmcmllbmQuIFRoZSByZWxhdGlvbnNoaXAgc2VlbWVkIHZlcnkgY2VyZWJyYWwgaW4gdGhhdCBhc3BlY3QsIGFzIHRob3VnaCBJIHdhcyB0aGUgb25seSBsb2dpY2FsIGNob2ljZS4gSXQgdG9vayBhIGxvbmcgdGltZSBmb3IgaGltIHRvIHByb3Bvc2UgYW5kIG9uIG91ciB3ZWRkaW5nIGRheSBoZSB0b2xkIG91ciBndWVzdHMgdGhhdCBJJ2QgY2hhbmdlZCBoaW0gYW5kIHNhdmVkIGhpbS4gV2hpbGUgdGhhdCB3YXMgdG90YWxseSBiZWF1dGlmdWwsIGl0IHN0aWxsIG1hZGUgbWUgd29uZGVyIGluIHRoZSBiYWNrIG9mIG15IG1pbmQgaWYgaGUgd2FzIHRoaW5raW5nLCAiU2hlJ3MgZ29vZCB0byBtZSBhbmQgdGhhdCdzIGJldHRlciB0aGFuIHdoYXQgaGFwcGVuZWQgaW4gdGhlIHBhc3QsIHNvIGlmIG5vdGhpbmcgZWxzZSwgc2hlJ3MgbG95YWwuIEknbGwga2VlcCBoZXIgYXJvdW5kLiIKCuKAiwoKSXQgd2Fzbid0IHVudGlsIHJpZ2h0IGFmdGVyIEkgaGFkIG91ciBzb24gYW5kIG15IG1vbSB3YXMgdmlzaXRpbmcgdGhhdCBoZSB2b2NhbGl6ZWQgaG93IG11Y2ggaGUgbG92ZWQgbWUgYW5kIGhvdyBoZSB3YXMgdXR0ZXJseSBkZXZvdGVkIHRvIG1lOyB0aGF0IEknZCByZXNjdWVkIGhpbSBmcm9tIGEgbG9uZWx5LCBiaXR0ZXIgZXhpc3RlbmNlLiBIZSB0b2xkIG15IG1vbSB0aGlzIG9uIGEgdHJpcCB0byB0aGUgc3RvcmUgd2hlbiBJIHdhc24ndCBhcm91bmQuIEhlIHdhbnRlZCBoZXIgdG8ga25vdyB0aGF0IGhlIHdhcyBhd2FyZSBzaGUgaGFkIGRvdWJ0cyBhYm91dCBoaW0gYmVjYXVzZSBoZSdkIGJlZW4gc28gbG9zdCBhbmQgc29tZXRpbWVzIHRvb2sgaXQgb3V0IG9uIG1lLiBTaGUgd2FzIGltcHJlc3NlZCB0aGF0IGhlIHdvdWxkIHRha2Ugb3duZXJzaGlwIG9mIGl0IHdoaWxlIHNvIGNsZWFybHkgYmVpbmcgaG9uZXN0IGFib3V0IGhpcyB0cnVlIGZlZWxpbmdzIGZvciBtZS4gV2hlbiB0aGV5IGdvdCBob21lLCBteSBtb20gdG9sZCBtZSBldmVyeXRoaW5nLiBJIGRpZG4ndCB0ZWxsIG15IGh1c2JhbmQgSSBrbmV3IHdoYXQgaGUnZCBzYWlkLCBidXQgdGhhdCBuaWdodCBoZSB0aGFua2VkIG1lIGZvciBoYXZpbmcgaGlzIHNvbiBhbmQgZm9yIGdpdmluZyBoaW0gYSBsaWZlIGhlIG5ldmVyIHRob3VnaHQgaGUnZCBoYXZlLiBJIGNyaWVkIGxpa2UgV0hPQS4=";

        size_t output_length = 0;
        size_t output_length_rvv = 0;

        int8_t *output_scalar = (int8_t *)malloc(N * sizeof(uint8_t));
        int8_t *output_rvv = (int8_t *)malloc(N * sizeof(uint8_t));

        output_scalar = (int8_t *)base64_decode((const unsigned char *)base64_data, N, output_scalar, &output_length);

        // measure scalar code
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        output_scalar = (int8_t *)base64_decode((const unsigned char *)base64_data, N, output_scalar, &output_length);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        timeElapsed_scalar = timespecDiff(&end, &start);
        printf("base64_scalar time (qs): %f\n", ((double)timeElapsed_scalar / 1000));

        base64_decode_rvv(base64_data, output_rvv, N, &output_length_rvv);

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        base64_decode_rvv(base64_data, output_rvv, N, &output_length_rvv);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        timeElapsed_scalar = timespecDiff(&end, &start);
        printf("base64_rvv time (qs): %f\n", ((double)timeElapsed_scalar / 1000));

        printf("--------------------\n");

        // for (int i = 0; i < 1910; i++)
        // {
        //     printf("%c", output_rvv[i]);
        // }
        // printf("\n\n");

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

#define N 65536

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
    printf("base64_scalar time: %f\n", ((double)timeElapsed_scalar / 1000));

    base64_encode_rvv((uint8_t *)inputData, output_vector, N);
    // base64_encode_asm((uint8_t *)inputData, (char *)output_vector, offsets, gather_index_lmul4, &length);

    // measure vector code
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    base64_encode_rvv((uint8_t *)inputData, output_vector, N);
    // base64_encode_asm((uint8_t *)inputData, (char *)output_vector, offsets, gather_index_lmul4, &length);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    timeElapsed_vector = timespecDiff(&end, &start);
    printf("base64_vector time: %f\n", ((double)timeElapsed_vector / 1000));

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
