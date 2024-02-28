#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void set_foreground(int fg_pid);
void handler_cntlc(int sig_num);
void handler_cntlz(int sig_num);
#endif

