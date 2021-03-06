#pragma once

#include "Tappt/KegeratorStateMachine/KegeratorStateMachine.h"
#include "Tappt/ServerLink/ServerLink.h"

TEST_CASE("ServerLink::Initialize", "[ServerLink]") {
  SECTION("empty input string") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    ServerLink link(&stateMock.get());
    link.Initialize("", "");
    Verify(Method(stateMock, Initialize)).Never();
  }

  SECTION("bad input string") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    ServerLink link(&stateMock.get());
    link.Initialize("", "{}");
    Verify(Method(stateMock, Initialize)).Never();
  }

  auto deviceSettingsComparer = [](DeviceSettings &s, DeviceSettings &other) {
    if (
      s.tapIds == NULL ||
      other.tapIds == NULL ||
      s.pulsesPerGallon == NULL ||
      other.pulsesPerGallon == NULL ||
      s.tapCount != other.tapCount
      ) {
      return false;
    }

    for (int ii = 0; ii < s.tapCount; ii++) {
      if (
        s.tapIds[ii] != other.tapIds[ii] ||
        s.pulsesPerGallon[ii] != other.pulsesPerGallon[ii]
        ) {
        return false;
      }
    }

    return s.authorizationToken.compareTo(other.authorizationToken) == 0 &&
      s.deviceId.compareTo(other.deviceId) == 0 &&
      s.deviceStatus == other.deviceStatus &&
      s.isScreenDisabled == other.isScreenDisabled &&
      s.isTOTPDisabled == other.isTOTPDisabled &&
      s.ledBrightness == other.ledBrightness &&
      s.secondsToStayOpen == other.secondsToStayOpen &&
      s.timeForValveOpen == other.timeForValveOpen &&
      s.shouldInvertScreen == other.shouldInvertScreen &&
      s.nfcStatus == other.nfcStatus;
  };

  auto serializeDeviceSettings = [](DeviceSettings &s) {
    char input[300];

    String tapIDs;
    String pulses;

    if (s.tapCount == 0)
    {
      tapIDs = ",";
      pulses = ",";
    }
    else
    {
      for (int ii = 0; ii < s.tapCount; ii++) {
        tapIDs += String(s.tapIds[ii]) + ",";
        pulses += String(s.pulsesPerGallon[ii]) + ",";
      }
    }

    snprintf(
      input,
      sizeof(input),
      "~%s~%s~%s~%d~%s~%d~%s~%s~%d~%d~%s~%d",
      s.deviceId.c_str(),
      s.authorizationToken.c_str(),
      tapIDs.c_str(),
      s.deviceStatus,
      pulses.c_str(),
      s.ledBrightness,
      s.isTOTPDisabled ? "true" : "false",
      s.isScreenDisabled ? "true" : "false",
      s.timeForValveOpen,
      s.secondsToStayOpen,
      s.shouldInvertScreen ? "true" : "false",
      s.nfcStatus
    );
    return String(input);
  };

  SECTION("zero tap input string") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    DeviceSettings s;
    s.authorizationToken = "totpKey";
    s.deviceId = "deviceID";
    s.deviceStatus = 0;
    s.pulsesPerGallon = new uint[0];
    s.tapCount = 0;
    s.tapIds = new uint32_t[0];

    String input = serializeDeviceSettings(s);

    ServerLink link(&stateMock.get());
    When(Method(stateMock, Initialize)
      .Matching([&](DeviceSettings *ds) {return deviceSettingsComparer(s, *ds); })).AlwaysReturn();

    link.Initialize("", input.c_str());
    Verify(Method(stateMock, Initialize)).Once();
  }

  SECTION("single tap input string") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    DeviceSettings s;
    s.authorizationToken = "totpKey";
    s.deviceId = "deviceID";
    s.deviceStatus = 0;
    s.pulsesPerGallon = new uint[1]{ 123 };
    s.tapCount = 1;
    s.tapIds = new uint32_t[1]{ 456 };

    String input = serializeDeviceSettings(s);

    ServerLink link(&stateMock.get());
    When(Method(stateMock, Initialize)
      .Matching([&](DeviceSettings *ds) {return deviceSettingsComparer(s, *ds); })).AlwaysReturn();

    link.Initialize("", input.c_str());
    Verify(Method(stateMock, Initialize)).Once();
  }

  SECTION("2 tap input string") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    DeviceSettings s;
    s.authorizationToken = "totpKey";
    s.deviceId = "deviceID";
    s.deviceStatus = 0;
    s.pulsesPerGallon = new uint[2]{ 123, 1234444 };
    s.tapCount = 2;
    s.tapIds = new uint32_t[2]{ 456, 1211112 };

    String input = serializeDeviceSettings(s);

    ServerLink link(&stateMock.get());
    When(Method(stateMock, Initialize)
      .Matching([&](DeviceSettings *ds) {return deviceSettingsComparer(s, *ds); })).AlwaysReturn();

    link.Initialize("", input.c_str());
    Verify(Method(stateMock, Initialize)).Once();
  }

  SECTION("10 tap input string") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    DeviceSettings s;
    s.authorizationToken = "totpKey";
    s.deviceId = "deviceID";
    s.deviceStatus = 0;
    s.pulsesPerGallon = new uint[10]{ 111,222,333,444,555,6666,7777,8888,999,10000 };
    s.tapCount = 10;
    s.tapIds = new uint32_t[10]{ 1,2,3,4,5,6,7,8,9,10 };

    String input = serializeDeviceSettings(s);

    ServerLink link(&stateMock.get());
    When(Method(stateMock, Initialize)
      .Matching([&](DeviceSettings *ds) {return deviceSettingsComparer(s, *ds); })).AlwaysReturn();

    link.Initialize("", input.c_str());
    Verify(Method(stateMock, Initialize)).Once();
  }

  SECTION("Device Status Active") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    DeviceSettings s;
    s.authorizationToken = "totpKey";
    s.deviceId = "deviceID";
    s.deviceStatus = DeviceStatus::ACTIVE;
    s.pulsesPerGallon = new uint[1]{ 1 };
    s.tapCount = 1;
    s.tapIds = new uint32_t[1]{ 1 };

    String input = serializeDeviceSettings(s);

    ServerLink link(&stateMock.get());
    When(Method(stateMock, Initialize)
      .Matching([&](DeviceSettings *ds) {return deviceSettingsComparer(s, *ds); })).AlwaysReturn();

    link.Initialize("", input.c_str());
    Verify(Method(stateMock, Initialize)).Once();
  }

  SECTION("Device Status Inactive") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    DeviceSettings s;
    s.authorizationToken = "totpKey";
    s.deviceId = "deviceID";
    s.deviceStatus = DeviceStatus::INACTIVE;
    s.pulsesPerGallon = new uint[1]{ 1 };
    s.tapCount = 1;
    s.tapIds = new uint32_t[1]{ 1 };

    String input = serializeDeviceSettings(s);

    ServerLink link(&stateMock.get());
    When(Method(stateMock, Initialize)
      .Matching([&](DeviceSettings *ds) {return deviceSettingsComparer(s, *ds); })).AlwaysReturn();

    link.Initialize("", input.c_str());
    Verify(Method(stateMock, Initialize)).Once();
  }

  SECTION("Device Status Cleaning") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    DeviceSettings s;
    s.authorizationToken = "totpKey";
    s.deviceId = "deviceID";
    s.deviceStatus = DeviceStatus::CLEANING;
    s.pulsesPerGallon = new uint[1]{ 1 };
    s.tapCount = 1;
    s.tapIds = new uint32_t[1]{ 1 };

    String input = serializeDeviceSettings(s);

    ServerLink link(&stateMock.get());
    When(Method(stateMock, Initialize)
      .Matching([&](DeviceSettings *ds) {return deviceSettingsComparer(s, *ds); })).AlwaysReturn();

    link.Initialize("", input.c_str());
    Verify(Method(stateMock, Initialize)).Once();
  }

  SECTION("Device Status Unlocked") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    DeviceSettings s;
    s.authorizationToken = "totpKey";
    s.deviceId = "deviceID";
    s.deviceStatus = DeviceStatus::UNLOCKED;
    s.pulsesPerGallon = new uint[1]{ 1 };
    s.tapCount = 1;
    s.tapIds = new uint32_t[1]{ 1 };

    String input = serializeDeviceSettings(s);

    ServerLink link(&stateMock.get());
    When(Method(stateMock, Initialize)
      .Matching([&](DeviceSettings *ds) {return deviceSettingsComparer(s, *ds); })).AlwaysReturn();

    link.Initialize("", input.c_str());
    Verify(Method(stateMock, Initialize)).Once();
  }


  SECTION("New Settings") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    DeviceSettings s;
    s.authorizationToken = "totpKey";
    s.deviceId = "deviceID";
    s.deviceStatus = DeviceStatus::UNLOCKED;
    s.pulsesPerGallon = new uint[1]{ 1 };
    s.tapCount = 1;
    s.tapIds = new uint32_t[1]{ 1 };
    s.isScreenDisabled = true;
    s.isTOTPDisabled = true;
    s.ledBrightness = 20;
    s.secondsToStayOpen = 3000;
    s.timeForValveOpen = 10;
    s.shouldInvertScreen = true;
    s.nfcStatus = 2;

    String input = serializeDeviceSettings(s);

    ServerLink link(&stateMock.get());
    When(Method(stateMock, Initialize)
      .Matching([&](DeviceSettings *ds) {return deviceSettingsComparer(s, *ds); })).AlwaysReturn();

    link.Initialize("", input.c_str());
    Verify(Method(stateMock, Initialize)).Once();
  }
}

TEST_CASE("ServerLink::Pour", "[ServerLink]") {
  SECTION("Fail for empty string") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    String input = "";

    ServerLink link(&stateMock.get());
    
    link.PourResponse("", input.c_str());
    Verify(Method(stateMock, StartPour)).Never();
  }

  SECTION("Succeeds for result where token is missing tilde") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    String input = "token";

    ServerLink link(&stateMock.get());
    When(Method(stateMock, StartPour)
      .Matching([&](
        String ds, 
        int constraintCount, 
        TapConstraint *constraints
      ) {return input == ds && constraintCount == 0 && constraints == NULL; }))
      .AlwaysReturn();

    link.PourResponse("", input.c_str());
  }

  SECTION("Fails for result where there are zero constraints") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    String input = "token~0~";

    ServerLink link(&stateMock.get());
    When(Method(stateMock, StartPour)
      .Matching([&](
        String ds,
        int constraintCount,
        TapConstraint *constraints
        ) {return String("token") == ds && constraintCount == 0 && constraints == NULL; }))
      .AlwaysReturn();
    link.PourResponse("", input.c_str());
  }

  SECTION("Succeeds for result where there is one constraint") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    String input = "token~1~1,2,100";

    ServerLink link(&stateMock.get());
    When(Method(stateMock, StartPour)
      .Matching([&](
        String ds,
        int constraintCount,
        TapConstraint *constraints
        ) {
          return String("token") == ds &&
            constraintCount == 1 &&
            constraints[0].tapIndex == 1 &&
            constraints[0].type == 2 &&
            constraints[0].pulses == 100;
          }
      ))
      .AlwaysReturn();

    link.PourResponse("", input.c_str());
  }

  SECTION("Succeeds for result where there is two constraints") {
    fakeit::Mock<KegeratorStateMachine> stateMock;
    String input = "token~2~1,2,100~4,1,300";

    ServerLink link(&stateMock.get());
    When(Method(stateMock, StartPour)
      .Matching([&](
        String ds,
        int constraintCount,
        TapConstraint *constraints
        ) {
          return String("token") == ds &&
            constraintCount == 2 &&
            constraints[0].tapIndex == 1 &&
            constraints[0].type == 2 &&
            constraints[0].pulses == 100 &&
            constraints[1].tapIndex == 4 &&
            constraints[1].type == 1 &&
            constraints[1].pulses == 300;
        }
      ))
      .AlwaysReturn();

    link.PourResponse("", input.c_str());
  }
}