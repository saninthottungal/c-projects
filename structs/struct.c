#include <stdio.h>

void printIdAndName(int id, char *name);

typedef struct {
  int id;
  char name[12];
  int age;
} Student;

int main() {
  Student student = {12, "sanin", 22};

  printIdAndName(student.id, student.name);
}

// 1. passing structure memeber as argument
void printIdAndName(int id, char *name) {
  printf("The ID of the student is %d and name is %s\n", id, name);
}