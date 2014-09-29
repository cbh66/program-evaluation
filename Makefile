
MAINFILE=evaluate.cpp
MAINOBJ=$(MAINFILE:.cpp=.o)
CXX=g++
CFLAGS=-c -Wall -Wextra -g
LDFLAGS=-Wall -Wextra -g

all: $(MAINOBJ) execute-process.o
	$(CXX) $(LDFLAGS) $(MAINOBJ) -lboost_program_options -lboost_filesystem -lboost_system execute-process.o -o test

%.o: %.cpp
	$(CXX) $(CFLAGS) $<

clean:
	rm -f *.o *.d *~ core.*
