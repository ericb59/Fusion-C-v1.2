ISMINGW := $(shell uname | grep MINGW)
ifneq ($(ISMINGW),)
	EXTENSION = .exe
else
	EXTENSION = 
endif

TARGETS	 = depack$(EXTENSION)
OBJECTS  = main.o CDecompress.o

BINDIR   = /usr/local/bin

CXXFLAGS = -Wall -O2
LDFLAGS  = 

DEPDIR = .deps
DEPFILE = $(DEPDIR)/$(*F)

all: $(TARGETS)

depack$(EXTENSION): $(DEPDIR) $(OBJECTS)
	g++ $(LDFLAGS) -o $@ $(OBJECTS)
	strip $@

clean:
	$(RM) $(OBJECTS) $(TARGETS)
	$(RM) -r $(DEPDIR)	

install: all
	cp -f $(TARGETS) $(BINDIR)

%.o : %.cpp
	$(CXX) -Wp,-MD,$(DEPFILE).d $(CXXFLAGS) -c -o $@ $<
	@cp $(DEPFILE).d $(DEPFILE).P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(DEPFILE).d >> $(DEPFILE).P; \
	rm -f $(DEPFILE).d

$(DEPDIR):
	@mkdir $(DEPDIR)
	
-include $(OBJECTS:%.o=$(DEPDIR)/%.P)
