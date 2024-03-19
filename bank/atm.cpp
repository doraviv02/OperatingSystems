#include <iostream>
#include "atm.h"

using namespace std;

ATM::ATM(int id, Bank* bank){
    this->id = id;
    this->bank = bank;
}

void ATM::open_account(int account_id, int password, int initial_ammount){
    if (bank->check_account(account_id) != -1){ //account id already used
        cout<<"Error "<<this->id<<": Your transaction failed - account with the same id exists"<<endl;
    }
    else{
        bank->add_account(account_id, password, initial_ammount);
        cout<<this->id<<": New account id is "<<account_id<<" with password "<<password<<" and initial balance "<<initial_ammount<<endl;
    
    }
}

void ATM::deposit(int account_id, int password, int amount){
    int i = check_account_id(account_id);
    if (i!=-1){
        if (check_password(i, password)){
            int bal = bank->deposit(i, amount);cout<<this->id<<": Account "
            <<account_id<<" new balance is "<<bal<<" after "<< amount << "$ was deposited"<<endl;
        }
    }
}

void ATM::withdraw(int account_id, int password, int amount){
    int i = check_account_id(account_id);
    if (i!=-1){
        if (check_password(i, password)){
            if (bank->withdraw(i, amount) == -1){
                cout<<"Error "<<this->id
                <<": Your transaction failed - account id "<<account_id
                <<"balance is lower than "<<amount<<endl;
            }
            else{
                cout<<this->id<<": Account "<<account_id<<" new balance is "<<
                bank->get_balance(i)<<" after "<<amount<<"$ was withdrew"<<endl;
            }
        }
    }
}

void ATM::check_balance(int account_id, int password){
    int i = check_account_id(account_id);
    if (i != -1){
        if (check_password(i, password)){
            cout<<this->id<< ": Account "<<account_id<<" balance is "<<bank->get_balance(i)<<endl;
        }
    }
}

void ATM::close_account(int account_id, int password){
    int i = check_account_id(account_id);
    if (i != -1){
        if (check_password(i, password)){
            int balance = bank->close_account(i);
            cout<<this->id<<": Account "<<account_id<<" is now closed. Balance was "<< balance<<endl;
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
                    cout<<this->id<<": Transfer "<<amount<<" from account "<<account_id<<" to account "<<target_account_id<<
                    " new account balance is "<<account_bal<<" new target account balance is "<<target_bal;
                }
            }

        }
    }
}


int ATM::check_account_id(int account_id){
    int i = bank->check_account(account_id);
    if (i!= -1) return i;
    else{
        cout<< "Error "<<this->id<<": Your transaction failed - account id "<<account_id<<" does not exist"<<endl;
        return -1;
        }
}

bool ATM::check_password(int index, int password){
    if (bank->check_password(index, password)) return true;
    else{
        cout<< "Error "<<this->id<<
        ": Your transaction failed- password for account id "<<index<<" is incorrect"<<endl;

        return false;
    }
}