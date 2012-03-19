#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "object.h"

void serialize(COObject *v, FILE *f);
COObject *unserialize(FILE *f);

#endif
