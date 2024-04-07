#ifndef BANK_H
#define BANK_H
#include <vector>
#include "account.h"
#include <fstream>
#include <unistd.h>
#include <pthread.h>
#include <algorithm>

using namespace std;

class Bank{
    public:
        Bank();
        //Bank(vector<account> accounts);
        void add_account(int account_id, int password, int initial_amount, double sleep_dur);
        int check_account(int account_id, double sleep_dur);
        bool check_password(int i, int password, double sleep_dur);
        int get_balance(int i, double sleep_dur);
        int get_account_id(int i, double sleep_dur);
        int deposit(int i, int amount, double sleep_dur);
        int withdraw(int i, int amount, double sleep_dur);
        int close_account(int i, double sleep_dur);
        void charge_commission();
        void print_accounts(int* done_flag);
        void bank_read_lock();
        void bank_read_unlock();
        void bank_write_lock();
        void bank_write_unlock();
        void bank_print_lock();
        void bank_print_unlock();
        ~Bank();

        ofstream atm_log_file;

    private:
        int balance;
        vector<account> accounts;
        pthread_mutex_t bank_mutex_read;
        pthread_mutex_t bank_mutex_write;
        pthread_mutex_t bank_mutex_print;
};

#endif