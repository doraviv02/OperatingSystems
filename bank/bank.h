#include <vector>
#include "account.h"

using namespace std;

class Bank{
    public:
        Bank();
        Bank(vector<account> accounts);
        void add_account(int account_id, int password, int initial_amount);
        int check_account(int account_id);
        bool check_password(int i, int password);
        int get_balance(int i) {return accounts[i].get_balance();}
        int deposit(int i, int amount) {return accounts[i].deposit_funds(amount);}
        int withdraw(int i, int amount);
        int close_account(int i);
        void charge_commission();
        void print_accounts();
    private:
        int balance;
        vector<account> accounts;
};