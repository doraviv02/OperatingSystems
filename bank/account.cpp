// implement acocunt
#include "account.h"

account::account(int account_i, int password, int initial_amount){
    this->account_id = account_id;
    this->password = password;
    this->balance = initial_amount;
}

bool account::check_password(int test_password){
    return test_password == password;
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

