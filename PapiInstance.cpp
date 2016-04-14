#include "PapiInstance.h"

#ifdef _LIB_MONITOR_
void *monitor_init_process(int *argc, char **argv, void *data){
	PapiW_start();
	return NULL;
}

void monitor_fini_process(int how, void *data){
	PapiW_stopAndPrint();
}
#endif

PapiInstance *Papi::create(){
	if(instance == nullptr){
			instance.reset(new PapiInstance());
	}
	return instance.get();
}


PapiInstance::PapiInstance() : eventSet(PAPI_NULL){
	int returnVal = PAPI_library_init(PAPI_VER_CURRENT);
	
	if((returnVal != PAPI_VER_CURRENT && returnVal > 0) || returnVal < 0){
		throw std::string("The Papi initialization failed");
	}

	if(PAPI_create_eventset(&eventSet) != PAPI_OK){
		throw std::string("Creation of event set failed.");
	}
}

PapiInstance::~PapiInstance(){
	std::vector<long long> vals(eventValMap.size());
	if(PAPI_stop(eventSet, vals.data()) != PAPI_OK){
		// Do some error handling here?
	}
	assert(vals.size() == eventRegChain.size() && "As many results as events");
	mapValuesToMapEntries(vals);
}

void PapiInstance::addEvent(int event){
	if(PAPI_add_event(eventSet, event) != PAPI_OK){
		throw std::string("Adding the event failed.");
	}
	eventValMap[event] = 0;
	eventRegChain.push_back(event);
}

void PapiInstance::start(){
	if(PAPI_start(eventSet) != PAPI_OK){
		throw std::string("Could not start measurement");
	}
}

void PapiInstance::stop(){
	std::vector<long long> vals(eventValMap.size());
	if(PAPI_stop(eventSet, vals.data()) != PAPI_OK){
		throw std::string("There went something wrong when trying to stop PAPI");
	}
	assert(vals.size() == eventRegChain.size() && "As many results as events");
	mapValuesToMapEntries(vals);
}
	
void PapiInstance::reset() {
	throw std::string("[ERROR IN PAPI WRAPPER] Not yet implemented.");
}

void PapiInstance::read(){
	std::vector<long long> vals(eventValMap.size());
	if(PAPI_read(eventSet, vals.data()) != PAPI_OK){
		throw std::string("Could not read cunters");
	}
	assert(vals.size() == eventRegChain.size() && "As many results as events");
	mapValuesToMapEntries(vals);
}

void PapiInstance::mapValuesToMapEntries(const std::vector<long long> &vals){
	for(int i = 0; i < vals.size(); ++i){
		eventValMap[eventRegChain.at(i)] = vals.at(i);
	}
}

long long PapiInstance::getEventValue(int event){
	if(eventValMap.find(event) == eventValMap.end()){
		throw std::string("The requested counter value was not found.");
	}

	return eventValMap[event];
}

Papi::Papi() : instance(nullptr) {}
