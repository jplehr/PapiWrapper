#include "PapiInstance.h"

#include <cmath>
#include <iostream>

int main(int argc, char **argv) {

  const int loopLimit(10000);

  Papi papi;
  auto instance = papi.create();

  instance->addEvent(PAPI_TOT_INS);
  instance->addEvent(PAPI_BR_MSP);
  instance->start();
  for (int i = 0; i < loopLimit; ++i) {
    auto res = std::pow(std::atoi(argv[1]), 2);
    if (int(res) % 10 == 0) {
      std::cout << "branch taken" << std::endl;
    }
  }

  instance->stop();

  std::cout << "Total instructions: " << instance->getEventValue(PAPI_TOT_INS)
            << std::endl;
  std::cout << "Branch instructins misspredicted: "
            << instance->getEventValue(PAPI_BR_MSP) << std::endl;

  return 0;
}
