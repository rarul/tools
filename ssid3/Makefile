TARGETS := ssid3
LDLIBS := -lstdc++
include ../mk/simple_compile.mk

ssid3: myfile.o myid3base.o myid3v1.o myid3v2.o ssid3.o myid3util.o

ssid3.o myid3base.o myid3v1.o myid3v2.o myfile.o: myfile.h
ssid3.o myid3base.o myid3v1.o myid3v2.o: myid3base.h
ssid3.o myid3v1.o: myid3v1.h
ssid3.o myid3v2.o: myid3v2.h
ssid3.o myid3base.o myid3v1.o myid3v2.o: ssid3.h
myid3v1.o myid3util.o: myid3util.h
