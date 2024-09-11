#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include <time.h>
#include <libb64rvv.h>


uint8_t *setupEncodingData(size_t size)
{

    srand(time(NULL));

    uint8_t *inputData = (uint8_t *)malloc(sizeof(uint8_t) * size);

    for (int i = 0; i < size; i++)
    {
        inputData[i] = rand() % (255 + 1 - 0) + 0;
    }

    return inputData;
}

char *setupDecodingData(size_t size)
{
    // char alphabet[26] = "MTIzNDU2NysvQUJDREVGR0g=";
    char alphabet[900] = "CgpIaSBldmVyeW9uZSwgaSByZWNlbnRseSBzdGFydGVkIHN0dWR5aW5nIHRoZSBSSVNDLVYgYXJjaGl0ZWN0dXJlLCBhbmQgbWFuYWdlZCB0byBtYWtlIG15IG93biAzMmJpdCB2ZXJzaW9uIGluIGEgZ2FtZSBjYWxsZWQgVHVyaW5nIGNvbXBsZXRlLiBUaGUgc3lzdGVtIGlzIGFibGUgdG8gZXhlY3V0ZSBldmVyeSBpbnN0cnVjdGlvbiBvZiB0aGUgYmFzZSBtb2R1bGVzLCBub3cgdGhhdCBpIHdhbnQgdG8gdHJ5IGFuZCBhZGQgc3VwcG9ydCBmb3IgZmxvYXRpbmcgcG9pbnQgbnVtYmVycywgaSdtIHN0dWNrIHdpdGggYSByZWFsbHkgc3R1cGlkIHF1ZXN0aW9uPwoKSSBhZGRlZCAzMiBzZXBhcmF0ZSByZWdpc3RlcnMgZm9yIHN0b3JpbmcgZmxvYXRzLCBhbmQgYW4gZW5jb2RlciBmb3IgdGhlIElFRUUtNzU0IGZvcm1hdC4gYnV0IGlmIGkgdXNlIHNvbWV0aGluZyBsaWtlCgpsaSB0MCwgNjU0MzIxCgpmY3Z0LnMudyBmdDAsIHQwCgpmdDAgd2lsbCBiZSBzZXQgdG8gNjU0MzIxLjAgKElFRUUgZW5jb2RlZCkKCkhlcmUgY29tZXMgdGhlIHN0dXBpZCBxdWVzdGlvbi4uLiBob3cgZG8gaSBwdXQgc3R1ZmYgYWZ0ZXIgdGhlIGRvdD8gZXZlcnkgbnVtYmVyIGkgY29udmVydCB3aWxsIGJlIGp1c3Qgbi4wCgpob3cgY2FuIGkgc2V0IGZ0MCB0byBzb21ldGhpbmcgbGlrZSAwLjYyIG9yIDEuND8=";

    char *inputData = (char *)malloc(sizeof(char) * size);

    for (int i = 0; i < size; i++)
    {
        inputData[i] = alphabet[i % 26];
    }

    return inputData;
}

void checkResults(uint8_t *output_scalar, uint8_t *output_vector, size_t length)
{
    size_t error = 0;
    for (int i = 0; i < length; i++)
    {
        if (output_scalar[i] != output_vector[i])
        {
            printf("Error at index %d! vector is 0x%02X scalar is 0x%02X\n", i, output_vector[i], output_scalar[i]);
            error = 1;
            break;
        }
    }
    if (!error)
    {
        printf("No Errors\n");
    }
}
