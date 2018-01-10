#ifndef COLLECTOR_H
#define COLLECTOR_H

class Collector {

public:
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void reset() = 0;
};

#endif
