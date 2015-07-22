#ifndef PAPI_INSTANCE_H
#define PAPI_INSTANCE_H

#include "papi.h"

#include <memory>
#include <vector>
#include <map>
#include <cassert>


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


class Papi {
	public:
		PapiInstance *create();

	private:
		std::unique_ptr<PapiInstance> instance;
};

#endif
