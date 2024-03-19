#include <stdio.h>
#include <vector>
#include <atm.h>

using namespace std;
 
void ATM_runner(){
    
}


int main(int argc, char* argv[])
{
    int N = argc-1;
    vector<ATM> ATMs;

    printf("You have entered %d arguments:\n", argc);
 
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
    return 0;
}

