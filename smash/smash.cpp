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
#include <iostream>
#include <iostream>
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

	signal(SIGINT, handler_cntlc);
	signal(SIGTSTP, handler_cntlz);

    set_foreground(0);
    set_fg_jobID(0);
    char empty[MAX_LINE_SIZE] = {'\0'};
    set_fg_cmdString(empty);
	
    while (1) {
        /* initialize for next line read*/
        lineSize[0]='\0';
        cmdString[0]='\0';

        cout<<"smash > ";
	 	// printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);    	
		cmdString[strlen(lineSize)-1]='\0';

        //PARSE INCOMING COMMAND
        int cmd_type = cmdParseType(lineSize);
        int arg_num, ret;
        switch (cmd_type) {
            case CMD_TYPE_ERR:
                continue;
            case CMD_TYPE_BUILTIN_FG:
                arg_num = cmdParseArgs(lineSize, args);
                //printf("[DEBUG] cmdString: <%s>\n", cmdString);
                ret = ExeCmd(jobs, args, arg_num);

                if (ret == CMD_RETURN_QUIT) {
                    return 0;
                }

                break;
            case CMD_TYPE_EXT_FG:
                cmdParseArgs(lineSize, args);
                //printf("[DEBUG] cmdString: <%s>\n", cmdString);
                ExeExternal(args, cmdString, 1, jobs);
                break;
            case CMD_TYPE_EXT_BG:

                cmdParseArgs(lineSize, args);
                //printf("[DEBUG] cmdString: <%s>\n", cmdString);
                ExeExternal(args, cmdString, 0, jobs);
                break;
        }


	}

}

