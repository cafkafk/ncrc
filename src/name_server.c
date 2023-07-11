/**
 * name_server.c
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

#include "name_server.h"

char listen_port[PORT_LEN];
char *socket_path = "data";

int num_active_clients = 0; // TODO max number of connections

pthread_mutex_t mutex;

int goodbye() {
  printf("Goodbye!\n");
  close(*socket_path);
  return 1;
}

int users_online = 0;
int users_accounts = 0;

char *from = "name_server.c";

int col = 0;

struct User *accounts = NULL;
struct User *online = NULL;

int main(int argc, char **argv) {
  license();
  ncrc();

  logger(LINFO, col, from, "main", "enterd");
  logger(LDEBUG, col, from, "main", "checking args");
  if (argc != MAIN_ARGNUM + 1) {
    logger(LFATAL, col, from, "main", "not enough args");
    logger(LTRACE, col, from, "main", "printing usage string");
    fprintf(stderr, "Usage: %s <listening port>\n", argv[0]);
    logger(LTRACE, col, from, "main", "exiting with failure");
    exit(EXIT_FAILURE);
  } else if (in_range(argv[1], 1, 65535)) {
    logger(LFATAL, col, from, "main", "invalid port number");
    logger(LTRACE, col, from, "main", "printing invalid port string");
    fprintf(stderr, ">> Invalid port number: %s\n", argv[1]);
    logger(LTRACE, col, from, "main", "exiting with failure");
    exit(EXIT_FAILURE);
  }
  logger(LTRACE, col, from, "main", "initializing child_pid, wpid");
  pid_t child_pid, wpid;
  logger(LTRACE, col, from, "main", "initializing status");
  int status = 0;
  logger(LINFO, col, from, "main", "forking");
  if (fork() != 0) {
    logger(LTRACE, col, from, "main grandma", "registering atexit goodbye");
    atexit((void *)goodbye);

    logger(LDEBUG, col, from, "main grandma", "awaiting 'main clients' exit");
    while ((wpid = wait(&status)) > 0)
      ;
    logger(LTRACE, col, from, "main grandma", "exit success");
    exit(EXIT_SUCCESS);
    // This will be the "gameloop" process
  } else {
    logger(LDEBUG, col, from, "main clients", "initializing mutex");
    pthread_mutex_init(&mutex, NULL);

    logger(LDEBUG, col, from, "main clients",
           "snprintf argv[1] to listen_port");
    snprintf(listen_port, PORT_LEN, "%s", argv[1]);

    logger(LTRACE, col, from, "main clients", "initializing serv_sock");
    int serv_sock;

    logger(LTRACE, col, from, "main clients", "initializing address");
    struct sockaddr_in address;

    logger(LTRACE, col, from, "main clients",
           "initializing addrelen as sizeoff(address)");
    int addrlen = sizeof(address);

    logger(LINFO, col, from, "main clients", "serv_sock = create_net_socket");
    serv_sock = create_net_socket(serv_sock, &address, listen_port);

    logger(LTRACE, col, from, "main clients", "initialize data_sock");
    int data_sock;

    logger(LTRACE, col, from, "main clients", "initialize data_name");
    struct sockaddr_un data_name;

    logger(LTRACE, col, from, "main clients",
           "initialize data_name_len as sizeof(data_name)");
    int data_name_len = sizeof(data_name);

    logger(LINFO, col, from, "main clients", "data_sock = create_named_socket");
    data_sock = create_named_socket(data_sock, &data_name, socket_path);

    logger(LINFO, col, from, "main clients", "forking");
    if (fork() == 0) {
      logger(LDEBUG, col, from, "main clients data", "closing serv_sock");
      close(serv_sock);

      logger(LTRACE, col, from, "main clients data", "initialiing data_status");
      int data_status;
      logger(LDEBUG, col, from, "main clients data",
             "listening on data socket");
      if (listen(data_sock, 3) < 0) {
        logger(LERROR, col, from, "main clients data",
               "error listening on data socket");
        logger(LTRACE, col, from, "main clients data", "exiting with failure");
        exit(EXIT_FAILURE);
      }
      logger(LINFO, col, from, "main clients data",
             "read_account of &accounts and accounts.txt");
      read_accounts_file(&accounts, "accounts.txt");

      logger(LINFO, col, from, "main clients data", "printing accounts");
      printList(accounts, 1); // FIXME second arg

      logger(LINFO, col, from, "main clients data", "entering data loop");
      while (1) {
        logger(LTRACE, col, from, "main clients data",
               "initializing client_socket");
        int client_socket;

        logger(LDEBUG, col, from, "main clients data",
               "waiting client_socket accept");
        if ((client_socket = accept(data_sock, (struct sockaddr *)&data_name,
                                    (socklen_t *)&data_name_len)) < 0) {

          logger(LERROR, col, from, "main clients data",
                 "error accepting data socket");

          logger(LTRACE, col, from, "main clients data",
                 "exiting with failure");
          exit(EXIT_FAILURE);
        }

        logger(LDEBUG, col, from, "main clients data", "run dataLoop");
        data_status = data_loop(client_socket, col, mutex, &accounts, &online);
      }
      if (data_status)
        logger(LDEBUG, col, from, "main clients data",
               "data socket goodbye, no issues");

      else
        logger(LWARN, col, from, "main clients data",
               "data socket goodbye, some issues");

    }
    // CLIENT HANDLER
    else { // parent
      logger(LINFO, col, from, "main clients client",
             "starting client handler loop");
      while (1) {
        logger(LINFO, col, from, "main clients client", "listen on serv_sock");
        if (listen(serv_sock, 3) < 0) {
          logger(LWARN, col, from, "main clients client",
                 "error listening server socket");
          logger(LTRACE, col, from, "main clients client", "exiting failure");
          exit(EXIT_FAILURE);
        }
        logger(LTRACE, col, from, "main clients client",
               "initializing new_socket");
        int new_socket;

        logger(LINFO, col, from, "main clients client", "accept new socket");
        if ((new_socket = accept(serv_sock, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0) {
          logger(LFATAL, col, from, "main clients client",
                 "failed to accept serv_sock");
          logger(LTRACE, col, from, "main clients client", "exiting failure");
          exit(EXIT_FAILURE);
        }

        logger(LINFO, col, from, "main clients client",
               "creating client  fork");
        if (fork() == 0) {
          logger(LDEBUG, col, from, "main clients client child",
                 "created client  fork");
          logger(LDEBUG, col, from, "main clients client child",
                 "closing serv_sock");
          close(serv_sock);
          logger(LINFO, col, from, "main clients client child",
                 "entering client loop");
          logger(LTRACE, col, from, "main clients client child",
                 "sendstr CLIENT HELLO");
          sendstr(new_socket, CLIENT_CONNECTED);
          if (client_loop(new_socket, col, socket_path))
            logger(LDEBUG, col, from, "main clients client child",
                   "client goodbye, no issues");
          else
            logger(LWARN, col, from, "main clients client child",
                   "client goodbye, some issues");

        } else { // parent
          logger(LDEBUG, col, from, "main clients client",
                 "closing new_socket");
          close(new_socket);
          logger(LINFO, col, from, "main clients client",
                 "new client connected");
        }
      }
    }
  }
}
