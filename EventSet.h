#ifndef EVENTSET_H
#define EVENTSET_H

#include <vector>

#include "Collector.h"

/**
 * Encapsulates the different collectors and provides abstract container.
 */
class EventSet {
	public:
		void start() __attribute__((no_instrument_function)) ;
		void stop() __attribute__((no_instrument_function)) ;
		void reset() __attribute__((no_instrument_function)) ;

		void addCollector(Collector *c) __attribute__((no_instrument_function)) ;

	private:
		std::vector<Collector *> collectors;
};

#endif
