#include <iostream>
#include <fstream>
#include <unistd.h>
#include "atm.h"


using namespace std;
extern pthread_mutex_t atm_mutex_log;
extern ostream atm_log_file;
extern void atm_log_lock();
extern void atm_log_unlock();

ATM::ATM(int id, Bank* bank){
    this->id = id;
    this->bank = bank;

}

ATM::~ATM() {
    //pthread_mutex_destroy(&(atm_mutex_log));
}


void ATM::open_account(int account_id, int password, int initial_amount){
    //bank->bank_read_lock();
    //bank->bank_write_lock();
    int MY_ID = id;
    //printf("[DEBUG] AAA %d\n", MY_ID);
    int account_index = bank->check_account(account_id, 0);
    //bank->bank_read_unlock();

    if (account_index == -1) {
        //bank->bank_write_lock();
        //atm_log_lock();
        //printf("[DEBUG] AAAAAA %d\n", MY_ID);
        bank->add_account(MY_ID, account_id, password, initial_amount, 1);
//        atm_log_file<<this->id<<": New account id is "<<account_id<<" with password "<<setw(4) << setfill('0') <<
//        password<<" and initial balance "<<initial_amount<<endl;
        //atm_log_unlock();

    }
    else {
        atm_log_lock();
        atm_log_file<<"Error "<<MY_ID<<": Your transaction failed - account with the same id exists"<<endl;
        sleep(1);
        atm_log_unlock();
    }
    //bank->bank_write_unlock();
}

void ATM::deposit(int account_id, int password, int amount){
    int i = check_account_id(account_id, 1);
    if (i!=-1){
        if (check_password(i, password, 1)){

            //atm_log_lock();
            bank->deposit(this->id, i, amount, 1);
//            atm_log_file<<this->id<<": Account "
//            <<account_id<<" new balance is "<<bal<<" after "<< amount << " $ was deposited"<<endl;
//            atm_log_unlock();
        }
        //else usleep(500000);
    }
    //else usleep(500000);
}

void ATM::withdraw(int account_id, int password, int amount){
    int i = check_account_id(account_id, 1);
    if (i!=-1){
        if (check_password(i, password, 1)){
            bank->withdraw(this->id, i, amount, 1);

//            if (bank->get_balance(i,0) < amount){//bank->withdraw(i, amount, 1) == -1){
//
//                atm_log_lock();
//                atm_log_file<<"Error "<<this->id
//                <<": Your transaction failed - account id "<<account_id
//                <<" balance is lower than "<<amount<<endl;
//                atm_log_unlock();
//            }
//            else{
//                //atm_log_lock();
//                bank->withdraw(this->id, i, amount, 1);
//                atm_log_file<<this->id<<": Account "<<account_id<<" new balance is "<<
//                bank->get_balance(i, 0)<<" after "<<amount<<" $ was withdrew"<<endl;
//                atm_log_unlock();
//            }
        }
        //else sleep(1);
    }
    //else sleep(1);
}

void ATM::check_balance(int account_id, int password){
    int i = check_account_id(account_id, 1);
    if (i != -1){
        if (check_password(i, password, 1)){
            //sleep(1);
            bank->get_balance(this->id, i, 1);
//            atm_log_lock();
//            atm_log_file<<this->id<< ": Account "<<account_id<<" balance is "<<bank->get_balance(i, 0)<<endl;
//            atm_log_unlock();
        }
    }
}

void ATM::close_account(int account_id, int password){
    int i = check_account_id(account_id, 1);
    if (i != -1){
        if (check_password(i, password, 1)){
            //bank->bank_write_lock();
            bank->close_account(this->id, i, 1);
            //bank->bank_write_unlock();

//            atm_log_lock();
//            atm_log_file<<this->id<<": Account "<<account_id<<" is now closed. Balance was "<< balance<<endl;
//            atm_log_unlock();
        }
    }
}

void ATM::transfer(int account_id, int password, int target_account_id, int amount){
    int source_index = check_account_id(account_id, 0.5);
    if(source_index != -1){
        int target_index = check_account_id(target_account_id, 0.5);
        if (target_index != -1){
            if (check_password(source_index, password, 0.5)){
                bank->transfer(this->id, source_index,target_index,amount, 1);
                //int account_bal = bank->withdraw(i,amount, 0.5);
//                if (bank->get_balance(i,0) >= amount){//account_bal != -1){
//                    //int target_bal = bank->deposit(j,amount, 0.5);
//                    atm_log_lock();
//                    int target_bal = bank->transfer(this->id, i,j,amount, 1);
//                    atm_log_file<<this->id<<": Transfer "<<amount<<" from account "<<account_id<<" to account "<<target_account_id<<
//                    " new account balance is "<<bank->get_balance(i,0)<<" new target account balance is "<<target_bal<<endl;
//                    atm_log_unlock();
//                }
//                else {
//                    atm_log_lock();
//                    atm_log_file<<"Error "<<this->id
//                        <<": Your transaction failed - account id "<<account_id
//                        <<" balance is lower than "<<amount<<endl;
//                    usleep(1000000);
//                    atm_log_unlock();
//                }
            }
            else{
                atm_log_lock();
                atm_log_file<< "Error "<<this->id<<
                ": Your transaction failed - password for account id "<<account_id<<" is incorrect"<<endl;
                usleep(500000);
                atm_log_unlock();
            }
        }
        else{
            atm_log_lock();
            atm_log_file<< "Error "<<this->id<<": Your transaction failed - account id "<<target_account_id<<" does not exist"<<endl;
            usleep(500000);
            atm_log_unlock();
        }
    }
    else{
        atm_log_lock();
        atm_log_file<< "Error "<<this->id<<": Your transaction failed - account id "<<account_id<<" does not exist"<<endl;
        usleep(500000);
        atm_log_unlock();
    
    }

}


int ATM::check_account_id(int account_id, double sleep_dur){
    //bank->bank_read_lock();
    int i = bank->check_account(account_id, 0);
    //bank->bank_read_unlock();
    if (i!= -1) return i;
    else{
        atm_log_lock();
        atm_log_file<< "Error "<<this->id<<": Your transaction failed - account id "<<account_id<<" does not exist"<<endl;
        usleep(1000000 * sleep_dur);
        atm_log_unlock();
        return -1;
        }
}

bool ATM::check_password(int index, int password, double sleep_dur){
    if (bank->check_password(index, password, 0)) return true;
    else{
        int account_id = bank->get_account_id(index, 0);
        atm_log_lock();
        atm_log_file<< "Error "<<this->id<<
        ": Your transaction failed - password for account id "<<account_id<<" is incorrect"<<endl;
        usleep(1000000 * sleep_dur);
        atm_log_unlock();

        return false;
    }
}