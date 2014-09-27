
MAINFILE=evaluate.cpp
MAINOBJ=$(MAINFILE:.cpp=.o)
CXX=g++
CFLAGS=-c -Wall -Wextra
LDFLAGS=-Wall -Wextra

all: $(MAINOBJ) execute-process.o
	$(CXX) $(LDFLAGS) $(MAINOBJ) -lboost_program_options execute-process.o -o test

%.o: %.cpp
	$(CXX) $(CFLAGS) $<

clean:
	rm -f *.o *.d *~ core.*
