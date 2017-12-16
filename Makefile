

VERSION   = 0.1.0

CXX      ?= g++ 
CXXFLAGS := -std=c++14 -I$(shell pwd) $(CXXFLAGS)
LDFLAGS  += 
prefix   ?= /usr/local

C9YHDR      = $(wildcard c9y/*.h)
C9YSRC     =  $(wildcard c9y/*.cpp)
TSTHDR      = $(wildcard c9y-test/*.h)
TSTSRC     =  $(wildcard c9y-test/*.cpp)

EXTRA_DIST = Makefile README.md
DIST_FILES = $(C9YHDR) $(C9YSRC) $(TSTHDR) $(TSTSRC) $(EXTRA_DIST)
 
ifeq ($(OS), Windows_NT)
  EXEEXT    = .exe  
  LIBEXT    = .a 
  LDFLAGS  += -lwinmm
else
  EXEEXT    =
  LIBEXT    = .so 
  CXXFLAGS += -fPIC  
endif
 
.PHONY: all check clean 
 
all: bin/libc9y$(LIBEXT)

ifeq ($(OS), Windows_NT) 
lib/libc9y.a: bin/c9y.dll

bin/c9y.dll: $(patsubst %.cpp, .obj/%.o, $(C9YSRC))
	mkdir -p bin
	mkdir -p lib
	$(CXX) -shared -fPIC $(CXXFLAGS) $^ -o $@ -Wl,--out-implib=lib/libc9y.a $(LDFLAGS)
else
lib/libc9y$(LIBEXT): $(patsubst %.cpp, .obj/%.o, $(C9YSRC))
	mkdir -p lib
	$(CXX) -shared -fPIC $(CXXFLAGS) $(LDFLAGS) $^ -o $@
endif 
 
check: bin/c9y-test$(EXEEXT)	
	LD_LIBRARY_PATH=.. ./bin/c9y-test$(EXEEXT)
 
bin/c9y-test$(EXEEXT): $(patsubst %.cpp, .obj/%.o, $(TSTSRC)) lib/libc9y$(LIBEXT)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@
 
clean: 
	rm -rf .obj */libc9y$(LIBEXT) */libc9y.a */c9y-test$(EXEEXT)
 
.obj/%.o : %.cpp
	mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) -MD -c $< -o $@	
 
ifneq "$(MAKECMDGOALS)" "clean"
-include $(patsubst %.cpp, .obj/%.d, $(C9YSRC))
-include $(patsubst %.cpp, .obj/%.d, $(TSTSRC))
endif