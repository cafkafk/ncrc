/**
 * read_login.c
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

#include "read_login.h"
#include "user.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Read accounts from account file and
// initialise them in user
// https://linux.die.net/man/3/getline
int read_accounts_file(struct User **user_list, char *path) {
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  fp = fopen(path, "r");
  if (fp == NULL)
    exit(EXIT_FAILURE);

  while ((read = getline(&line, &len, fp)) != -1) {
    char *token = strtok(line, " ");
    char *nick = token;
    token = strtok(NULL, " ");
    char *pass = token;
    token = strtok(NULL, " ");
    char *addr = token;
    token = strtok(NULL, " ");
    char *port = token;
    token = strtok(NULL, " ");
    push(user_list, nick, pass, addr, port);
    // printf("%s %s %s %s\n", nick, pass, addr, port);
  }

  if (line)
    free(line);

  return EXIT_SUCCESS;
}
