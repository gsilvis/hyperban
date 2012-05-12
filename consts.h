#ifndef __HYPERBAN__CONSTS_H
#define __HYPERBAN__CONSTS_H

#include <math.h>

/* This number is the distance from the origin to the corner of a square with
   72-degree angles.  You'll probably want to divide it by sqrt(2) when using
   it for things. */
#define MAGIC 0.687121499445025

#define MORE_MAGIC (MAGIC/sqrtf(2.0))

#endif /* __HYPERBAN__CONSTS_H */
