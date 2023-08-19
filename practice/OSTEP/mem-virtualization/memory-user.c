#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <memory_in_MB>\n", argv[0]);
        return 1;
    }

    long memory_in_MB = strtol(argv[1], NULL, 10);
    long long num_elements = (memory_in_MB * 1024 * 1024) / sizeof(int);

    int *array = (int *)malloc(num_elements * sizeof(int));
    if (array == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    printf("Allocated %ld MB of memory.\n", memory_in_MB);

    // Touch each entry in the array
    for (long long i = 0; i < num_elements; ++i) {
        array[i] = 0;
    }

    printf("Array elements cleared.\n");

    // Run indefinitely (Ctrl+C to stop) or specify a time in seconds
    int run_time = -1; // Set to -1 for indefinite run
    if (argc > 2) {
        run_time = atoi(argv[2]);
    }

    printf("modifying array elements\n");
    time_t start_time = time(NULL);
    while (run_time < 0 || time(NULL) - start_time < run_time) {
        // Continuously touch each entry in the array
        for (long long i = 0; i < num_elements; ++i) {
            array[i] = (array[i] + 1) % 256;
        }
    }

    // Free allocated memory
    free(array);

    printf("Memory released.\n");

    return 0;
}
