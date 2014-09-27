
MAINFILE=test-main.cpp
MAINOBJ=$(MAINFILE:.cpp=.o)
CXX=g++
CFLAGS=-c -Wall -Wextra
LDFLAGS=-Wall -Wextra

all: $(MAINOBJ) execute-process.o
	$(CXX) $(LDFLAGS) $(MAINOBJ) execute-process.o -o test

%.o: %.cpp
	$(CXX) $(CFLAGS) $<

clean:
	rm -f *.o *.d *~ core.*
