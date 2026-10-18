CXX = g++
CXXFLAGS = -Werror -Wall -g -O3 

SPH_DEPS = include/kernel.hpp include/particle.hpp include/SPH.hpp

SPH_OBJS = src/kernel.o src/particle.o src/SPH.o main.o

.PHONY: all clean

all: SPH

%.o: %.cpp $(SPH_DEPS) 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

SPH: $(SPH_OBJS)
	$(CXX) $(CXXFLAGS) -o SPH $(SPH_OBJS)

clean:
	rm -f *.o SPH src/*.o