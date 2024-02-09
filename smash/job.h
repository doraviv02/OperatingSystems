// Job header file
#include <time.h>

#ifndef JOB_H
#define JOB_H

class job{
    private:
        int job_id;
        int pid;
        char* command;
        bool isStopped;
        bool isDone;
        time_t start;
        time_t end;
    public:
        job(int job_id, int pid, char* command, bool isStopped, bool isDone, time_t start, time_t end);
        int getJobId();
        int getPid();
        char* getCommand();
        bool getIsStopped();
        bool getIsDone();
        time_t getStart();
        time_t getEnd();
        void setJobId(int job_id);
        void setPid(int pid);
        void setCommand(char* command);
        void setIsStopped(bool isStopped);
        void setIsDone(bool isDone);
        void setStart(time_t start);
        void setEnd(time_t end);
};

#endif