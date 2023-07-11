/**
 * named_socket.c
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

// Inspired by
// https://www.gnu.org/software/libc/manual/html_node/Local-Socket-Example.html
#include "named_socket.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>

int create_named_socket(int sock, struct sockaddr_un *name, char *path) {
  name->sun_family = AF_LOCAL;
  strncpy(name->sun_path, path, sizeof(name->sun_path));
  name->sun_path[sizeof(name->sun_path) - 1] = '\0';
  size_t size;

  sock = socket(PF_LOCAL, SOCK_STREAM, 0);
  if (sock < 0)
    perror("socket");

  size = (offsetof(struct sockaddr_un, sun_path) + strlen(name->sun_path));

  if (bind(sock, (struct sockaddr *)name, size) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }
  return sock;
}
