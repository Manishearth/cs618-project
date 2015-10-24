
#------ MAKE CHANGES TO BASE_DIR : Please put the path to base directory of your pristine gcc-4.7.2 build -----------#
include paths.mk
CPP = $(INSTALL)/bin/g++
CC = $(INSTALL)/bin/gcc
NEW_PATH = $(BASE_DIR)/gcc

GCCPLUGINS_DIR:= $(shell $(CPP) -print-file-name=plugin)
INCLUDE= -I$(GCCPLUGINS_DIR)/include -I$(NEW_PATH) -I$(NEW_PATH)/include

FLAGS= -fPIC -flto -flto-partition=none -Wno-literal-suffix -fno-rtti

%.o: %.c
	$(CC) $(FLAGS) $(INCLUDE) -c $< 

%.o: %.cpp
	$(CPP) $(FLAGS) -std=c++0x -O0 $(INCLUDE) -c $< 

plugin.so: block_information.o parser.o analysis_plugin.o
	$(CPP) $(INCLUDE) $(FLAGS) -O3 -shared $^ -o $@


TEST_OBJS=test.o
test: $(TEST_OBJS) plugin.so
	$(CPP) -fplugin=./plugin.so $(TEST_OBJS) -flto -flto-partition=none -O3 -fdump-ipa-all -fdump-tree-all -o result 

clean:
	\rm -f *.c.* *~ a.out result*
	\rm plugin.so *.o 

