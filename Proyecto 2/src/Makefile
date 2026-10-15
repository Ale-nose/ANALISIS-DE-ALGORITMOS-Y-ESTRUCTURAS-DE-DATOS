include ../../common/Makefile

HEADERX += $(shell find ../include -type f -name '*.hpp')

MOC_FILES := $(shell find . -type f -name 'moc_*.cpp')
INPUTFX := $(filter-out $(MOC_FILES),$(INPUTFX))

FLAG += $(shell pkg-config --cflags Qt5Widgets Qt5Multimedia) -fPIC
FLAG += -I../include -I../include/core -I../include/config -I../include/game
LIBS += $(shell pkg-config --libs Qt5Widgets Qt5Multimedia)