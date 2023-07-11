/**
 * peer.c
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

/* NOTE,BUG,TODO: user.h from A6's lookup is
 * not able to handle responses above a certain limit,
 * this was considered as an A6 out of scope situation,
 * but it should be remembered. A rewrite *might* not
 * imply a rewrite for this client, as the "packetization"
 * is internal to the server and the TCP socket should
 * just handle the long reply IIRC, but BEWARE. */

#include "peer.h"

/* IPv4 */
#define DOMAIN AF_INET
/* TCP */
#define TCP SOCK_STREAM
/* IP header protocol val */
#define PROTOCOL 0
/* NO_FLAGS */
#define NO_FLAGS 0
/* BUFFER SIZE */
#define BUFFMAX 1024

/* To be in spec, this was added.
 * It makes it so new messages
 * aren't echoed to the client
 * before show. This is not like
 * most IRC client's, and it
 * also has the risk that a message
 * comes in without the user seeing
 * it, even though the person
 * sending might have thought that
 * it was delivered. Then the user
 * might close this client, and
 * lose the message forever when
 * memory is cleared...
 * It is here to comply with spec
 * anyways. */
#define BLINDSIGHT_ON 0

struct User Peers;

/* C makes reusing hard
 * so this "User" is a
 * messages linked list.
 * */
struct User Messages;

/* This is not intuitive, but
 * beats a timed check by a
 * mile */
pthread_mutex_t logged_in;

/* Thread ID for message listener*/
pthread_t tid;

/* P2P_socket file descriptor */
int p2pfd;
struct sockaddr_in p2paddress;

/* User nick when logged in */
char *me;
char default_me[] = "anonymous";

int create_client_socket(char *arg1, char *arg2) {
  /* Create socket file descriptor */
  int sockfd;

  /* Initialize char-arr for addr and port */
  char addr[INET_ADDRSTRLEN], port[5] = {0};
  /* Create socketaddr_in for connecting to server */
  struct sockaddr_in servaddr;

  /* Assign to argv 1 2 */
  snprintf(addr, 16, "%s", arg1);
  snprintf(port, 8, "%s", arg2);

  /* If sockfd fails */
  if ((sockfd = socket(DOMAIN, TCP, PROTOCOL)) < 0) {
    printf("\n Failed making socket \n");
    return EXIT_FAILURE;
  }

  /* Construct server address struct */
  servaddr.sin_family = DOMAIN;
  servaddr.sin_port = htons(atoi(port));

  if (inet_pton(DOMAIN, addr, &servaddr.sin_addr) <= 0) {
    printf("\nAddress wrong\n");
    return EXIT_FAILURE;
  }

  /* Connect */
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    printf("\n Could not connect\n");
    return EXIT_FAILURE;
  }
  return sockfd;
}

/* local */
/* Will not queue on server
 * or anything of the sort.
 * If recipient drops before
 * transaction finishes, the
 * message is lost. */
int msg(char *cmd) {
  int sockfd;

  /* example:
   * /msg nick Rolling crypto should be (relatively) easy, unlike irc! */
  char *nick = strtok(strdup(cmd), " "); /* "/msg" */
  char *msg;
  nick = strtok(NULL, " "); /* nick */
  msg = strtok(NULL, "\n"); /* msg */

  /* Before doing more, lets assure ourselves */
  if (nick_in_list((struct User **)&Peers, nick)) {
    printf("%s not online.\n", nick);
    return EXIT_FAILURE;
  }

  /* Magic number from lookup nick (A6) */
  /* Refactor out of scope for A7 unfortunately */
  char res[2048] = {0};

  /* DANGER ZONE */
  strcpy(res, lookup_nick((struct User **)&Peers, nick));
  /* DANGER ZONE */

  /* ONLINE generalized suprisingly well... */
  if (!strcmp(res, LOOKUP_FAIL_USER_NOT_ONLINE)) {
    printf("%s not online.\n", nick);
    return 0;
  }

  char *addr, *port;
  char *tmp = strtok(strdup(res), ":");
  tmp = strtok(NULL, ":");
  addr = strdup(tmp);

  tmp = strtok(NULL, ":");
  port = tmp;

  addr = strtok(addr, " ");
  addr = strtok(addr, " ");

  port = strtok(port, "\n");

  sockfd = create_client_socket(addr, port);

  /* We don't do this unless everything else we can
   * confirm is good, because it is more complex,
   * that's why many things in general seem to have
   * some absolutely wierd order. */
  char buff[BUFFMAX] = {0};

  /* Remember that our A6 reference document does not specify
   * any p2p at all */
  /* Identity is not safe, it can trivially be spoofed,
   * but this has the "advantage" of being bare bones,
   * and as such, the send nick could, with larger
   * buffers be a pgp signature or what have you. */
  /* The name server could have been used as an
   * authenticating source, but watch Poul Henning Kamp,
   * fossdem 14, we don't want central authentication!
   * It should only be done if there was ever implemented
   * messaging over the server, we'll let p2p spec
   * be more loose as to provide some room for
   * growth. */
  strncat(buff, me, BUFFMAX);
  strncat(buff, ": ", BUFFMAX);
  strncat(buff, msg, BUFFMAX); /* "Safe" */
  /* Should message be truncated or discarded */
  /* NOTE: not "safe default" (not truncated is safe default imo)
   * because it would be annoying. Argument is
   * least friction. */
  if (sizeof(msg) > BUFFMAX) {
    /* TODO */
    /* Out of scope, but here we could create
     * a break up into BUFFMAX chunks and send
     * them iteratively, but it is so out of
     * scope, and who sends more than 1024 on
     * IRC unless sharing "data"? "1024 Ought to
     * be enough for anybody". */
    printf("Message to long, should it be send truncated as:\n%s\n");
    /* NOTE: Recipient connection might die in the meanwhile */
    char yn[BUFFMAX] = {0}; /* FIXME, more realistic */
    fgets(yn, BUFFMAX, stdin);
    if (strncmp(yn, "n", 1))
      if (strncmp(yn, "N", 1)) {
        /* Let's be nice and tell them*/
        close(sockfd);
        return 0;
      }
  }

  send(sockfd, buff, BUFFMAX, NO_FLAGS);
  close(sockfd);
  return 0;
}

int show(char *cmd) {
  if (pthread_mutex_trylock(&logged_in) == 0) {
    printListNick(&Messages);
    /* Remove queue. Looks stupid,
       but easy on computation. */
    struct User newMessages;
    Messages = newMessages;
    pthread_mutex_unlock(&logged_in);
  } else {
    printf("Not logged in or currently recieving messages...\n");
    printf("Try again in a moment.\n");
  }
  return 0;
}

int create_listener_socket(char *addrarg, char *portarg) {
  /* Create socket file descriptor */
  int sockfd;

  /* Initialize char-arr for addr and port */
  char addr[INET_ADDRSTRLEN], port[5] = {0};

  /* Assign to args */
  snprintf(addr, 16, "%s", addrarg);
  snprintf(port, 8, "%s", portarg);

  /* Create socketaddr_in */

  p2paddress.sin_family = DOMAIN;
  p2paddress.sin_addr.s_addr = INADDR_ANY;
  p2paddress.sin_port = htons(atoi(port));
  // AF_INET: ipv4, SOCK_STREAM: TCP, 0, ip protocol
  if ((sockfd = socket(DOMAIN, TCP, NO_FLAGS)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (int[]){1},
                 sizeof(int))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  if (bind(sockfd, (struct sockaddr *)&p2paddress, sizeof(p2paddress)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  return sockfd;
}

/* inbound */
int lookup_ok(char *cmd, char *res) {
  /* This is actually the simplest solution.
   * It might look dirty, it might feel dirty
   * but it is optimal. */
  struct User newPeers;
  Peers = newPeers;

  /* Lines to skip */
  /* First one-two lines are irrelavant
   * depending on single or all user
   * lookup request. */
  int offset = 0;

  /* Because we don't want to
   * crash on "/lookup " it's
   * 10 not 9 */
  if (strlen(cmd) >= 10)
    /* Online users only returned
     * on lookup of all */
    offset = offset + 1;

  /*  */
  char *line = res;
  char *tok;

  /* Loop through each line recieved from lookup
   * and push users that are online into our
   * local memory copy. */
  while ((tok = strtok_r(line, "\n", &line))) {
    if (offset > 1) {
      char *nick, *addr, *port;
      char *pass = ""; /* NOT USED */
      char *usr = strtok(strdup(tok), ":");
      nick = usr;
      usr = strtok(NULL, ":");
      addr = usr;
      usr = strtok(NULL, ":");
      port = usr;
      push((struct User **)&Peers, nick, pass, addr, port);
    } else
      offset++; /* We don't want to do this each iteration */
  }
  return EXIT_SUCCESS;
}

/* We don't care to do something
 * about errors here, as in, it
 * would just be annoying if we
 * told the user each time, and
 * could be a vector for spamming.
 * This is in contrast to sending */
void *peer_handler(void *peerfd) {
  char buff[BUFFMAX] = {0};
  pthread_mutex_lock(&logged_in);
  recv((int)peerfd, buff, BUFFMAX, NO_FLAGS);
  /* User repurposed for messages i know it is
   * disgusting^w brilliant, no worries. */
  push((struct User **)&Messages, buff, "", "", "");
  close((int)peerfd);
  if (!BLINDSIGHT_ON)
    printf("%s\n", buff);
  /* C reusability. Some would
   * make this into a struct to avoid
   * the fact that C can't handle
   * argument overloading, other
   * simply use other langs. */
  pthread_mutex_unlock(&logged_in);
  pthread_exit(EXIT_SUCCESS);
}

/* Handle inbound peer messages */
/* We ultimately just let most
 * errors happen, as we don't
 * need to stop for them, don't
 * want to tell the user, BUT
 * we do remember the critical
 * errors. */
void *p2p_handler() {
  int peerfd;
  while (true) {
    /* We will detach anyways */
    pthread_t temp;

    if (listen(p2pfd, 3) < 0) {
      printf("listening failed\n"); /* FIXME */
      continue;
    }

    unsigned int p2paddresslen = sizeof(p2paddress);
    if ((peerfd = accept(p2pfd, (struct sockaddr *)&p2paddress,
                         &p2paddresslen)) < 0) {
      printf("accept failed\n"); /* FIXME */
      continue;
    }

    if (pthread_create(&temp, NULL, peer_handler, (void *)peerfd)) {
      printf("create failed\n"); /* FIXME */
      continue;
    }
  }
}

int login_ok(char *cmd, char *res) {
  /* Initialize char-arr for addr and port */
  char *addr, *port;

  printf("We're in...\n");

  pthread_mutex_unlock(&logged_in);

  char *tok = strtok(strdup(cmd), " ");

  /* /login */
  tok = strtok(NULL, " ");

  /* nick */
  tok = strtok(NULL, " ");
  me = tok;

  /* pass */
  tok = strtok(NULL, " ");

  /* addr */
  addr = tok;
  tok = strtok(NULL, " ");

  /* port */
  port = tok;

  /* Create socket */
  p2pfd = create_listener_socket(addr, port);

  /* Create thread for recieving p2p messages */
  pthread_create(&tid, NULL, p2p_handler, (void *)&tid);
  return EXIT_SUCCESS;
}

int logout_ok(char *cmd) {
  me = default_me;
  pthread_mutex_lock(&logged_in);
  return EXIT_SUCCESS;
}

/* Show the digest of Peers; what we ate */
int weate() {
  printList(&Peers, 0);
  return 0; /* Skip networking */
}

/* Routes local commands */
/* -1 errs, 0 skip networking */
int filter_out(char *cmd, int sockfd) {
  if (strlen(cmd) > 4)
    if (!strncmp(cmd, "/msg ", 5))
      return msg(cmd);
  if (strlen(cmd) > 4)
    if (!strncmp(cmd, "/show ", 5))
      return show(cmd);
  if (strlen(cmd) > 9)
    if (!strncmp(cmd, "/makeerror", 10))
      return -1;
  if (strlen(cmd) > 5)
    if (!strncmp(cmd, "/weate", 6))
      return weate();
  return sockfd;
}

/* Handle inbound signals */
int filter_in(char *cmd, char *res) {
  if (strlen(res) > 8)
    if (!strncmp(res, "LOOKUP OK", 9))
      return lookup_ok(cmd, res);
  if (strlen(res) > 7)
    if (!strncmp(res, "LOGIN OK", 8))
      return login_ok(cmd, res);
  if (strlen(res) > 8)
    if (!strncmp(res, "LOGOUT OK", 9))
      return logout_ok(res);
  return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
  /* Early exit */
  if (argc != 3) {
    /* FIXME usage string */
    return EXIT_FAILURE;
  }

  char buff[BUFFMAX] = {0};

  /* Create socket file descriptor */
  int sockfd;

  me = default_me;

  if ((sockfd = create_client_socket(argv[1], argv[2])))
    ;

  /* Get first CLIENT CONNECTED */
  recv(sockfd, buff, BUFFMAX, NO_FLAGS);
  if (!strcmp(buff, "CLIENT CONNECTED"))
    /* Failure */
    return EXIT_FAILURE;
  /* Success */
  printf("%s", buff);

  /* Initialize mutex for logging in */
  pthread_mutex_init(&logged_in, NULL);

  /* Not logged in initially */
  pthread_mutex_lock(&logged_in);

  /* REPL */
  while (true) {
    /* Read */
    char cmd[BUFFMAX] = {0};
    char res[BUFFMAX] = {0};
    int sig = -1; /* Signal at sentinel */

    /* Prompt */
    printf(">");
    /* !!!DANGER ZONE!!! */
    if (fgets(cmd, BUFFMAX, stdin) == NULL)
      return EXIT_FAILURE; /* e.g. ctrl-d */
    /* !!!DANGER ZONE EXITED!!! */

    /* EVAL */
    /* Change sig if local cmd detected */
    if ((sig = filter_out(cmd, sockfd)) < 0)
      return EXIT_FAILURE;
    else if (sig == 0)
      continue; /* Everything was handled locally */
    else {
      send(sockfd, cmd, BUFFMAX, NO_FLAGS);
      recv(sockfd, res, BUFFMAX, NO_FLAGS);
      /* PRINT */
      if (filter_in(cmd, res) == EXIT_SUCCESS)
        printf("%s", res);
      else
        printf("%s", res);
    }
    /* LOOP */
  }
  return EXIT_SUCCESS;
}
