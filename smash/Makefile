# Makefile for the smash program
CC = g++
CFLAGS = -std=c++11 -g -Wall -Werror -pedantic-errors -DNDEBUG
CCLINK = $(CC)
OBJS = smash.o commands.o signals.o job.o
RM = rm -f
# Creating the  executable
smash: $(OBJS)
	$(CCLINK) $(CFLAGS) -o smash $(OBJS)
# Creating the object files
job.o: job.cpp job.h
commands.o: commands.cpp commands.h job.h signals.h
smash.o: smash.cpp commands.h signals.h
signals.o: signals.cpp signals.h
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.* smash

