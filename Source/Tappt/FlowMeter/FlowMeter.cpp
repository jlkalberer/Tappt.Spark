#include "FlowMeter.h"

/*
void pour() {
  if ((millis() - time) >= 1000) {
    detachInterrupt(FLOW);
    time = millis();

    if (flowCount == PULSE_EPSILON) {
      waitCount++;
      int maxWait = totalPulses > PULSE_EPSILON ? 3 : 5;
      if (waitCount > maxWait)
      {
        state = DONE_POURING;
      }

    } else {
      waitCount = 0;
      Serial.print("P:");
      Serial.println(flowCount);
      totalPulses += flowCount;
      flowCount = 0;
    }
    attachInterrupt(FLOW, flowCounter, FALLING);
  }
}
*/