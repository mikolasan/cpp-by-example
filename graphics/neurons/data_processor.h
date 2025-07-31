#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include <bx/math.h>


class DataProcessor {
  virtual bx::Vec3 get_area_size() const = 0;
};


#endif