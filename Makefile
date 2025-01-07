.PHONY: all clear

CXX=c++ -Wall -std=c++11 -g -O0

all: carian

carian: carian.o
	$(CXX) -o $@ $^ -lboost_program_options

%.o : %.cc
	$(CXX) -c $<

clean:
	rm -vf carian
	rm -vf *.o
