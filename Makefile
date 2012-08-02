
VERSION   = 0.2.2
CXX      ?= g++
CXXFLAGS ?= -g -Wall 
CXXFLAGS += -std=c++0x -DVERSION=\"$(VERSION)\" -Ic9y
prefix   ?= /usr/local/

c9y_headers = $(wildcard c9y/*.h)
c9y_src     = $(wildcard c9y/*.cpp)
c9y_libs    = -lsigc-2.0
test_src    = $(wildcard test/*.cpp)
test_libs   = -lUnitTest++ $(c9y_libs)

ifeq ($(MSYSTEM), MINGW32)
  EXEEXT=.exe
  LIBEXT=.dll
else
  EXEEXT=
  LIBEXT=.so
  c9y_libs += -lpthread
endif

.PHONY: all clean check dist install uninstall
.SUFFIXES: .o .cpp

all: c9y$(LIBEXT)

c9y$(LIBEXT): $(patsubst %.cpp, %.o, $(c9y_src))
	$(CXX) -shared -fPIC $(CXXFLAGS) $(LDFLAGS) $^ $(c9y_libs) -Wl,--out-implib=$(patsubst %$(LIBEXT),lib%.a, $@) -o $@

clean:
	rm -f */*.o */*.d c9y$(LIBEXT) test$(EXEEXT)

check: test$(EXEEXT)
	./test$(EXEEXT)

test$(EXEEXT): c9y$(LIBEXT) $(patsubst %.cpp, %.o, $(test_src))
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(test_libs) -o $@
		
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -MD -c $< -o $(patsubst %.cpp, %.o, $<)

install:
	mkdir -p $(prefix)include/c9y/
	cp c9y/*.h $(prefix)include/c9y
	mkdir -p $(prefix)bin
	cp c9y$(LIBEXT) $(prefix)bin/
	mkdir -p $(prefix)lib
	cp libc9y.a $(prefix)lib/

uninstall:
	rm -rf $(prefix)include/c9y
	rm -rf $(prefix)bin/c9y$(LIBEXT)
	rm -rf $(prefix)lib/libc9y.a
	
ifneq "$(MAKECMDGOALS)" "clean"
deps  = $(patsubst %.cpp, %.d, $(c9y_src))
deps += $(patsubst %.cpp, %.d, $(test_src))
-include $(deps)
endif