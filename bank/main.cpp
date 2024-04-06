#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "atm.h"
#include "bank.h"

using namespace std;

struct ATM_Command{
    int ATM_id;
    char* command;
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
//        printf("[DEBUG] ATM %d Opening account %d with password %d and initial amount %d\n", id, account_id,
//               pass, initial);
    }
    else if (command == "D"){
        string amount;
        getline(ss, amount, ' '); //get the amount
        int am = stoi(amount);
        ATMs[id].deposit(account_id, pass, am);
        //printf("[DEBUG] ATM %d Depositing %d to account %d with password %d\n", id, am, account_id, pass);
    }
    else if (command == "W"){
        string amount;
        getline(ss, amount, ' '); //get the amount
        int am = stoi(amount);
        ATMs[id].withdraw(account_id, pass, am);
        //printf("[DEBUG] ATM %d Withdrawing %d from account %d with password %d\n", id, am, account_id, pass);
    }
    else if (command == "B"){
        ATMs[id].check_balance(account_id, pass);
        //printf("[DEBUG] ATM %d Checking balance of account %d with password %d\n", id, account_id, pass);
    }
    else if (command == "Q"){ //close account
        ATMs[id].close_account(account_id, pass);
        //printf("[DEBUG] ATM %d Closing account %d with password %d\n", id, account_id, pass);
    }
    else if (command == "T"){
        string target_account, amount;
        getline(ss, target_account, ' '); //get the target account
        getline(ss, amount, ' '); //get the amount
        int target = stoi(target_account);
        int am = stoi(amount);
        ATMs[id].transfer(account_id, pass, target, am);
//        printf("[DEBUG] ATM %d Transferring %d from account %d with password %d to account %d\n", id,
//               am, account_id, pass, target);
    }
    
}

void* ATM_runner(void* arg) // Run ATM thread
{
    //printf("[DEBUG] aaa\n");
    ATM_Command* atm_command = (ATM_Command*) arg;
    char* input_file_name = atm_command->command;
    int ATM_id = atm_command->ATM_id;

    ifstream file(input_file_name);
    string line;
    printf("[DEBUG] ATM_runner %d %s\n", ATM_id, input_file_name);
    while(getline(file, line)){
        //parse the line and do command somehow
        //printf("[DEBUG] ATM %d running %s\n", ATM_id, line.c_str());
        parse_command(line, ATM_id);
        usleep(100000); //sleep for 100ms
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
    int ATM_total_num = argc - 1;
    pthread_t *threads = new pthread_t[ATM_total_num + 1];
    ATM_Command *ATM_Commands = new ATM_Command[ATM_total_num];
    //pthread_t threads[ATM_total_num + 1]; //last thread is the bank thread
    //printf("[DEBUG] Number of ATMs: %d\n", ATM_total_num);

    //vector<ATM_Command> ATM_Commands;
    for (int i = 0; i < ATM_total_num; i++) {

        ATMs.push_back(ATM(i, bank));
        //ATM_Command atm_command = {i, argv[i+1]};
        ATM_Commands[i] = {i, argv[i+1]};
        printf("[DEBUG] Adding ATM #%d from %s\n", i, argv[i+1]);
        pthread_create(&threads[i], NULL, ATM_runner, &ATM_Commands[i]);
        //printf("%s\n", argv[i]);
    }

    for (int i = 0; i < ATM_total_num; i++) {
        pthread_join(threads[i], NULL);
    }

    delete[] threads;
    delete[] ATM_Commands;
    return 0;
}

