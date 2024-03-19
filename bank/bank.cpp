#include <cstdlib>
#include <iostream>
# include "bank.h"

Bank::Bank(vector<account> accounts){
    this->accounts = accounts;
    this->balance = 0;
}

int Bank::check_account(int account_id){
    for (int i = 0; i < accounts.size(); i++){
        if (accounts[i].get_account_id() == account_id){
            return i;
        }
    }
    return -1;
}

bool Bank::check_password(int i, int password){
    return accounts[i].check_password(password);
}

int Bank::withdraw(int i, int amount){
    if (accounts[i].withdraw_funds(amount)){
        balance -= amount;
        return accounts[i].get_balance();
    }
    return -1;
}

int Bank::close_account(int i){
    int balance = accounts[i].get_balance();
    accounts.erase(accounts.begin() + i);
    return balance;
}

void Bank::charge_commission(){
    //get random number from 1-5
    int percent = rand()%5 + 1;
    for (int i = 0; i < accounts.size(); i++)
    {
        int amount = accounts[i].get_balance() * percent / 100;
        accounts[i].withdraw_funds(amount);
        balance += amount;
    }   
   
}

void Bank::print_accounts(){
    printf("\033[2J\033[1;1H");
    cout<<"Current Bank Status"<<endl;
    for (int i = 0; i < accounts.size(); i++){
        cout<<"Account "<<accounts[i].get_account_id()<<": Balance - "
        <<accounts[i].get_balance()<<" $, Account Password - "<<accounts[i].get_password()<<endl;
    }
    cout<<"The Bank has "<<balance<<" $"<<endl;
}