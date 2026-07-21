// The process image contains data of the process
// There can't exist loose data- Each data belongs logically to a module
#ifndef PROCESSIMAGE_H_
#define PROCESSIMAGE_H_

#include "bme.h"

struct ProcessImage {
  struct BME280_for_LCD bme280;
  struct BME280_for_LCD bme280_memory;
  uint32_t cntr_10ms;
  volatile uint8_t call_bme_cyclic_flag;
};

#endif /* PROCESSIMAGE_H_ */
