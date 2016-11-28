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
DEFINES=$(PlatformDefine) -D_CHK_MEM_LEAK_ -D_MONIT_PERFORMANCE_
CFLAGS= -c -Wno-deprecated -pthread -w
C_INC=/usr/include
INCLUDES=-I$(C_INC)
SYS_LIBS=-lpthread -luuid
LIBPATHS=-L"/lib$(PlatformNum)" -L"/usr/lib$(PlatformNum)" $(SYS_LIBS)
LFLAGS= $(LIBPATHS) 
######################################
CC = gcc
LD = gcc
AR = ar
RM = rm
MKDIR = mkdir
CP = cp
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
ROOT_DIR=../../..
INC_DIR=$(ROOT_DIR)/Inc
TMP_DIR=$(ROOT_DIR)/_Temp/$(Base_Name)/$(ConfigurationName)
LIB_DIR=$(ROOT_DIR)/Lib$(PlatformNum)/$(ConfigurationName)
######################################
Out_Dir =$(TMP_PATH)
Target_Dir =$(LIB_DIR)
Target_Name=$(Base_Name)
Target=$(Target_Dir)/lib$(Target_Name).a
######################################
MODE = debug

Base_Name=ThrdMsg

Source=.

Copy_Source_Headers = $(shell ls IThrdMsg.h)
Copy_Target_Headers = $(INC_DIR)/$(Base_Name)/$(Copy_Source_Headers)

Objects = $(patsubst %.c,$(Out_Dir)/%.o,$(shell find -P . -name "*.c"))

all : $(Out_Dir) $(Target_Dir) $(Target) $(Copy_Target_Headers)
rebuild : clean all

$(Target) : $(Objects)
	$(AR) -rv $@ $^
	
$(Out_Dir) :
	$(MKDIR) -p $@
	
$(Target_Dir) :
	$(MKDIR) -p $@

$(Out_Dir)/%.o : %.c
	@if [ -d $(shell dirname $@) ] ; \
	then \
		echo ">>>>>>>> $< ......"; \
	else \
		echo ">>>>>>>> $< ......"; \
		$(MKDIR) -p $(shell dirname $@); \
	fi
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<
	
$(Copy_Target_Headers) :
	@if [ -d $(shell dirname $@) ] ; \
	then \
		echo "copy $(Copy_Source_Headers) to $(shell dirname $@)"; \
	else \
		echo "copy $(Copy_Source_Headers) to $(shell dirname $@)"; \
		$(MKDIR) -p $(shell dirname $@); \
	fi
	$(CP) $(Copy_Source_Headers) $(shell dirname $@)

.PHONY:clean	
clean :
	$(RM) -f $(Target)
	$(RM) -f $(Objects)	
