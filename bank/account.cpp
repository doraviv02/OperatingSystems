// implement acocunt
#include "account.h"

account::account(int account_id, int password, int initial_amount){
    this->account_id = account_id;
    this->password = password;
    this->balance = initial_amount;
    this->account_read_count = 0;
    pthread_mutex_init(&account_mutex_read, NULL);
    pthread_mutex_init(&account_mutex_write, NULL);
}

account::~account() {
    pthread_mutex_destroy(&account_mutex_read);
    pthread_mutex_destroy(&account_mutex_write);
}

void account::account_write_lock() {
    pthread_mutex_lock(&account_mutex_write);
}

void account::account_write_unlock() {
    pthread_mutex_unlock(&account_mutex_write);
}

void account::account_read_lock() {
    pthread_mutex_lock(&account_mutex_read);
    account_read_count++;
    if (account_read_count == 1) {
        account_write_lock();
    }
    pthread_mutex_unlock(&account_mutex_read);
}

void account::account_read_unlock() {
    pthread_mutex_lock(&account_mutex_read);
    account_read_count--;
    if (account_read_count == 0) {
        account_write_unlock();
    }
    pthread_mutex_unlock(&account_mutex_read);
}

int account::deposit_funds(int amount){
    balance += amount;
    return balance;
}

int account::withdraw_funds(int amount){
    if (balance >= amount){
        balance -= amount;
        return balance;
    }
    return -1;
}

