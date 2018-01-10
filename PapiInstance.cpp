#include "PapiInstance.h"
#include <algorithm>

#ifdef _LIB_MONITOR_
void *monitor_init_process(int *argc, char **argv, void *data){
	PapiW_start();
	return NULL;
}

void monitor_fini_process(int how, void *data){
	PapiW_stopAndPrint();
}
#endif

void PapiW_start() {
	printf("Starting Papi Measurement\n");
  PapiInstance *instance = papi.create();
  instance->addEvent(PAPI_TOT_INS);
  instance->addEvent(PAPI_L1_ICM);
  instance->start();
}

void PapiW_stopAndPrint() {
  PapiInstance *instance = papi.create();
  instance->stop();
  std::cout << "\nThe measured total instructions were: "
            << instance->getEventValue(PAPI_TOT_INS) << "\n";
  std::cout << "Collected L1_ICM Events: "
            << instance->getEventValue(PAPI_L1_ICM) << std::endl;
}


PapiInstance *Papi::create(){
	if(instance == nullptr){
			instance.reset(new PapiInstance());
	}
	return instance.get();
}

std::string getPAPIError(int err) {
	char *errstr = PAPI_strerror(err);
	std::string str;
	if(errstr){
		str = std::string(errstr);
	}
	return str;
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
	int state;
	int err = PAPI_state(eventSet, &state);

	if(err != PAPI_OK){
		throw std::string("EventSet state check failed with " + getPAPIError(err));
	}

	if(state == PAPI_RUNNING) {
		return;
	}

	PAPI_start(eventSet);
	
	if(err != PAPI_OK){
		throw std::string("Could not start measurement ("+  getPAPIError(err) + "): " + std::to_string(err));
	}
}

void PapiInstance::stop(){
	std::vector<long long> vals(eventValMap.size());
	int err = PAPI_stop(eventSet, vals.data());
	if(err != PAPI_OK){
		throw std::string("There went something wrong when trying to stop PAPI " + getPAPIError(err));
	}
	assert(vals.size() == eventRegChain.size() && "As many results as events");
	mapValuesToMapEntries(vals);
}
	
void PapiInstance::reset() {
	for(auto &p : eventValMap){
		p.second = 0;
	}
	std::for_each(eventRegChain.begin(), eventRegChain.end(), [](int &ec) {PAPI_reset(ec);});
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
