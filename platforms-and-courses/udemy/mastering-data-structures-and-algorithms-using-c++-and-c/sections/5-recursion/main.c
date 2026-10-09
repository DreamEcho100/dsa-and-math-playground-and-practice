#include <stdio.h>

void tower_of_hanoi(int size, int src, int intermediary, int dest) {
  if (size > 0) {
    tower_of_hanoi(size - 1, src, dest, intermediary);
    printf("(%d, %d)\n", src, dest);
    tower_of_hanoi(size - 1, intermediary, src, dest);
  }
}

int main() {
  tower_of_hanoi(4, 1, 2, 4);
  return 0;
}
