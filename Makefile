CXXFLAGS=-Wall -ansi -g -O2 -DDEBUG -m32
SOURCES=play.cc base.cc winning.cc
OBJECTS=play.o base.o winning.o

all: play compute_winning

run: play
	cd caia && ./caiaio -d 2>&1

play: $(OBJECTS)
	g++ $(CXXFLAGS) -o play $(OBJECTS)

compute_winning: compute_winning.cc base.o
	g++ $(CXXFLAGS) -o compute_winning compute_winning.cc base.o

print_fields: print_fields.cc base.o
	g++ $(CXXFLAGS) -o print_fields print_fields.cc base.o
	
submission.cc: $(SOURCES)
	./compile.pl $(SOURCES) > submission.cc

submission: submission.cc
	g++ -Wall -O2 -lm -m32 submission.cc -o submission

clean:
	-rm play compute_winning print_fields
	-rm $(OBJECTS)
	-rm submission.cc submission

