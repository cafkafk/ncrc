/**
 * users.c
 *
 *  Copyright 2020 by Christina E. Sørensen <cafkafk@meber.fsf.org>
 *
 * This file is part of ncrc.
 *
 * ncrc is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * ncrc is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ncrc.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 */

#include "user.h"

char *ufrom = "user.c";

// Push new user with nick, pass, addr, port into head_ref
int push(struct User **head_ref, char nick[MAX_NICK], char pass[MAX_PASS],
         char addr[INET_ADDRSTRLEN], char port[PORT_LEN]) {
  struct User *new_user = (struct User *)malloc(sizeof(struct User));
  new_user->nick = malloc(sizeof(char *[MAX_NICK]));
  new_user->pass = malloc(sizeof(char *[MAX_PASS]));
  new_user->addr = malloc(sizeof(char *[INET_ADDRSTRLEN]));
  new_user->port = malloc(sizeof(char *[PORT_LEN]));
  strcpy(new_user->nick, nick);
  strcpy(new_user->pass, pass);
  strcpy(new_user->addr, addr);
  strcpy(new_user->port, port);
  new_user->next = (*head_ref);
  (*head_ref) = new_user;
  return EXIT_SUCCESS;
}

// Remove user from user_list by finding nick
// Return EXIT_SUCCESS if user removed
// Return EXIT_FAILURE if user not found
// Linear time, nothing fancy
int remove_user_by_nick(struct User **user_list, char *nick) {
  struct User *temp = *user_list, *prev;

  // If user found at head, and head not empty
  if (temp != NULL && strcmp(temp->nick, nick) == 0) {
    *user_list = temp->next; // Changed head
    free(temp);              // free old head
    return EXIT_SUCCESS;
  }

  // Move to user/null link
  while (temp != NULL && strcmp(temp->nick, nick) != 0) {
    prev = temp;
    temp = temp->next;
  }

  // If null link, we did not find user
  if (temp == NULL)
    return EXIT_FAILURE;

  // ELSE we found user
  prev->next = temp->next;

  free(temp->nick);
  free(temp->pass);
  free(temp->addr);
  free(temp->port);
  free(temp);
  return EXIT_SUCCESS;
}

// Returns EXIT_SUCCESS if user credentials exists
// Returns EXIT_FAILURE if user credentials does not exists
// Credentials are just nick and pass, not addr or port
int check_login(struct User **user_list, char nick[MAX_NICK],
                char pass[MAX_PASS], char addr[INET_ADDRSTRLEN],
                char port[PORT_LEN]) {
  struct User *curr = (*user_list);
  while (curr != NULL) {
    // printf("check_login: cn: %s, cp: %s, n: %s, p: %s\n", curr->nick,
    // curr->pass, nick, pass);
    if (strcmp(curr->nick, nick) == 0) {
      // printf("check_login: Correct nick...\n");
      if (strcmp(curr->pass, pass) == 0) {
        // printf("check_login: Correct pass...\n");
        // printf("check_login: Login valid!\n");
        return EXIT_SUCCESS;
      }
    }
    curr = curr->next;
  }
  // printf("check_login: Login invalid!\n");
  return EXIT_FAILURE;
}

// Returns EXIT_SUCCESS if user nick exists
// Returns EXIT_FAILURE if user nick does not exists
int nick_in_list(struct User **user_list, char nick[MAX_NICK]) {
  struct User *curr = (*user_list);
  while (curr != NULL) {
    if (strcmp(curr->nick, nick) == 0)
      return EXIT_SUCCESS;
    curr = curr->next;
  }
  return EXIT_FAILURE;
}

// Take accounts, online, and user parameters nick, pass, addr, port
// If user is not online and credentials are correct, add user to online
// and return EXIT_SUCCESS, if not return EXIT_FAILURE
const char *login(struct User **accounts, struct User **online,
                  char nick[MAX_NICK], char pass[MAX_PASS],
                  char addr[INET_ADDRSTRLEN], char port[PORT_LEN]) {
  // Check if already online
  if (nick_in_list(online, nick)) {
    // Check credentials OK
    if (check_login(accounts, nick, pass, addr, port)) {
      // Credential not OK
      return LOGIN_FAIL_CREDENTIAL;
    }
    // Add online
    else if (push(online, nick, pass, addr, port)) {
      return LOGIN_FAIL_PUSH;
    }
    // Add failed
    else {
      // Add success
      return LOGIN_OK;
    }
  } else
    return LOGIN_FAIL_ALREADY_ONLINE;
}

// Take accounts, online, and user parameters nick, pass, addr, port
// If user is not online and credentials are correct, add user to online
// and return EXIT_SUCCESS, if not return EXIT_FAILURE
const char *logout(struct User **accounts, struct User **online,
                   char nick[MAX_NICK], char pass[MAX_PASS],
                   char addr[INET_ADDRSTRLEN], char port[PORT_LEN]) {

  logger(LTRACE, 1, ufrom, "logout", "enterd logout"); // FIXME color
  logger(LTRACE, 1, ufrom, "logout",
         "if nick in list is online"); // FIXME color

  if (nick_in_list(online, nick)) {

    logger(LINFO, 1, ufrom, "logout",
           "return LOGOUT_FAIL_NOT_ONLINE"); // FIXME color
    return LOGOUT_FAIL_NOT_ONLINE;
  }

  logger(LTRACE, 1, ufrom, "logout",
         "check login accounts nick pass addr port"); // FIXME color

  if (check_login(accounts, nick, pass, addr, port)) {

    logger(LINFO, 1, ufrom, "logout",
           "return LOGOUT_FAIL_WRONG_CREDENTIALS"); // FIXME color
    return LOGOUT_FAIL_WRONG_CREDENTIALS;           // Credential not OK

  } else if (remove_user_by_nick(online, nick)) {

    logger(LTRACE, 1, ufrom, "logout",
           "if remove_user_by_nick online nick"); // FIXME color
    logger(LINFO, 1, ufrom, "logout",
           "return LOGOUT_FAIL_REMOVE"); // FIXME color
    return LOGOUT_FAIL_REMOVE;           // Remove failed

  } else {

    logger(LINFO, 1, ufrom, "logout",
           "return LOGOUT_OK"); // FIXME color
    return LOGOUT_OK;           // User removed
  }
}

/* This was made for peers, BEWARE */
void printListNick(struct User *user) {
  int the_ugly = 0;
  while (user != NULL) {
    /* NOTE: The "Head" is all null */
    if (the_ugly == 0) {
      the_ugly++;
      printf(ANSI_COLOR_MAGENTA "|!" ANSI_COLOR_RESET " %-10s\n",
             "Oldest at TOP!");
    } else {
      printf(ANSI_COLOR_MAGENTA "<" ANSI_COLOR_RESET " %-10s\n", user->nick);
    }
    user = user->next;
  }
  printf(ANSI_COLOR_MAGENTA "|" ANSI_COLOR_RESET " %-10s\n",
         "NO MORE IN INBOX");
}

// This function prints contents of linked list starting from
// the given node
// Is p 1, prints passwords
void printList(struct User *user, int p) {
  // printf("\t");
  printf(ANSI_COLOR_MAGENTA
         "+---------------+---------------+-----------------------+---------+"
         "\n" ANSI_COLOR_RESET);
  printf(ANSI_COLOR_MAGENTA "|" ANSI_COLOR_RED " %-10s\t" ANSI_COLOR_MAGENTA
                            "|" ANSI_COLOR_RED " %-10s\t" ANSI_COLOR_MAGENTA
                            "|" ANSI_COLOR_RED " %-16s\t" ANSI_COLOR_MAGENTA
                            "|" ANSI_COLOR_RED " %-7s " ANSI_COLOR_MAGENTA
                            "|" ANSI_COLOR_RESET "\n",
         "NICK", "PASS", "ADDR", "PORT");
  while (user != NULL) {
    printf(ANSI_COLOR_MAGENTA
           "+---------------+---------------+-----------------------+---------+"
           "\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_MAGENTA "|" ANSI_COLOR_RESET " %-10s\t", user->nick);
    if (p)
      printf(ANSI_COLOR_MAGENTA "|" ANSI_COLOR_RESET " %-10s\t", user->pass);
    else
      printf(ANSI_COLOR_MAGENTA "|" ANSI_COLOR_RESET " %-10s\t", "****");
    printf(ANSI_COLOR_MAGENTA "|" ANSI_COLOR_RESET " %-16s\t", user->addr);
    printf(ANSI_COLOR_MAGENTA "|" ANSI_COLOR_RESET " %-7s " ANSI_COLOR_MAGENTA
                              "|" ANSI_COLOR_RESET "\n",
           user->port);
    // printf("->");
    user = user->next;
  }
  printf(ANSI_COLOR_MAGENTA
         "+---------------+---------------+-----------------------+---------+"
         "\n" ANSI_COLOR_RESET);
}

int count_users(struct User *user) {
  int i = 0;
  while (user->next != NULL) {
    printf("Looping i:%d\n", i);
    i++;
    user = user->next;
  }
  printf("Loop done i:%d\n", i);
  return i;
}

const char *lookup_nick(struct User **user_list, char nick[MAX_NICK]) {
  struct User *curr = (*user_list);
  char *temp = malloc(2048);
  while (curr != NULL) {
    if (strcmp(curr->nick, nick) == 0) {
      sprintf(temp, "%s: %s:%s\n", curr->nick, curr->addr, curr->port);
      return temp;
    }
    curr = curr->next;
  }
  free(temp);
  return LOOKUP_FAIL_USER_NOT_ONLINE;
}

const char *lookup_all(struct User **user) {
  struct User *curr = (*user);
  char *res = malloc(12048);
  sprintf(res, "%d users online\n", count_users(user)); // BUG iNcOmPaTiBlE tYpE
  // 12048 bytes = 1.2 kb
  // No problem
  while (curr != NULL) {
    // printf("Looping on users nick:%s\n", user->next->nick);
    char *temp = malloc(4048);
    sprintf(temp, "%s: %s:%s\n", curr->nick, curr->addr, curr->port);
    strcat(res, temp);
    free(temp);
    curr = curr->next;
  }
  return res;
}
