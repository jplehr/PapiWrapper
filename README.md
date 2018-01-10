# Simple c++ wrapper for PAPI perf counter
The library can be used as a C++ tool, which requires to add the library as a build dependency.
The other possibility is to use it as an ```LD_PRELOAD``` library or with libmonitor[1].

## Example usage
```{.cpp}
int main(int argc, char **argv) {
Papi papi;
auto instance = papi.create();
instance->addEvent(PAPI_TOT_INS);
instance->start();
/** Some workload */
instance->stop();
std::cout << "total instructions " << instance->getEventValue(PAPI_TOT_INS) << std::endl;
return 0;
}
```

## Not thread safe
In its current state the library is not thread safe.
Thread support may be added in the future.


[1]https://github.com/HPCToolkit/libmonitor 
