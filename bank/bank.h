#ifndef BANK_H
#define BANK_H
#include <vector>
#include "account.h"
#include <pthread.h>

using namespace std;

class Bank{
    public:
        Bank();
        //Bank(vector<account> accounts);
        void add_account(int account_id, int password, int initial_amount);
        int check_account(int account_id);
        bool check_password(int i, int password);
        int get_balance(int i);
        int get_account_id(int i);
        int deposit(int i, int amount);
        int withdraw(int i, int amount);
        int close_account(int i);
        void charge_commission();
        void print_accounts();
        void bank_read_lock();
        void bank_read_unlock();
        void bank_write_lock();
        void bank_write_unlock();
        void bank_print_lock();
        void bank_print_unlock();
        ~Bank();
    private:
        int balance;
        vector<account> accounts;
};

#endif