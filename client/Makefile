CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -I../engine -I../dependencies/glm/include
CFLAGS = -Wall -std=c++20 -fexceptions
RCFLAGS = 
RESINC = 
LIBDIR = 
LIB = -lengine
LDFLAGS = 

# --- DEBUG CONFIGURATION ---
INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g -D_DEBUG
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR) -L../engine/bin/Debug
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/client

# --- RELEASE CONFIGURATION ---
INC_RELEASE = $(INC)
# NOTA: Manteniamo -D_DEBUG anche in release per permettere l'esecuzione dei test
CFLAGS_RELEASE = $(CFLAGS) -O2 -D_DEBUG
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR) -L../engine/bin/Release
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s -Wl,-rpath='$$ORIGIN'
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = bin/Release/client

# --- AUTO-DISCOVERY ---
# 1. Trova tutti i file Test (es. HanoiTest.cpp)
TEST_SRCS = $(wildcard *Test.cpp)
# 2. File sorgente standard
STD_SRCS = main.cpp hanoi.cpp

# Genera liste di oggetti
TEST_OBJS_DEBUG = $(patsubst %.cpp,$(OBJDIR_DEBUG)/%.o,$(TEST_SRCS))
STD_OBJS_DEBUG = $(patsubst %.cpp,$(OBJDIR_DEBUG)/%.o,$(STD_SRCS))

TEST_OBJS_RELEASE = $(patsubst %.cpp,$(OBJDIR_RELEASE)/%.o,$(TEST_SRCS))
STD_OBJS_RELEASE = $(patsubst %.cpp,$(OBJDIR_RELEASE)/%.o,$(STD_SRCS))

# Oggetti finali per il linking
OBJ_DEBUG = $(STD_OBJS_DEBUG) $(TEST_OBJS_DEBUG)
OBJ_RELEASE = $(STD_OBJS_RELEASE) $(TEST_OBJS_RELEASE)

all: debug release

clean: clean_debug clean_release

before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

# Regola generica compilazione Debug
$(OBJDIR_DEBUG)/%.o: %.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $< -o $@

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)

before_release: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

# Regola generica compilazione Release
$(OBJDIR_RELEASE)/%.o: %.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c $< -o $@

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE)

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release test