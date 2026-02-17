CXX = g++
AR = ar
LD = g++
WINDRES = windres

# Aggiunto -I. per trovare gli header nella cartella corrente durante la compilazione dei test
INC =  -I../dependencies/glm/include/ -I../dependencies/freeimage/include/ -I.
CFLAGS = -Wall -std=c++20 -fPIC -m64 -fexceptions
RCFLAGS = 
RESINC = 
LIBDIR = 
LIB = -lGL -lglut -lfreeimage -lGLU
LDFLAGS = -m64

# --- DEBUG CONFIG ---
INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g -D_DEBUG
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/libengine.so

# --- RELEASE CONFIG ---
INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = bin/Release/libengine.so

# --- OBJECTS ---
ENGINE_OBJECTS = camera.o orthographicCamera.o perspectiveCamera.o engine.o infiniteLight.o light.o list.o material.o mesh.o node.o object.o omnidirectionalLight.o ovoReader.o spotLight.o texture.o

OBJ_DEBUG = $(addprefix $(OBJDIR_DEBUG)/, $(ENGINE_OBJECTS))
OBJ_RELEASE = $(addprefix $(OBJDIR_RELEASE)/, $(ENGINE_OBJECTS))

# --- TEST CONFIG ---
TEST_SRC = engine_test.cpp
TEST_OBJ = $(OBJDIR_DEBUG)/engine_test.o
OUT_TEST = bin/Debug/engine_test_runner

all: debug release

clean: clean_debug clean_release
	rm -f $(OUT_TEST) $(TEST_OBJ)

# --- DEBUG RULES ---
before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) -shared $(LIBDIR_DEBUG) $(OBJ_DEBUG)  -o $(OUT_DEBUG) $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/%.o: %.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $< -o $@

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)

# --- RELEASE RULES ---
before_release: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) -shared $(LIBDIR_RELEASE) $(OBJ_RELEASE)  -o $(OUT_RELEASE) $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/%.o: %.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c $< -o $@

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE)

# --- TEST RULES (Nuova Sezione) ---

# Compila il file engine_test.cpp
$(TEST_OBJ): $(TEST_SRC)
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $< -o $@

# Linka il test runner: usa gli oggetti dell'engine (OBJ_DEBUG) direttamente
# per evitare problemi di path con le shared libraries nel CI.
$(OUT_TEST): before_debug $(OBJ_DEBUG) $(TEST_OBJ)
	$(LD) -o $(OUT_TEST) $(TEST_OBJ) $(OBJ_DEBUG) $(LIBDIR_DEBUG) $(LDFLAGS_DEBUG) $(LIB_DEBUG)

# Target da chiamare nel GitLab pipeline
test: $(OUT_TEST)
	./$(OUT_TEST)

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release test