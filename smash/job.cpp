// Job Class implementation
#include "job.h"


job::job(int job_id, int pid, char* command, bool isStopped, bool isDone, time_t start, time_t end){
    this->job_id = job_id;
    this->pid = pid;
    this->command = command;
    this->isStopped = isStopped;
    this->isDone = isDone;
    this->start = start;
    this->end = end;
}

int job::getJobId(){
    return this->job_id;
}

int job::getPid(){
    return this->pid;
}

char* job::getCommand(){
    return this->command;
}

bool job::getIsStopped(){
    return this->isStopped;
}

bool job::getIsDone(){
    return this->isDone;
}

time_t job::getStart(){
    return this->start;
}

time_t job::getEnd(){
    return this->end;
}

void job::setJobId(int job_id){
    this->job_id = job_id;
}

void job::setPid(int pid){
    this->pid = pid;
}

void job::setCommand(char* command){
    this->command = command;
}

void job::setIsStopped(bool isStopped){
    this->isStopped = isStopped;
}

void job::setIsDone(bool isDone){
    this->isDone = isDone;
}

void job::setStart(time_t start){
    this->start = start;
}

void job::setEnd(time_t end){
    this->end = end;
}