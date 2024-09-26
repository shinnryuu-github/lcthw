#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/** Our old friend die from ex17. */
void die(const char *message) {
  if (errno) {
    perror(message);
  } else {
    printf("ERROR: %s\n", message);
  }

  exit(1);
}

// a typedef creates a fake type, in this
// case for a function pointer
typedef int (*compare_cb)(int a, int b);
typedef int *(*sort)(int *numbers, int count, compare_cb cmp);

/**
 * A classic bubble sort function that uses the
 * compare_cb to do the sorting.
 */
int *bubble_sort(int *numbers, int count, compare_cb cmp) {
  int temp = 0;
  int i = 0;
  int j = 0;
  int *target = malloc(count * sizeof(int));

  if (!target)
    die("Memory error.");

  memcpy(target, numbers, count * sizeof(int));

  for (i = 0; i < count; i++) {
    for (j = 0; j < count - 1; j++) {
      if (cmp(target[j], target[j + 1]) > 0) {
        temp = target[j + 1];
        target[j + 1] = target[j];
        target[j] = temp;
      }
    }
  }

  return target;
}

int *selection_sort(int *numbers, int count, compare_cb cmp) {
  int *sorted = malloc(count * sizeof(int));
  if (!sorted)
    return NULL;

  for (int i = 0; i < count; i++) {
    sorted[i] = numbers[i];
  }

  for (int i = 0; i < count - 1; i++) {
    int min_index = i;
    for (int j = i + 1; j < count; j++) {
      if (cmp(sorted[j], sorted[min_index]) < 0) {
        min_index = j;
      }
    }
    int temp = sorted[i];
    sorted[i] = sorted[min_index];
    sorted[min_index] = temp;
  }
  return sorted;
}

int sorted_order(int a, int b) { return a - b; }

int reverse_order(int a, int b) { return b - a; }

int strange_order(int a, int b) {
  if (a == 0 || b == 0) {
    return 0;
  } else {
    return a % b;
  }
}

/**
 * Used to test that we are sorting things correctly
 * by doing the sort and printing it out.
 */
void test_sorting(sort sortmd, int *numbers, int count, compare_cb cmp) {
  int i = 0;
  clock_t start = clock();
  int *sorted = sortmd(numbers, count, cmp);
  clock_t end = clock();
  double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

  if (!sorted)
    die("Failed to sort as requested.");

  // for (i = 0; i < count; i++) {
  //    printf("%d ", sorted[i]);
  // }
  printf("\n");
  printf("Time taken: %f seconds\n", time_taken);
  free(sorted);
}

int main(int argc, char *argv[]) {
  int i = 0;
  int numbers[10000];
  int count = 0;
  while (count < 10000 && scanf("%d", &numbers[count]) == 1) {
    count++;
  }
  if (count == 0) {
    die("No numbers provided.");
  }

  test_sorting(bubble_sort, numbers, count, sorted_order);
  test_sorting(bubble_sort, numbers, count, reverse_order);
  test_sorting(bubble_sort, numbers, count, strange_order);

  test_sorting(selection_sort, numbers, count, sorted_order);
  test_sorting(selection_sort, numbers, count, reverse_order);
  test_sorting(selection_sort, numbers, count, strange_order);

  return 0;
}
