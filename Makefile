# ╔══════════════════════════════════════════════════════╗
#   Directories                                         
# ╚══════════════════════════════════════════════════════╝
SRC_DIR        = src
INC_DIR        = include
MAIN_DIR       = main
TEST_DIR       = tests
BUILD_DIR      = build/debug
BIN_DIR        = bin
TEST_BUILD_DIR = $(BUILD_DIR)/$(TEST_DIR)
TEST_LOG_DIR   = testOutput/$(shell powershell -NoProfile -Command "Get-Date -Format 'yy_MM_dd'")

# ╔══════════════════════════════════════════════════════╗
#   Project Configuration                                
# ╚══════════════════════════════════════════════════════╝
TARGET   = $(BIN_DIR)/circuit_simulator.exe
CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g -MMD -MP -I$(INC_DIR)

# ╔══════════════════════════════════════════════════════╗
#   File Discovery                                       
# ╚══════════════════════════════════════════════════════╝
SRCS      = $(wildcard $(SRC_DIR)/*.cpp)
MAIN      = $(wildcard $(MAIN_DIR)/*.cpp)
TESTS     = $(wildcard $(TEST_DIR)/*.cpp)

SRC_OBJS  = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))
MAIN_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(MAIN))
TEST_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(TESTS))

TEST_BINS = $(TEST_OBJS:.o=.exe)
TEST_RUNS = $(TEST_BINS:.exe=.run)

DEPS      = $(SRC_OBJS:.o=.d) $(MAIN_OBJS:.o=.d) $(TEST_OBJS:.o=.d)

# ╔══════════════════════════════════════════════════════╗
#   Top-Level Targets                                    
# ╚══════════════════════════════════════════════════════╝
all: build

rebuild: clean all

test: run-tests

# ╔══════════════════════════════════════════════════════╗
#   Main Build Rules
# ╚══════════════════════════════════════════════════════╝
IN       ?= main/netlist.txt
OUT      ?= main/output.log
CSV      ?= main/ac_analysis_results.csv
RUN_ARGS  = -i $(IN) -o $(OUT) -ac_csv $(CSV) -v

run: build
	./$(TARGET) $(RUN_ARGS)

build: $(TARGET)

$(TARGET): $(SRC_OBJS) $(MAIN_OBJS)
	@if not exist "$(subst /,\,$(BIN_DIR))" mkdir "$(subst /,\,$(BIN_DIR))"
	$(CXX) $^ -o $@

$(BUILD_DIR)/%.o: %.cpp
	@if not exist "$(subst /,\,$(patsubst %/,%,$(dir $@)))" mkdir "$(subst /,\,$(patsubst %/,%,$(dir $@)))"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ╔══════════════════════════════════════════════════════╗
#   Test Build Rules                                     
# ╚══════════════════════════════════════════════════════╝
run-tests: $(TEST_RUNS)

$(TEST_RUNS): $(TEST_BUILD_DIR)/%.run: $(TEST_BUILD_DIR)/%.exe
	-@if not exist "$(subst /,\,$(TEST_LOG_DIR))" mkdir "$(subst /,\,$(TEST_LOG_DIR))" 2>nul
	@echo [TEST] $*
	@"$(subst /,\,$<)" > "$(subst /,\,$(TEST_LOG_DIR))\$*.log" 2>&1 && echo        PASSED || echo        FAILED

build-tests: $(TEST_BINS)

$(BUILD_DIR)/%.exe: $(BUILD_DIR)/%.o $(SRC_OBJS) 
	@if not exist "$(subst /,\,$(TEST_BUILD_DIR))" mkdir "$(subst /,\,$(TEST_BUILD_DIR))"
	$(CXX) $^ -o $@

# $(TEST_BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
# Covered by the generic build rule for all .cpp files, so no need to redefine here.

# ╔══════════════════════════════════════════════════════╗
#   Dependencies                                         
# ╚══════════════════════════════════════════════════════╝
-include $(DEPS)

# ╔══════════════════════════════════════════════════════╗
#   Utility Targets                                      
# ╚══════════════════════════════════════════════════════╝
usage:
	@echo "Usage:"
	@echo "  make            				- Build the main application"
	@echo "  make run [IN=] [OUT=] [CSV=]	- Run the main application with arguments"
	@echo "  make test       				- Build and run all tests, logging output to $(TEST_LOG_DIR)"
	@echo "  make clean      				- Remove all build artifacts"
	@echo "  make rebuild    				- Clean and then build everything from scratch"

# ╔══════════════════════════════════════════════════════╗
#   Clean                                                
# ╚══════════════════════════════════════════════════════╝
clean:
	-del /f /q "$(subst /,\,$(TARGET))" 2>nul
	-del /f /s /q "$(subst /,\,$(BUILD_DIR))\*.o" 2>nul
	-del /f /s /q "$(subst /,\,$(BUILD_DIR))\*.d" 2>nul
	-del /f /s /q "$(subst /,\,$(BUILD_DIR))\*.exe" 2>nul

# ╔══════════════════════════════════════════════════════╗
#   Phony Targets                                        
# ╚══════════════════════════════════════════════════════╝
.PHONY: all build rebuild run build-tests run-tests test clean usage $(TEST_RUNS)
.PRECIOUS: $(TEST_OBJS)