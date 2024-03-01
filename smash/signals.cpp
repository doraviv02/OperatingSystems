// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
#include "commands.h"
#include "signals.h"
#include "job.h"

#define MAX_LINE_SIZE 80

int cur_fg_pid;
char cmdString[MAX_LINE_SIZE];

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
    printf("smash: got ctrl-Z\n");
    if (cur_fg_pid != 0){
        printf("[DEBUG] Foreground process found!\n");

        CleanJobs(jobs);

        //TODO: Add job, handle cmdString for foreground process
        int new_id;
        time_t start = time(NULL);
        if (!jobs.empty())
            new_id = jobs[jobs.size()-1].getJobId() + 1;
        else new_id = 1;

        job new_job = job(new_id, cur_fg_pid, cmdString, true, start, 0);
        jobs.push_back(new_job);

        if (kill(cur_fg_pid, SIGSTOP) != 0) {
            perror("smash error: kill failed");
            return;
        }
        set_foreground(0);
   }
}