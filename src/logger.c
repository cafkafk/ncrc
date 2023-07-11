/**
 * logger.c
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

#include "logger.h"

#include <stdio.h>
#include <stdlib.h>

const char *level_col(int level) {
  switch (level) {
  case LTRACE: {
    char *res = ANSI_COLOR_NONE;
    return res;
  }
  case LDEBUG: {
    char *res = ANSI_COLOR_WHITE_BOLD;
    return res;
  }
  case LINFO: {
    char *res = ANSI_COLOR_BLUE;
    return res;
  }
  case LWARN: {
    char *res = ANSI_COLOR_MAGENTA;
    return res;
  }
  case LERROR: {
    char *res = ANSI_COLOR_RED;
    return res;
  }
  case LFATAL: {
    char *res = ANSI_COLOR_BLACK_RED_BG;
    return res;
  }
  }
}
int logger(int level, int col, char *from, char *func, char *msg) {
  if (col == NONE) {
    printf("[ %18s] %s: %s\n", from, func, msg);
    return 0;
  }
  // printf("level: %s test string \n", level_col(level));
  printf(ANSI_COLOR_GREEN "[ %18s]" ANSI_COLOR_YELLOW
                          " %s: %s%s" ANSI_COLOR_RESET "\n",
         from, func, level_col(level), msg);
  return 0;
}
