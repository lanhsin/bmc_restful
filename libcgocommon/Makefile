RM=rm -f

INCLUDES= \
	-Iinclude \

CPPFLAGS=-g -std=c++11 $(INCLUDES)
LDFLAGS=-g 

SRCS= src/error.cpp src/general.cpp src/sysfs.cpp src/access.cpp src/chips.cpp src/fan.cpp src/hwmon.cpp
OBJS=$(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@

.PHONY: all cgocommon clean install

all: cgocommon

cgocommon: $(OBJS)
	$(AR) rvs libcgocommon.a $(OBJS)

install: cgocommon
	cp libcgocommon.a /usr/lib # FIXME: better use install
	cp -r include/* /usr/include

main.o: main.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@

test: $(OBJS) main.o
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(OBJS) main.o
	$(RM) libcgocommon.a