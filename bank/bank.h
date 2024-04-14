#ifndef BANK_H
#define BANK_H
#include <vector>
#include "account.h"
#include <fstream>
#include <unistd.h>
#include <pthread.h>
#include <algorithm>
#include <math.h>
#include <errno.h>

using namespace std;

class Bank{
    public:
        Bank();
        //Bank(vector<account> accounts);
        void add_account(int atm, int account_id, int password, int initial_amount, double sleep_dur);
        int check_account(int account_id, double sleep_dur);
        bool check_password(int i, int password, double sleep_dur);
        int get_balance(int atm, int i, double sleep_dur);
        int get_account_id(int i, double sleep_dur);
        int deposit(int atm, int i, int amount, double sleep_dur);
        int withdraw(int atm, int i, int amount, double sleep_dur);
        int transfer(int atm, int i, int j, int amount, double sleep_dur);
        int close_account(int atm, int i, double sleep_dur);
        void charge_commission();
        void print_accounts(int* done_flag);
        void bank_read_lock();
        void bank_read_unlock();
        void bank_write_lock();
        void bank_write_unlock();
        ~Bank();



    private:
        int balance;

};

#endif