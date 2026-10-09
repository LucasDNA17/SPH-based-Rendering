CXX = g++
CXXFLAGS = -Werror -Wall -g -O3 

SPH_DEPS = include/kernel.hpp include/particle.hpp include/SPH.hpp
GENERATE_DEPS = include/generator.hpp

GEN_OBJS = src/generator.o generate.o
SPH_OBJS = src/kernel.o src/particle.o src/SPH.o main.o

.PHONY: all clean

all: gen SPH

%.o: %.cpp $(SPH_DEPS) $(GENERATE_DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

gen: $(GEN_OBJS)
	$(CXX) $(CXXFLAGS) -o gen $(GEN_OBJS)

SPH: $(SPH_OBJS)
	$(CXX) $(CXXFLAGS) -o SPH $(SPH_OBJS)

clean:
	rm -f *.o gen SPH src/*.o