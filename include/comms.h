#ifndef COMMS_C__
#define COMMS_C__

#include "sensors.h"

void comms_init(void);

void comms_send(SensorData *data);

#endif  // COMMS_C__