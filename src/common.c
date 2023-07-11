// From the A6 source code, by the CompSys course
#include "common.h"

/*
 * below two utility functions are for parsing user input.
 * YOU ONLY NEED A WORKING UNDERSTANDING OF parse_command,
 * and how it stores parsed arguments (see common.h).
 */
command_t parse_command(char *user_input, args_t args) {

  for (int i = 0; i < MAX_USER_ARGNUM; i++)
    args[i] = NULL;

  char *saveptr;
  char *command_str = strtok_r(user_input, " \n", &saveptr);
  if (command_str == NULL)
    return ERROR;

  if (string_equal(command_str,
                   "/msg")) { // if command_str is /msg, recipient username
    args[0] =
        strtok_r(NULL, " \n", &saveptr); // and actual message will be stored in
    args[1] =
        strtok_r(NULL, "\n\x0", &saveptr); // first two arguments, respectively.

    if (args[0] == NULL || args[1] == NULL)
      return ERROR;
    else
      return MSG;
  }

  unsigned int num_args = extract_args(
      saveptr, args); // args now contains arguments to command_str.

  // clang at it again
  return string_equal(command_str, "/login") && num_args == 4
             ? LOGIN
             : string_equal(command_str, "/lookup") && num_args == 0
                   ? LOOKUP_ALL
                   : string_equal(command_str, "/lookup") && num_args == 1
                         ? LOOKUP_SINGLE
                         : string_equal(command_str, "/show") && num_args <= 1
                               ? SHOW
                               : string_equal(command_str, "/logout") &&
                                         num_args == 0
                                     ? LOGOUT
                                     : string_equal(command_str, "/_logout") &&
                                               num_args == 4
                                           ? LOGOUT_SERVER
                                           : string_equal(command_str,
                                                          "/exit") &&
                                                     num_args == 0
                                                 ? EXIT
                                                 : ERROR;
}

/*
 * auxiliary function used by parse_command.
 */
inline size_t extract_args(char *input, args_t args) {

  char *saveptr;
  size_t num_args = 0;
  while ((input = strtok_r(input, " \n\x0", &saveptr)) !=
         NULL) { // and still tokens left to extract

    num_args++;
    if (num_args > MAX_USER_ARGNUM)
      break;

    args[num_args - 1] = input;
    input = NULL; // strtok needs to be called with a null pointer to keep
                  // searching.
  }
  return num_args;
}
