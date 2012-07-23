source = send1way.cpp socket.cpp buffer-setpoint.cpp hist.cpp controlled-delay.cpp rate-estimate.cpp payload.cpp
objects = socket.o hist.o rate-estimate.o payload.o
executables = send1way buffer-setpoint controlled-delay

CXX = g++
CXXFLAGS = -g -O3 -std=c++0x -ffast-math -pedantic -Werror -Wall -Wextra -Weffc++ -fno-default-inline -pipe -D_FILE_OFFSET_BITS=64 -D_XOPEN_SOURCE=500 -D_GNU_SOURCE
LIBS = -lm -lrt

all: $(executables)

send1way: send1way.o $(objects)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)

buffer-setpoint: buffer-setpoint.o $(objects)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)

controlled-delay: controlled-delay.o $(objects)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

-include depend

depend: $(source)
	$(CXX) $(INCLUDES) -MM $(source) > depend

.PHONY: clean
clean:
	-rm -f $(executables) depend *.o *.rpo
