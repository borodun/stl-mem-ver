LIBNAME = memver
LIBFILE = lib/lib$(LIBNAME).so
LIB_OBJS := lib/revision.o \
			lib/segment.o \
			lib/versioned.o

PROGNAME = demo
PROG_OBJS = test/main.o

CXX = g++
CXXFLAGS = -Wall -Wpointer-arith -Werror=vla -Wendif-labels -Wmissing-format-attribute \
			-Wimplicit-fallthrough=3 -Wcast-function-type -Wshadow=compatible-local \
			-Wformat-security -fno-strict-aliasing -fwrapv -Wno-format-truncation \
			-Wno-stringop-truncation -g -O2 -fPIC -std=c++20 \
			-fsanitize=address

all: $(PROGNAME)
	chmod +x $^

$(PROGNAME): $(LIBFILE) $(PROG_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(PROG_OBJS) -o $@ -l$(LIBNAME)

$(PROGNAME): LDFLAGS = -L./lib -Itest/include/ -Ilib/include/

$(LIBFILE): $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIB_OBJS) -shared -o $@

$(PROG_OBJS): LDFLAGS = -Itest/include/ -Ilib/include/
$(LIB_OBJS): LDFLAGS = -Ilib/include/

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c $< -o $@

clean:
	rm $(LIB_OBJS) $(LIBFILE) $(PROG_OBJS) $(PROGNAME)

run: $(PROGNAME)
	LD_LIBRARY_PATH=./lib/ ./$^

.PHONY: all clean run
