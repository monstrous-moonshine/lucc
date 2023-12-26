CXXFLAGS = -g -MMD

SRCS = main.cpp parse.cpp scan.cpp
OBJS = $(SRCS:%.cpp=build/%.o)
DEPS = $(SRCS:%.cpp=build/%.d)

mycc: $(OBJS)
	$(CXX) -o $@ $^

build/%.o build/%.d: build
build/%.o: %.cpp
	$(CXX) -c -o $@ $^ $(CXXFLAGS)

build:
	mkdir -p $@

clean:
	$(RM) mycc $(OBJS) $(DEPS)

.PHONY: clean

-include $(DEPS)
