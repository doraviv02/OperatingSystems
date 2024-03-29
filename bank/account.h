#ifndef ACCOUNT_H
#define ACCOUNT_H

// implement account

class account{
    public:
        account(int account_id, int password, int initial_amount);
        int get_account_id() {return account_id;}
        bool check_password(int test_password) {return test_password == password;}
        int get_password() {return password;} 
        int get_balance() {return balance;}
        int deposit_funds(int amount);
        bool withdraw_funds(int amount);
        ~account() = default;
    private:
        int account_id;
        int password;
        int balance;
};


#endif