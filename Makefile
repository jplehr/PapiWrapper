
PAPI := /opt/papi/install

PAPI_LIB_DIR = $(PAPI)/lib
PAPI_INC_DIR = $(PAPI)/include

LDFLAGS = -L$(PAPI_LIB_DIR) -lpapi

CXXFLAGS = -g -std=c++14 --shared -fPIC -I$(PAPI_INC_DIR)

SOURCES=PapiInstance.cpp \
				EventSet.cpp \
				Recorder.cpp \
				TimerCollector.cpp

all: lib test empty_measurement lib-monitor

lib: $(SOURCES)
	$(CXX) $(CXXFLAGS) -D_NO_CTOR_ATTR $(SOURCES) -o libpapicpp.$(CXX).so $(LDFLAGS)

lib-monitor: $(SOURCES)
	$(CXX) -D_LIB_MONITOR_ $(CXXFLAGS) $(SOURCES) -o libpapicpp.monitor.$(CXX).so $(LDFLAGS)

example: example.cpp lib
	$(CXX) -std=c++14 -I$(PAPI_INC_DIR) example.cpp -o example-libpapicpp -L. -lpapicpp.$(CXX) $(LDFLAGS)

lib-static: $(SOURCES)
	$(CXX) -g -std=c++14 -I$(PAPI_INC_DIR) $(SOURCES) -c -o libpapicpp.g++.st.o 

empty_measurement: empty_measurement.c
	$(CC) -g --shared -fPIC -I. empty_measurement.c -o libem.so

run-example: example
	export LD_LIBRARY_PATH=`pwd`:$(LD_LIBRARY_PATH)
	./example-libpapicpp 123533

clean:
	rm libpapicpp.$(CXX).so example-libpapicpp libpapicpp.monitor.$(CXX).so

clean-all: clean
	rm libpapicpp.* example-libpapicpp libem.so

