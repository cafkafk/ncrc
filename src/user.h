/**
 * user.h
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

#pragma once

#include "logger.h"
#include "user_strings.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netinet/in.h> // NOTE: Supposedly, the compiler will
                        // remove the useless parts, but still
                        // this looks like an easy target for
                        // debloating, we can just define inet
                        // ourselves

#define MAX_PASS 32
#define MAX_NICK 2048     /* FIXME This is extremly stupid */
#define USERNAME_LEN 32   // max length of a valid username.
#define MAX_LINE 128      // max length of a line of user input.
#define MAX_USER_ARGNUM 4 // max number of arguments to peer commands.

#define IP_LEN 16
#define PORT_LEN 8

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

// Defines moved#define MAX_USERS 16

#define MAX_ACCOUNTS 64

struct User {
  struct User *next;
  char *pass; //[MAX_PASS];
  char *nick; //[MAX_NICK];
  char *addr; //[INET_ADDRSTRLEN];
  char *port; //[PORT_LEN]; // This should not be nescesarry
};

int push(struct User **head_ref, char nick[MAX_NICK], char pass[MAX_PASS],
         char addr[INET_ADDRSTRLEN], char port[PORT_LEN]);
int deleteNode(struct User **head_ref, char *nick);
int nick_in_list(struct User **user_list, char nick[MAX_NICK]);
void printList(struct User *user, int p);
void printListNick(struct User *user);
const char *login(struct User **accounts, struct User **online,
                  char nick[MAX_NICK], char pass[MAX_PASS],
                  char addr[INET_ADDRSTRLEN], char port[PORT_LEN]);

const char *logout(struct User **accounts, struct User **online,
                   char nick[MAX_NICK], char pass[MAX_PASS],
                   char addr[INET_ADDRSTRLEN], char port[PORT_LEN]);

const char *lookup_all(struct User **user);
const char *lookup_nick(struct User **user_list, char nick[MAX_NICK]);
