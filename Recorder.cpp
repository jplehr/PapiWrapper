#include "Recorder.h"
#include "TimerCollector.h"

Recorder::Recorder() {}

void Recorder::addTimer(int groupId) {
  if (events.find(groupId) != events.end()) {
    // we already have a collector that has this ID
    auto e = *events.find(groupId);
    // TODO Add the collectors handling.
    e.second.addCollector(new TimerCollector());
  }
}

void Recorder::addPAPI(int PapiEvent, int groupId) {}
