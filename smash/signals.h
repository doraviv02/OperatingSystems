#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

extern int cur_fg_pid;
extern char cur_fg_cmdString[MAX_LINE_SIZE];

void set_foreground(int fg_pid);
void set_fg_cmdString(char cmdString[MAX_LINE_SIZE]);
void handler_cntlc(int sig_num);
void handler_cntlz(int sig_num);
#endif

