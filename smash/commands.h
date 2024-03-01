#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#define MAX_LINE_SIZE 81
#define MAX_ARG 20
#define CMD_TYPE_BUILTIN_FG 0
#define CMD_TYPE_EXT_BG 1
#define CMD_TYPE_EXT_FG 2
#define CMD_TYPE_ERR -1
#include "job.h"

using namespace std;

int cmdParseArgs(char* lineSize, char* args[MAX_ARG]);
int ExeCmd(vector<job> &jobs, char* args[MAX_ARG], int num_arg);
void ExeExternal(char* args[MAX_ARG], char* cmdString, bool isFg, vector<job> &jobs);
int CleanJobs(vector<job> &jobs);
int cmdParseType(char* lineSize);
int isBuiltIn(char* cmd);
bool compareJobsByJobID(job& job1, job& job2);

#endif

