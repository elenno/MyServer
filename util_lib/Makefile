MAKEDIR = nedmalloc

SVR_PROJ_PATH=..
INCLUDE = -I.\
           -I/usr/local/include\
	   -I/usr/local/lib
		   
CC = gcc
CXX = g++
CXXFLAGS = -Wall -g -D_GNU_SOURCE  
CXXFLAGS +=$(FLAGS)
AR = ar -rc

RM = rm -f

OBJS=$(patsubst %.cpp, %.o, $(wildcard *.cpp))

TARGETS=libutil.a

all: $(TARGETS)

$(TARGETS): $(OBJS)   
	@echo "start building..$@"
	ar -r $(TARGETS) $(OBJS)
	@echo

.c.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

.cpp.o:
	$(CXX) $(CFLAGS) -c $(INCLUDE) $(DEPEND_LIB) $< 
	echo $@
%:%.c
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS)
	echo $@

clean:
	rm -f *.o
	rm -f $(TARGET)
