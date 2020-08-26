#include "PapiInstance.h"
#include <algorithm>
#include <vector>
#include <string.h>

#ifdef _LIB_MONITOR_
void *monitor_init_process(int *argc, char **argv, void *data) {
	PapiW_start();
	return NULL;
}

void monitor_fini_process(int how, void *data) { PapiW_stopAndPrint(); }
#endif

// returns -1 if event code does not exist
int getEnvEventCode(char *event_code) {
	std::map<std::string, int> event_map = {
		{ "BR_CN",   PAPI_BR_CN },
		{ "BR_INS",  PAPI_BR_INS },
		{ "BR_MSP",  PAPI_BR_MSP },
		{ "BR_NTK",  PAPI_BR_NTK },
		{ "BR_PRC",  PAPI_BR_PRC },
		{ "FP_INS",  PAPI_FP_INS },
		{ "FP_OPS",  PAPI_FP_OPS },
		{ "L1_DCA",  PAPI_L1_DCA },
		{ "L1_DCM",  PAPI_L1_DCM },
		{ "L1_ICA",  PAPI_L1_ICA },
		{ "L1_ICM",  PAPI_L1_ICM },
		{ "L1_TCA",  PAPI_L1_TCA },
		{ "L1_TCM",  PAPI_L1_TCM },
		{ "L2_DCA",  PAPI_L2_DCA },
		{ "L2_DCM",  PAPI_L2_DCM },
		{ "L2_DCR",  PAPI_L2_DCR },
		{ "L2_DCW",  PAPI_L2_DCW },
		{ "L2_ICH",  PAPI_L2_ICH },
		{ "L2_ICR",  PAPI_L2_ICR },
		{ "L2_ICA",  PAPI_L2_ICA },
		{ "L2_ICM",  PAPI_L3_ICM },
		{ "L2_TCA",  PAPI_L2_TCA },
		{ "L2_TCM",  PAPI_L2_TCM },
		{ "L2_TCR",  PAPI_L2_TCR },
		{ "L2_TCW",  PAPI_L2_TCW },
		{ "L2_LDM",  PAPI_L2_LDM },
		{ "L2_STM",  PAPI_L2_STM },
		{ "L3_DCA",  PAPI_L3_DCA },
		{ "L3_DCM",  PAPI_L3_DCM },
		{ "L3_DCR",  PAPI_L3_DCR },
		{ "L3_DCW",  PAPI_L3_DCW },
		{ "L3_ICA",  PAPI_L3_ICA },
		{ "L3_ICR",  PAPI_L3_ICR },
		{ "L3_ICM",  PAPI_L3_ICM },
		{ "L3_TCA",  PAPI_L3_TCA },
		{ "L3_TCR",  PAPI_L3_TCR },
		{ "L3_TCW",  PAPI_L3_TCW },
		{ "L3_TCM",  PAPI_L3_TCM },
		{ "L3_LDM",  PAPI_L3_LDM },
		{ "LD_INS",  PAPI_LD_INS },
		{ "SR_INS",  PAPI_SR_INS },
		{ "LST_INS", PAPI_LST_INS },
		{ "STL_ICY", PAPI_STL_ICY },
		{ "TLB_IM",  PAPI_TLB_IM },
		{ "TOT_INS", PAPI_TOT_INS },
		{ "VEC_DP",  PAPI_VEC_DP },
		{ "VEC_SP",  PAPI_VEC_SP },
	};

	auto it = event_map.find(event_code);

	if (it != event_map.end()) {
		return it->second;
	} else {
		return -1;
	}
}

void PapiW_start() {
	const char *env_codes = getenv("LIB_PAPI_EVENTS");

	PapiInstance *instance = papi.create();

	if (env_codes == nullptr) {
		std::cout << "LIB_PAPI_EVENTS not defined. Measuring nothing." << std::endl;
	} else {
		char *codes_buffer = new char[strlen(env_codes)+1];
		strcpy(codes_buffer, env_codes);

		char delim[] = " ";
		char *token = strtok(codes_buffer, delim);

		while (token) {
			int event = getEnvEventCode(token);
			if (event == -1) {
				std::cout << "WARNING: " << token << " is either not a known PAPI event "
						  << "or hasn't yet been implemented in papi-wrap." << std::endl;
			} else {
				std::cout << "Measuring " << token << ".." << std::endl;
				try {
					instance->addEvent(event);
				} catch (std::string &error) {
					std::cout << "  Error when adding this event: " << error << std::endl;
				}
			}
			token = strtok(NULL, delim);
		}

		delete[] codes_buffer;
	}

	instance->start();
}

void PapiW_stopAndPrint() {
	PapiInstance *instance = papi.create();
	instance->stop();

	const char *env_codes = getenv("LIB_PAPI_EVENTS");

	if (env_codes == nullptr) {
		return;
	}

	char *codes_buffer = new char[strlen(env_codes)+1];
	strcpy(codes_buffer, env_codes);

	std::vector<int> results;
	std::vector<std::string> events;

	char delim[] = " ";
	char *token = strtok(codes_buffer, delim);

	while (token) {
		int event = getEnvEventCode(token);
		if (event != -1) {
			results.push_back(event);
			std::string s(token);
			events.push_back(s);
		}
		token = strtok(NULL, delim);
	}

	delete[] codes_buffer;

	for (int i = 0; i < results.size(); i++) {
		try {
			long long v = instance->getEventValue(results.at(i));
			std::cout << "Collected " << events.at(i) << " Events: " << v << std::endl;
		} catch (...) {}
	}
}

PapiInstance *Papi::create() {
	if (instance == nullptr) {
		instance.reset(new PapiInstance());
	}
	return instance.get();
}

std::string getPAPIError(int err) {
	char *errstr = PAPI_strerror(err);
	std::string str;
	if (errstr) {
		str = std::string(errstr);
	}
	return str;
}

PapiInstance::PapiInstance() : eventSet(PAPI_NULL) {
	int returnVal = PAPI_library_init(PAPI_VER_CURRENT);


	if ((returnVal != PAPI_VER_CURRENT && returnVal > 0) || returnVal < 0) {
		throw std::string("The Papi initialization failed");
	}

	if (PAPI_create_eventset(&eventSet) != PAPI_OK) {
		throw std::string("Creation of event set failed.");
	}
}

PapiInstance::~PapiInstance() {
	std::vector<long long> vals(eventValMap.size());
	if (PAPI_stop(eventSet, vals.data()) != PAPI_OK) {
		// TODO Do some error handling here
	}
	assert(vals.size() == eventRegChain.size() && "As many results as events");
	mapValuesToMapEntries(vals);
}

void PapiInstance::addEvent(int event) {
	int returnval = PAPI_add_event(eventSet, event);
	if (returnval == PAPI_EINVAL) {
		throw std::string("Argument error. Maybe you've try measuring less events at the same time.");
	} else if (returnval == PAPI_ENOMEM) {
		throw std::string("Insufficient memory to complete the operation.");
	} else if (returnval == PAPI_ENOEVST) {
		throw std::string("The event set specified does not exist.");
	} else if (returnval == PAPI_EISRUN) {
		throw std::string("The event set is currently counting events.");
	} else if (returnval == PAPI_ECNFLCT) {
		throw std::string("The underlying counter hardware can not count this event and other events in the event set simultaneously.");
	} else if (returnval == PAPI_ENOEVNT) {
		throw std::string("The PAPI preset is not available on the underlying hardware.");
	} else if (returnval == PAPI_EBUG) {
		throw std::string("Internal error, please send mail to (PAPI) developers.");
	} else if (returnval != PAPI_OK) {
		throw std::string("Adding the event failed.");
	}
	eventValMap[event] = 0;
	eventRegChain.push_back(event);
}

void PapiInstance::start() {
	int state;
	int err = PAPI_state(eventSet, &state);

	if (err != PAPI_OK) {
		throw std::string("EventSet state check failed with " + getPAPIError(err));
	}

	if (state == PAPI_RUNNING) {
		return;
	}

	PAPI_start(eventSet);

	if (err != PAPI_OK) {
		throw std::string("Could not start measurement (" + getPAPIError(err) +
							"): " + std::to_string(err));
	}
}

void PapiInstance::stop() {
	std::vector<long long> vals(eventValMap.size());
	int err = PAPI_stop(eventSet, vals.data());
	if (err != PAPI_OK) {
		throw std::string("There went something wrong when trying to stop PAPI " +
							getPAPIError(err));
	}
	assert(vals.size() == eventRegChain.size() && "As many results as events");
	mapValuesToMapEntries(vals);
}

void PapiInstance::reset() {
	for (auto &p : eventValMap) {
		p.second = 0;
	}
	std::for_each(eventRegChain.begin(), eventRegChain.end(),
					[](int &ec) { PAPI_reset(ec); });
}

void PapiInstance::read() {
	std::vector<long long> vals(eventValMap.size());
	if (PAPI_read(eventSet, vals.data()) != PAPI_OK) {
		throw std::string("Could not read cunters");
	}
	assert(vals.size() == eventRegChain.size() && "As many results as events");
	mapValuesToMapEntries(vals);
}

void PapiInstance::mapValuesToMapEntries(const std::vector<long long> &vals) {
	for (int i = 0; i < vals.size(); ++i) {
		eventValMap[eventRegChain.at(i)] = vals.at(i);
	}
}

long long PapiInstance::getEventValue(int event) {
	if (eventValMap.find(event) == eventValMap.end()) {
		throw std::string("The requested counter value was not found.");
	}

	return eventValMap[event];
}

Papi::Papi() : instance(nullptr) {}
