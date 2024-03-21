#include <stdio.h>
#include <atm.h>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <sstream>

#include "bank.h"

using namespace std;

struct ATM_Command{
    int ATM_id;
    string command;
};

Bank* bank = new Bank();
vector<ATM> ATMs;


void parse_command(string line, int id){
    string command, account, password;
    stringstream ss(line);

    getline(ss, command, ' '); //get the command
    getline(ss, account, ' '); //get the account
    getline(ss, password, ' '); //get the password
    int account_id = stoi(account);
    int pass = stoi(password);

    if (command == "O"){
        string initial_amount;
        getline(ss, initial_amount, ' '); //get the initial amount
        int initial = stoi(initial_amount);
        ATMs[id].open_account(account_id, pass, initial);
    }
    else if (command == "D"){
        string amount;
        getline(ss, amount, ' '); //get the amount
        int am = stoi(amount);
        ATMs[id].deposit(account_id, pass, am);
    }
    else if (command == "W"){
        string amount;
        getline(ss, amount, ' '); //get the amount
        int am = stoi(amount);
        ATMs[id].withdraw(account_id, pass, am);
    }
    else if (command == "B"){
        ATMs[id].check_balance(account_id, pass);
    }
    else if (command == "Q"){ //close account
        ATMs[id].close_account(account_id, pass);
    }
    else if (command == "T"){
        string target_account, amount;
        getline(ss, target_account, ' '); //get the target account
        getline(ss, amount, ' '); //get the amount
        int target = stoi(target_account);
        int am = stoi(amount);
        ATMs[id].transfer(account_id, pass, target, am);
    }
    
}

void* ATM_runner(void* arg) // Run ATM thread
{
    ATM_Command* atm_command = (ATM_Command*) arg;
    ifstream file(atm_command->command);
    string line;
    while(getline(file, line)){
        //parse the line and do command somehow
        parse_command(line, atm_command->ATM_id);
        sleep(0.1); //sleep for 100ms
    }
    file.close();
    pthread_exit(NULL);
}


void* Bank_runner(void* arg){
    while(true){
        sleep(3); //sleep for 3 seconds
        bank->charge_commission();
    
    }
}

int main(int argc, char* argv[])
{
    pthread_t threads[argc]; //last thread is the bank thread
    printf("You have entered %d arguments:\n", argc);
 
    for (int i = 0; i < argc; i++) {
        ATMs.push_back(ATM(i, bank));
        ATM_Command atm_command = {i, argv[i]};
        pthread_create(&threads[i], NULL, ATM_runner, &atm_command);
        //printf("%s\n", argv[i]);
    }

    return 0;
}

