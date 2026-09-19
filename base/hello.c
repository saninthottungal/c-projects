#include <stdio.h>

int main() {
  int arr[6] = {10, 20, 30, 40, 50};
  int new_element;

  printf("Enter the new element: ");
  scanf("%d", &new_element);

  for (int i = 4; i >= 0; i--) {
    if (new_element >= arr[i]) {
      arr[i + 1] = new_element;
      break;
    } else {
      arr[i + 1] = arr[i];
    }
  }

  printf("\nThe new array is: ");
  for (int i = 0; i < 6; i++) {
    printf("%d ", arr[i]);
  }

  printf("\n");

  return 0;
}