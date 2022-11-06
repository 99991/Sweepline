CXX = g++

CXXFLAGS = \
	-std=c++11 \
	-O3

LDFLAGS = \
	-lgmp \
	-lgmpxx

all: main example_segments example_intersections benchmark

main: main.cpp sweepline.hpp intrusive_list.hpp
	$(CXX) $(CXXFLAGS) $<  $(LDFLAGS) -o $@

example_segments: example_segments.cpp sweepline.hpp intrusive_list.hpp
	$(CXX) $(CXXFLAGS) $<  $(LDFLAGS) -o $@

example_intersections: example_intersections.cpp sweepline.hpp intrusive_list.hpp
	$(CXX) $(CXXFLAGS) $<  $(LDFLAGS) -o $@

benchmark: benchmark.cpp sweepline.hpp intrusive_list.hpp
	$(CXX) $(CXXFLAGS) $<  $(LDFLAGS) -o $@

clean:
	rm -f main example_segments example_intersections benchmark
