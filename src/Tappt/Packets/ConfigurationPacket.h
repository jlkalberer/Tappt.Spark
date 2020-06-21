#pragma once

#include "Tappt/Pins.h"
#include "Tappt/Packets/PacketBase.h"

class ConfigurationPacket : public PacketBase {
public:
  ConfigurationPacket();

  bool IsReady();
  void PrepareNextResponse(uint32_t pulses);

  virtual void ResetDataPacket();

private:
  bool isReady = true;
  uint8_t destination = 0;
};
