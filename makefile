
EXECUTABLES = shell
OBJECTS = 
CXXFLAGS= -ggdb
CXX = g++
STDFLAGS= -std=c++0x

all: $(EXECUTABLES)

shell: myshell.cc
	$(CXX) $(CXXFLAGS) $(STDFLAGS) myshell.cc -o shell

clean:
	rm -f $(OBJECTS) $(EXECUTABLES) *.o *~
