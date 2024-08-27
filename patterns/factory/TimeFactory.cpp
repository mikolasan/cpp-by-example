#include "TimeFactory.h"

TimeFactory* TimeFactory::factory = nullptr;
std::vector<BuildFunction> TimeFactory::builders = std::vector<BuildFunction>();
