//		commands.cpp
//********************************************
#include "commands.h"
#include "job.h"
#include "signals.h"
#include <iostream>
#include <linux/limits.h>
#include <algorithm>
#define DIFF_ARGS 2

using namespace std;
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
char old_pwd[PATH_MAX] = "";

int ExeCmd(vector<job> &jobs, char* args[MAX_ARG], int num_arg)
{
	char* cmd = args[0];
    char cwd[PATH_MAX];
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
    CleanJobs(jobs);

    if (!strcmp(cmd, "cd") )
	{
 		if (num_arg > 1) printf("smash error: cd: too many arguments\n");
		else{
			if(strcmp(args[1], "-") == 0){
				if (strlen(old_pwd) == 0)
					printf("smash error: cd: OLDPWD not set\n");
				else{

                    if (getcwd(cwd, PATH_MAX) == NULL) {
                        perror("smash error: getcwd failed");
                        return CMD_RETURN_ERR;
                    }

					if (chdir(old_pwd) == -1) {
                        perror("smash error: chdir failed");
                        return CMD_RETURN_ERR;
                    }
					strcpy(old_pwd, cwd);
				}

			}
			else{
                getcwd(old_pwd, PATH_MAX);
				chdir(args[1]);
			}
		}
	} 
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		if (getcwd(cwd, MAX_LINE_SIZE) == NULL) {
            perror("smash error: getcwd failed");
            return CMD_RETURN_ERR;
        }
		printf("%s\n", cwd);
	}
	/*************************************************/
	else if (!strcmp(cmd, "jobs")) 
	{
        sort(jobs.begin(), jobs.end(), compareJobsByJobID);
		int job_id, pid;
		char* command;
		bool isStopped;
		time_t start;
		time_t* now = new time_t;
		time(now);

        for (auto it = jobs.begin(); it != jobs.end(); it++) {
            job_id = it->getJobId();
            pid = it->getPid();
            command = it->getCommand();
            isStopped = it->getIsStopped();
            start = it->getStart();

            if (isStopped){
                printf("[%d] %s : %d %d secs (Stopped)\n", job_id, command, pid, (int) difftime(*now, start));
            }
            else printf("[%d] %s : %d %d secs\n", job_id, command, pid, (int) difftime(*now, start));
        }
		
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		pid_t pid = getpid();
		printf("smash pid is %d\n", pid);
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
        int jobID, selPID;
        vector<job>::iterator it;

		if (num_arg > 1) {
            fprintf(stderr, "smash error: fg: invalid arguments\n");
            return CMD_RETURN_ERR;
        }
        else if (num_arg == 1) {
            int arg_len = strlen(args[1]);
            for (int i = 0; i < arg_len; i++) {
                if (!isdigit(args[1][i])) {
                    fprintf(stderr, "smash error: fg: invalid arguments\n");
                    return CMD_RETURN_ERR;
                }
            }

            jobID = stoi(args[1]);
            int foundJob = 0;
            for (it = jobs.begin(); it != jobs.end(); it++) {
                if (it->getJobId() == jobID) {
                    foundJob = 1;
                    break;
                }
            }
            if (!foundJob) {
                fprintf(stderr, "smash error: fg: job-id %d does not exist\n", jobID);
                return CMD_RETURN_ERR;
            }
        }
        else {
            if (jobs.size() == 0) {
                fprintf(stderr, "smash error: fg: jobs list is empty\n");
                return CMD_RETURN_ERR;
            }

            int maxJobID = 0;
            for (it = jobs.begin(); it != jobs.end(); it++) {
                if (it->getJobId() > maxJobID) {
                    maxJobID = it->getJobId();
                }
            }

            for (it = jobs.begin(); it != jobs.end(); it++) {
                if (it->getJobId() == maxJobID) {
                    break;
                }
            }
        }

        selPID = it->getPid();
        char *cmdString = it->getCommand();

        printf("%s : %d\n", cmdString, selPID);

        set_foreground(selPID);
        set_fg_cmdString(cmdString);
        set_fg_jobID(jobID);

        jobs.erase(it);

        if (kill(selPID, SIGCONT) != 0) {
            perror("smash error: kill failed");
            return CMD_RETURN_ERR;
        }

        if (waitpid(selPID, NULL, WUNTRACED) == -1) {
            perror("smash error: waitpid failed");
            return CMD_RETURN_ERR;
        }

        set_foreground(0);
        char empty[MAX_LINE_SIZE] = {'\0'};
        set_fg_cmdString(empty);

	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
        int jobID, selPID;
        vector<job>::iterator it;

        if (num_arg > 1) {
            fprintf(stderr, "smash error: bg: invalid arguments\n");
            return CMD_RETURN_ERR;
        }
        else if (num_arg == 1) {
            int arg_len = strlen(args[1]);
            for (int i = 0; i < arg_len; i++) {
                if (!isdigit(args[1][i])) {
                    fprintf(stderr, "smash error: bg: invalid arguments\n");
                    return CMD_RETURN_ERR;
                }
            }

            jobID = stoi(args[1]);
            int foundJob = 0;
            for (it = jobs.begin(); it != jobs.end(); it++) {
                if (it->getJobId() == jobID) {

                    if (!it->getIsStopped()) {
                        fprintf(stderr, "smash error: bg: job-id %d is already running in the background\n", jobID);
                        return CMD_RETURN_ERR;
                    }

                    foundJob = 1;
                    break;
                }
            }
            if (!foundJob) {
                fprintf(stderr, "smash error: bg: job-id %d does not exist\n", jobID);
                return CMD_RETURN_ERR;
            }
        }
        else {
            if (jobs.size() == 0) {
                fprintf(stderr, "smash error: bg: there are no stopped jobs to resume\n");
                return CMD_RETURN_ERR;
            }

            int maxJobID = 0;
            for (it = jobs.begin(); it != jobs.end(); it++) {
                if (it->getJobId() > maxJobID && it->getIsStopped()) {
                    maxJobID = it->getJobId();
                }
            }

            if (!maxJobID) {
                fprintf(stderr, "smash error: bg: there are no stopped jobs to resume\n");
                return CMD_RETURN_ERR;
            }

            for (it = jobs.begin(); it != jobs.end(); it++) {
                if (it->getJobId() == maxJobID) {
                    break;
                }
            }
        }

        selPID = it->getPid();
        char *cmdString = it->getCommand();

        printf("%s : %d\n", cmdString, selPID);

        it->setIsStopped(false);

        if (kill(selPID, SIGCONT) != 0) {
            perror("smash error: kill failed");
            return CMD_RETURN_ERR;
        }

	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
        if (num_arg == 0 || jobs.size() == 0) return CMD_RETURN_QUIT;

        if (!strcmp(args[1], "kill")) {
            sort(jobs.begin(), jobs.end(), compareJobsByJobID);

            vector<job>::iterator it;
            for (it = jobs.begin(); it != jobs.end(); ) {
                int pid = it->getPid();

                printf("[%d] %s - Sending SIGTERM... ", it->getJobId(), it->getCommand());

                if (kill(pid, SIGTERM) != 0) {
                    perror("smash error: kill failed");
                    return CMD_RETURN_ERR;
                }

                time_t start = time(NULL);
                time_t now = time(NULL);
                while (waitpid(pid, NULL, WNOHANG) == 0 && difftime(now, start) < SIGTERM_TIMEOUT) {
                    now = time(NULL);

                    sleep(1);
                }

                if (difftime(now, start) >= SIGTERM_TIMEOUT) {
                    printf("(5 sec passed) Sending SIGKILL... ");
                    if (kill(pid, SIGKILL) != 0) {
                        perror("smash error: kill failed");
                        return CMD_RETURN_ERR;
                    }
                }

                printf("Done.\n");

                it = jobs.erase(it);
            }

            return CMD_RETURN_QUIT;
        }
	} 
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
   		if (num_arg != 2 || args[1][0] != '-') {
            fprintf(stderr, "smash error: kill: invalid arguments\n");
            return CMD_RETURN_ERR;
        }

        int arg_len = strlen(args[1]);
        for (int i = 1; i < arg_len; i++) {
            if (!isdigit(args[1][i])) {
                fprintf(stderr, "smash error: kill: invalid arguments\n");
                return CMD_RETURN_ERR;
            }
        }

        arg_len = strlen(args[2]);
        for (int i = 0; i < arg_len; i++) {
            if (!isdigit(args[2][i])) {
                fprintf(stderr, "smash error: kill: invalid arguments\n");
                return CMD_RETURN_ERR;
            }
        }

        int signum, jobID, pid;
        sscanf(args[1], "-%d", &signum);
        sscanf(args[2], "%d", &jobID);

        printf("[DEBUG] signum = %d jobID = %d\n", signum, jobID);

        if (jobs.size() == 0) {
            fprintf(stderr, "smash error: kill: job-id %d does not exist\n", jobID);
            return CMD_RETURN_ERR;
        }
        for (auto it = jobs.begin(); it != jobs.end(); it++) {
            if (it->getJobId() == jobID) {
                pid = it->getPid();
                if (kill(pid, signum) != 0) {
                    perror("smash error: kill failed");
                    return CMD_RETURN_ERR;
                }
                if (signum == SIGSTOP || signum == SIGTSTP) {
                    it->setIsStopped(true);
                }
                return CMD_RETURN_OK;
            }
        }
        fprintf(stderr, "smash error: kill: job-id %d does not exist\n", jobID);
        return CMD_RETURN_ERR;

	} 
	/*************************************************/	
	else if (!strcmp(cmd, "diff"))
	{
   		if (num_arg != DIFF_ARGS){
			fprintf(stderr, "smash error: diff: invalid arguments\n");
		}
		else{
			FILE *f1 = fopen(args[1], "r");
            if (f1 == NULL) {
                perror("smash error: fopen failed");
                return CMD_RETURN_ERR;
            }
			//TODO: figure out system call error
			FILE *f2 = fopen(args[2], "r");
            if (f2 == NULL) {
                perror("smash error: fopen failed");
                return CMD_RETURN_ERR;
            }
			// compare the two files character wise
			int c1 = getc(f1), c2 = getc(f2);
			
		
			while (c1 == c2 && c1 != EOF && c2 != EOF){
				c1 = getc(f1);
				c2 = getc(f2);
			}
			if (c1 == EOF && c2 == EOF){
				printf("0\n");
			}
			else{
				printf("1\n");
			}
		}
	} 
	/*************************************************/	
//	else // external command
//	{
// 		ExeExternal(args, cmdString, true, jobs);
//	 	return 0;
//	}
//	if (illegal_cmd == true)
//	{
//		printf("smash error: > \"%s\"\n", cmdString);
//		return 1;
//	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString, bool isFg, vector<job> &jobs)
{
	int pID;
    	switch(pID = fork()) 
	{
    		case -1: 
					// Add your code here (error)
					
					perror("smash error: fork failed");

				break;
        	case 0 :
                	// Child Process
               		setpgrp();
					
			        // Add your code here (execute an external command)
					//set_foreground(getpid());
					//cout<<args[0]<<" "<<args[1]<<endl;
					//cout<<cmdString<<endl;

                    //CleanJobs(jobs);
					execv(args[0], args);
				break;
			
			default:
                    printf("[DEBUG] PID = %d\n", pID);
                	// Add your code here
					if (isFg){
						//(foreground)
						set_foreground(pID);
                        set_fg_cmdString(cmdString);
						waitpid(pID, NULL, WUNTRACED);
                        printf("[DEBUG] pid %d state change\n", pID);
						set_foreground(0);
					}
					else{
						// (background)
                        printf("[DEBUG] Cleaning stopped jobs...");
                        CleanJobs(jobs);
                        printf("Done\n");

						int new_id;
						time_t start = time(NULL);
						if (!jobs.empty()) {
                            printf("[DEBUG] Jobs exist, generating new job ID...");
                            new_id = jobs[jobs.size() - 1].getJobId() + 1;
                            printf("%d\n", new_id);
                        }
                        else new_id = 1;

                        printf("[DEBUG] Creating job for <%s>\n", cmdString);
						job new_job = job(new_id, pID, cmdString, false, start, 0);
                        printf("[DEBUG] Job created for <%s>\n", new_job.getCommand());
                        jobs.push_back(new_job);
                        printf("[DEBUG] Job added\n");
					}
				break;
	}
}


int cmdParseArgs(char* lineSize, char* args[MAX_ARG])
{
    printf("[DEBUG] Parsing args...\n");
	int i = 0, num_arg = 0;
	const char* delimiters = " \t\n";
    char* cmd;

    cmd = strtok(lineSize, delimiters);
    if (cmd == NULL)
        return -1;

    args[0] = cmd;
    for (i=1; i<MAX_ARG; i++)
    {
        args[i] = strtok(NULL, delimiters);
        if (args[i] != NULL)
            num_arg++;

    }

    printf("[DEBUG] Num args = %d\n", num_arg);
    i = 0;
    while (args[i] != NULL) printf("<%s> ", args[i++]);
    printf("\n");
    //ExeExternal(args, lineSize, false, jobs);

	return num_arg;

}

//**************************************************************************************
// function name: CleanJobs
// Description: delete terminated jobs from job list (non-blocking)
// Parameters: jobs vector
// Returns: -1-waitpid error, else-number of deleted jobs
//**************************************************************************************
int CleanJobs(vector<job> &jobs) {
    //vector<job>::iterator ptr;
    int pid, cleaned = 0;

    //non-blocking wait for all children
    while ((pid = waitpid(-1, NULL, WNOHANG)) != 0) {
        if (pid == -1) {
            if (errno == ECHILD) {
                return 0;
            }
            else {
                perror("smash error: waitpid failed");
                return -1;
            }
        }
        for (auto it = jobs.begin(); it != jobs.end(); it++) {
            if (pid == it->getPid()) {
                jobs.erase(it);
                cleaned++;
                break;
            }
        }
    }
    return cleaned;
}

int cmdParseType(char* lineSize) {
    char* cmd;
    const char* delimiters = " \t\n";
    int i = strlen(lineSize) - 1;
    printf("[DEBUG] Last index = %d\n", i);
    int bgtoken = 0;

    while (i >= 0 && isspace(lineSize[i])) i--;
    if (i >= 0 && lineSize[i] == '&') {
        printf("[DEBUG] Found &\n");
        lineSize[i] = '\0';
        bgtoken = 1;
    }
    else if (i < 0) {
        return CMD_TYPE_ERR;
    }
    else {
        lineSize[i+1] = '\0';
    }

    printf("[DEBUG] Truncuated full command: %s\n", lineSize);

    int start_index = strspn(lineSize, delimiters);
    printf("[DEBUG] start_index = %d\n", start_index);
    int cmd_len = strcspn(lineSize + start_index, delimiters);
    printf("[DEBUG] cmd_len = %d\n", cmd_len);

    cmd = (char*)malloc(cmd_len + 1);
    strncpy(cmd, lineSize + start_index, cmd_len);
    cmd[cmd_len] = '\0';

//    if (cmd == NULL)
//        return CMD_TYPE_ERR;

    printf("Command: %s\n", cmd);

    if (isBuiltIn(cmd)) {
        printf("[DEBUG] Built-in command\n");
        free(cmd);
        return CMD_TYPE_BUILTIN_FG;
    }
    else {
        printf("[DEBUG] External command ");
        free(cmd);

        if (bgtoken) {
            printf("BG\n");
            return CMD_TYPE_EXT_BG;
        }
        else {
            printf("FG\n");
            return CMD_TYPE_EXT_FG;
        }
    }
}

int isBuiltIn(char* cmd) {
    if (strcmp(cmd, "showpid") != 0 &&
        strcmp(cmd, "pwd") != 0 &&
        strcmp(cmd, "cd") != 0 &&
        strcmp(cmd, "jobs") != 0 &&
        strcmp(cmd, "kill") != 0 &&
        strcmp(cmd, "fg") != 0 &&
        strcmp(cmd, "bg") != 0 &&
        strcmp(cmd, "quit") != 0 &&
        strcmp(cmd, "diff") != 0) {
        return 0;
    }
    else
        return 1;
}

bool compareJobsByJobID(job& job1, job& job2) {
    return job1.getJobId() < job2.getJobId();
}

