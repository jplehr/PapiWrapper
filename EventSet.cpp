#include "EventSet.h"

void EventSet::start(){
	for(Collector *c : collectors){
		c->start();
	}
}

void EventSet::stop(){
	for(Collector *c : collectors){
		c->stop();
	}
}

void EventSet::reset(){
	for(Collector *c : collectors){
		c->reset();
	}
}

void EventSet::addCollector(Collector *c){
	collectors.push_back(c);
}
