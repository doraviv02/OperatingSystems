#ifndef ATM_H
#define ATM_H

// implement ATM and functions
# include "bank.h"

// TODO: add log mutex
class ATM{
public:
    ATM(int id, Bank* bank);
    void open_account(int account_id, int password, int initial_ammount);
    void deposit(int account_id, int password, int amount);
    void withdraw(int account_id, int password, int amount);
    void check_balance(int account_id, int password);
    void close_account(int account_id, int password);
    void transfer(int account_id, int password, int target_account_id, int amount);
    void atm_log_lock();
    void atm_log_unlock();
    ~ATM();

private:
    int id;
    Bank* bank;
    int check_account_id(int acount_id);
    bool check_password(int index, int password);
};

#endif