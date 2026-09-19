#ifndef MATH_UTILS_H
#define MATH_UTILS_H

struct Node {
  int data;
  struct Node *next;
};

void insert(struct Node **head);

#endif