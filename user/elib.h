#ifndef ELIB_H
#define ELIB_H

void exit(void);
void create(void (*entry)(void), const char *name);

#endif