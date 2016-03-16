#ifndef PAPI_INSTANCE_H
#define PAPI_INSTANCE_H

#include "papi.h"

#include <memory>
#include <vector>
#include <map>
#include <cassert>
#include <iostream>

#include "Collector.h"

extern "C" {

	void PapiW_start() __attribute__((no_instrument_function));
	void PapiW_stopAndPrint() __attribute__((no_instrument_function));

}


/**
 * This class is the interface to take measurements using the PAPI performance counters.
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

		long long getEventValue(int event) __attribute__((no_instrument_function)) ;

		~PapiInstance();

	private:

		void mapValuesToMapEntries(const std::vector<long long> &vals) __attribute__((no_instrument_function));

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

void PapiW_start() {
		PapiInstance *instance = papi.create();
		instance->addEvent(PAPI_TOT_INS);
		instance->addEvent(PAPI_BR_MSP);
		instance->start();
}

void PapiW_stopAndPrint(){
	PapiInstance *instance = papi.create();
	instance->stop();
		std::cout << "\nThe measured total instructions were: " << instance->getEventValue(PAPI_TOT_INS) << "\n";
		std::cout << "The measured mispredicted branches were: " << instance->getEventValue(PAPI_BR_MSP) << std::endl;
}



#endif
