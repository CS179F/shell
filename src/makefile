
EXECUTABLES = shell
OBJECTS = 
CXXFLAGS= -ggdb
CXX = g++
STDFLAGS= -std=c++0x
all: $(EXECUTABLES)

source: 
	./sourcec11

shell: myshell.cc
	$(CXX) $(CXXFLAGS) $(STDFLAGS) -lreadline -pthread myshell.cc thread.h Devices.h -o shell
	
test: testing.cc
	$(CXX) $(CXXFLAGS) $(STDFLAGS) testing.cc -o test

device: 
	$(CXX) $(CXXFLAGS) -std=c++11 -pthread Devices.cc -o Devices

history: history.cc
	$(CXX) $(CXXFLAGS) $(STDFLAGS) history.cc -o history
	
clean:
	rm -f $(OBJECTS) $(EXECUTABLES) *.o *~
