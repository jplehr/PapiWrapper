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
		{ "L1_DCM", PAPI_L1_DCM },
		{ "L1_ICM", PAPI_L1_ICM },
		{ "L2_DCM", PAPI_L2_DCM },
		{ "L2_ICM", PAPI_L2_ICM },
		{ "L3_DCM", PAPI_L3_DCM },
		{ "L3_ICM", PAPI_L3_ICM },
		{ "L1_TCM", PAPI_L1_TCM },
		{ "L2_TCM", PAPI_L2_TCM },
		{ "L3_TCM", PAPI_L3_TCM },
		{ "CA_SNP", PAPI_CA_SNP },
		{ "CA_SHR", PAPI_CA_SHR },
		{ "CA_CLN", PAPI_CA_CLN },
		{ "CA_INV", PAPI_CA_INV },
		{ "CA_ITV", PAPI_CA_ITV },
		{ "L3_LDM", PAPI_L3_LDM },
		{ "L3_STM", PAPI_L3_STM },
		{ "BRU_IDL", PAPI_BRU_IDL },
		{ "FXU_IDL", PAPI_FXU_IDL },
		{ "FPU_IDL", PAPI_FPU_IDL },
		{ "LSU_IDL", PAPI_LSU_IDL },
		{ "TLB_DM", PAPI_TLB_DM },
		{ "TLB_IM", PAPI_TLB_IM },
		{ "TLB_TL", PAPI_TLB_TL },
		{ "L1_LDM", PAPI_L1_LDM },
		{ "L1_STM", PAPI_L1_STM },
		{ "L2_LDM", PAPI_L2_LDM },
		{ "L2_STM", PAPI_L2_STM },
		{ "BTAC_M", PAPI_BTAC_M },
		{ "PRF_DM", PAPI_PRF_DM },
		{ "L3_DCH", PAPI_L3_DCH },
		{ "TLB_SD", PAPI_TLB_SD },
		{ "CSR_FAL", PAPI_CSR_FAL },
		{ "CSR_SUC", PAPI_CSR_SUC },
		{ "CSR_TOT", PAPI_CSR_TOT },
		{ "MEM_SCY", PAPI_MEM_SCY },
		{ "MEM_RCY", PAPI_MEM_RCY },
		{ "MEM_WCY", PAPI_MEM_WCY },
		{ "STL_ICY", PAPI_STL_ICY },
		{ "FUL_ICY", PAPI_FUL_ICY },
		{ "STL_CCY", PAPI_STL_CCY },
		{ "FUL_CCY", PAPI_FUL_CCY },
		{ "HW_INT", PAPI_HW_INT },
		{ "BR_UCN", PAPI_BR_UCN },
		{ "BR_CN", PAPI_BR_CN },
		{ "BR_TKN", PAPI_BR_TKN },
		{ "BR_NTK", PAPI_BR_NTK },
		{ "BR_MSP", PAPI_BR_MSP },
		{ "BR_PRC", PAPI_BR_PRC },
		{ "FMA_INS", PAPI_FMA_INS },
		{ "TOT_IIS", PAPI_TOT_IIS },
		{ "TOT_INS", PAPI_TOT_INS },
		{ "INT_INS", PAPI_INT_INS },
		{ "FP_INS", PAPI_FP_INS },
		{ "LD_INS", PAPI_LD_INS },
		{ "SR_INS", PAPI_SR_INS },
		{ "BR_INS", PAPI_BR_INS },
		{ "VEC_INS", PAPI_VEC_INS },
		{ "FLOPS", PAPI_FLOPS },
		{ "RES_STL", PAPI_RES_STL },
		{ "FP_STAL", PAPI_FP_STAL },
		{ "TOT_CYC", PAPI_TOT_CYC },
		{ "IPS", PAPI_IPS },
		{ "LST_INS", PAPI_LST_INS },
		{ "SYC_INS", PAPI_SYC_INS },
		{ "L1_DCH", PAPI_L1_DCH },
		{ "L2_DCH", PAPI_L2_DCH },
		{ "L1_DCA", PAPI_L1_DCA },
		{ "L2_DCA", PAPI_L2_DCA },
		{ "L3_DCA", PAPI_L3_DCA },
		{ "L1_DCR", PAPI_L1_DCR },
		{ "L2_DCR", PAPI_L2_DCR },
		{ "L3_DCR", PAPI_L3_DCR },
		{ "L1_DCW", PAPI_L1_DCW },
		{ "L2_DCW", PAPI_L2_DCW },
		{ "L3_DCW", PAPI_L3_DCW },
		{ "L1_ICH", PAPI_L1_ICH },
		{ "L2_ICH", PAPI_L2_ICH },
		{ "L3_ICH", PAPI_L3_ICH },
		{ "L1_ICA", PAPI_L1_ICA },
		{ "L2_ICA", PAPI_L2_ICA },
		{ "L3_ICA", PAPI_L3_ICA },
		{ "L1_ICR", PAPI_L1_ICR },
		{ "L2_ICR", PAPI_L2_ICR },
		{ "L3_ICR", PAPI_L3_ICR },
		{ "L1_ICW", PAPI_L1_ICW },
		{ "L2_ICW", PAPI_L2_ICW },
		{ "L3_ICW", PAPI_L3_ICW },
		{ "L1_TCH", PAPI_L1_TCH },
		{ "L2_TCH", PAPI_L2_TCH },
		{ "L3_TCH", PAPI_L3_TCH },
		{ "L1_TCA", PAPI_L1_TCA },
		{ "L2_TCA", PAPI_L2_TCA },
		{ "L3_TCA", PAPI_L3_TCA },
		{ "L1_TCR", PAPI_L1_TCR },
		{ "L2_TCR", PAPI_L2_TCR },
		{ "L3_TCR", PAPI_L3_TCR },
		{ "L1_TCW", PAPI_L1_TCW },
		{ "L2_TCW", PAPI_L2_TCW },
		{ "L3_TCW", PAPI_L3_TCW },
		{ "FML_INS", PAPI_FML_INS },
		{ "FAD_INS", PAPI_FAD_INS },
		{ "FDV_INS", PAPI_FDV_INS },
		{ "FSQ_INS", PAPI_FSQ_INS },
		{ "FNV_INS", PAPI_FNV_INS },
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
				std::cout << "[Warning] " << token
									<< " is not a known PAPI event." << std::endl;
			} else {
				std::cout << "Measuring " << token << std::endl;
				try {
					instance->addEvent(event);
				} catch (std::string &error) {
					std::cout << "[Error] " << error << std::endl;
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
		throw std::string("Argument error. Maybe try measuring less events at the same time.");
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
