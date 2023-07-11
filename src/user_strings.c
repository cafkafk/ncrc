/**
 * user_strings.c
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

#include "user_strings.h"

#include <stdio.h>
#include <stdlib.h>

/* NOTICE NOTICE NOTICE */
/* DONT CONFUSE stdlib EXIT_FAILURE for user_strings.h EXIT_FAIL */
/* THERE IS NO CHECKING THAT THE STRING IS SENSIBLE
 * I.E. MALFORMED REQUEST WILL DEFAULT TO FAILURE
 * I.E.I.E. THERE IS NO MALFORMED SIGNAL
 * I.E.I.E.I.E. MALFORMED REQUEST WILL HAPPEN SILENTLY
 * This was choosen on purpose, but NOTE it
 */

void ncrc() {
  printf("   ____  _______________\n");
  printf("  / __ \\/ ___/ ___/ ___/\n");
  printf(" / / / / /__/ /  / /__\n");
  printf("/_/ /_/\\___/_/   \\___/\n");
}

void license() {
  printf("ncrc  Copyright (C) 2020  Christina E. Soerensen\n");
  printf("This program comes with ABSOLUTELY NO WARRANTY; \n");
  // for details type "         "`show w'.");
  printf("This is free software, and you are welcome to redistribute it\n");
  printf("under certain conditions; \n");
  // FIXME interactive mode
  // type `show c' for details.");
}

/* map login cmd to exit status */
int login_parse(char *cmd) {
  if (strcmp(cmd, LOGIN_OK))
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

/* map logout cmd to exit status */
int logout_parse(char *cmd) {
  if (strcmp(cmd, LOGOUT_OK))
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

/* map lookup cmd to exit status */
int lookup_parse(char *cmd) {
  if (strcmp(cmd, LOOKUP_OK))
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

/* map show cmd to exit status */
int show_parse(char *cmd) {
  if (strcmp(cmd, SHOW_OK))
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

/* map msg cmd to exit status */
int msg_parse(char *cmd) {
  if (strcmp(cmd, MSG_OK))
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

/* TODO NO PARSER FOR ERROR OR EXIT YET TODO */
