/**
 * name_server.h
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

#pragma once

#include "client_handler.h"
#include "data_handler.h"
#include "logger.h"
#include "named_socket.h"
#include "net_socket.h"
#include "read_login.h"
#include "socket_utils.h"
#include "user.h"
#include "user_strings.h"

#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

#define MAIN_ARGNUM 1

// Defines moved
