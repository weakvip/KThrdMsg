######################################
_platform_= $(shell uname -i|grep 64)
ifeq ("$(_platform_)","")
Platform=i386
PlatformNum=
PlatformName=
PlatformDefine=
else
Platform=x86_64
PlatformNum=64
PlatformName=x64
PlatformDefine=-D__x64__
endif
######################################
STATIC_LIB_SUFFIX=lib
SHARE_LIB_SUFFIX=so
######################################
CFLAGS=
LFLAGS=
DEFINES=$(PlatformDefine)
######################################
CC = g++
LD = g++
AR = ar
RM = rm
MKDIR = mkdir
######################################
MODE ?= debug
ifeq ($(MODE),debug)
ConfigurationName=Debug
CFLAGS += -ggdb
DEFINES += -D_DEBUG
else
ConfigurationName=Release
CFLAGS += -ggdb -O2
LFLAGS += -O2
endif
######################################
DEFINES += -D_CHK_MEM_LEAK_ -D_MONIT_PERFORMANCE_ -std=c++0x
C_INC=/usr/include
CFLAGS += -c -Wno-deprecated -pthread -w
SYS_LIBS=-lpthread -luuid
INCLUDES=-I$(C_INC)
LIBPATHS=-L"/lib$(PlatformNum)" -L"/usr/lib$(PlatformNum)" $(SYS_LIBS)
######################################
ROOT_DIR=../../
INC_DIR=$(ROOT_DIR)Inc
LIB_DIR=$(ROOT_DIR)Lib$(PlatformNum)/$(ConfigurationName)
TMP_DIR=$(ROOT_DIR)/_TEMP/$(BaseName)
######################################
LIBPATHS+=-L$(LIB_DIR)

INCLUDES += -I$(ROOT_DIR)		      \
            -I$(INC_DIR)/KThrdMsg\
            $(NULL)
			
LIBS =      -ldl					\
            -lpthread\
			-lThrdMsg\
            $(NULL)

LFLAGS += $(LIBPATHS)
#######################################
Out_Dir = $(TMP_DIR)
Target_Dir = $(ROOT_DIR)/_Bin/$(ConfigurationName)

Base_Name=KThrdMsgTest

Target_Name=$(Base_Name)
Target=$(Target_Dir)/$(Target_Name)

Source=.

Objects =  $(patsubst %.cpp,$(Out_Dir)/%.o,$(shell find -P . -name "*.cpp"))
Objects += $(patsubst %.c,$(Out_Dir)/%.o,$(shell find -P . -name "*.c"))

all : $(Out_Dir) $(Target_Dir) $(Target)
rebuild : clean all

debug:
	@echo "$(Objects)"

$(Target) : $(Objects)
	$(LD) $(LFLAGS) -o $@ $^ $(LIBS)
	
$(Target_Dir) :
	$(MKDIR) -p $@
	
$(Out_Dir) :
	$(MKDIR) -p $@

$(Out_Dir)/%.o : %.cpp
	@if [ -d $(shell dirname $@) ] ; \
	then \
		echo ">>>>>>>> $< ......"; \
	else \
		echo ">>>>>>>> $< ......"; \
		$(MKDIR) -p $(shell dirname $@); \
	fi
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(Out_Dir)/%.o : %.c
	@if [ -d $(shell dirname $@) ] ; \
	then \
		echo ">>>>>>>> $< ......"; \
	else \
		echo ">>>>>>>> $< ......"; \
		$(MKDIR) -p $(shell dirname $@); \
	fi
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

clean :
	$(RM) -f $(Target)
	$(RM) -f $(Objects)
