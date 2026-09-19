#include <stdio.h>
#include <stdlib.h>

struct Node {
  int data;
  struct Node *next;
};

int main() {
  struct Node *head = (struct Node *)malloc(sizeof(struct Node));
  head->data = 10;

  head->next = (struct Node *)malloc(sizeof(struct Node));
  head->next->data = 12;
  head->next->next = NULL;

  struct Node *temp = head;

  while (temp != NULL) {
    printf("data: %d\n", temp->data);
    temp = temp->next;
  }

  // Remember to free allocated memory
  free(head->next);
  free(head);

  return 0;
}