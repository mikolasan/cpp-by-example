#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include <vector>


class DataProcessor {
  virtual std::vector<size_t> get_area_size() const = 0;
};


#endif