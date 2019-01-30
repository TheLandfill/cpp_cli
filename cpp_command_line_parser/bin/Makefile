PRODUCT := libcpp_cmd_line_parser.a
BINDIR  := .
INCDIR  := ../includes
LIBDIR	:= ../libs
SRCDIR  := ../src
OBJDIR  := ../obj

CXX := g++
LINKER := g++
INCLUDES := -I$(INCDIR)
LIBRARIES := -L$(LIBDIR)
CXXFLAGS := -O3 -Wall -Wextra -std=c++11 -MMD -MP

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
$(BINDIR)/$(PRODUCT): $(OBJ) $(LIB)
# 	Make a library
	ar rcs $(BINDIR)/$(PRODUCT) $^
#	Make a program
#	$(LINKER) $(CXXFLAGS) $^ -o $@

# Compile individual .cpp source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# -MM flag on gcc gets the dependency files
# $< is the first dependency, which is usually the .cpp file
# Get the dependency files
#$(OBJDIR)/%.cpp.d: $(SRCDIR)/%.cpp
#	$(CXX) $(INCLUDES) -MM $< | sed -e 's%^%$@ %' -e 's% % $(OBJDIR)/%' > $@

-include $(DEP)

.PHONY: clean

clean:
	rm -f $(OBJDIR)/* $(PRODUCT)
