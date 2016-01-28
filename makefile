IDIR=include
CXX=g++
DEBUG=-g
CFLAGS=-std=c++0x -c -o
LFLAGS=-std=c++0x -lpthread $(DEBUG)
SDIR=src
ODIR=obj
LDIR=lib
LIBS=-lm
BDIR=bin
DEPS=$(IDIR)/CSVReader.h $(IDIR)/Dictionary.h $(IDIR)/KB.h $(IDIR)/Lexicon.h $(IDIR)/Parameter.h $(IDIR)/STtable.h

all: $(BDIR)/training $(BDIR)/recall

$(ODIR)/training.o:$(SDIR)/Training.cpp $(IDIR)/Training.h $(DEPS)
	$(CXX) $(CFLAGS) $@ $<
$(ODIR)/recall.o:$(SDIR)/Recall.cpp $(IDIR)/Recall.h $(DEPS)
	$(CXX) $(CFLAGS) $@ $<

$(BDIR)/training:$(ODIR)/training.o $(DEPS)
	$(CXX) -o $@ $^ $(LFLAGS)
$(BDIR)/recall:$(ODIR)/recall.o $(DEPS)
	$(CXX) -o $@ $^ $(LFLAGS)
 
.PHONY: clean

clean:
	rm -f $(BDIR)/* $(ODIR)/*.o 


