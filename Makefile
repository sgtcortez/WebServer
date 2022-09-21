CPP=clang++

CFLAGS=-g -Wall -Wextra -std=c++11
#CFLAGS+=-Werror
CFLAGS+=-O0
CFLAGS+=-DDEBUG=1

LINK=-lpthread

STATICS=\
	Socket.o 		 \
	IONotification.o \

%.o: %.cpp %.hpp
	$(CPP) $(CFLAGS) -c $< -o $@

main.out: Main.cpp $(STATICS)
	$(CPP) $(CFLAGS) -o $@ $^ $(LINK)

.PHONY: clean

clean:
	rm -f *.o *.so *.out