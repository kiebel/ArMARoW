#include <stdlib.h>

template <int number_of_fillbytes>
struct MessageLossMeasurement {
    uint32_t global_sequence_number;
    char fillbytes[number_of_fillbytes];
};
