#include <cstdlib>
#include <iostream>
#include <iomanip>
# include "bank.h"

//pthread_mutex_t bank_mutex_read;
//pthread_mutex_t bank_mutex_write;
vector<account> accounts;
pthread_mutex_t atm_mutex_log;
ofstream atm_log_file;
pthread_mutex_t bank_mutex_read;
pthread_mutex_t bank_mutex_write;

int bank_read_count = 0;

bool compare_accounts(account& a, account& b) {
    return a.get_account_id() < b.get_account_id();
}

Bank::Bank(){
    this->balance = 0;

    int err = 0;
    err += pthread_mutex_init(&bank_mutex_read, NULL);
    err += pthread_mutex_init(&bank_mutex_write, NULL);
    //err += pthread_mutex_init(&atm_mutex_log, NULL);

    if (err != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_init failed");
        exit(1);
    }
//    printf("[DEBUG] Opening log file...");
//    atm_log_file.open("log.txt", ofstream::out | ofstream::app);
//    printf("done\n");
}

Bank::~Bank() {
    int err = 0;
    err += pthread_mutex_destroy(&bank_mutex_read);
    err += pthread_mutex_destroy(&bank_mutex_write);
    //err += pthread_mutex_destroy(&(atm_mutex_log));

    if (err != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_destroy failed");
        exit(1);
    }

    //atm_log_file.close();
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

void atm_log_lock() {
    //printf("[DEBUG] %d Aquiring log mutex %p...\n", this->id, &(atm_mutex_log));
    if (pthread_mutex_lock(&(atm_mutex_log)) != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_lock failed");
        exit(1);
    }
    //printf("[DEBUG] %d Aquired log mutex %p!\n", this->id, &(atm_mutex_log));
}

void atm_log_unlock() {
    //printf("[DEBUG] %d unlocking log mutex %p...\n", this->id, &(atm_mutex_log));
    if (pthread_mutex_unlock(&(atm_mutex_log)) != 0 && errno != 0) {
        perror("Bank error: pthread_mutex_unlock failed");
        exit(1);
    }
    //printf("[DEBUG] %d unlocking done %p!\n", this->id, &(atm_mutex_log));
}

void Bank::add_account(int atm, int account_id, int password, int initial_amount, double sleep_dur){

    account* a = new account(account_id, password, initial_amount);
    accounts.push_back(*a);
    atm_log_lock();
    atm_log_file<<atm<<": New account id is "<<account_id<<" with password "<<setw(4) << setfill('0') <<
                password<<" and initial balance "<<initial_amount<<endl;
    usleep(1000000 * sleep_dur);
    atm_log_unlock();
}

int Bank::check_account(int account_id, double sleep_dur){
    //printf("[DEBUG] Bank Checking if account %d exists out of %d accounts...\n", account_id, (int)accounts.size());
    //bank_read_lock();

    usleep(1000000 * sleep_dur);
    vector<account>::iterator it;
    int i = 0;
    for (it = accounts.begin(); it < accounts.end(); it++) {
        int current_id = it->get_account_id();

        if (current_id == account_id){
            //bank_read_unlock();
            return i;
        }
        i++;
    }

//    for (int i = 0; i < accounts.size(); i++){
//
//        //accounts[i].account_read_lock();
//        int current_id = accounts[i].get_account_id();
//        //accounts[i].account_read_unlock();
//
//        if (current_id == account_id){
//            //bank_read_unlock();
//            return i;
//        }
//
//    }
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

int Bank::deposit(int atm, int i, int amount, double sleep_dur) {
    accounts[i].account_write_lock();

    int ret = accounts[i].deposit_funds(amount);

    atm_log_lock();
    atm_log_file<<atm<<": Account "
                <<accounts[i].get_account_id()<<" new balance is "<<ret<<" after "<< amount << " $ was deposited"<<endl;
    usleep(1000000 * sleep_dur);
    atm_log_unlock();

    accounts[i].account_write_unlock();

    return ret;
}

int Bank::get_account_id(int i, double sleep_dur) {
    //accounts[i].account_read_lock();

    int ret = accounts[i].get_account_id();
    usleep(1000000 * sleep_dur);

    //accounts[i].account_read_unlock();

    return ret;
}

int Bank::get_balance(int atm, int i, double sleep_dur) {
    accounts[i].account_read_lock();

    int ret = accounts[i].get_balance();
    atm_log_lock();
    atm_log_file<<atm<< ": Account "<<accounts[i].get_account_id()<<" balance is "<<ret<<endl;
    usleep(1000000 * sleep_dur);
    atm_log_unlock();
    accounts[i].account_read_unlock();

    return ret;
}

int Bank::withdraw(int atm, int i, int amount, double sleep_dur){
    accounts[i].account_write_lock();

    int withdraw_ret = accounts[i].withdraw_funds(amount);

    if (withdraw_ret == -1) {
        atm_log_lock();
        atm_log_file<<"Error "<<atm
                    <<": Your transaction failed - account id "<<accounts[i].get_account_id()
                    <<" balance is lower than "<<amount<<endl;
        usleep(1000000 * sleep_dur);
        atm_log_unlock();
        accounts[i].account_write_unlock();
        return -1;
    }

    atm_log_lock();
    atm_log_file<<atm<<": Account "<<accounts[i].get_account_id()<<" new balance is "<<
                withdraw_ret<<" after "<<amount<<" $ was withdrew"<<endl;
    usleep(1000000 * sleep_dur);
    atm_log_unlock();


    accounts[i].account_write_unlock();

    return withdraw_ret;
}

int Bank::transfer(int atm, int source_index, int target_index, int amount, double sleep_dur){
    accounts[min(source_index, target_index)].account_write_lock();
    accounts[max(source_index, target_index)].account_write_lock();
    int withdraw_ret = accounts[source_index].withdraw_funds(amount);
    if (withdraw_ret == -1) {
        atm_log_lock();
        atm_log_file<<"Error "<<atm
                    <<": Your transaction failed - account id "<<accounts[source_index].get_account_id()
                    <<" balance is lower than "<<accounts[source_index].get_balance()<<endl;
        usleep(1000000 * sleep_dur);
        atm_log_unlock();
        accounts[max(source_index, target_index)].account_write_unlock();
        accounts[min(source_index, target_index)].account_write_unlock();
        return -1;
    }
    int deposit_ret = accounts[target_index].deposit_funds(amount);
    atm_log_lock();
    atm_log_file<<atm<<": Transfer "<<amount<<" from account "<<accounts[source_index].get_account_id()
                <<" to account "<<accounts[target_index].get_account_id()<<
                " new account balance is "<<withdraw_ret<<" new target account balance is "<<deposit_ret<<endl;
    usleep(1000000 * sleep_dur);
    atm_log_unlock();
    accounts[max(source_index, target_index)].account_write_unlock();
    accounts[min(source_index, target_index)].account_write_unlock();

    return deposit_ret;
}

int Bank::close_account(int atm, int i, double sleep_dur){
    accounts[i].account_read_lock();
    int account_id = accounts[i].get_account_id();
    int balance = accounts[i].get_balance();
    accounts[i].account_read_unlock();

    accounts.erase(accounts.begin() + i);

    atm_log_lock();
    atm_log_file<<atm<<": Account "<<account_id<<" is now closed. Balance was "<< balance<<endl;
    usleep(1000000 * sleep_dur);
    atm_log_unlock();

    return balance;
}

void Bank::charge_commission(){
    //get random number from 1-5
    int percent = rand()%5 + 1;
    vector<account>::iterator it;

    for (it = accounts.begin(); it < accounts.end(); it++)
    {
        //accounts[i].account_read_lock();
        it->account_write_lock();
        int amount = floor((double)it->get_balance() * (double)percent / 100.0);
        int id = it->get_account_id();
        //accounts[i].account_read_unlock();

        if (pthread_mutex_lock(&(atm_mutex_log)) != 0) {
            perror("Bank error: pthread_mutex_lock failed");
            exit(1);
        }

        //accounts[i].account_write_lock();
        it->withdraw_funds(amount);
        balance += amount;
        atm_log_lock();
        atm_log_file << "Bank: commissions of " << percent << " % were charged, the bank gained " << amount <<
        " $ from account " << id << endl;
        atm_log_unlock();

        // balance += amount;
        // atm_log_file << "Bank: commissions of " << percent << " % were charged, the bank gained " << amount <<
        // " $ from account " << id << endl;
        // Use log file as shared file
        // if (pthread_mutex_lock(&(atm_mutex_log)) != 0) {
        //     perror("Bank error: pthread_mutex_lock failed");
        //     exit(1);
        // }
        
        if (pthread_mutex_unlock(&(atm_mutex_log)) != 0) {
            perror("Bank error: pthread_mutex_unlock failed");
            exit(1);
        }

        it->account_write_unlock();
    }   

}

void Bank::print_accounts(int* done_flag){
    int number_of_accounts, mutex_success;
    while(1) {
        mutex_success = 1;
        //bank_read_lock();
        number_of_accounts = accounts.size();
        //bank_read_unlock();

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