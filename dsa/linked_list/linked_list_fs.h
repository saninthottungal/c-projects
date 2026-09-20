#ifndef MATH_UTILS_H
#define MATH_UTILS_H

struct Node {
  int data;
  struct Node *next;
};

void insertNode(struct Node **head);
void deleteNode(struct Node **head);
void displayList(struct Node *head);
void updateNode(struct Node *head);

#endif