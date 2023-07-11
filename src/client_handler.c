/**
 * client_handler.c
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

#include "client_handler.h"
#include "common.h"
#include "logger.h"
#include "user.h"
#include "user_strings.h"

#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>

char *cfrom = "user.c";

int connect_data_sock(char *socket_path) {
  int data_sock;
  struct sockaddr_un data_name;
  if ((data_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }
  memset(&data_name, 0, sizeof(data_name));
  data_name.sun_family = AF_UNIX;
  if (*socket_path == '\0') {
    *data_name.sun_path = '\0';
    strncpy(data_name.sun_path + 1, socket_path + 1,
            sizeof(data_name.sun_path) - 2);
  } else {
    strncpy(data_name.sun_path, socket_path, sizeof(data_name.sun_path) - 1);
  }

  if (connect(data_sock, (struct sockaddr *)&data_name, sizeof(data_name)) ==
      -1) {
    perror("connect error");
    exit(-1);
  }
  return data_sock;
}

int dc_logout(int col, char *nick, char *pass, char *addr, char *port,
              char *socket_path, int logged_in) {
  if (!logged_in) {
    logger(LDEBUG, col, cfrom, "dc_logout", "client was not logged in...");
    return 0;
  }
  logger(LDEBUG, col, cfrom, "dc_logout", "client was logged in");
  logger(LDEBUG, col, cfrom, "dc_logout", "logging client out");

  logger(LTRACE, col, cfrom, "dc_logout", "initialize cmd");
  char cmd[MAX_COMMAND_LENGTH] = {0};
  logger(LTRACE, col, cfrom, "dc_logout", "initialize replbuffer");
  char replbuffer[1024] = {0};

  logger(LTRACE, col, cfrom, "dc_logout",
         "snprintf replbuffer with nick pass addr port");
  snprintf(replbuffer, 1024, "/_logout %s %s %s %s", nick, pass, addr, port);

  logger(LTRACE, col, cfrom, "dc_logout", "connect_data_sock(socket_path)");
  int data_sock = connect_data_sock(socket_path);

  logger(LTRACE, col, cfrom, "dc_logout", "sendstr data_sock replbuffer");
  sendstr(data_sock, replbuffer);

  logger(LTRACE, col, cfrom, "dc_logout", "recv data_sock itno cmd");
  int ans = recv(data_sock, cmd, sizeof(cmd), MSG_FIN);
  logger(LTRACE, col, cfrom, "dc_logout", "close data_sock");
  close(data_sock);

  logger(LTRACE, col, cfrom, "dc_logout", "if logout_parse(cmd)");
  if (logout_parse(cmd)) {
    logger(LWARN, col, cfrom, "dc_logout",
           "FAILED LOGIN CLIENT OUT, CLIENT WILL REMAIN LOGGED IN");
    logger(LWARN, col, cfrom, "dc_logout", "set logged_in = 1");
    logged_in = 1;
  } else {
    logger(LINFO, col, cfrom, "dc_logout", "logout success");
    logger(LTRACE, col, cfrom, "dc_logout", "set logged_in = 0");
    logged_in = 0;
    logger(LTRACE, col, cfrom, "dc_logout", "set nick empty");
    *nick = (char)"";
    logger(LTRACE, col, cfrom, "dc_logout", "set pass empty");
    *pass = (char)"";
    logger(LTRACE, col, cfrom, "dc_logout", "set addr empty");
    *addr = (char)"";
    logger(LTRACE, col, cfrom, "dc_logout", "set port empty");
    *port = (char)"";
  }
  logger(LINFO, col, cfrom, "dc_logout", "exit failure");
  return 0;
}

int client_loop(int new_socket, int col, char *socket_path) {
  int logged_in = 0;
  char nick[MAX_NICK] = "";
  char pass[MAX_PASS] = "";
  char addr[INET_ADDRSTRLEN] = "";
  char port[PORT_LEN] = "";

  while (1) {
    char buffer[1024] = {0};
    command_t command;
    // Initialize args with sentinel NULl
    args_t args = {NULL, NULL, NULL, NULL};
    int recv_res;
    if ((recv_res = recv(new_socket, buffer, sizeof(buffer), MSG_MORE))) {
      command = parse_command(buffer, args);
      switch (command) {
      case LOGIN: {
        if (logged_in) {
          sendstr(new_socket, LOGIN_FAIL_YOU_ARE_LOGGED_IN);
          break;
        }

        char cmd[MAX_COMMAND_LENGTH] = {0};
        char replbuffer[1024] = {0};

        snprintf(replbuffer, 1024, "/login %s %s %s %s", args[0], args[1],
                 args[2], args[3]);

        int data_sock = connect_data_sock(socket_path);

        sendstr(data_sock, replbuffer);

        int ans = recv(data_sock, cmd, sizeof(cmd), MSG_FIN);
        close(data_sock);

        if (login_parse(cmd))
          logged_in = 0;
        else {
          logged_in = 1;
          strcpy(nick, args[0]);
          strcpy(pass, args[1]);
          strcpy(addr, args[2]);
          strcpy(port, args[3]);
        }
        sendstr(new_socket, cmd);
      } break;

      case LOOKUP_ALL: {

        printf("CLIENT LOOKUP_ALL: INITIALIZING cmd, replbuffer\n");
        char cmd[MAX_COMMAND_LENGTH] = {0};
        char dat[12048] = {0};
        char replbuffer[1024] = {0};

        printf("CLIENT LOOKUP_ALL: sprintf\n");
        snprintf(replbuffer, 1024, "/lookup");

        printf("CLIENT LOOKUP_ALL: connecting to data socket\n");
        int data_sock = connect_data_sock(socket_path);

        printf("CLIENT LOOKUP_ALL: sending replbuffer to data socket\n");
        sendstr(data_sock, replbuffer);

        printf("CLIENT LOOKUP_ALL: recieving ans,cmd from data_sock\n");
        int ans = recv(data_sock, cmd, sizeof(cmd), MSG_MORE);

        printf("CLIENT LOOKUP_ALL: sending cmd to new_sock\n");
        sendstr(new_socket, cmd);

        printf("CLIENT LOOKUP_ALL: sending continue to data_sock\n");
        sendstr(data_sock, "CONT");

        printf("CLIENT LOOKUP_ALL: recieving sans,cmd from data_sock\n");
        int sans = recv(data_sock, dat, 12048, MSG_SYN);
        printf("%s", dat);

        printf("CLIENT LOOKUP_ALL: sending cmd to new_sock\n");
        sendstr(new_socket, dat);

        // printf("CLIENT LOOKUP_ALL: closing data_sock\n");
        // close(data_sock);

        printf("CLIENT LOOKUP_ALL: RETURNING\n");
        break;
      }

      case LOOKUP_SINGLE: {
        printf("CLIENT LOOKUP_SINGLE: INITIALIZING cmd, replbuffer\n");
        char cmd[MAX_COMMAND_LENGTH] = {0};
        char dat[12048] = {0};
        char replbuffer[1024] = {0};

        printf("CLIENT LOOKUP_SINGLE: sprintf\n");
        snprintf(replbuffer, 1024, "/lookup %s", args[0]);

        printf("CLIENT LOOKUP: request to send will be: %s\n", replbuffer);

        printf("CLIENT LOOKUP_SINGLE: connecting to data socket\n");
        int data_sock = connect_data_sock(socket_path);

        printf("CLIENT LOOKUP_SINGLE: sending replbuffer to data socket\n");
        sendstr(data_sock, replbuffer);

        printf("CLIENT LOOKUP_SINGLE: recv on data_sock\n");
        int ans = recv(data_sock, cmd, sizeof(cmd), MSG_MORE);

        printf("CLIENT LOOKUP_SINGLE: sending cmd to new socket\n");
        sendstr(new_socket, cmd);
        printf("CLIENT LOOKUP_SINGLE: parse cmd\n");
        if (!lookup_parse(cmd)) {

          printf("CLIENT LOOKUP_SINGLE: will continue recv\n");
          printf("CLIENT LOOKUP_SINGLE: sending continue to data_sock\n");
          sendstr(data_sock, "CONT");

          printf("CLIENT LOOKUP_SINGLE: recv on data_sock\n");
          int sans = recv(data_sock, dat, sizeof(dat), MSG_MORE);

          printf("CLIENT LOOKUP_SINGLE: sending cmd to client socket\n");
          sendstr(new_socket, dat);
          printf("CLIENT LOOKUP_SINGLE: GOODBYE\n\n");
          break;
        }
        printf("CLIENT LOOKUP_SINGLE: no more to read\n");
        printf("CLIENT LOOKUP_SINGLE: GOODBYE\n\n");
      } break;

      case LOGOUT: {
        logger(LTRACE, col, cfrom, "client_loop", "case LOGOUT");
        logger(LTRACE, col, cfrom, "client_loop", "if not logged_in");
        if (!logged_in) {
          logger(LINFO, col, cfrom, "client_loop",
                 "sendstr to new_socket LOGOUT_FAIL_YOU_ARE_NOT_LOGGED_IN");
          sendstr(new_socket, LOGOUT_FAIL_YOU_ARE_NOT_LOGGED_IN);
          break;
        }

        logger(LTRACE, col, cfrom, "client_loop", "initialize cmd");
        char cmd[MAX_COMMAND_LENGTH] = {0};
        logger(LTRACE, col, cfrom, "client_loop", "initialize replbuffer");
        char replbuffer[1024] = {0};

        // printf("%s %s %s %s\n", nick, pass, addr, port);
        // printf("%s %s %s %s\n", *nick, *pass, *addr, *port);

        logger(LTRACE, col, cfrom, "client_loop",
               "snprintf replbuffer with nick pass addr port");
        snprintf(replbuffer, 1024, "/_logout %s %s %s %s", nick, pass, addr,
                 port);

        logger(LTRACE, col, cfrom, "client_loop",
               "connect_data_sock(socket_path)");
        int data_sock = connect_data_sock(socket_path);

        logger(LTRACE, col, cfrom, "client_loop",
               "sendstr data_sock replbuffer");
        sendstr(data_sock, replbuffer);

        logger(LTRACE, col, cfrom, "client_loop", "recv data_sock itno cmd");
        int ans = recv(data_sock, cmd, sizeof(cmd), MSG_FIN);
        logger(LTRACE, col, cfrom, "client_loop", "close data_sock");
        close(data_sock);

        logger(LTRACE, col, cfrom, "client_loop", "if logout_parse(cmd)");
        if (logout_parse(cmd)) {
          logger(LTRACE, col, cfrom, "client_loop", "logout failed in");
          logger(LTRACE, col, cfrom, "client_loop", "set logged_in = 1");
          logged_in = 1;
        } else {
          logger(LTRACE, col, cfrom, "client_loop", "logout success");
          logger(LTRACE, col, cfrom, "client_loop", "set logged_in = 0");
          logged_in = 0;
          logger(LTRACE, col, cfrom, "client_loop", "set nick empty");
          *nick = (char)"";
          logger(LTRACE, col, cfrom, "client_loop", "set pass empty");
          *pass = (char)"";
          logger(LTRACE, col, cfrom, "client_loop", "set addr empty");
          *addr = (char)"";
          logger(LTRACE, col, cfrom, "client_loop", "set port empty");
          *port = (char)"";
        }
        logger(LTRACE, col, cfrom, "client_loop", "sendstr new_socket cmd");
        sendstr(new_socket, cmd);
      } break;
      case SHOW:
        sendstr(new_socket, "Show!\n");
        break;

      case MSG:
        sendstr(new_socket, "MSG!\n");
        break;

      // case LOOKUP_SINGLE:
      // case LOOKUP_ALL:
      case LOGOUT_SERVER: // NOTE ONLY LOGOUT ALLOWED ON CLIENT
      case ERROR:
        sendstr(new_socket, "ERROR!\n");
        break;

      case EXIT:
        sendstr(new_socket, "EXIT!\n");
        logger(LINFO, col, cfrom, "client_loop", "exit failure");
        return dc_logout(col, nick, pass, addr, port, socket_path, logged_in);
        break;

      default:
        sendstr(new_socket, "WAT!\n");
        send(new_socket, buffer, strlen(buffer), MSG_MORE);
        break;
      }
    } else if (recv_res == 0) {
      logger(LINFO, col, cfrom, "client_loop", "exit failure");
      return dc_logout(col, nick, pass, addr, port, socket_path, logged_in);
    } else {
      logger(LINFO, col, cfrom, "client_loop", "exit unspecified yet");
      return 1;
    }
  }
}
