CXX      =  g++
CXXFLAGS = -Wall -ansi -pedantic -g
LIB      = -lSDL -lSDL_image
INCLUDES = -I ~/eigen -I include

CXXFLAGS += $(INCLUDES)

SRC_PATH = src
OBJ_PATH = obj
BIN_PATH = bin

SRC_FILES = $(shell find $(SRC_PATH) -type f -name '*.cpp')
OBJ_FILES = $(patsubst $(SRC_PATH)/%.cpp, $(OBJ_PATH)/%.o, $(SRC_FILES))

EXEC = trifocal

DIRNAME  = $(shell basename $$PWD)
BACKUP   = $(shell date +`basename $$PWD`-%m.%d.%H.%M.tgz)
STDNAME  = $(DIRNAME).tgz

all: $(BIN_PATH)/$(EXEC)
	@echo
	@echo [--DONE--]
	@echo

$(BIN_PATH)/$(EXEC): $(OBJ_FILES)
	$(CXX) -o $@ $^ $(LIB)
	@echo "--------------------------------------------------------------"
	@echo "                 to execute type: ./$(BIN_PATH)/$(EXEC) &"
	@echo "--------------------------------------------------------------"

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $^

clean :	
	@echo "**************************"
	@echo "CLEAN"
	@echo "**************************"
	$(RM) *~ $(OBJ_FILES) $(BIN_PATH)/$(EXEC)  

bigclean :
	@echo "**************************"
	@echo "BIG CLEAN"
	@echo "**************************"
	find . -name '*~' -exec rm -fv {} \;
	$(RM) *~ $(OBJ_FILES) $(BIN_PATH)/$(EXEC)  output/*

tar : clean 
	@echo "**************************"
	@echo "TAR"
	@echo "**************************"
	cd .. && tar cvfz $(BACKUP) $(DIRNAME)
