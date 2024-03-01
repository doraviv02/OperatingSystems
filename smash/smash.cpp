/*	smash.cpp
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <vector>
#include "job.h"
#include "commands.h"
#include "signals.h"


#define MAX_JOBS 100

using namespace std;

char* L_Fg_Cmd;
vector<job> jobs;
//job jobs[MAX_JOBS];
char lineSize[MAX_LINE_SIZE]; 
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE]; 	 
    char* args[MAX_ARG];

	
	//signal declaretions
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	 /* add your code here */
	
	/************************************/
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	//set your signal handlers here
	/* add your code here */
	signal(SIGINT, handler_cntlc);
	signal(SIGTSTP, handler_cntlz);
	/************************************/

	/************************************/
	// Init globals 

    set_foreground(0);
    set_fg_jobID(0);
    char empty[MAX_LINE_SIZE] = {'\0'};
    set_fg_cmdString(empty);
	
    while (1) {
        /* initialize for next line read*/
        lineSize[0]='\0';
        cmdString[0]='\0';

	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);    	
		cmdString[strlen(lineSize)-1]='\0';

        int cmd_type = cmdParseType(lineSize);
        int arg_num;
        switch (cmd_type) {
            case CMD_TYPE_ERR:
                continue;
            case CMD_TYPE_BUILTIN_FG:
                arg_num = cmdParseArgs(lineSize, args);
                printf("[DEBUG] cmdString: <%s>\n", cmdString);
                ExeCmd(jobs, args, arg_num);
                break;
            case CMD_TYPE_EXT_FG:

                cmdParseArgs(lineSize, args);
                printf("[DEBUG] cmdString: <%s>\n", cmdString);
                ExeExternal(args, cmdString, 1, jobs);
                break;
            case CMD_TYPE_EXT_BG:

                cmdParseArgs(lineSize, args);
                printf("[DEBUG] cmdString: <%s>\n", cmdString);
                ExeExternal(args, cmdString, 0, jobs);
                break;
        }

	 				// built in commands
		//ExeCmd(jobs, lineSize, cmdString);
	}
	// deallocate jobs vector
	jobs.clear();
}

