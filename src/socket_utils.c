/**
 * socket_uitls.c
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
#include "socket_utils.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>

int sendstr(int socket, char buf[]) {
  if (-1 == send(socket, buf, strlen(buf), MSG_MORE)) {
    perror("client");
    // Why should we die on this? Let's just tell it.
    return (1);
  }
  return 0;
}

int perror_helper(char *str) {
  perror(str);
  exit(EXIT_FAILURE);
}

int in_range(char *str, int a, int b) {
  // The Z symbol for integers is from german zahlen
  int zahl;
  if (sscanf(str, "%d", &zahl) < 0)
    return EXIT_FAILURE;
  else if (a > zahl || zahl > b)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
