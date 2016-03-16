
PAPI=/opt/papi/install

PAPI_LIB_DIR=$(PAPI)/lib
PAPI_INC_DIR=$(PAPI)/include

LDFLAGS=-L$(PAPI_LIB_DIR) -lpapi

CXXFLAGS= -g -std=c++11 --shared -fPIC -I$(PAPI_INC_DIR)

SOURCES=PapiInstance.cpp \
				EventSet.cpp \
				Recorder.cpp \
				TimerCollector.cpp

all: lib test empty_measurement

lib: $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o libpapicpp.so $(LDFLAGS)

test: test.cpp lib
	$(CXX) -std=c++11 -I$(PAPI_INC_DIR) test.cpp -o test-libpapicpp -L. -lpapicpp $(LDFLAGS)

check: test
	./test-libpapicpp 123533


empty_measurement: empty_measurement.c
	$(CC) -g --shared -fPIC -I. empty_measurement.c -o libem.so

clean:
	rm libpapicpp.so test-libpapicpp
