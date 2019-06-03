PRODUCT := SUBCOMMAND_EXAMPLE
BINDIR  := .
INCDIR  := ../includes
INCDIR_EXT := ../external_includes
LIBDIR	:= ../libs
SRCDIR  := ../src
OBJDIR  := ../obj

MKDIR_P = mkdir -p

CXX := g++
LINKER := g++
INCLUDES := -I$(INCDIR) -I$(INCDIR_EXT)
LIBRARIES := -L$(LIBDIR)
CXXFLAGS := -O0 -ggdb -Wall -Wextra -std=c++11 -MMD -MP -ffunction-sections -fdata-sections -flto

# Finds all .cpp files and puts them into SRC
SRC := $(wildcard $(SRCDIR)/*.cpp)
# Creates .o files for every .cpp file in SRC (patsubst is pattern substitution)
OBJ := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))
# Creates .d files (dependencies) for every .cpp file in SRC
DEP := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.d,$(SRC))
# Finds all lib*.a files and puts them into LIB
LIB := $(wildcard $(LIBDIR)/lib*.a)


# $^ is list of dependencies and $@ is the target file
# Link all the object files or make a library
$(BINDIR)/$(PRODUCT): directories $(OBJ) $(LIB) remove_help
# 	Make a library
#	ar rcs $(BINDIR)/$(PRODUCT) $(OBJ) $(LIB)
#	Make a program
	$(LINKER) -Wl,--gc-sections $(CXXFLAGS) $(OBJ) $(LIB) -o $@

# Compile individual .cpp source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# -MM flag on gcc gets the dependency files
# $< is the first dependency, which is usually the .cpp file
# Get the dependency files
#$(OBJDIR)/%.cpp.d: $(SRCDIR)/%.cpp
#	$(CXX) $(INCLUDES) -MM $< | sed -e 's%^%$@ %' -e 's% % $(OBJDIR)/%' > $@

-include $(DEP)

.PHONY: directories

directories: $(OBJDIR)

$(OBJDIR):
	$(MKDIR_P) $(OBJDIR)

.PHONY: clean

clean: remove_help
	rm -f $(OBJDIR)/* $(PRODUCT)

.PHONY: remove_help

remove_help:
	rm -f $(BINDIR)/.*_help_file
