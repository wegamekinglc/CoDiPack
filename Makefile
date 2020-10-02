# names of the basic directories
BUILD_DIR    = build
DOC_DIR      = documentation
EXAMPLE_DIR  = $(DOC_DIR)/examples
TUTORIAL_DIR = $(DOC_DIR)/tutorials

#list all source files in DOC_DIR
TUTORIAL_FILES  = $(wildcard $(TUTORIAL_DIR)/*.cpp)
EXAMPLE_FILES  = $(wildcard $(EXAMPLE_DIR)/*.cpp)

#list all dependency files in BUILD_DIR
DEP_FILES   = $(wildcard $(BUILD_DIR)/*.d)
DEP_FILES  += $(wildcard $(BUILD_DIR)/**/*.d)
DEP_FILES  += $(wildcard $(BUILD_DIR)/**/**/*.d)

CODI_DIR := .

FLAGS = -Wall -pedantic -DCODI_OptIgnoreInvalidJacobies=true -DCODI_EnableAssert=true -I$(CODI_DIR)/include -fopenmp

ifeq ($(CPP14), yes)
  FLAGS += -std=c++14
else
  FLAGS += -std=c++11
endif
ifeq ($(OPT), yes)
  CXX_FLAGS := -O3 $(FLAGS)
else
  CXX_FLAGS := -O0 -g $(FLAGS)
endif

ifeq ($(CXX), )
	CXX := g++
else
	CXX := $(CXX)
endif

TUTORIALS = $(patsubst %.cpp,$(BUILD_DIR)/%.exe,$(TUTORIAL_FILES))
EXAMPLES = $(patsubst %.cpp,$(BUILD_DIR)/%.exe,$(EXAMPLE_FILES))

# set default rule
all: tutorials examples

$(BUILD_DIR)/%.exe : %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) $< -o $@
	@$(CXX) $(CXX_FLAGS) $< -MM -MP -MT $@ -MF $@.d

tutorials: $(TUTORIALS)

examples: $(EXAMPLES)

doc:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/documentation
	doxygen

.PHONY: clean
clean:
	rm -fr $(BUILD_DIR)

-include $(DEP_FILES)
