#include <iostream>
#include <fstream>
#include <unistd.h>
#include "atm.h"


using namespace std;
pthread_mutex_t atm_mutex_log;

ATM::ATM(int id, Bank* bank){
    this->id = id;
    this->bank = bank;
    pthread_mutex_init(&atm_mutex_log, NULL);
}

ATM::~ATM() {
    pthread_mutex_destroy(&atm_mutex_log);
}

void ATM::atm_log_lock() {
    pthread_mutex_lock(&atm_mutex_log);
}

void ATM::atm_log_unlock() {
    pthread_mutex_unlock(&atm_mutex_log);
}

void ATM::open_account(int account_id, int password, int initial_amount){
    bank->bank_read_lock();
    int account_index = bank->check_account(account_id);
    bank->bank_read_unlock();

    if (account_index == -1) {
        bank->bank_write_lock();
        bank->add_account(account_id, password, initial_amount);
        bank->bank_write_unlock();

        printf("[DEBUG] %d Aquiring log mutex...\n", this->id);
        pthread_mutex_lock(&atm_mutex_log);
        printf("[DEBUG] %d Aquired log mutex!\n", this->id);
        cout<<this->id<<": New account id is "<<account_id<<" with password "<<password<<" and initial balance "<<initial_amount<<endl;
        printf("[DEBUG] %d unlocking log mutex...\n", this->id);
        pthread_mutex_unlock(&atm_mutex_log);
        printf("[DEBUG] %d unlocking done!\n", this->id);
    }
    else {
        printf("[DEBUG] %d Aquiring log mutex...\n", this->id);
        pthread_mutex_lock(&atm_mutex_log);
        printf("[DEBUG] %d Aquired log mutex!\n", this->id);
        cout<<"Error "<<this->id<<": Your transaction failed – account with the same id exists"<<endl;
        printf("[DEBUG] %d unlocking log mutex...\n", this->id);
        pthread_mutex_unlock(&atm_mutex_log);
        printf("[DEBUG] %d unlocking done!\n", this->id);
    }
}

void ATM::deposit(int account_id, int password, int amount){
    int i = check_account_id(account_id);
    if (i!=-1){
        if (check_password(i, password)){
            int bal = bank->deposit(i, amount);

            printf("[DEBUG] %d Aquiring log mutex...\n", this->id);
            pthread_mutex_lock(&atm_mutex_log);
            printf("[DEBUG] %d Aquired log mutex!\n", this->id);
            cout<<this->id<<": Account "
            <<account_id<<" new balance is "<<bal<<" after "<< amount << " $ was deposited"<<endl;
            printf("[DEBUG] %d unlocking log mutex...\n", this->id);
            pthread_mutex_unlock(&atm_mutex_log);
            printf("[DEBUG] %d unlocking done!\n", this->id);
        }
    }
}

void ATM::withdraw(int account_id, int password, int amount){
    int i = check_account_id(account_id);
    if (i!=-1){
        if (check_password(i, password)){
            if (bank->withdraw(i, amount) == -1){

                printf("[DEBUG] %d Aquiring log mutex...\n", this->id);
                pthread_mutex_lock(&atm_mutex_log);
                printf("[DEBUG] %d Aquired log mutex!\n", this->id);
                cout<<"Error "<<this->id
                <<": Your transaction failed - account id "<<account_id
                <<" balance is lower than "<<amount<<endl;
                printf("[DEBUG] %d unlocking log mutex...\n", this->id);
                pthread_mutex_unlock(&atm_mutex_log);
                printf("[DEBUG] %d unlocking done!\n", this->id);
            }
            else{
                printf("[DEBUG] %d Aquiring log mutex...\n", this->id);
                pthread_mutex_lock(&atm_mutex_log);
                printf("[DEBUG] %d Aquired log mutex!\n", this->id);
                cout<<this->id<<": Account "<<account_id<<" new balance is "<<
                bank->get_balance(i)<<" after "<<amount<<"$ was withdrew"<<endl;
                printf("[DEBUG] %d unlocking log mutex...\n", this->id);
                pthread_mutex_unlock(&atm_mutex_log);
                printf("[DEBUG] %d unlocking done!\n", this->id);
            }
        }
    }
}

void ATM::check_balance(int account_id, int password){
    int i = check_account_id(account_id);
    if (i != -1){
        if (check_password(i, password)){
            printf("[DEBUG] %d Aquiring log mutex...\n", this->id);
            pthread_mutex_lock(&atm_mutex_log);
            printf("[DEBUG] %d Aquired log mutex!\n", this->id);
            cout<<this->id<< ": Account "<<account_id<<" balance is "<<bank->get_balance(i)<<endl;
            printf("[DEBUG] %d unlocking log mutex...\n", this->id);
            pthread_mutex_unlock(&atm_mutex_log);
            printf("[DEBUG] %d unlocking done!\n", this->id);
        }
    }
}

void ATM::close_account(int account_id, int password){
    int i = check_account_id(account_id);
    if (i != -1){
        if (check_password(i, password)){
            bank->bank_write_lock();
            int balance = bank->close_account(i);
            bank->bank_write_unlock();

            printf("[DEBUG] %d Aquiring log mutex...\n", this->id);
            pthread_mutex_lock(&atm_mutex_log);
            printf("[DEBUG] %d Aquired log mutex!\n", this->id);
            cout<<this->id<<": Account "<<account_id<<" is now closed. Balance was "<< balance<<endl;
            printf("[DEBUG] %d unlocking log mutex...\n", this->id);
            pthread_mutex_unlock(&atm_mutex_log);
            printf("[DEBUG] %d unlocking done!\n", this->id);
        }
    }
}

void ATM::transfer(int account_id, int password, int target_account_id, int amount){
    int i = check_account_id(account_id);
    if(i != -1){
        int j = check_account_id(target_account_id);
        if (j != -1){
            if (check_password(i, password)){
                int account_bal = bank->withdraw(i,amount);
                if (account_bal != -1){
                    int target_bal = bank->deposit(j,amount);

                    printf("[DEBUG] %d Aquiring log mutex...\n", this->id);
                    pthread_mutex_lock(&atm_mutex_log);
                    printf("[DEBUG] %d Aquired log mutex!\n", this->id);
                    cout<<this->id<<": Transfer "<<amount<<" from account "<<account_id<<" to account "<<target_account_id<<
                    " new account balance is "<<account_bal<<" new target account balance is "<<target_bal<<endl;
                    printf("[DEBUG] %d unlocking log mutex...\n", this->id);
                    pthread_mutex_unlock(&atm_mutex_log);
                    printf("[DEBUG] %d unlocking done!\n", this->id);
                }
                else {
                    printf("[DEBUG] %d Aquiring log mutex...\n", this->id);
                    pthread_mutex_lock(&atm_mutex_log);
                    printf("[DEBUG] %d Aquired log mutex!\n", this->id);
                    cout<<"Error "<<this->id
                        <<": Your transaction failed - account id "<<account_id
                        <<" balance is lower than "<<amount<<endl;
                    printf("[DEBUG] %d unlocking log mutex...\n", this->id);
                    pthread_mutex_unlock(&atm_mutex_log);
                    printf("[DEBUG] %d unlocking done!\n", this->id);
                }
            }

        }
    }
}


int ATM::check_account_id(int account_id){
    bank->bank_read_lock();
    int i = bank->check_account(account_id);
    bank->bank_read_unlock();
    if (i!= -1) return i;
    else{
        printf("[DEBUG] %d Aquiring log mutex...\n", this->id);
        pthread_mutex_lock(&atm_mutex_log);
        printf("[DEBUG] %d Aquired log mutex!\n", this->id);
        cout<< "Error "<<this->id<<": Your transaction failed - account id "<<account_id<<" does not exist"<<endl;
        printf("[DEBUG] %d unlocking log mutex...\n", this->id);
        pthread_mutex_unlock(&atm_mutex_log);
        printf("[DEBUG] %d unlocking done!\n", this->id);
        return -1;
        }
}

bool ATM::check_password(int index, int password){
    if (bank->check_password(index, password)) return true;
    else{
        int account_id = bank->get_account_id(index);
        printf("[DEBUG] %d Aquiring log mutex...\n", this->id);
        pthread_mutex_lock(&atm_mutex_log);
        printf("[DEBUG] %d Aquired log mutex!\n", this->id);
        cout<< "Error "<<this->id<<
        ": Your transaction failed- password for account id "<<account_id<<" is incorrect"<<endl;
        printf("[DEBUG] %d unlocking log mutex...\n", this->id);
        pthread_mutex_unlock(&atm_mutex_log);
        printf("[DEBUG] %d unlocking done!\n", this->id);

        return false;
    }
}