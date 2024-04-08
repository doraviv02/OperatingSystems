// implement acocunt
#include "account.h"




account::account(int account_id, int password, int initial_amount){
    this->account_id = account_id;
    this->password = password;
    this->balance = initial_amount;
    this->account_read_count = 0;
    if (pthread_mutex_init(&account_mutex_read, NULL) != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_init failed");
        exit(1);
    }
    if (pthread_mutex_init(&account_mutex_write, NULL) != 0 && errno != 0){
        perror("Bank error: pthread_mutex_init failed");
        exit(1);
    }
}

account::~account() {
    if (pthread_mutex_destroy(&account_mutex_read) != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_destroy failed");
        exit(1);
    }
    if (pthread_mutex_destroy(&account_mutex_write) != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_destroy failed");
        exit(1);
    }
}

void account::account_write_lock() {
    //printf("[DEBUG] Acquiring writer mutex %p for account %d\n", &account_mutex_write, this->account_id);
    if (pthread_mutex_lock(&account_mutex_write) != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_lock failed");
        exit(1);
    }
    //printf("[DEBUG] Acquired writer mutex %p for account %d\n", &account_mutex_write, this->account_id);
}

void account::account_write_unlock() {
    //printf("[DEBUG] Unlocking writer mutex %p for account %d\n", &account_mutex_write, this->account_id);
    if (pthread_mutex_unlock(&account_mutex_write) != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_unlock failed");
        exit(1);
    }
    //printf("[DEBUG] Unlocked writer mutex %p for account %d\n", &account_mutex_write, this->account_id);
}

void account::account_read_lock() {
    //printf("[DEBUG] Acquiring reader mutex %p for account %d...", &account_mutex_read, this->account_id);
    if (pthread_mutex_lock(&account_mutex_read) != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_lock failed");
        exit(1);
    }
    account_read_count++;
    if (account_read_count == 1) {
        account_write_lock();
    }
    //printf(" %d readers\n", account_read_count);
    if (pthread_mutex_unlock(&account_mutex_read) != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_unlock failed");
        exit(1);
    }
    //printf("[DEBUG] Acquired reader mutex %p for account %d\n", &account_mutex_read, this->account_id);
}

void account::account_read_unlock() {
    //printf("[DEBUG] Unlocking reader mutex %p for account %d...", &account_mutex_read, this->account_id);
    if (pthread_mutex_lock(&account_mutex_read) != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_lock failed");
        exit(1);
    }
    account_read_count--;
    if (account_read_count == 0) {
        account_write_unlock();
    }
    //printf(" %d readers\n", account_read_count);
    if (pthread_mutex_unlock(&account_mutex_read) != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_unlock failed");
        exit(1);
    }
    //printf("[DEBUG] Unlocked reader mutex %p for account %d\n", &account_mutex_read, this->account_id);
}

int account::account_read_trylock() {
    if (pthread_mutex_trylock(&account_mutex_read) != 0) return -1;
    account_read_count++;
    if (account_read_count == 1) {
        if (pthread_mutex_trylock(&account_mutex_write) != 0) {
            account_read_count--;
            if (pthread_mutex_unlock(&account_mutex_read) != 0 && errno != 0) {
                perror("Bank error: pthread_mutex_unlock failed");
                exit(1);
            }
            return -1;
        }
    }
    if (pthread_mutex_unlock(&account_mutex_read ) != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_unlock failed");
        exit(1);
    }
    return 0;
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

