CXXFLAGS = -g -MMD

SRCS = main.cpp parse.cpp scan.cpp
OBJS = $(SRCS:%.cpp=build/%.o)
DEPS = $(SRCS:%.cpp=build/%.d)

lucc: $(OBJS)
	$(CXX) -o $@ $^

build/%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(OBJS): | build
build:
	mkdir -p $@

clean:
	$(RM) -r lucc build

.PHONY: clean

-include $(DEPS)
