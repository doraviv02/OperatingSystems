#include <iostream>
#include <fstream>
#include <unistd.h>
#include "atm.h"


using namespace std;
extern pthread_mutex_t atm_mutex_log;

ATM::ATM(int id, Bank* bank){
    this->id = id;
    this->bank = bank;

}

ATM::~ATM() {
    //pthread_mutex_destroy(&(atm_mutex_log));
}

void ATM::atm_log_lock() {
    //printf("[DEBUG] %d Aquiring log mutex %p...\n", this->id, &(atm_mutex_log));
    pthread_mutex_lock(&(atm_mutex_log));
    //printf("[DEBUG] %d Aquired log mutex %p!\n", this->id, &(atm_mutex_log));
}

void ATM::atm_log_unlock() {
    //printf("[DEBUG] %d unlocking log mutex %p...\n", this->id, &(atm_mutex_log));
    pthread_mutex_unlock(&(atm_mutex_log));
    //printf("[DEBUG] %d unlocking done %p!\n", this->id, &(atm_mutex_log));
}

void ATM::open_account(int account_id, int password, int initial_amount){
    bank->bank_read_lock();
    int account_index = bank->check_account(account_id);
    bank->bank_read_unlock();

    if (account_index == -1) {
        bank->bank_write_lock();
        bank->add_account(account_id, password, initial_amount);
        bank->bank_write_unlock();

        this->atm_log_lock();
        sleep(1);
        cout<<this->id<<": New account id is "<<account_id<<" with password "<<password<<" and initial balance "<<initial_amount<<endl;
        this->atm_log_unlock();
    }
    else {
        this->atm_log_lock();
        sleep(1);
        cout<<"Error "<<this->id<<": Your transaction failed – account with the same id exists"<<endl;
        this->atm_log_unlock();
    }
}

void ATM::deposit(int account_id, int password, int amount){
    int i = check_account_id(account_id);
    if (i!=-1){
        if (check_password(i, password)){
            int bal = bank->deposit(i, amount);

            this->atm_log_lock();
            sleep(1);
            cout<<this->id<<": Account "
            <<account_id<<" new balance is "<<bal<<" after "<< amount << " $ was deposited"<<endl;
            this->atm_log_unlock();
        }
    }
}

void ATM::withdraw(int account_id, int password, int amount){
    int i = check_account_id(account_id);
    if (i!=-1){
        if (check_password(i, password)){
            if (bank->withdraw(i, amount) == -1){

                this->atm_log_lock();
                sleep(1);
                cout<<"Error "<<this->id
                <<": Your transaction failed - account id "<<account_id
                <<" balance is lower than "<<amount<<endl;
                this->atm_log_unlock();
            }
            else{
                this->atm_log_lock();
                sleep(1);
                cout<<this->id<<": Account "<<account_id<<" new balance is "<<
                bank->get_balance(i)<<" after "<<amount<<"$ was withdrew"<<endl;
                this->atm_log_unlock();
            }
        }
    }
}

void ATM::check_balance(int account_id, int password){
    int i = check_account_id(account_id);
    if (i != -1){
        if (check_password(i, password)){
            this->atm_log_lock();
            sleep(1);
            cout<<this->id<< ": Account "<<account_id<<" balance is "<<bank->get_balance(i)<<endl;
            this->atm_log_unlock();
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

            this->atm_log_lock();
            sleep(1);
            cout<<this->id<<": Account "<<account_id<<" is now closed. Balance was "<< balance<<endl;
            this->atm_log_unlock();
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

                    this->atm_log_lock();
                    sleep(1);
                    cout<<this->id<<": Transfer "<<amount<<" from account "<<account_id<<" to account "<<target_account_id<<
                    " new account balance is "<<account_bal<<" new target account balance is "<<target_bal<<endl;
                    this->atm_log_unlock();
                }
                else {
                    this->atm_log_lock();
                    sleep(1);
                    cout<<"Error "<<this->id
                        <<": Your transaction failed - account id "<<account_id
                        <<" balance is lower than "<<amount<<endl;
                    this->atm_log_unlock();
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
        this->atm_log_lock();
        sleep(1);
        cout<< "Error "<<this->id<<": Your transaction failed - account id "<<account_id<<" does not exist"<<endl;
        this->atm_log_unlock();
        return -1;
        }
}

bool ATM::check_password(int index, int password){
    if (bank->check_password(index, password)) return true;
    else{
        int account_id = bank->get_account_id(index);
        this->atm_log_lock();
        sleep(1);
        cout<< "Error "<<this->id<<
        ": Your transaction failed- password for account id "<<account_id<<" is incorrect"<<endl;
        this->atm_log_unlock();

        return false;
    }
}