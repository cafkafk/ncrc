/**
 * data_handler.c
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

#include "data_handler.h"

char *dfrom = "data_handler.c";

int data_loop(int sock, int col, pthread_mutex_t mutex, struct User **accounts,
              struct User **online) {
  logger(LDEBUG, col, dfrom, "data_loop", "enterd dataloop");
  logger(LTRACE, col, dfrom, "data_loop", "initializing buffer[1024]");

  char buffer[1024] = {0};

  logger(LTRACE, col, dfrom, "data_loop", "initializing command");
  command_t command;

  logger(LTRACE, col, dfrom, "data_loop", "initializing args");
  args_t args;

  logger(LTRACE, col, dfrom, "data_loop", "initializing recv_res");
  int recv_res;

  logger(LDEBUG, col, dfrom, "data_loop", "recieving sock to buffer");
  if ((recv_res = recv(sock, buffer, sizeof(buffer), MSG_MORE))) {
    logger(LDEBUG, col, dfrom, "data_loop", "parsing command");
    command = parse_command(buffer, args);

    logger(LTRACE, col, dfrom, "data_loop", "entering switch");
    switch (command) {
    case LOGIN: {
      logger(LTRACE, col, dfrom, "data_loop", "case login");
      logger(LDEBUG, col, dfrom, "data_loop", "lock mutex");
      pthread_mutex_lock(&mutex);

      logger(LTRACE, col, dfrom, "data_loop", "initialize cmd");
      char *cmd;

      logger(LDEBUG, col, dfrom, "data_loop", "login accounts with args");
      cmd = login(accounts, online, args[0], args[1], args[2], args[3]);

      logger(LDEBUG, col, dfrom, "data_loop", "sending res to sock");
      sendstr(sock, cmd);

      logger(LDEBUG, col, dfrom, "data_loop", "unlocking mutex");
      pthread_mutex_unlock(&mutex);

      logger(LDEBUG, col, dfrom, "data_loop", "exit success");
      return EXIT_SUCCESS;
    };

    // TODO Should read x bytes each time it recieves continue, using
    // the last node as the starting point
    case LOOKUP_ALL: {
      logger(LTRACE, col, dfrom, "data_loop", "case lookup_all");
      logger(LTRACE, col, dfrom, "data_loop", "initialize cmd");
      char *cmd;
      logger(LDEBUG, col, dfrom, "data_loop", "locking mutex");
      pthread_mutex_lock(&mutex);

      logger(LDEBUG, col, dfrom, "data_loop", "calling lookup all");
      cmd = lookup_all(online);

      logger(LTRACE, col, dfrom, "data_loop", "send lookup ok");
      sendstr(sock, LOOKUP_OK);

      logger(LDEBUG, col, dfrom, "data_loop", "open /dev/null for writing");
      int devNull = open("/dev/null", O_WRONLY);

      logger(LDEBUG, col, dfrom, "data_loop",
             "awaiting recv and writing it to /dev/null");
      recv(sock, devNull, 32, MSG_MORE);

      logger(LDEBUG, col, dfrom, "data_loop", "sending cmd to sock");
      sendstr(sock, cmd);

      logger(LDEBUG, col, dfrom, "data_loop", "free(cmd)");
      free(cmd);

      logger(LDEBUG, col, dfrom, "data_loop", "unlock mutex");
      pthread_mutex_unlock(&mutex);

      logger(LTRACE, col, dfrom, "data_loop", "returning success");
      return EXIT_SUCCESS;
    };

    case LOOKUP_SINGLE: {
      logger(LTRACE, col, dfrom, "data_loop", "case lookup_single");
      logger(LTRACE, col, dfrom, "data_loop", "initialize cmd");
      char *cmd;
      logger(LTRACE, col, dfrom, "data_loop", "locking mutex");
      pthread_mutex_lock(&mutex);

      logger(LTRACE, col, dfrom, "data_loop", "calling lookup_nick on online");
      cmd = lookup_nick(online, args[0]);

      logger(LTRACE, col, dfrom, "data_loop",
             "checking return from lookup_nick");
      if (strcmp(cmd, LOOKUP_FAIL_USER_NOT_ONLINE) != 0) {
        logger(LTRACE, col, dfrom, "data_loop", "user was found");
        logger(LTRACE, col, dfrom, "data_loop", "sending lookup_ok");
        sendstr(sock, LOOKUP_OK);

        logger(LDEBUG, col, dfrom, "data_loop", "open /dev/null for writing");
        int devNull = open("/dev/null", O_WRONLY);

        logger(LDEBUG, col, dfrom, "data_loop",
               "awaiting recv and writing it to /dev/null");
        recv(sock, devNull, 32, MSG_MORE);

        logger(LDEBUG, col, dfrom, "data_loop", "sending cmd to sock");
        sendstr(sock, cmd);

        logger(LDEBUG, col, dfrom, "data_loop", "free(cmd)");
        free(cmd);

        logger(LDEBUG, col, dfrom, "data_loop", "unlock mutex");
        pthread_mutex_unlock(&mutex);

        logger(LTRACE, col, dfrom, "data_loop", "returning success");
        return EXIT_SUCCESS;
      }
      logger(LTRACE, col, dfrom, "data_loop", "user not found");
      logger(LTRACE, col, dfrom, "data_loop", "sending cmd to sock");
      sendstr(sock, cmd);

      logger(LDEBUG, col, dfrom, "data_loop", "unlock mutex");
      pthread_mutex_unlock(&mutex);

      logger(LTRACE, col, dfrom, "data_loop", "returning success");
      return EXIT_SUCCESS;
    };

    case LOGOUT_SERVER: {
      logger(LTRACE, col, dfrom, "data_loop", "case lookup_single");

      logger(LTRACE, col, dfrom, "data_loop", "locking mutex");
      pthread_mutex_lock(&mutex);

      logger(LTRACE, col, dfrom, "data_loop", "initialize cmd");
      char *cmd;

      logger(LTRACE, col, dfrom, "data_loop", "logout accounts online");
      cmd = logout(accounts, online, args[0], args[1], args[2], args[3]);

      logger(LDEBUG, col, dfrom, "data_loop", "unlock mutex");
      pthread_mutex_unlock(&mutex);

      logger(LDEBUG, col, dfrom, "data_loop", "send cmd to sock");
      sendstr(sock, cmd);
      logger(LDEBUG, col, dfrom, "data_loop", "return success");
      return EXIT_SUCCESS;
    } break;
    case SHOW:
      sendstr(sock, "Show!\n");
      break;

    case MSG:
      sendstr(sock, "MSG!\n");
      break;

    // Not allowed on data handler
    // case LOOKUP:
    case LOGOUT:
    case ERROR:
      sendstr(sock, "ERROR!\n");
      break;

    case EXIT:
      sendstr(sock, "EXIT!\n");
      return EXIT_SUCCESS;
      break;

    default:
      sendstr(sock, "WAT!\n");
      send(sock, buffer, strlen(buffer), MSG_MORE);
      break;
    }
    // Breaks without return lead here
    printf("DATA LOOP FELL THROUGH SWITCH! WHAT ARE YOU DOING PROGRAMMER!\n ");
    return EXIT_FAILURE;

  } else if (recv_res == 0)
    return 0;
  else
    return 1;
}
