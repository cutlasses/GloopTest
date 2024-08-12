# Project Name
TARGET = Gloop

# Sources
CPP_SOURCES = BitCrush.cpp Board.cpp ButtonStrip.cpp DebugOutput.cpp EncoderValueTypes.cpp Flash.cpp Gloop.cpp GloopInterface.cpp GraphicalDisplay.cpp Looper.cpp PlayHead.cpp Profiler.cpp ExternalTriggers.cpp RandomLFO.cpp TapeEffect.cpp

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

