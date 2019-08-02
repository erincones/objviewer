# Project name
PROJECT := objviewer


# Directories
SRC := src
INCLUDE := include
LIB := lib
BUILD := build
BIN := bin


# Directories management
MKDIR := mkdir -pv
RM := rm -rfv
CP := cp -ruv


# Compiler
LINK := -ldl -lGL -lglfw
FLAGS = -Wall -Wextra
CCFLAGS = -std=c11 $(FLAGS)
CXXFLAGS = -std=c++11 $(FLAGS)


# Main target
TARGET := $(BIN)/$(PROJECT)

# Targets
.PHONY: release debug clean

release: FLAGS += -Os
release: $(TARGET)

debug: FLAGS += -ggdb3
debug: $(TARGET)

clean:
	$(RM) $(BUILD) $(BIN)


# Directories creation
.PRECIOUS: $(BUILD)/ $(BUILD)%/ $(BIN)/
.SECONDEXPANSION:

$(BUILD)/:
	$(MKDIR) $@

$(BUILD)%/:
	$(MKDIR) $@

$(BIN)/:
	$(MKDIR) $@


# C files
CCSOURCES := $(shell find $(SRC) -type f -name *.c)
CCOBJECTS := $(patsubst $(SRC)/%,$(BUILD)/%,$(CCSOURCES:.c=.o))

# CXX files
CXXSOURCES := $(shell find $(SRC) -type f -name *.cpp)
CXXOBJECTS := $(patsubst $(SRC)/%,$(BUILD)/%,$(CXXSOURCES:.cpp=.o))


# Compilation
$(TARGET): $(CCOBJECTS) $(CXXOBJECTS) | $$(@D)/
	$(CXX) -o $@ $^ $(LINK)

$(BUILD)/%.o: $(SRC)/%.c | $$(@D)/
	$(CC) $(CCFLAGS) -o $@ -c $<

$(BUILD)/%.o: $(SRC)/%.cpp | $$(@D)/
	$(CXX) $(CXXFLAGS) -o $@ -c $<