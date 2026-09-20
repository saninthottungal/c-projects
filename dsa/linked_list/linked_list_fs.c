#include "linked_list_fs.h"
#include <_stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void insertNode(struct Node **head) {
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

void deleteNode(struct Node **head) {

  if (*head == NULL) {
    printf("List is empty!");
    return;
  }

  int data;
  bool foundAnyData = false;
  printf("Enter the element you want to delete: ");
  scanf("%d", &data);

  struct Node *ptr = *head;

  if (ptr->data == data) {
    *head = ptr->next;
    free(ptr);

    foundAnyData = true;
  }

  ptr = *head;
  struct Node *temp = NULL;

  while (ptr != NULL) {
    temp = ptr->next;

    if (temp->data == data) {
      ptr->next = temp->next;
      free(temp);

      foundAnyData = true;
    }

    ptr = ptr->next;
  }

  if (foundAnyData) {
    printf("Deleted element %d successfully!", data);
  } else {
    printf("Element %d Not found!", data);
  }
}

void displayList(struct Node *head) {
  if (head == NULL) {
    printf("List is empty!");
    return;
  }

  struct Node *ptr = head;

  while (ptr != NULL) {
    printf("Data: %d\n", ptr->data);
    ptr = ptr->next;
  }
}

void updateNode(struct Node *head) {
  if (head == NULL) {
    printf("List is empty!");
    return;
  }

  int oldData, newData;
  bool hasFoundAny = false;

  printf("Enter the element you want to update: ");
  scanf("%d", &oldData);

  printf("Enter the element you want to replace with: ");
  scanf("%d", &newData);

  struct Node *ptr = head;

  while (ptr != NULL) {
    if (ptr->data == oldData) {
      ptr->data = newData;
      hasFoundAny = true;
    }

    ptr = ptr->next;
  }

  if (hasFoundAny) {
    printf("Updated all instances of %d with %d", oldData, newData);
  } else {
    printf("Element %d not found!", oldData);
  }
}