#ifndef RECORDER_H
#define RECORDER_H

#include "papi.h"

#include "Collector.h"
#include "EventSet.h"
#include <map>

/**
 * The Recorder provides the facilities to record all kinds of events.
 * The user adds the events she wants to collect.
 * Possibly assign a group ID, such that not all events are started all the
 * time.
 */
class Recorder {

public:
  explicit Recorder() __attribute__((no_instrument_function));
  void addTimer(int groupId = 0) __attribute__((no_instrument_function));
  void addPAPI(int PapiEvent, int groupId = 0)
      __attribute__((no_instrument_function));

private:
  std::map<int, EventSet> events;
};

#endif
