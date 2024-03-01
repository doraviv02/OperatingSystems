// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
#include "commands.h"
#include "signals.h"
#include "job.h"

int cur_fg_pid;
int cur_fg_jobID;
char cur_fg_cmdString[MAX_LINE_SIZE];

void set_foreground(int fg_pid){
      cur_fg_pid = fg_pid;
}

void set_fg_cmdString(char cmdString[MAX_LINE_SIZE]) {
    strcpy(cur_fg_cmdString, cmdString);
}

void set_fg_jobID(int jobID) {
    cur_fg_jobID = jobID;
}

/* Name: handler_cntlc
   Synopsis: handle the Control-C */
void handler_cntlc(int sig_num){
     printf("smash: caught ctrl-C\n");
     if (cur_fg_pid != 0) {
       if (kill(cur_fg_pid, SIGKILL) != 0) {
           perror("smash error: kill failed");
           return;
       }

       printf("process %d was killed\n", cur_fg_pid);
       set_foreground(0);
       char empty[MAX_LINE_SIZE] = {'\0'};
       set_fg_cmdString(empty);
     }
     else {
         printf("smash > ");
     }
}

/* Name: handler_cntlz
   Synopsis: handle the Control-Z */
void handler_cntlz(int sig_num){
     printf("smash: caught ctrl-Z\n");
     if (cur_fg_pid != 0){
        printf("[DEBUG] Foreground process found!\n");

        CleanJobs(jobs);

        int jobID;
        if (cur_fg_jobID == 0) {
            if (!jobs.empty())
                jobID = jobs[jobs.size() - 1].getJobId() + 1;
            else jobID = 1;
        }
        else {
            jobID = cur_fg_jobID;
        }

        time_t start = time(NULL);

        job new_job = job(jobID, cur_fg_pid, cur_fg_cmdString, true, start, 0);
        jobs.push_back(new_job);


        if (kill(cur_fg_pid, SIGSTOP) != 0) {
            perror("smash error: kill failed");
            return;
        }
        printf("process %d was stopped\n", cur_fg_pid);

        set_foreground(0);
        set_fg_jobID(0);
        char empty[MAX_LINE_SIZE] = {'\0'};
        set_fg_cmdString(empty);
    }
     else {
         printf("smash > ");
     }
}