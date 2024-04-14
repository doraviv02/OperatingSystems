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

extern pthread_mutex_t atm_mutex_log;
extern ofstream atm_log_file;

struct ATM_Command{
    int ATM_id;
    char* command;
};

Bank* bank = new Bank();
vector<ATM> ATMs;

int check_input(int argc, char* argv[]) {
    if (argc <= 1) {
        cerr << "Bank error: illegal arguments" << endl;
        return -1;
    }
    for (int i = 1; i < argc; i++) {
        ifstream file(argv[i]);
        if (!file) {
            cerr << "Bank error: illegal arguments" << endl;
            return -1;
        }
        file.close();
    }
    return 0;
}

void parse_command(string line, int id){
    string command, account, password;
    stringstream ss(line);

    getline(ss, command, ' '); //get the command
    getline(ss, account, ' '); //get the account
    getline(ss, password, ' '); //get the password
    int account_id = stoi(account);
    int pass = stoi(password);

    printf("[DEBUG %d] %s\n", id, line.c_str());
    if (command == "O"){
        string initial_amount;
        getline(ss, initial_amount, ' '); //get the initial amount
        int initial = stoi(initial_amount);
        bank->bank_write_lock();
        ATMs[id].open_account(account_id, pass, initial);
        bank->bank_write_unlock();
//        printf("[DEBUG] ATM %d Opening account %d with password %d and initial amount %d\n", id, account_id,
//               pass, initial);
    }
    else if (command == "D"){
        string amount;
        getline(ss, amount, ' '); //get the amount
        int am = stoi(amount);
        bank->bank_read_lock();
        ATMs[id].deposit(account_id, pass, am);
        bank->bank_read_unlock();
        //printf("[DEBUG] ATM %d Depositing %d to account %d with password %d\n", id, am, account_id, pass);
    }
    else if (command == "W"){
        string amount;
        getline(ss, amount, ' '); //get the amount
        int am = stoi(amount);
        bank->bank_read_lock();
        ATMs[id].withdraw(account_id, pass, am);
        bank->bank_read_unlock();
        //printf("[DEBUG] ATM %d Withdrawing %d from account %d with password %d\n", id, am, account_id, pass);
    }
    else if (command == "B"){
        bank->bank_read_lock();
        ATMs[id].check_balance(account_id, pass);
        bank->bank_read_unlock();
        //printf("[DEBUG] ATM %d Checking balance of account %d with password %d\n", id, account_id, pass);
    }
    else if (command == "Q"){ //close account
        bank->bank_write_lock();
        ATMs[id].close_account(account_id, pass);
        bank->bank_write_unlock();
        //printf("[DEBUG] ATM %d Closing account %d with password %d\n", id, account_id, pass);
    }
    else if (command == "T"){
        string target_account, amount;
        getline(ss, target_account, ' '); //get the target account
        getline(ss, amount, ' '); //get the amount
        int target = stoi(target_account);
        int am = stoi(amount);

        bank->bank_read_lock();
        ATMs[id].transfer(account_id, pass, target, am);
        bank->bank_read_unlock();
//        printf("[DEBUG] ATM %d Transferring %d from account %d with password %d to account %d\n", id,
//               am, account_id, pass, target);
    }
    printf("[DEBUG %d] %s DONE\n", id, line.c_str());
    
}

void* ATM_runner(void* arg) // Run ATM thread
{
    //printf("[DEBUG] aaa\n");
    ATM_Command* atm_command = (ATM_Command*) arg;
    char* input_file_name = atm_command->command;
    int ATM_id = atm_command->ATM_id;
    //printf("[DEBUG] ATM ID %d from %s\n", ATM_id, input_file_name);
    ifstream file(input_file_name);

    string line;
    while(getline(file, line)){
        //parse the line and do command somehow
        parse_command(line, ATM_id);
        usleep(100000); //sleep for 100ms
    }
    file.close();
    pthread_exit(NULL);
}

// Commissions thread
void* Bank_runner(void* arg){
    while(true){
        sleep(3); //sleep for 3 seconds
        //TODO: improve parallelism?
        //TODO: check if commission order matters
        bank->bank_read_lock(); //prevent accounts from being added/removed while taking commissions
        bank->charge_commission();
        bank->bank_read_unlock();
        if (*((int*)arg) == 1) pthread_exit(NULL);; //end thread if ATMs done
    }
}

// Print Status thread0
void* status_runner(void* arg) {
    while(true) {
        usleep(500000); //sleep every 0.5 seconds
        bank->print_accounts((int*)arg);
        if (*((int*)arg) == 1) pthread_exit(NULL);; //end thread if ATMs done
    }
}

int main(int argc, char* argv[])
{
    if (check_input(argc, argv) != 0) return 1;

    int err_chk = 0;
    int done_flag = 0;
    int ATM_total_num = argc - 1;
    pthread_t *threads = new pthread_t[ATM_total_num + 2];
    ATM_Command *ATM_Commands = new ATM_Command[ATM_total_num];

    pthread_mutex_init(&atm_mutex_log, NULL);

    atm_log_file.open("log.txt");

    for (int i = 0; i < ATM_total_num; i++) {

        ATMs.push_back(ATM(i+1, bank));
        ATM_Commands[i] = {i, argv[i+1]};

        err_chk = pthread_create(&threads[i], NULL, ATM_runner, (void*)&ATM_Commands[i]);
        if (err_chk != 0) {
            perror("Bank error: pthread_create failed");
            delete[] threads;
            delete[] ATM_Commands;
            delete bank;
            return 1;
        }
        //printf("%s\n", argv[i]);
    }

    //Create status printing thread
    err_chk = pthread_create(&threads[ATM_total_num], NULL, status_runner, (void*)&done_flag);
    if (err_chk != 0) {
        perror("Bank error: pthread_create failed");
        delete[] threads;
        delete[] ATM_Commands;
        delete bank;
        return 1;
    }


    // Create commissions thread
    err_chk = pthread_create(&threads[ATM_total_num+1], NULL, Bank_runner, (void*)&done_flag);
    if (err_chk != 0) {
        perror("Bank error: pthread_create failed");
        delete[] threads;
        delete[] ATM_Commands;
        delete bank;
        return 1;
    }



    // Wait for all ATM threads to finish
    for (int i = 0; i < ATM_total_num; i++) {
        err_chk = pthread_join(threads[i], NULL);
        if (err_chk != 0) {
            perror("Bank error: pthread_join failed");
            delete[] threads;
            delete[] ATM_Commands;
            delete bank;
            return 1;
        }
    }

    done_flag = 1; //Signal other threads to finish
    err_chk = pthread_join(threads[ATM_total_num], NULL);
    if (err_chk != 0) {
        perror("Bank error: pthread_join failed");
        delete[] threads;
        delete[] ATM_Commands;
        delete bank;
        return 1;
    }
//
    err_chk = pthread_join(threads[ATM_total_num+1], NULL);
    if (err_chk != 0) {
        perror("Bank error: pthread_join failed");
        delete[] threads;
        delete[] ATM_Commands;
        delete bank;
        return 1;
    }


    delete[] threads;
    pthread_mutex_destroy(&atm_mutex_log);
    atm_log_file.close();
    delete[] ATM_Commands;
    delete bank;
    return 0;
}

