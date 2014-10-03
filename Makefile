
PROGNAME=evaluate
FILES=evaluate.cpp execute-process.cpp timer.cpp tester.cpp
OBJS=$(FILES:.cpp=.o)
CXX=g++
CFLAGS=-c -Wall -Wextra -g
LDFLAGS=-Wall -Wextra -g
LIBS=-lboost_program_options -lboost_filesystem -lboost_system

evaluate: $(OBJS)
	$(CXX) $(LDFLAGS) $(LIBS) $(OBJS) -o $(PROGNAME)

%.o: %.cpp
	$(CXX) $(CFLAGS) $<

clean:
	rm -f *.o *.d *~ core.*
