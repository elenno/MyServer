SVR_PROJ_PATH=..
INC_PATH = -I.\
	   -I$(SVR_PROJ_PATH)/inc/aposto-mongodb\
           -I$(SVR_PROJ_PATH)/util_lib\
           -I$(SVR_PROJ_PATH)/MyUtilLib\
           -I$(SVR_PROJ_PATH)/MyNetworkLib\
           -I$(SVR_PROJ_PATH)/CommonDef\
           -I/usr/local/include/boost\
	   -I/usr/local/lib\
           -I/usr/local/include
		  
DEPEND_LIB = $(SVR_PROJ_PATH)/lib/libjsoncpp.a\
             $(SVR_PROJ_PATH)/lib/libnedmalloc.a\
             $(SVR_PROJ_PATH)/lib/libutil.a\
             $(SVR_PROJ_PATH)/lib/libmynetwork.a\
             $(SVR_PROJ_PATH)/lib/libmyutil.a\
             -lmongoclient -lboost_thread -lboost_filesystem -lboost_program_options -lboost_system -lboost_date_time
              
CXX = g++
#CXXFLAGS = -Wall -g -D_GNU_SOURCE 
CXXFLAGS = -Wno-unknown-pragmas -g3 -o2 -D_GNU_SOURCE 
CXXFLAGS +=$(FLAGS)

RM = rm -f

OBJS=$(patsubst %.cpp, %.o, $(wildcard *.cpp))

TARGETS=gamesvr

all: $(TARGETS)

%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $(INC_PATH) $< -o $@

$(TARGETS): $(OBJS)
	@echo "start building..$@"
	$(CXX) $(CXXFLAGS)  $^ $(DEPEND_LIB) -o $@
	@echo "done building.. :)"
clean:
	$(RM) $(OBJS) $(TARGETS)
	@echo
dist:
	cp $(TARGETS) $(SVR_PROJ_PATH)dist/bin/


#########################################################################################################
# -Wno-unknown-pragmas 去掉一些不必要的pragma warning警告                                               #
# -g3 带更多的编译信息  -o2 编译优化                                                                    #
# -D_GNU_SOURCE 采用GNU标准进行编译                                                                     #
# patsubst                                                                                              #
# wildcard *.cpp 搜索当前目录下所有.cpp结尾文件，并生成一个以空格为间隔的列表                           #
# $(patsubst %.cpp, %.o, $(wildcard *.cpp)) 找到所有当前目录下的.cpp结尾的文件，并替换成以.o结尾        #
# $@  表示目标文件                                                                                      #
# $^  表示所有的依赖文件                                                                                #
# $<  表示第一个依赖文件                                                                                #
# $?  表示比目标还要新的依赖文件列表                                                                    #
#########################################################################################################
