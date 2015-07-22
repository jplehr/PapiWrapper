#ifndef PAPI_INSTANCE_H
#define PAPI_INSTANCE_H

#include "papi.h"

#include <memory>
#include <vector>
#include <map>
#include <cassert>

/**
 * This class is the interface to take measurements using the PAPI performance counters.
 * It let's you add an event you want to capture.
 * 
 */
class PapiInstance{
	public:
		void addEvent(int event);
		void start();
		void stop();
		void read();
		long long getEventValue(int event);

	private:

		void mapValuesToMapEntries(const std::vector<long long> &vals);

		friend class Papi;
		PapiInstance();
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
		PapiInstance *create();

	private:
		std::unique_ptr<PapiInstance> instance;
};

#endif
