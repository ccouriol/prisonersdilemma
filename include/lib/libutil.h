#include <libconfig.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef LIBUTIL_H
#define LIBUTIL_H

char *read_config(char *param);
int verifyIP(char *string_ip);

#endif /* LIBUTIL_H */