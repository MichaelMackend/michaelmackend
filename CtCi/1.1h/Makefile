
SERVER_TARGET = app
SERVER_DRIVER = appsrc/main.cpp
SERVER_OBJECT = build/main.o


CC = g++
CFLAGS = -std=c++11

OUTPUT = bin/
BUILDDIR = build/
SRCDIR = src/
TESTDIR = test/

SRCEXT = cpp

SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))

#sources :
#	@echo "SOURCES=$(SOURCES)"

TESTSOURCES = $(shell find $(TESTDIR) -type f -name *.$(SRCEXT))

#testsources :
#	@echo "TESTSOURCES=$(TESTSOURCES)"

OBJECTS = $(patsubst $(SRCDIR)%,$(BUILDDIR)%,$(SOURCES:.$(SRCEXT)=.o))

#objects :
#	@echo "OBJECTS=$(OBJECTS)"

TESTOBJECTS = $(patsubst $(TESTDIR)%,$(BUILDDIR)%,$(TESTSOURCES:.$(SRCEXT)=.o))

#testobjects :
#	@echo "TESTOBJECTS=$(TESTOBJECTS)"

INCLUDE = -I include

LIBDIR = -L lib

LIB =

all : $(SERVER_TARGET) tester

$(SERVER_TARGET) : $(OBJECTS)
	@mkdir -p $(OUTPUT)
	$(CC) $(CFLAGS) -g $(INCLUDE) -c $(SERVER_DRIVER) -o $(SERVER_OBJECT)
	$(CC) $(OBJECTS) $(SERVER_OBJECT) $(LIBDIR) $(LIB)  -o $(OUTPUT)$(SERVER_TARGET)

$(BUILDDIR)%.o : $(SRCDIR)%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -g $(INCLUDE) -c $^ -o $@

clean :
	rm -fdr $(OUTPUT) $(BUILDDIR)

tester: $(TESTOBJECTS) $(OBJECTS)
	@mkdir -p $(OUTPUT)
	$(CC) $(CFLAGS) $(TESTOBJECTS) $(OBJECTS) $(LIBDIR) $(LIBDIR) $(LIB) -o $(OUTPUT)tester

$(BUILDDIR)%.o : $(TESTDIR)%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -g $(INCLUDE) -c $^ -o $@

test : tester
	./bin/tester


