// Job Class implementation
#include "job.h"
#define MAX_LINE_SIZE 80


job::job(int job_id, int pid, char* command, bool isStopped, time_t start, time_t end){
    this->job_id = job_id;
    this->pid = pid;
    this->command = new char[MAX_LINE_SIZE];
    strcpy(this->command, command);
    this->isStopped = isStopped;
    this->start = start;
    this->end = end;
}

job::job(const job& other) {
    this->job_id = other.job_id;
    this->pid = other.pid;
    this->command = new char[MAX_LINE_SIZE];
    strcpy(this->command, other.command);
    this->isStopped = other.isStopped;
    this->start = other.start;
    this->end = other.end;
}

job& job::operator=(const job& other) {
    if (this != &other) {
        delete[] this->command;
        this->job_id = other.job_id;
        this->pid = other.pid;
        this->command = new char[MAX_LINE_SIZE];
        strcpy(this->command, other.command);
    }
    return *this;
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


void job::setStart(time_t start){
    this->start = start;
}

void job::setEnd(time_t end){
    this->end = end;
}

job::~job(){
    delete[] this->command;
}