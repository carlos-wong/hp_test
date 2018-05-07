.PHONY: clean all

all: clean libArkNaviProtocol.so protocoltest

COMPILER := arm-none-linux-gnueabi-
CCCOMPILER ?= ${COMPILER}gcc
CPPCOMPILER ?= ${COMPILER}g++

ark_headers := $(wildcard *.h)
customers_sources := $(wildcard customers/*.c)
customers_objects := $(patsubst %.c, %.o, $(customers_sources))

protocoltest: protocoltest.o libArkNaviProtocol.so
	${CPPCOMPILER} -o protocoltest $< -L. -lArkNaviProtocol -lpthread
	mkdir -p ./out
	cp $@ ./out

protocoltest.o: protocoltest.cpp ArkNaviProtocol.h
	${CPPCOMPILER} -c $<

libArkNaviProtocol.so: ArkNaviProtocol.o GPIOSet.o $(customers_objects)
	${CCCOMPILER} -shared $^ -o $@
	mkdir -p ./out
	cp ${ark_headers} $@ ./out
	rm ./out/GPIOSet.h

GPIOSet.o: GPIOSet.c
	${CCCOMPILER} -c -fPIC $<
ArkNaviProtocol.o: ArkNaviProtocol.c ${ark_headers}
	${CCCOMPILER} -c -fPIC $<

%.o: %.c
	${CCCOMPILER} -c -fPIC $^ -o $@

clean:
	rm -rf *.o libArkNaviProtocol.so protocoltest out/* customers/*.o
