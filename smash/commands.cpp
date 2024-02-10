//		commands.cpp
//********************************************
#include "commands.h"
#include "job.h"
#define DIFF_ARGS 2

using namespace std;
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
char old_pwd[MAX_LINE_SIZE] = "";

int ExeCmd(vector<job> &jobs, char* lineSize, char* cmdString)
{
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	const char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
 
	}
	char new_pwd[MAX_LINE_SIZE];
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) 
	{
 		if (num_arg > 1) printf("smash error: cd: too many arguments\n");
		else{
			if(strcmp(args[1], "-") == 0){
				if (strlen(old_pwd) == 0)
					printf("smash error: cd: OLDPWD not set\n");
				else{
					strcpy(new_pwd, getcwd(pwd, MAX_LINE_SIZE));
					chdir(old_pwd);
					strcpy(old_pwd, new_pwd);
				}

			}
			else{
				strcpy(old_pwd, getcwd(pwd, MAX_LINE_SIZE));
				chdir(args[1]);
			}
		}
	} 
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		getcwd(pwd, MAX_LINE_SIZE);
		printf("%s\n", pwd);
	}
	/*************************************************/
	else if (!strcmp(cmd, "jobs")) 
	{
	
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
		
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
  		
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
   		
	} 
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
   		
	} 
	/*************************************************/	
	else if (!strcmp(cmd, "diff"))
	{
   		if (num_arg != DIFF_ARGS){
			printf("smash error: diff: invalid arguments\n");
		}
		else{
			FILE *f1 = fopen(args[1], "r");
			//TODO: figure out system call error
			FILE *f2 = fopen(args[2], "r");
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
	else // external command
	{
 		ExeExternal(args, cmdString);
	 	return 0;
	}
	if (illegal_cmd == true)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	int pID;
    	switch(pID = fork()) 
	{
    		case -1: 
					// Add your code here (error)
					
					/* 
					your code
					*/
				break;
        	case 0 :
                	// Child Process
               		setpgrp();
					
			        // Add your code here (execute an external command)
					
					/* 
					your code
					*/
				break;
			
			default:
                	// Add your code here
					
					/* 
					your code
					*/
				break;
	}
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, vector<job> &jobs)
{

	char* Command;
	const char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		// Add your code here (execute a in the background)
					
		/* 
		your code
		*/
		
	}
	return -1;
}

