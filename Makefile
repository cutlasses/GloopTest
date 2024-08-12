# Project Name
TARGET = Gloop

# Sources
CPP_SOURCES = Board.cpp GloopTest.cpp

# Library Locations
LIBDAISY_DIR = ../libdaisy/
DAISYSP_DIR = ../DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

BUILDDIR := build

CPPFLAGS += --include CompileSwitches.h --include Board.h

CPP_STANDARD = -std=gnu++17

#OPT = -Os

#LDFLAGS = -u -printf_float

