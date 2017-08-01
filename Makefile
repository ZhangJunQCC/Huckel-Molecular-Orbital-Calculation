# Here you can define CODELINUX, CODEWINDOWS, or CODEMACOSX
CODEINFO = -DCODEMAJORVER=0 \
	   -DCODEMINORVER=1 \
	   -DCODELINUX 

CXX     = g++
CXXFLAG =-O3 --std=c++0x -fopenmp  -static-libstdc++ -static-libgcc -static
CC      = gcc
CCFLAG  =-O3 -fopenmp -Wall

INCDIR  =./include
LIBDIR  =./lib
SRCS    =./src
BINS    =./bin
INCS    =-I$(INCDIR) -I$(SRCS) 
LIBS    =-L$(LIBDIR) -llapack -ltmglib -lrefblas -lgfortran -lquadmath
OBJS    =./obj
DEPS    =./depend

TARGET   = $(BINS)/hmo
SOURCES := $(wildcard $(SRCS)/*.cpp)
OBJECTS := $(patsubst $(SRCS)%.cpp,$(OBJS)%.o,$(SOURCES))

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAG) $(INCS) -o $@ $^ $(LIBS)


sinclude $(patsubst $(SRCS)%.cpp,$(DEPS)%.d,$(SOURCES))

$(OBJS)/%.o: $(SRCS)/%.cpp
	$(CXX) $(CXXFLAG) $(CODEINFO) $(INCS) $(LIBS) -c -o $@ $< 


$(DEPS)/%.d: $(SRCS)/%.cpp
	@set -e; \
	rm -f $@; \
	$(CXX) -MM $(CXXFLAG) $(CODEINFO) $(INCS) $< > $@.$$$$;\
	sed 's/\([a-zA-Z0-9]*\)\.o[ :]/$(patsubst ./%,%,$(OBJS)\/$(shell v1=$@; v2=$${v1%/*}; v3=$${v2#*/}; echo $${v3} | sed 's/\//\\\//g'))\/\1.o $(patsubst ./%,%,$(DEPS)\/$(shell v1=$@; v2=$${v1%/*}; v3=$${v2#*/}; echo $${v3} | sed 's/\//\\\//g'))\/\1.d:/g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY: clean
clean:
	rm -rf $(OBJECTS) $(TARGET) $(DEPS)/*.d*

