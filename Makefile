
PAPI=/shared/apps/papi/5.4.0

PAPI_LIB_DIR=$(PAPI)/lib
PAPI_INC_DIR=$(PAPI)/include

LDFLAGS=-L$(PAPI_LIB_DIR) -lpapi

CXXFLAGS= -g -std=c++11 --shared -fPIC -I$(PAPI_INC_DIR)

SOURCES=PapiInstance.cpp

all: lib test

lib: $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o libpapicpp.so $(LDFLAGS)

test: test.cpp
	$(CXX) -std=c++11 -I$(PAPI_INC_DIR) test.cpp -o test-libpapicpp -L. -lpapicpp $(LDFLAGS)

check: test
	./test-libpapicpp 12353
