# compiler settings 
CXX = g++
CXXFLAGS = -I ./inc -std=c++14 -DPRINTTIME=true -DNDEBUG 
OPTFLAGS = -flto -funroll-loops -finline-functions -O3 -ffast-math
WARNINGS = -g -Wall

# Source files and object files
SRCDIR = src
OBJDIR = obj
INCDIR = inc
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

# testcase file

CASENAME = c123456# change here
# CASENAME = c432# change here
CASEDIR = testcase/$(CASENAME)

NETLIST = $(CASENAME).v
LIBRARY = testcase/test_lib.lib
# LIBRARY = testcase/sample2_Late.lib
PATTERN = $(CASENAME).pat

TARGET = sta

all: $(TARGET)

$(OBJDIR):
	@mkdir $(OBJDIR)

$(TARGET): main.cpp $(OBJS)
	$(CXX) $(WARNINGS) $(CXXFLAGS) $(OPTFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(WARNINGS) $(CXXFLAGS) $(OPTFLAGS) -c $< -o $@

run:$(TARGET)
	time ./$(TARGET) $(CASEDIR)/$(NETLIST) -l $(LIBRARY) -i $(CASEDIR)/$(PATTERN)

clean:
	rm -rf $(OBJDIR) $(TARGET) *.txt