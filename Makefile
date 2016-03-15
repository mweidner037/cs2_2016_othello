CC          = g++
#CC          = icc
#TODO: does icc make the write executables for the cluster?
CFLAGS      = -Wall -ansi -pedantic -std=c++11 -O2
#CFLAGS      = -Wall -ansi -pedantic -std=c++11 -ggdb
#TODO: test changing O2 to O3
OBJS        = player.o board.o
PLAYERNAME  = mweidner

all: $(PLAYERNAME) testgame
	
$(PLAYERNAME): $(OBJS) wrapper.o
	$(CC) -o $@ $^

testgame: testgame.o
	$(CC) -o $@ $^

testminimax: $(OBJS) testminimax.o
	$(CC) -o $@ $^

%.o: %.cpp
	$(CC) -c $(CFLAGS) -x c++ $< -o $@
	
java:
	make -C java/

cleanjava:
	make -C java/ clean

clean:
	rm -f *.o $(PLAYERNAME) testgame testminimax
	
.PHONY: java testminimax
