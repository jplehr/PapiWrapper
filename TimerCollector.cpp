#include "TimerCollector.h"

void TimerCollector::start() {
  begin = std::chrono::high_resolution_clock::now();
};

void TimerCollector::stop() {
  end = std::chrono::high_resolution_clock::now();
};

void TimerCollector::reset(){
    //			start = 0;
    //			end = 0;
};

TimerCollector::nanos TimerCollector::getTime() { return nanos(end - begin); }
