#ifndef PAPI_INSTANCE_H
#define PAPI_INSTANCE_H

#include "papi.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "Collector.h"

/**
 * This class is the interface to take measurements using the PAPI performance
 * counters.
 * It let's you add an event you want to capture.
 *
 */
class PapiInstance : public Collector {
public:
	void addEvent(int event) __attribute__((no_instrument_function));
	void start() __attribute__((no_instrument_function));
	void stop() __attribute__((no_instrument_function));
	void read() __attribute__((no_instrument_function));
	void reset() __attribute__((no_instrument_function));

	long long getEventValue(int event) __attribute__((no_instrument_function));

	~PapiInstance();

private:
	void mapValuesToMapEntries(const std::vector<long long> &vals)
			__attribute__((no_instrument_function));

	friend class Papi;
	PapiInstance() __attribute((no_instrument_function));
	PapiInstance(const PapiInstance &other) = delete;
	PapiInstance operator=(const PapiInstance &other) = delete;

	int eventSet;
	std::vector<int> eventRegChain;
	std::map<int, long long> eventValMap;
};

/**
 * The class is used to obtain an instance of the PapiInstance class,
 * which gives you access to the PAPI functionality itself.
 */
class Papi {
public:
	Papi() __attribute__((no_instrument_function));
	PapiInstance *create() __attribute__((no_instrument_function));

private:
	std::unique_ptr<PapiInstance> instance;
};

static Papi papi;

extern "C" {

#ifdef _LIB_MONITOR_
#define CTOR_ATTRIBUTE __attribute__((no_instrument_function))
#define DTOR_ATTRIBUTE __attribute__((no_instrument_function))
#elif _NO_CTOR_ATTR
#define CTOR_ATTRIBUTE __attribute__((no_instrument_function))
#define DTOR_ATTRIBUTE __attribute__((no_instrument_function))
#else
#define CTOR_ATTRIBUTE __attribute__((constructor, no_instrument_function))
#define DTOR_ATTRIBUTE __attribute__((destructor, no_instrument_function))
#endif

/**
 * These two functions are used in an LD_PRELOAD setting
 */
void PapiW_start() CTOR_ATTRIBUTE;
void PapiW_stopAndPrint() DTOR_ATTRIBUTE;
int getEnvEventCode(char *event_code) __attribute__((no_instrument_function));

/** -------- */

#ifdef _LIB_MONITOR_
void *monitor_init_process(int *argc, char **argv, void *data);
void monitor_fini_process(int how, void *data);
#endif
}

#endif
