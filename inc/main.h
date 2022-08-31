#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>

#define RES "\x1b[0m"
#define BOLD "\x1b[1m"
#define ERR_CLR  "\x1b[31m\x1b[1m"
#define WARN_CLR "\x1b[95m\x1b[1m"

typedef enum {
    false, true
} bool;


#endif