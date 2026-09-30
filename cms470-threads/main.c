// Travis Fine cms470

#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

// Threads take one argument so this keeps the array and its size together.
struct sortingParams {
    double *array;
    int size;
};

//Keep the two halves, their sizes, and the final array in one place.
struct mergingParams {
    double *first;
    double *second;
    double *result;
    int firstSize;
    int secondSize;
};

// Use selection sort to sort the numbers right in this array
void *selectionSort(void *argument) {
    struct sortingParams *params = argument;
    double *array = params->array;

    // Find the smallest number left and put it in the next spot.
    for (int i = 0; i < params->size - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < params->size; j++) {
            if (array[j] < array[minIndex]) {
                minIndex = j;
            }
        }

        // Swap the two numbers. Use a double so the decimasl stay intact.
        double temp = array[i];
        array[i] = array[minIndex];
        array[minIndex] = temp;
    }
    return NULL;
}

//Combine the sorted halves in one pass which takes O(n) time
void *mergeArrays(void *argument) {
    struct mergingParams *params = argument;
    int i = 0;
    int j = 0;
    int k = 0;

    // Pick the smaller number, then move forward in that half.
    while (i < params->firstSize && j < params->secondSize) {
        if (params->first[i] <= params->second[j]) {
            params->result[k++] = params->first[i++];
        } else {
            params->result[k++] = params->second[j++];
        }
    }

    // Copy over whatever is left once one half runs out of numbers.
    while (i < params->firstSize) {
        params->result[k++] = params->first[i++];
    }
    while (j < params->secondSize) {
        params->result[k++] = params->second[j++];
    }
    return NULL;
}

//If something goes wrong with a thread, show the error and stop.
void checkThread(int error) {
    if (error != 0) {
        fprintf(stderr, "Thread error: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
}

// Use a timer that is not affected by changes to the system clock.
void getTime(struct timespec *timeValue) {
    if (clock_gettime(CLOCK_MONOTONIC, timeValue) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
}

// Work out the time difference and change it to miliseconds.
double milliseconds(struct timespec begin, struct timespec end) {
    double seconds = end.tv_sec - begin.tv_sec;
    seconds += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
    return seconds * 1000.0;
}

int main(int argc, char *argv[]) {
    // Make sure there is one positive whole number, not something like 10abc.
    if (argc != 2) {
        fprintf(stderr, "Usage: %s N (positive integer)\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *end;
    errno = 0;
    long input = strtol(argv[1], &end, 10);
    if (errno != 0 || end == argv[1] || *end != '\0' ||
        input < 1 || input > INT_MAX ||
        (unsigned long)input > SIZE_MAX / sizeof(double)) {
        fprintf(stderr, "N must be a positive integer that fits in an array.\n");
        return EXIT_FAILURE;
    }
    int n = (int)input;

    // If N odd give the second half the extra number.
    int firstSize = n / 2;
    int secondSize = n - firstSize;
    double *A = malloc((size_t)n * sizeof(double));
    double *B = malloc((size_t)n * sizeof(double));
    double *C = malloc((size_t)n * sizeof(double));

    // For N = 1, give the empty first half a spare slot to avoid malloc(0).
    double *firstHalf = malloc((size_t)(firstSize > 0 ? firstSize : 1) * sizeof(double));
    double *secondHalf = malloc((size_t)secondSize * sizeof(double));
    if (A == NULL || B == NULL || C == NULL ||
        firstHalf == NULL || secondHalf == NULL) {
        fprintf(stderr, "Not enough memory for the arrays.\n");
        free(A);
        free(B);
        free(C);
        free(firstHalf);
        free(secondHalf);
        return EXIT_FAILURE;
    }

    // Use the current time as the seed
    srand((unsigned int)time(NULL));

    for (int i = 0; i < n; i++) {
        A[i] = 1.0 + 999.0 * ((double)rand() / RAND_MAX);
        B[i] = A[i];
        if (i < firstSize) {
            firstHalf[i] = A[i];
        } else {
            secondHalf[i - firstSize] = A[i];
        }
    }

    // Get all the copies ready before timing so both methods use the same numbers.
    struct timespec begin, finish;
    pthread_t threadB, threadA1, threadA2, threadM;
    struct sortingParams whole = {B, n};
    struct sortingParams first = {firstHalf, firstSize};
    struct sortingParams second = {secondHalf, secondSize};
    struct mergingParams merge = {firstHalf, secondHalf, C, firstSize, secondSize};

    //Sort the whole array with one thread, like the assignment pseudocode shows.
    getTime(&begin);
    checkThread(pthread_create(&threadB, NULL, selectionSort, &whole));
    checkThread(pthread_join(threadB, NULL));
    getTime(&finish);
    double oneTime = milliseconds(begin, finish);

    // Start both threads before waiting so they can work at the same time.
    getTime(&begin);
    checkThread(pthread_create(&threadA1, NULL, selectionSort, &first));
    checkThread(pthread_create(&threadA2, NULL, selectionSort, &second));
    checkThread(pthread_join(threadA1, NULL));
    checkThread(pthread_join(threadA2, NULL));

    // Both halves are done now, so merge
    checkThread(pthread_create(&threadM, NULL, mergeArrays, &merge));
    checkThread(pthread_join(threadM, NULL));
    getTime(&finish);
    double twoTime = milliseconds(begin, finish);

    // After timing check that the results match and the number are in order.
    int correct = 1;
    for (int i = 0; i < n; i++) {
        if (B[i] != C[i] || (i > 0 && B[i - 1] > B[i])) {
            correct = 0;
        }
    }
    if (correct) {
        printf("Sorting is done in %.3fms when one thread is used\n", oneTime);
        printf("Sorting is done in %.3fms when two threads are used\n", twoTime);
    } else {
        fprintf(stderr, "Error: sorting results are incorrect.\n");
    }

    // All the threads are finished, so free arrays.
    free(A);
    free(B);
    free(C);
    free(firstHalf);
    free(secondHalf);
    return correct ? EXIT_SUCCESS : EXIT_FAILURE;
}
