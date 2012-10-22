source =  wifi-rbs.cpp socket.cpp transmitter.cpp
objects = socket.o
executables = wifi-rbs transmitter

CXX = g++
CXXFLAGS = -g -O3 -std=c++0x -ffast-math -pedantic -Werror -Wall -Wextra -Weffc++ -fno-default-inline -pipe -D_FILE_OFFSET_BITS=64 -D_XOPEN_SOURCE=500 -D_GNU_SOURCE
LIBS = -lm -lrt

all: $(executables)

wifi-rbs: wifi-rbs.o $(objects)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)

transmitter: transmitter.o $(objects) 
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

-include depend

depend: $(source)
	$(CXX) $(INCLUDES) -MM $(source) > depend

.PHONY: clean
clean:
	-rm -f $(executables) depend *.o *.rpo

