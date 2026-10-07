CC = g++ -Werror -Wall
FLAGS = -g -O3

gen:
	$(CC) $(FLAGS) -o gen src/generator.cpp src/generate.cpp

SPH:
	$(CC) $(FLAGS) -o SPH src/particle.cpp src/kernel.cpp src/SPH.cpp main.cpp

clean:
	rm gen SPH