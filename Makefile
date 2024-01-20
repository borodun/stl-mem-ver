LIBNAME = memver
LIB_DIR = lib
LIBFILE = $(LIB_DIR)/lib$(LIBNAME).so
LIB_SOURCES = $(wildcard $(LIB_DIR)/*.cpp)
LIB_OBJS = $(LIB_SOURCES:$(LIB_DIR)/%.cpp=$(LIB_DIR)/%.o)
LIB_DEPS = $(wildcard $(LIB_DIR)/include/*.h)

PROGNAME = demo
PROG_DIR = test
PROG_OBJS = $(PROG_DIR)/main.o
PROG_DEPS = $(wildcard $(PROG_DIR)/include/*.h)

CXX = g++
CXXFLAGS = -Wall -Wpointer-arith -Werror=vla -Wendif-labels -Wmissing-format-attribute \
			-Wimplicit-fallthrough=3 -Wcast-function-type -Wshadow=compatible-local \
			-Wformat-security -fno-strict-aliasing -fwrapv -Wno-format-truncation \
			-Wno-stringop-truncation -g -Og -ggdb3 -fno-omit-frame-pointer -fPIC -std=c++20 \
			-fconcepts-diagnostics-depth=2 \
			# -fsanitize=address

all: $(PROGNAME)
	chmod +x $^

$(PROGNAME): $(LIBFILE) $(PROG_OBJS) $(PROG_DEPS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(PROG_OBJS) -o $@ -l$(LIBNAME)

$(PROGNAME): LDFLAGS = -L./lib -Itest/include/ -Ilib/include/

$(LIBFILE): $(LIB_OBJS) $(LIB_DEPS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIB_OBJS) -shared -o $@

$(PROG_OBJS): LDFLAGS = -Itest/include/ -Ilib/include/
$(LIB_OBJS): LDFLAGS = -Ilib/include/

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c $< -o $@

doc:
	doxygen doxygen.conf

clean:
	rm $(LIB_OBJS) $(LIBFILE) $(PROG_OBJS) $(PROGNAME)

run: $(PROGNAME)
	LD_LIBRARY_PATH=./lib/ ./$^

.PHONY: all clean run doc
