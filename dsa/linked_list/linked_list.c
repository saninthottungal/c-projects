#include <stdio.h>
#include <stdlib.h>

#include "linked_list_fs.h"

int main() {
  struct Node *head = NULL;

  printf("\nWelcome to the Linked list program: ");

  while (1) {
    int choice;

    printf("\n1. Insert\n2. Delete\n3. Display\n4. Search\n5. Display\n6. "
           "Exit\n");
    printf("\nEnter the action you want to perform: ");

    scanf("%d", &choice);
    printf("\n");

    switch (choice) {

    case 1:
      insertNode(&head);
      break;

    case 2:
      deleteNode(&head);
      break;

    case 6:
      printf("Thank you!\n");
      return 0;

    default:
      printf("Entered action is invalid, please try again!");
    }

    printf("\n");
  }

  return 0;
}