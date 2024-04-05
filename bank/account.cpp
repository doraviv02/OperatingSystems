// implement acocunt
#include "account.h"

account::account(int account_id, int password, int initial_amount){
    this->account_id = account_id;
    this->password = password;
    this->balance = initial_amount;
}

int account::deposit_funds(int amount){
    balance += amount;
    return balance;
}

bool account::withdraw_funds(int amount){
    if (balance >= amount){
        balance -= amount;
        return true;
    }
    return false;
}

