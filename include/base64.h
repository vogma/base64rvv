#include <libb64rvv.h>
#include <getopt.h>

static const struct option long_options[] = {
    {"input_file", required_argument, 0, 'i'},
    {"output_file", required_argument, 0, 'o'},
    {"help", no_argument, 0, 'h'}};

struct arguments
{
    int printHelp;
    char *input_path;
    char *output_path;
    uint8_t *input_data;
    size_t file_size;
};

void print_help()
{
    printf("Help\n");
}

void parse_arguments(int argc, char *argv[], struct arguments *args)
{
    while (1)
    {
        int index = -1;
        int result = getopt_long(argc, argv, "i:o:", long_options, &index);
        if (result == -1)
            break; /* end of list */
        switch (result)
        {
        case 'i':
            args->input_path = optarg;
            break;
        case 'o':
            args->output_path = optarg;
            break;
        case 'h':
            args->printHelp = 1;
            break;
        default: /* unknown */
            break;
        }
    }
}

void readInputFile(struct arguments *args)
{
    FILE *fptr;

    fptr = fopen(args->input_path, "r");

    args->input_data = NULL;

    if (fptr != NULL)
    {
        /* Go to the end of the file. */
        if (fseek(fptr, 0L, SEEK_END) == 0)
        {
            /* Get the size of the file. */
            long bufsize = ftell(fptr);
            if (bufsize == -1)
            { /* Error */
            }

            /* Allocate our buffer to that size. */
            args->input_data = malloc(sizeof(uint8_t) * (bufsize));
            args->file_size = bufsize;

            /* Go back to the start of the file. */
            if (fseek(fptr, 0L, SEEK_SET) != 0)
            { /* Error */
            }

            /* Read the entire file into memory. */
            fread(args->input_data, sizeof(char), bufsize, fptr);
            if (ferror(fptr) != 0)
            {
                fputs("Error reading file", stderr);
            }
            // else
            // {
            //     args->input_data[newLen++] = '\0'; /* Just to be safe. */
            // }
        }
        fclose(fptr);
    }
    else
    {
        printf("Error opening file");
    }
}
