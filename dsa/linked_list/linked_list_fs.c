#include "linked_list_fs.h"
#include <stdio.h>
#include <stdlib.h>

void insert(struct Node **head) {
  int data;
  printf("Enter the element you want to insert: ");
  scanf("%d", &data);

  struct Node *newNode = malloc(sizeof(struct Node));
  if (newNode == NULL) {
    printf("Failed to allocate memory!");
    return;
  } else {
    newNode->data = data;
    newNode->next = NULL;
  }

  // case when linked list is empty!
  if (*head == NULL) {
    *head = newNode;
  } else {
    struct Node *ptr = *head;
    while (ptr->next != NULL) {
      ptr = ptr->next;
    }

    ptr->next = newNode;
  }

  printf("New element %d inserted in the list!", data);
}