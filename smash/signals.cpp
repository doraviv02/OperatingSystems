// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
#include "signals.h"

int cur_fg_pid;

void set_foreground(int fg_pid){
      cur_fg_pid = fg_pid;
}


/* Name: handler_cntlc
   Synopsis: handle the Control-C */
void handler_cntlc(int sig_num){
   if (cur_fg_pid != 0){
      printf("smash: got ctrl-C\n");
      kill(cur_fg_pid, SIGKILL);
   }
    }

/* Name: handler_cntlz
   Synopsis: handle the Control-Z */
void handler_cntlz(int sig_num){
   if (cur_fg_pid != 0){
      printf("smash: got ctrl-Z\n");
   }
    }