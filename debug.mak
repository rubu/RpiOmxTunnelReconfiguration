#Generated by VisualGDB (http://visualgdb.com)
#DO NOT EDIT THIS FILE MANUALLY UNLESS YOU ABSOLUTELY NEED TO
#USE VISUALGDB PROJECT PROPERTIES DIALOG INSTEAD

BINARYDIR := Debug

#Toolchain
CC := gcc
CXX := g++
LD := $(CXX)
AR := ar
OBJCOPY := objcopy

#Additional flags
PREPROCESSOR_MACROS := DEBUG
INCLUDE_DIRS :=/opt/vc/include /opt/vc/include/interface/vcos/pthreads /opt/vc/include/interface/vmcs_host/linux
LIBRARY_DIRS :=/opt/vc/lib
LIBRARY_NAMES :=bcm_host openmaxil
ADDITIONAL_LINKER_INPUTS :=
MACOS_FRAMEWORKS :=

CFLAGS := -ggdb -ffunction-sections -O0 -std=c++0x
CXXFLAGS := -ggdb -ffunction-sections -O0 -std=c++0x
ASFLAGS :=
LDFLAGS := -Wl,-gc-sections
COMMONFLAGS :=

START_GROUP := -Wl,--start-group
END_GROUP := -Wl,--end-group

#Additional options detected from testing the toolchain
IS_LINUX_PROJECT := 1
