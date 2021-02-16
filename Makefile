PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

VPATH=${PROJECT_ROOT}:${PROJECT_ROOT}/MicroChip/WirelessProtocols/MiWi/:${PROJECT_ROOT}/MicroChip/WirelessProtocols/:${PROJECT_ROOT}/MicroChip/Transceivers/MRF24J40/

BUILD_MODE?=debug

MIWI_OBJS=MiWi.o SymbolTime.o MRF24J40.o


OBJS = interface.o ${MIWI_OBJS} CatFeeder.o 

CFLAGS +=-I${PROJECT_ROOT}inc -I${PROJECT_ROOT}/MicroChip/Include

LDFLAGS += -lwiringPi -lpthread

ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g
else ifeq ($(BUILD_MODE),run)
	CFLAGS += -O2
else ifeq ($(BUILD_MODE),linuxtools)
	CFLAGS += -g -pg -fprofile-arcs -ftest-coverage
	LDFLAGS += -pg -fprofile-arcs -ftest-coverage
else
    $(error Build mode $(BUILD_MODE) not supported by this Makefile)
endif

all:	CatFeeder

CatFeeder:	$(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o:	$(PROJECT_ROOT)%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o:	$(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr CatFeeder $(OBJS)
