#ifndef TIMER_COLLECTOR_H
#define TIMER_COLLECTOR_H

#include "Collector.h"
#include <chrono>

class TimerCollector : public Collector {

	public:
		virtual ~TimerCollector(){}

		void start() __attribute__((no_instrument_function));

		void stop() __attribute__((no_instrument_function));

		void reset() __attribute__((no_instrument_function));

		typedef std::chrono::nanoseconds nanos;
		nanos getTime() __attribute__((no_instrument_function));

	private:
		std::chrono::high_resolution_clock::time_point begin, end;
};


#endif
