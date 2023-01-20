#include <stdio.h>
#include <stdbool.h>

#include "Shell.h"
#include "StringVector.h"

#ifndef TRAVAIL_H
#define TRAVAIL_H

struct Travail {
    pid_t    pid;
    char * name;
    bool running;
};

void travail_init( struct Travail *t );
void travail_run( struct Shell *shell, const struct StringVector *vector);
void travail_free( struct Travail *t );

#endif /* TRAVAIL_H */