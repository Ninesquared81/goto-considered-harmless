#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define INIT_DATA_SIZE 1024
#define SOURCE_BUFFER_SIZE 1024*1024

#define USAGE "Usage: gotoch [options] [source]\n"

int main(int argc, char **argv) {
    int64_t *data = calloc(INIT_DATA_SIZE, sizeof *data);
    int ip = 0;
    int dp = 0;
    char *source = calloc(SOURCE_BUFFER_SIZE, sizeof *source);
    int64_t source_length = 0;
    if (argc == 1) {
        fgets(source, SOURCE_BUFFER_SIZE, stdin);
        source_length = strlen(source);
    }
    else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr,
                USAGE
                "Arguments:\n"
                "  source\t\tThe filename of the source code to interpret.\n"
                "\t\t\tNOTE: if omitted, stdin will be used instead.\n"
                "Options:\n"
                "  -h, --help\t\tDisplay this help message.\n"
                "\n"
            );
        exit(0);
    }
    else if (argc > 2) {
        fprintf(stderr, "Unexpected argument.\n" USAGE);
        exit(1);
    }
    else {
        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            fprintf(stderr, "Could not open the file %s", argv[1]);
            exit(1);
        }
        source_length = fread(source, sizeof *source, SOURCE_BUFFER_SIZE - 1, file);
        if (ferror(file)) {
            perror("There was an error reading the source file");
            fclose(file);
            exit(1);
        }
        fclose(file);
    }

    for ( ; ip < source_length; ++ip) {
        if (ip < 0) {
            ip = 0;
        }
        switch (source[ip]) {
            /* Core operations */

        case '?':
            if (data[dp] == 0) {
                ++dp;
            }
            else {
                ip += data[dp];
            }
            break;
        case '<': {
            int64_t current = data[dp];
            data[dp] -= data[dp-1];
            data[dp-1] += current;
            --dp;
            break;
        }
        case '+':
            data[dp] += dp;
            break;
        case '-':
            data[dp] -= dp;
            break;

            /* I/O operations */
        case ',': {
            int ch = getchar();
            if (ch == EOF) {
                ch = 0;
            }
            data[dp] = ch;
            break;
        }
        case '.':
            putchar(data[dp]);
            break;
        case '#':
            printf("%lld", data[dp]);
            break;
        }
    }
}
