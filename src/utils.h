#ifndef UTILS_H

#ifdef HANDMADE_SLOW
#define Assert(expression) if(!(expression)) { *(int *)0 = 0;}
#else
#define Assert(expression)
#endif

#define kilobytes(value) ((value)*1024)
#define megabytes(value) (kilobytes(value)*1024)
#define gigabytes(value) (megabytes(value)*1024)

#define NEW(type) (type*) malloc(sizeof(type))

#define UTILS_H
#endif // UTILS_H