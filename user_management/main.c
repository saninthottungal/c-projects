#include <stdio.h>
#include <string.h>

#define MAX_USERS 50

typedef struct {
  char username[20];
  char password[20];
} User;

int registerUser(User users[], int index);
void loginUser(User users[]);
int checkIfUserAlreadyExists(User arr[], char username[]);
int checkIfUsernamePasswordMatches(User users[], char username[],
                                   char password[]);

int main(int argc, char const *argv[]) {
  User users[MAX_USERS] = {0};
  int counter = 0;

  while (1) {
    int userSelection;
    printf("\n1. Register\n2. Login\n3. Exit\n");
    printf("Select an option: ");
    scanf(" %d", &userSelection);

    switch (userSelection) {

    case 1: {
      int isSuccess = registerUser(users, counter);
      if (isSuccess) {
        printf("\nUser registration completed successfully!\n");
        counter++;
      }

      break;
    }

    case 2:
      loginUser(users);

      break;

    case 3:
      printf("Exited\n");
      return 0;
    default:

      return 1;
    }
  }
  return 0;
}

int registerUser(User users[], int index) {

  User user;
  printf("\nEnter username(MAX 20 characters): ");
  scanf(" %s", user.username);
  printf("Enter password(MAX 20 characters): ");
  scanf(" %s", user.password);

  int alreadyExists = checkIfUserAlreadyExists(users, user.username);

  if (alreadyExists) {
    printf("\nUser Already Exists. Please login to continue.\n");

    return 0;
  } else {
    users[index] = user;
  }

  return 1;
}

void loginUser(User users[]) {
  User user;

  printf("\nEnter username: ");
  scanf(" %s", user.username);
  printf("Enter password: ");
  scanf(" %s", user.password);

  int alreadyExists = checkIfUserAlreadyExists(users, user.username);

  if (alreadyExists) {
    int passwordMatches =
        checkIfUsernamePasswordMatches(users, user.username, user.password);

    if (passwordMatches) {
      printf("\nUser logged in Successfully!\n");
    } else {
      printf("\nusername or password is incorrect!\n");
    }
  } else {
    printf("\nUser not found, please register to continue!\n");
  }
}

int checkIfUserAlreadyExists(User users[], char username[]) {
  int doesUserExists = 0;

  for (int i = 0; i < MAX_USERS; i++) {

    if (strcmp(users[i].username, username) == 0) {
      doesUserExists = 1;
      return doesUserExists;
    }
  }

  return doesUserExists;
}

int checkIfUsernamePasswordMatches(User users[], char username[],
                                   char password[]) {
  int doesPasswordMatch = 0;
  for (int i = 0; i < MAX_USERS; i++) {

    if (strcmp(users[i].username, username) == 0 &&
        strcmp(users[i].password, password) == 0) {
      doesPasswordMatch = 1;
      return doesPasswordMatch;
    }
  }

  return 0;
}
