#include <cstdlib>
#include <iostream>
#include <iomanip>
# include "bank.h"

//pthread_mutex_t bank_mutex_read;
//pthread_mutex_t bank_mutex_write;
pthread_mutex_t atm_mutex_log;

int bank_read_count = 0;

bool compare_accounts(account& a, account& b) {
    return a.get_account_id() < b.get_account_id();
}

Bank::Bank(){
    this->balance = 0;

    int err = 0;
    err += pthread_mutex_init(&bank_mutex_read, NULL);
    err += pthread_mutex_init(&bank_mutex_write, NULL);
    err += pthread_mutex_init(&atm_mutex_log, NULL);

    if (err != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_init failed");
        exit(1);
    }
    atm_log_file.open("log.txt", ofstream::out | ofstream::app);
}

Bank::~Bank() {
    int err = 0;
    err += pthread_mutex_destroy(&bank_mutex_read);
    err += pthread_mutex_destroy(&bank_mutex_write);
    err += pthread_mutex_destroy(&(atm_mutex_log));

    if (err != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_destroy failed");
        exit(1);
    }

    atm_log_file.close();
}

// Bank::Bank(vector<account> accounts){
//     this->accounts = accounts;
//     this->balance = 0;
// }

void Bank::bank_write_lock() {
    if (pthread_mutex_lock(&bank_mutex_write) != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_lock failed");
        exit(1);
    }
}

void Bank::bank_write_unlock() {
    if (pthread_mutex_unlock(&bank_mutex_write) != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_unlock failed");
        exit(1);
    }
}

void Bank::bank_read_lock() {
    if (pthread_mutex_lock(&bank_mutex_read) && errno != 0) {
        perror("Bank error: pthread_mutex_lock failed");
        exit(1);
    }
    bank_read_count++;
    if (bank_read_count == 1) {
        bank_write_lock();
    }
    if (pthread_mutex_unlock(&bank_mutex_read) && errno != 0) {
        perror("Bank error: pthread_mutex_unlock failed");
        exit(1);
    }
}

void Bank::bank_read_unlock() {
    if (pthread_mutex_lock(&bank_mutex_read) && errno != 0) {
        perror("Bank error: pthread_mutex_lock failed");
        exit(1);
    }
    bank_read_count--;
    if (bank_read_count == 0) {
        bank_write_unlock();
    }
    if (pthread_mutex_unlock(&bank_mutex_read) && errno != 0) {
        perror("Bank error: pthread_mutex_unlock failed");
        exit(1);
    }
}


void Bank::add_account(int account_id, int password, int initial_amount, double sleep_dur){
    account* a = new account(account_id, password, initial_amount);
    accounts.push_back(*a);
    usleep(1000000 * sleep_dur);
}

int Bank::check_account(int account_id, double sleep_dur){
    //printf("[DEBUG] Bank Checking if account %d exists out of %d accounts...\n", account_id, (int)accounts.size());
    //bank_read_lock();

    usleep(1000000 * sleep_dur);
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

bool Bank::check_password(int i, int password, double sleep_dur){
    accounts[i].account_read_lock();

    int ret = accounts[i].check_password(password);
    usleep(1000000 * sleep_dur);

    accounts[i].account_read_unlock();

    return ret;
}

int Bank::deposit(int i, int amount, double sleep_dur) {
    accounts[i].account_read_lock();

    int ret = accounts[i].deposit_funds(amount);
    usleep(1000000 * sleep_dur);

    accounts[i].account_read_unlock();

    return ret;
}

int Bank::get_account_id(int i, double sleep_dur) {
    accounts[i].account_read_lock();

    int ret = accounts[i].get_account_id();
    usleep(1000000 * sleep_dur);

    accounts[i].account_read_unlock();

    return ret;
}

int Bank::get_balance(int i, double sleep_dur) {
    accounts[i].account_read_lock();

    int ret = accounts[i].get_balance();
    usleep(1000000 * sleep_dur);

    accounts[i].account_read_unlock();

    return ret;
}

int Bank::withdraw(int i, int amount, double sleep_dur){
    accounts[i].account_write_lock();
    int withdraw_ret = accounts[i].withdraw_funds(amount);
    usleep(1000000 * sleep_dur);
    accounts[i].account_write_unlock();

    return withdraw_ret;
}

int Bank::close_account(int i, double sleep_dur){
    accounts[i].account_read_lock();
    int balance = accounts[i].get_balance();
    usleep(1000000 * sleep_dur);
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
        int amount = floor((double)accounts[i].get_balance() * (double)percent / 100.0);
        int id = accounts[i].get_account_id();
        accounts[i].account_read_unlock();

        accounts[i].account_write_lock();
        accounts[i].withdraw_funds(amount);
        accounts[i].account_write_unlock();

        balance += amount;

        // Use log file as shared file
        if (pthread_mutex_lock(&(atm_mutex_log)) != 0  && errno != 0) {
            perror("Bank error: pthread_mutex_lock failed");
            exit(1);
        }
        atm_log_file << "Bank: commissions of " << percent << " % were charged, the bank gained " << amount <<
        " $ from account " << id << endl;
        if (pthread_mutex_unlock(&(atm_mutex_log)) != 0  && errno != 0) {
            perror("Bank error: pthread_mutex_unlock failed");
            exit(1);
        }
    }   

}

void Bank::print_accounts(int* done_flag){
    int number_of_accounts, mutex_success;
    while(1) {
        mutex_success = 1;
        bank_read_lock();
        number_of_accounts = accounts.size();
        bank_read_unlock();

        for (int i = 0; i < number_of_accounts; i++) {
            if (accounts[i].account_read_trylock() != 0) {
                for (int j = 0; j < i; j++) {
                    accounts[j].account_read_unlock();
                }
                mutex_success = 0;
                break;
            }
        }

        if (mutex_success || *done_flag) break;

        usleep(50000);
    }

    // This thread is the only one printing to stdout, so locking isn't necessary
    printf("\033[2J\033[1;1H");
    cout<<"Current Bank Status"<<endl;

    vector<account> sorted_accounts = accounts;
    sort(sorted_accounts.begin(), sorted_accounts.end(), compare_accounts);
    for (int i = 0; i < number_of_accounts; i++) {
        accounts[i].account_read_unlock();
    }

    for (int i = 0; i < number_of_accounts; i++){
        cout<<"Account "<<sorted_accounts[i].get_account_id()<<": Balance - "
        <<sorted_accounts[i].get_balance()<<" $, Account Password - "<< setw(4) << setfill('0') <<
        sorted_accounts[i].get_password()<<endl;
    }
    cout<<"The Bank has "<<balance<<" $"<<endl;


}