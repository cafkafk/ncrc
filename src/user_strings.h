/**
 * user_strings.h
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

#ifndef __USER_STRINGS_H_
#define __USER_STRINGS_H_

#define MAX_COMMAND_LENGTH 32

#define CLIENT_CONNECTED "CLIENT CONNECTED\n"

#define LOGIN_OK "LOGIN OK\n"
#define LOGIN_FAIL "LOGIN FAIL\n"
#define LOGIN_FAIL_PUSH "LOGIN FAIL: PUSH\n"
#define LOGIN_FAIL_CREDENTIAL "LOGIN FAIL: CREDENTIALS\n"
#define LOGIN_FAIL_ALREADY_ONLINE "LOGIN FAIL: ALREADY ONLINE\n"
#define LOGIN_FAIL_YOU_ARE_LOGGED_IN "LOGIN FAIL: YOU ARE LOGGED IN\n"

#define LOOKUP_OK "LOOKUP OK\n"
#define LOOKUP_FAIL "LOOKUP FAIL\n"
#define LOOKUP_FAIL_USER_NOT_ONLINE "LOOKUP FAIL: USER NOT ONLINE\n"

#define LOGOUT_OK "LOGOUT OK\n"
#define LOGOUT_FAIL "LOGOUT FAIL\n"
#define LOGOUT_FAIL_NOT_ONLINE "LOGOUT FAIL: NOT ONLINE\n"
#define LOGOUT_FAIL_WRONG_CREDENTIALS "LOGOUT FAIL: WRONG CREDENTIALS\n"
#define LOGOUT_FAIL_REMOVE "LOGOUT FAIL: REMOVE\n"
#define LOGOUT_FAIL_YOU_ARE_NOT_LOGGED_IN "LOGOUT FAIL: YOU ARE NOT LOGGED IN\n"

#define SHOW_OK "SHOW OK\n"
#define SHOW_FAIL "SHOW FAIL\n"

#define MSG_OK "MSG OK\n"
#define MSG_FAIL "MSG FAIL\n"

// This is wierd but just roll with it for now
#define ERROR_OK "MSG OK\n"
#define ERROR_FAIL "MSG FAIL\n"

#define EXIT_OK "EXIT OK\n"
#define EXIT_FAIL "EXIT FAIL\n"

int login_parse(char *cmd);
int logout_parse(char *cmd);
int lookup_parse(char *cmd);
int show_parse(char *cmd);
int msg_parse(char *cmd);

void ncrc();
void license();

#endif // __USER_STRINGS_H_
