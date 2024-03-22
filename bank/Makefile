# Makefile for the bank program
CC = g++
CFLAGS = -std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG -pthread
CCLINK = $(CC)
OBJS = bank.o atm.o account.o main.o
RM = rm -f
# Creating the  executable
Bank: $(OBJS)
	$(CCLINK) $(CFLAGS) $(OBJS) -o Bank 
# Creating the object files
account.o: account.cpp account.h
bank.o: bank.cpp bank.h account.h
atm.o: atm.cpp atm.h bank.h
main.o: main.cpp bank.h atm.h
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.* smash
