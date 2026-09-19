#include <stdio.h>

typedef struct {
  int id;
  char name[12];
  int age;
} Student;

void printIdAndName(int id, char *name);
void printDetails(Student student);
void printUsingPointer(Student *student);

int main() {
  Student student = {12, "sanin", 22};

  printUsingPointer(&student);
}

// 1. passing structure memeber as argument
void printIdAndName(int id, char *name) {
  printf("The ID of the student is %d and name is %s\n", id, name);
}

// 2. passing structure variable as argument
void printDetails(Student student) {
  printf("\nID: %d\nName: %s\nAge: %d\n", student.id, student.name,
         student.age);
}

// 3. passing pointer to structure as argument
void printUsingPointer(Student *student) {
  student->age = 122;
  printf("\nID: %d\nName: %s\nAge: %d\n", student->id, student->name,
         student->age);
}
