SYSCONF_LINK = g++
CPPFLAGS     = --std=c++17
LDFLAGS      =
LIBS         = -lm

DESTDIR = ./build/
TARGET  = main

OBJECTS := $(patsubst %.cpp,%.o,$(wildcard src/*.cpp src/**/*.cpp))

all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) -Wall $(LDFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS)

$(OBJECTS): %.o: %.cpp
	$(SYSCONF_LINK) -Wall $(CPPFLAGS) -c $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.tga

