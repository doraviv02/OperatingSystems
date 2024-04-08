#ifndef ATM_H
#define ATM_H

// implement ATM and functions
# include "bank.h"
#include <pthread.h>
#include <iomanip>
#include <errno.h>

class ATM{
public:
    ATM(int id, Bank* bank);
    ATM(const ATM& atm) {
        id = atm.id;
        bank = atm.bank;
    }
    void open_account(int account_id, int password, int initial_ammount);
    void deposit(int account_id, int password, int amount);
    void withdraw(int account_id, int password, int amount);
    void check_balance(int account_id, int password);
    void close_account(int account_id, int password);
    void transfer(int account_id, int password, int target_account_id, int amount);
    ~ATM();

private:
    int id;
    Bank* bank;
    int check_account_id(int acount_id);
    bool check_password(int index, int password);
    void atm_log_lock();
    void atm_log_unlock();

};

#endif