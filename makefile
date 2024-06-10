CXX := g++
WX_CFLAGS := $(shell wx-config --cxxflags)
WX_LIBS := $(shell wx-config --libs)
MATHPLOT_LIB := -lwxmathplot

all: FinanceTracker

FinanceTracker: src/FinanceTracker.cpp
	$(CXX) -o FinanceTracker src/FinanceTracker.cpp $(WX_CFLAGS) $(WX_LIBS) $(MATHPLOT_LIB)

clean:
	rm -f FinanceTracker