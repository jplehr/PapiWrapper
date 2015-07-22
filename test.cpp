#include "PapiInstance.h"

#include <iostream>

int main(int argc, char **argv){
	Papi papi;
	PapiInstance *instance = papi.create();
	
	instance->addEvent(PAPI_TOT_INS);
	instance->start();
	instance->stop();

	std::cout << "Total instructions: " << instance->getEventValue(PAPI_TOT_INS) << std::endl;

	return 0;
}
