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
// Parameters: reference to jobs, argument array, command string
// Returns: 0 - success, 1 - failure, -1 - quit smash
//**************************************************************************************
char old_pwd[PATH_MAX] = "";
char cwd[PATH_MAX];

int ExeCmd(vector<job> &jobs, char* args[MAX_ARG], int num_arg)
{
	char* cmd = args[0];

    CleanJobs(jobs);

    /*************************************************/
    if (!strcmp(cmd, "cd") )
	{
 		return ExeCmd_cd(jobs, args, num_arg);
	} 
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		return ExeCmd_pwd(jobs, args, num_arg);
	}
	/*************************************************/
	else if (!strcmp(cmd, "jobs")) 
	{
        return ExeCmd_jobs(jobs, args, num_arg);
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		return ExeCmd_showpid(jobs, args, num_arg);
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
        return ExeCmd_fg(jobs, args, num_arg);
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
        return ExeCmd_bg(jobs, args, num_arg);

	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
        return ExeCmd_quit(jobs, args, num_arg);
	} 
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
   		return ExeCmd_kill(jobs, args, num_arg);

	} 
	/*************************************************/	
	else if (!strcmp(cmd, "diff"))
	{
   		return ExeCmd_diff(jobs, args, num_arg);
	} 
	/*************************************************/	

    return 0;
}

int ExeCmd_showpid(vector<job> &jobs, char* args[MAX_ARG], int num_arg) {
    pid_t pid = getpid();

    // printf("smash pid is %d\n", pid);
    cout << "smash pid is " << pid << endl;

    return CMD_RETURN_OK;
}

int ExeCmd_cd(vector<job> &jobs, char* args[MAX_ARG], int num_arg) {
    char buff[PATH_MAX] = "";
    if (num_arg > 1) {
        // printf("smash error: cd: too many arguments\n");
        cerr<< "smash error: cd: too many arguments\n";
        return CMD_RETURN_ERR;
    }
    else{
        if(strcmp(args[1], "-") == 0){
            if (strlen(old_pwd) == 0) {
                // printf("smash error: cd: OLDPWD not set\n");
                cerr<< "smash error: cd: OLDPWD not set\n";
                return CMD_RETURN_ERR;
            }
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
                return CMD_RETURN_OK;
            }
        }
        else{

            if (getcwd(buff, PATH_MAX) == NULL) {
                perror("smash error: getcwd failed");
                return CMD_RETURN_ERR;
            }
            if (chdir(args[1]) == -1) {
                perror("smash error: chdir failed");
                return CMD_RETURN_ERR;
            }
            strcpy(old_pwd, buff);
            return CMD_RETURN_OK;
        }
    }
}

int ExeCmd_pwd(vector<job> &jobs, char* args[MAX_ARG], int num_arg) {
    if (getcwd(cwd, MAX_LINE_SIZE) == NULL) {
        perror("smash error: getcwd failed");
        return CMD_RETURN_ERR;
    }
    // printf("%s\n", cwd);
    cout << cwd << endl;
    return CMD_RETURN_OK;
}

int ExeCmd_jobs(vector<job> &jobs, char* args[MAX_ARG], int num_arg) {
    sort(jobs.begin(), jobs.end(), compareJobsByJobID); //sort jobs by jobID

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
            // printf("[%d] %s : %d %d secs (Stopped)\n", job_id, command, pid, (int) difftime(*now, start));
            cout << "[" << job_id << "] " << command << " : " << pid << " " << (int) difftime(*now, start) << " secs (Stopped)" << endl;
        }
        else
            cout<< "[" << job_id << "] " << command << " : " << pid << " " << (int) difftime(*now, start) << " secs" << endl;
            // printf("[%d] %s : %d %d secs\n", job_id, command, pid, (int) difftime(*now, start));
    }
    delete now;
    return CMD_RETURN_OK;
}

int ExeCmd_kill(vector<job> &jobs, char* args[MAX_ARG], int num_arg) {
    //check format: kill -<signum> <job_id>
    if (num_arg != 2 || args[1][0] != '-') {
        // fprintf(stderr, "smash error: kill: invalid arguments\n");
        cerr << "smash error: kill: invalid arguments" << endl;
        return CMD_RETURN_ERR;
    }

    //are arguments integers?
    int arg_len = strlen(args[1]);
    for (int i = 1; i < arg_len; i++) {
        if (!isdigit(args[1][i])) {
            // fprintf(stderr, "smash error: kill: invalid arguments\n");
            cerr << "smash error: kill: invalid arguments" << endl;
            return CMD_RETURN_ERR;
        }
    }

    arg_len = strlen(args[2]);
    for (int i = 0; i < arg_len; i++) {
        if (!isdigit(args[2][i])) {
            // fprintf(stderr, "smash error: kill: invalid arguments\n");
            cerr << "smash error: kill: invalid arguments" << endl;
            return CMD_RETURN_ERR;
        }
    }

    //parse arguments
    int signum, jobID, pid;
    sscanf(args[1], "-%d", &signum);
    sscanf(args[2], "%d", &jobID);

    //printf("[DEBUG] signum = %d jobID = %d\n", signum, jobID);

    if (jobs.size() == 0) {
        // fprintf(stderr, "smash error: kill: job-id %d does not exist\n", jobID);
        cerr << "smash error: kill: job-id " << jobID << " does not exist" << endl;
        return CMD_RETURN_ERR;
    }
    for (auto it = jobs.begin(); it != jobs.end(); it++) {
        if (it->getJobId() == jobID) {
            pid = it->getPid();
            if (kill(pid, signum) != 0) {
                perror("smash error: kill failed");
                return CMD_RETURN_ERR;
            }

            //if sent stop signal, update job object (probably not necessary but doesn't work, I think...)
            if (signum == SIGSTOP || signum == SIGTSTP) {
                it->setIsStopped(true);
            }

            return CMD_RETURN_OK;
        }
    }

    // fprintf(stderr, "smash error: kill: job-id %d does not exist\n", jobID);
    cerr << "smash error: kill: job-id " << jobID << " does not exist" << endl;
    return CMD_RETURN_ERR;
}

int ExeCmd_fg(vector<job> &jobs, char* args[MAX_ARG], int num_arg) {
    int jobID, selPID;
    vector<job>::iterator it;

    if (num_arg > 1) {
        // fprintf(stderr, "smash error: fg: invalid arguments\n");
        cerr << "smash error: fg: invalid arguments" << endl;
        return CMD_RETURN_ERR;
    }
    else if (num_arg == 1) {

        //check if argument is number
        int arg_len = strlen(args[1]);
        for (int i = 0; i < arg_len; i++) {
            if (!isdigit(args[1][i])) {
                // fprintf(stderr, "smash error: fg: invalid arguments\n");
                cerr << "smash error: fg: invalid arguments" << endl;
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
            // fprintf(stderr, "smash error: fg: job-id %d does not exist\n", jobID);
            cerr << "smash error: fg: job-id " << jobID << " does not exist" << endl;
            return CMD_RETURN_ERR;
        }
    }
    else { //no arguments, select largest job ID

        if (jobs.size() == 0) {
            // fprintf(stderr, "smash error: fg: jobs list is empty\n");
            cerr << "smash error: fg: jobs list is empty" << endl;
            return CMD_RETURN_ERR;
        }

        sort(jobs.begin(), jobs.end(), compareJobsByJobID); //sort jobs by jobID

        it = jobs.end();
        it--; //final element in sorted array is largest job ID

        jobID = it->getJobId();
    }

    selPID = it->getPid();
    char *cmdString = it->getCommand();
    // printf("%s : %d\n", cmdString, selPID);
    cout << cmdString << " : " << selPID << endl;

    set_foreground(selPID);
    set_fg_cmdString(cmdString);
    set_fg_jobID(jobID);

    jobs.erase(it);

    if (kill(selPID, SIGCONT) != 0) {
        perror("smash error: kill failed");
        return CMD_RETURN_ERR;
    }

    //wait for foreground process to change status
    if (waitpid(selPID, NULL, WUNTRACED) == -1) {
        perror("smash error: waitpid failed");
        return CMD_RETURN_ERR;
    }

    set_foreground(0);
    char empty[MAX_LINE_SIZE] = {'\0'};
    set_fg_cmdString(empty);
    set_fg_jobID(0);

    return CMD_RETURN_OK;
}

int ExeCmd_bg(vector<job> &jobs, char* args[MAX_ARG], int num_arg) {
    int jobID, selPID;
    vector<job>::iterator it;

    if (num_arg > 1) {
        // fprintf(stderr, "smash error: bg: invalid arguments\n");
        cerr << "smash error: bg: invalid arguments" << endl;
        return CMD_RETURN_ERR;
    }
    else if (num_arg == 1) {

        //is argument number?
        int arg_len = strlen(args[1]);
        for (int i = 0; i < arg_len; i++) {
            if (!isdigit(args[1][i])) {
                // fprintf(stderr, "smash error: bg: invalid arguments\n");
                cerr << "smash error: bg: invalid arguments" << endl;
                return CMD_RETURN_ERR;
            }
        }

        jobID = stoi(args[1]);
        int foundJob = 0;
        for (it = jobs.begin(); it != jobs.end(); it++) {
            if (it->getJobId() == jobID) {

                if (!it->getIsStopped()) {
                    // fprintf(stderr, "smash error: bg: job-id %d is already running in the background\n", jobID);
                    cerr << "smash error: bg: job-id " << jobID << " is already running in the background" << endl;
                    return CMD_RETURN_ERR;
                }

                foundJob = 1;
                break;
            }
        }
        if (!foundJob) {
            // fprintf(stderr, "smash error: bg: job-id %d does not exist\n", jobID);
            cerr << "smash error: bg: job-id " << jobID << " does not exist" << endl;
            return CMD_RETURN_ERR;
        }
    }
    else { //no argument, get stopped job with the largest ID
        if (jobs.size() == 0) {
            // fprintf(stderr, "smash error: bg: there are no stopped jobs to resume\n");
            cerr << "smash error: bg: there are no stopped jobs to resume" << endl;
            return CMD_RETURN_ERR;
        }

        sort(jobs.begin(), jobs.end(), compareJobsByJobID); //sort jobs by jobID

        it = jobs.end();
        it--; //final element in sorted array is largest job ID
        while (it->getIsStopped() == 0) { //go backwards until reaching stopped process
            if (it == jobs.begin()) { //if we got through all jobs without finding a stopped one
                // fprintf(stderr, "smash error: bg: there are no stopped jobs to resume\n");
                cerr << "smash error: bg: there are no stopped jobs to resume" << endl;
                return CMD_RETURN_ERR;
            }
            it--;
        }
        //jobID = it->getJobId();
    }

    selPID = it->getPid();
    char *cmdString = it->getCommand();

    // printf("%s : %d\n", cmdString, selPID);
    cout << cmdString << " : " << selPID << endl;

    it->setIsStopped(false);

    if (kill(selPID, SIGCONT) != 0) {
        perror("smash error: kill failed");
        return CMD_RETURN_ERR;
    }

    return CMD_RETURN_OK;
}

int ExeCmd_quit(vector<job> &jobs, char* args[MAX_ARG], int num_arg) {
    //if there's not 1 argument, or if there are no jobs to kill anyway...
    if (num_arg != 1 || jobs.size() == 0)
        return CMD_RETURN_QUIT;

    if (!strcmp(args[1], "kill")) {
        sort(jobs.begin(), jobs.end(), compareJobsByJobID);

        vector<job>::iterator it;
        for (it = jobs.begin(); it != jobs.end(); ) {

            int jobId = it->getJobId();
            char* comm = it->getCommand();
            cout << "[" << jobId << "] " << comm << " - Sending SIGTERM... ";
            fflush(stdout); //stdout is line buffered, we need to flush messages without \n

            int pid = it->getPid();
            if (kill(pid, SIGTERM) != 0) {
                perror("smash error: kill failed");
                return CMD_RETURN_ERR;
            }

            //Every 1 second, check (non-blockingly) if the process has been terminated, or if timeout reached
            time_t start = time(NULL);
            time_t now = time(NULL);
            while (waitpid(pid, NULL, WNOHANG) == 0 && difftime(now, start) < SIGTERM_TIMEOUT) {
                now = time(NULL);
                sleep(1);
            }

            if (difftime(now, start) >= SIGTERM_TIMEOUT) {
                cout << "(5 sec passed) Sending SIGKILL... ";
                if (kill(pid, SIGKILL) != 0) {
                    perror("smash error: kill failed");
                    return CMD_RETURN_ERR;
                }
            }

            cout << "Done." << endl;
            // printf("Done.\n");

            //erase job and iterate to next
            it = jobs.erase(it);

        }

        return CMD_RETURN_QUIT;
    }
    else {
        return CMD_RETURN_QUIT;
    }

}

int ExeCmd_diff(vector<job> &jobs, char* args[MAX_ARG], int num_arg) {
    if (num_arg != DIFF_ARGS){
        cerr<< "smash error: diff: invalid arguments\n";
        // fprintf(stderr, "smash error: diff: invalid arguments\n");
        return CMD_RETURN_ERR;
    }
    else{
        FILE *f1 = fopen(args[1], "r");
        if (f1 == NULL) {
            perror("smash error: fopen failed");
            return CMD_RETURN_ERR;
        }
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
            cout<< "0\n";
            // printf("0\n");
            // 
        }
        else{
            cout<< "1\n";
            // printf("1\n");
        }

        return CMD_RETURN_OK;
    }
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
    switch(pID = fork()) {
        case -1:
            perror("smash error: fork failed");
            break;
        case 0 :
            // Child Process
            setpgrp();

            execv(args[0], args);
            perror("smash error: execv failed");
            break;

        default:
            //printf("[DEBUG] PID = %d\n", pID);
            if (isFg){
                //(foreground)
                set_foreground(pID);
                set_fg_cmdString(cmdString);
                waitpid(pID, NULL, WUNTRACED);
                //printf("[DEBUG] pid %d state change\n", pID);
                set_foreground(0);
            }
            else{
                // (background)
                //printf("[DEBUG] Cleaning stopped jobs...");
                CleanJobs(jobs);
                //printf("Done\n");

                int new_id;
                time_t start = time(NULL);
                if (!jobs.empty()) {
                    //printf("[DEBUG] Jobs exist, generating new job ID...");
                    new_id = jobs[jobs.size() - 1].getJobId() + 1;
                    //printf("%d\n", new_id);
                }
                else new_id = 1;

                //printf("[DEBUG] Creating job for <%s>\n", cmdString);
                job new_job = job(new_id, pID, cmdString, false, start, 0);
                //printf("[DEBUG] Job created for <%s>\n", new_job.getCommand());
                jobs.push_back(new_job);
                //printf("[DEBUG] Job added\n");
            }
            break;
	}
}

//**************************************************************************************
// function name: ExeExternal
// Description: parses command string into arguments array
// Parameters: external command string, external command arguments
// Returns: number of arguments not including command on success, -1 on faliure
//**************************************************************************************
int cmdParseArgs(char* lineSize, char* args[MAX_ARG])
{
    //printf("[DEBUG] Parsing args...\n");
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

    //printf("[DEBUG] Num args = %d\n", num_arg);
    //i = 0;
    //while (args[i] != NULL) printf("<%s> ", args[i++]);
    //printf("\n");
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

//**************************************************************************************
// function name: cmdParseType
// Description: parse the type of command: invalid, built-in, external BG or external FG
// Parameters: command string
// Returns: command type (see commands.h)
//**************************************************************************************
int cmdParseType(char* lineSize) {
    char* cmd;
    const char* delimiters = " \t\n";
    int i = strlen(lineSize) - 1;
    //printf("[DEBUG] Last index = %d\n", i);
    int bgtoken = 0;

    //skip trailing whitespace
    while (i >= 0 && isspace(lineSize[i])) i--;
    if (i >= 0 && lineSize[i] == '&') { //if reached &, take note and trim it
        //printf("[DEBUG] Found &\n");
        lineSize[i] = '\0';
        bgtoken = 1;
    }
    else if (i < 0) { //all whitespace
        return CMD_TYPE_ERR;
    }
    else { //trim whitespace
        lineSize[i+1] = '\0';
    }

    //printf("[DEBUG] Truncuated full command: %s\n", lineSize);

    int start_index = strspn(lineSize, delimiters); //skip over heading whitespace
    //printf("[DEBUG] start_index = %d\n", start_index);
    int cmd_len = strcspn(lineSize + start_index, delimiters); //count non-whitespace chars in a row
    //printf("[DEBUG] cmd_len = %d\n", cmd_len);

    //copy first argument (command)
    cmd = (char*)malloc(cmd_len + 1);
    strncpy(cmd, lineSize + start_index, cmd_len);
    cmd[cmd_len] = '\0';

//    if (cmd == NULL)
//        return CMD_TYPE_ERR;

    //printf("[DEBUG] Command: %s\n", cmd);

    if (isBuiltIn(cmd)) {
        //printf("[DEBUG] Built-in command\n");
        free(cmd);
        return CMD_TYPE_BUILTIN_FG;
    }
    else {
        //printf("[DEBUG] External command ");
        free(cmd);

        if (bgtoken) {
            //printf("BG\n");
            return CMD_TYPE_EXT_BG;
        }
        else {
            //printf("FG\n");
            return CMD_TYPE_EXT_FG;
        }
    }
}

//check if command is a built-in one
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

