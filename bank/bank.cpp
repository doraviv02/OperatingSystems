#include <cstdlib>
#include <iostream>
# include "bank.h"

pthread_mutex_t bank_mutex_read;
pthread_mutex_t bank_mutex_write;
pthread_mutex_t bank_mutex_print;
int bank_read_count = 0;

Bank::Bank(){
    this->balance = 0;
    pthread_mutex_init(&bank_mutex_read, NULL);
    pthread_mutex_init(&bank_mutex_write, NULL);
    pthread_mutex_init(&bank_mutex_print, NULL);
}

Bank::~Bank() {
    pthread_mutex_destroy(&bank_mutex_read);
    pthread_mutex_destroy(&bank_mutex_write);
    pthread_mutex_destroy(&bank_mutex_print);
}

// Bank::Bank(vector<account> accounts){
//     this->accounts = accounts;
//     this->balance = 0;
// }

void Bank::bank_write_lock() {
    pthread_mutex_lock(&bank_mutex_write);
}

void Bank::bank_write_unlock() {
    pthread_mutex_unlock(&bank_mutex_write);
}

void Bank::bank_read_lock() {
    pthread_mutex_lock(&bank_mutex_read);
    bank_read_count++;
    if (bank_read_count == 1) {
        bank_write_lock();
    }
    pthread_mutex_unlock(&bank_mutex_read);
}

void Bank::bank_read_unlock() {
    pthread_mutex_lock(&bank_mutex_read);
    bank_read_count--;
    if (bank_read_count == 0) {
        bank_write_unlock();
    }
    pthread_mutex_unlock(&bank_mutex_read);
}

void Bank::bank_print_lock() {
    pthread_mutex_lock(&bank_mutex_print);
}

void Bank::bank_print_unlock() {
    pthread_mutex_unlock(&bank_mutex_print);
}

void Bank::add_account(int account_id, int password, int initial_amount){
    account* a = new account(account_id, password, initial_amount);
    //bank_write_lock();
    accounts.push_back(*a);
    //bank_write_unlock();
}

int Bank::check_account(int account_id){
    //printf("[DEBUG] Bank Checking if account %d exists out of %d accounts...\n", account_id, (int)accounts.size());
    //bank_read_lock();
    for (int i = 0; i < accounts.size(); i++){

        accounts[i].account_read_lock();
        int current_id = accounts[i].get_account_id();
        accounts[i].account_read_unlock();

        if (current_id == account_id){
            //bank_read_unlock();
            return i;
        }

    }
    //bank_read_unlock();
    return -1;
}

bool Bank::check_password(int i, int password){
    accounts[i].account_read_lock();

    int ret = accounts[i].check_password(password);

    accounts[i].account_read_unlock();

    return ret;
}

int Bank::deposit(int i, int amount) {
    accounts[i].account_read_lock();

    int ret = accounts[i].deposit_funds(amount);

    accounts[i].account_read_unlock();

    return ret;
}

int Bank::get_account_id(int i) {
    accounts[i].account_read_lock();

    int ret = accounts[i].get_account_id();

    accounts[i].account_read_unlock();

    return ret;
}

int Bank::get_balance(int i) {
    accounts[i].account_read_lock();

    int ret = accounts[i].get_balance();

    accounts[i].account_read_unlock();

    return ret;
}

int Bank::withdraw(int i, int amount){
    accounts[i].account_write_lock();
    int withdraw_ret = accounts[i].withdraw_funds(amount);
    accounts[i].account_write_unlock();
//    if (withdraw_ret){
//        balance += amount;
//
//        accounts[i].account_read_lock();
//        int new_balance = accounts[i].get_balance();
//        accounts[i].account_read_unlock();
//        return new_balance;
//    }
    //return -1;
    return withdraw_ret;
}

int Bank::close_account(int i){
    accounts[i].account_read_lock();
    int balance = accounts[i].get_balance();
    accounts[i].account_read_unlock();

    accounts.erase(accounts.begin() + i);

    return balance;
}

void Bank::charge_commission(){
    //get random number from 1-5
    int percent = rand()%5 + 1;
    for (int i = 0; i < accounts.size(); i++)
    {
        accounts[i].account_read_lock();
        int amount = accounts[i].get_balance() * percent / 100;
        accounts[i].account_read_unlock();

        accounts[i].account_write_lock();
        accounts[i].withdraw_funds(amount);
        accounts[i].account_write_unlock();

        balance += amount;
    }   
   
}

//TODO: figure out if this needs mutex
void Bank::print_accounts(){
    printf("\033[2J\033[1;1H");
    cout<<"Current Bank Status"<<endl;
    for (int i = 0; i < accounts.size(); i++){
        cout<<"Account "<<accounts[i].get_account_id()<<": Balance - "
        <<accounts[i].get_balance()<<" $, Account Password - "<<accounts[i].get_password()<<endl;
    }
    cout<<"The Bank has "<<balance<<" $"<<endl;
}