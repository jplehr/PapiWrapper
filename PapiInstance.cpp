#include "PapiInstance.h"
#include <algorithm>

#ifdef _LIB_MONITOR_
void *monitor_init_process(int *argc, char **argv, void *data) {
  PapiW_start();
  return NULL;
}

void monitor_fini_process(int how, void *data) { PapiW_stopAndPrint(); }
#endif

// returns -1 if 'LIB_PAPI_EVENT' is not set or set incorrectly
int getEnvEventCode() {
  std::map<std::string, int> event_map = {
    { "BR_CN",   PAPI_BR_CN },
    { "BR_INS",  PAPI_BR_INS },
    { "BR_MSP",  PAPI_BR_MSP },
    { "BR_NTK",  PAPI_BR_NTK },
    { "FP_INS",  PAPI_FP_INS },
    { "FP_OPS",  PAPI_FP_OPS },
    { "L1_DCM",  PAPI_L1_DCM },
    { "L1_ICM",  PAPI_L1_ICM },
    { "L2_DCA",  PAPI_L2_DCA },
    { "L2_DCM",  PAPI_L2_DCM },
    { "L2_DCR",  PAPI_L2_DCR },
    { "L2_DCW",  PAPI_L2_DCW },
    { "L2_ICH",  PAPI_L2_ICH },
    { "L2_ICM",  PAPI_L3_ICM },
    { "L2_TCM",  PAPI_L2_TCM },
    { "L3_TCM",  PAPI_L3_TCM },
    { "LD_INS",  PAPI_LD_INS },
    { "SR_INS",  PAPI_SR_INS },
    { "STL_ICY", PAPI_STL_ICY },
    { "TLB_IM",  PAPI_TLB_IM },
    { "TOT_INS", PAPI_TOT_INS },
    { "VEC_DP",  PAPI_VEC_DP },
    { "VEC_SP",  PAPI_VEC_SP },
  };

  const char *env_code = getenv("LIB_PAPI_EVENT");

  if (env_code != NULL) {
    auto it = event_map.find(getenv("LIB_PAPI_EVENT"));

    if (it != event_map.end())
      return it->second;
  }

  return -1;
}

void PapiW_start() {
  printf("Starting Papi Measurement\n");
  PapiInstance *instance = papi.create();
  instance->addEvent(PAPI_TOT_INS);
  instance->addEvent(PAPI_L1_ICM);
  instance->start();
}

void PapiW_start() {
  printf("Starting Papi Measurement\n");

  int env_event_code = getEnvEventCode();
  if (env_event_code == -1) {
    std::cout << "WARNING: Environment variable 'LIB_PAPI_EVENT' is ";

    const char *env = getenv("LIB_PAPI_EVENT");

    if (env == NULL) {
      std::cout << "not set.\n";
    } else {
      std::cout << "set to '" << env << "', which is not a valid event.\n";
    }
  }

  PapiInstance *instance = papi.create();

  if (env_event_code != -1) {
    instance->addEvent(env_event_code);
  }

  instance->start();
}

void PapiW_stopAndPrint() {
  PapiInstance *instance = papi.create();
  instance->stop();
  
  int env_event_code = getEnvEventCode();
  if (env_event_code != -1) {
    std::cout << "Collected " << getenv("LIB_PAPI_EVENT") << " Events: "
              << instance->getEventValue(getEnvEventCode()) << std::endl;
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
  if (PAPI_add_event(eventSet, event) != PAPI_OK) {
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
