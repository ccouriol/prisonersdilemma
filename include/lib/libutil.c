/*!
 * \file libutil.c
 * \author Clément Couriol
 * \version 0.1
 * \date 15/12/2021
 * \brief
 * \remarks None
 *
 *
 *
 */

/*! Importation of librairies*/
#include "./libutil.h"

/*!
 * \fn char *read_config(char *param)
 * \author Clément Couriol
 * \version 0.1
 * \date  15/12/2021
 * \brief Read a parameter from the configuration file
 * \remarks None
 * \param param
 * \return
 */
char *read_config(char *param) {
  config_t cfg;
  char *str1;
  char *config_file_name = "config.txt";

  config_init(&cfg);

  /* Read the file. If there is an error, report it and exit. */
  if (!config_read_file(&cfg, config_file_name))
    printf("\n%s:%d - %s", config_error_file(&cfg), config_error_line(&cfg),
           config_error_text(&cfg));

  if (config_lookup_string(&cfg, param, &str1))
    return str1;

  config_destroy(&cfg);
  return NULL;
}

/*!
 * \fn int verifyIP(char *string_ip)
 * \author Clément Couriol
 * \version 0.1
 * \date  01/12/2021
 * \brief Returns 0 if the ip is valid, 1 if not
 * \remarks None
 * \param string_ip
 * \return
 */
int verifyIP(char *string_ip) {
  regex_t regex;
  int reti;
  char *rgx = "^(\\b25[0-5]|\\b2[0-4][0-9]|\\b[01]?[0-9][0-9]?)(\\.(25[0-5]|2["
              "0-4][0-9]|[01]?[0-9][0-9]?)){3}$";

  reti = regcomp(&regex, rgx, REG_EXTENDED);
  if (reti)
    return EXIT_FAILURE;

  reti = regexec(&regex, string_ip, 0, NULL, 0);
  if (!reti)
    return EXIT_SUCCESS;
  else if (reti == REG_NOMATCH)
    return EXIT_FAILURE;
  else
    return EXIT_FAILURE;

  regfree(&regex);
}