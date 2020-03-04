CPP = clang++
CPPFLAGS = -Iinclude -std=c++17 -g --pedantic -Wall
SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:src/%.cpp=obj/%.o)

TESTS = $(wildcard test/*.cpp)

$(shell mkdir -p build)
$(shell mkdir -p obj)

all: build/test

build/test: $(OBJS) $(TESTS)
	$(CPP) $(CPPFLAGS) $^ -o $@ -lgtest

obj/%.o: src/%.cpp
	$(CPP) $(CPPFLAGS) $^ -c -o $@

clean:
	rm -r obj
	rm -r build