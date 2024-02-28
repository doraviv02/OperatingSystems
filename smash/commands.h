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
#define MAX_LINE_SIZE 80
#define MAX_ARG 20
#include "job.h"

using namespace std;

int BgCmd(char* lineSize, vector<job> &jobs);
int ExeCmd(vector<job> &jobs, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], char* cmdString, bool isFg, vector<job> &jobs);
#endif

