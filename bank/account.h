#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <pthread.h>
// implement account

class account{
    public:
        account(int account_id, int password, int initial_amount);
        int get_account_id() {return account_id;}
        bool check_password(int test_password) {return test_password == password;}
        int get_password() {return password;} 
        int get_balance() {return balance;}
        int deposit_funds(int amount);
        int withdraw_funds(int amount);
        void account_read_lock();
        void account_read_unlock();
        void account_write_lock();
        void account_write_unlock();
        ~account();


    private:
        int account_id;
        int password;
        int balance;
        int account_read_count;

};


#endif