MAKEDIR = nedmalloc

SVR_PROJ_PATH=..
INCLUDE = -I.\
           -I/usr/local/include\
	   -I/usr/local/lib\
           -I$(SVR_PROJ_PATH)/inc\
           -I$(SVR_PROJ_PATH)/util_lib\
           -I$(SVR_PROJ_PATH)/MyUtilLib\
           -I/usr/local/include/boost

DEPEND_LIB = -lboost_thread -lboost_filesystem -lboost_program_options -lboost_system -lboost_date_time\
	     $(SVR_PROJ_PATH)/lib/libjsoncpp.a\
             $(SVR_PROJ_PATH)/lib/libmyutil.a
		   
CC = gcc
CXX = g++
CXXFLAGS = -Wall -g -D_GNU_SOURCE  
CXXFLAGS +=$(FLAGS)
AR = ar -rc

RM = rm -f

OBJS=$(patsubst %.cpp, %.o, $(wildcard *.cpp))

TARGETS=libmynetwork.a

all: $(TARGETS)

$(TARGETS): $(OBJS)   
	@echo "start building..$@"
	ar -r $(TARGETS) $(OBJS)
	cp $@ $(SVR_PROJ_PATH)/lib
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
