// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
#include "commands.h"
#include "signals.h"
#include "job.h"

int cur_fg_pid;
char cur_fg_cmdString[MAX_LINE_SIZE];

void set_foreground(int fg_pid){
      cur_fg_pid = fg_pid;
}

void set_fg_cmdString(char cmdString[MAX_LINE_SIZE]) {
    strcpy(cur_fg_cmdString, cmdString);
}

/* Name: handler_cntlc
   Synopsis: handle the Control-C */
void handler_cntlc(int sig_num){
   if (cur_fg_pid != 0){
      printf("smash: got ctrl-C\n");
       if (kill(cur_fg_pid, SIGKILL) != 0) {
           perror("smash error: kill failed");
           return;
       }

       set_foreground(0);
       char empty[MAX_LINE_SIZE] = {'\0'};
       set_fg_cmdString(empty);
   }
}

/* Name: handler_cntlz
   Synopsis: handle the Control-Z */
void handler_cntlz(int sig_num){
    printf("smash: got ctrl-Z\n");
    if (cur_fg_pid != 0){
        printf("[DEBUG] Foreground process found!\n");

        CleanJobs(jobs);

        int new_id;
        time_t start = time(NULL);
        if (!jobs.empty())
            new_id = jobs[jobs.size()-1].getJobId() + 1;
        else new_id = 1;

        job new_job = job(new_id, cur_fg_pid, cur_fg_cmdString, true, start, 0);
        jobs.push_back(new_job);


        if (kill(cur_fg_pid, SIGSTOP) != 0) {
            perror("smash error: kill failed");
            return;
        }

        set_foreground(0);
        char empty[MAX_LINE_SIZE] = {'\0'};
        set_fg_cmdString(empty);
    }
}