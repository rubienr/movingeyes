#include "ServoTier.h"

#include <HardwareSerial.h>
#include <Wire.h>

#include "../configuration.h"

namespace eyes {
namespace servo {

//--------------------------------------------------------------------------------------------------

ServoTier::ServoTier(TwoWire &wire, PCA9685_ServoEvaluator &servo_evaluator, PCA9685_PhaseBalancer balancer)
: wire{ wire }, controller{ this->wire, balancer }, servo_evaluator{ servo_evaluator } {}

//--------------------------------------------------------------------------------------------------

void ServoTier::setup() {
    wire.begin();
    wire.setClock(400000);

    controller.resetDevices();
    controller.init(0x0);
    controller.setPWMFrequency(50);
}

//--------------------------------------------------------------------------------------------------

void ServoTier::enableServos() { process(); }

//--------------------------------------------------------------------------------------------------

void ServoTier::disableServos() {
    static uint16_t idle_pwm[intern::RawActuation::CHANNELS_COUNT]{ 0 };
    controller.setChannelsPWM(0, intern::RawActuation::CHANNELS_COUNT, idle_pwm);
}

//--------------------------------------------------------------------------------------------------

void ServoTier::process() {
    raw_actuation_values.channels_pwm[0] =
    servo_evaluator.pwmForAngle(raw_actuation_values.bearing.biasedValue());
    raw_actuation_values.channels_pwm[1] =
    servo_evaluator.pwmForAngle(raw_actuation_values.elevation.biasedValue());
    raw_actuation_values.channels_pwm[2] =
    servo_evaluator.pwmForAngle(-raw_actuation_values.left.lid.upper.biasedValue());
    raw_actuation_values.channels_pwm[3] =
    servo_evaluator.pwmForAngle(-raw_actuation_values.left.lid.lower.biasedValue());
    raw_actuation_values.channels_pwm[4] =
    servo_evaluator.pwmForAngle(raw_actuation_values.right.lid.upper.biasedValue());
    raw_actuation_values.channels_pwm[5] =
    servo_evaluator.pwmForAngle(raw_actuation_values.right.lid.lower.biasedValue());

#if defined(DEBUG_SERVO_TIER)
    intern::RawActuationHelper::println(raw_actuation_values, "ServoTier::process: ");
#endif

    controller.setChannelsPWM(0, intern::RawActuation::CHANNELS_COUNT, raw_actuation_values.channels_pwm);
}

//--------------------------------------------------------------------------------------------------

void EyesActuationHelper::print(const EyesActuation &o, const String &prefix) {
    Serial.print(prefix);

    Serial.print(" B=");
    Serial.print(o.bearing);
    Serial.print(" E=");
    Serial.print(o.elevation);

    Serial.print(" left.lid.upper=");
    Serial.print(o.left.lid.upper);
    Serial.print(" left.lid.lower=");
    Serial.print(o.left.lid.lower);

    Serial.print(" right.lid.upper=");
    Serial.print(o.right.lid.upper);
    Serial.print(" right.lid.lower=");
    Serial.print(o.right.lid.lower);
}

//--------------------------------------------------------------------------------------------------

void EyesActuationHelper::println(const EyesActuation &o, const String &prefix) {
    print(o, prefix);
    Serial.println();
}


//--------------------------------------------------------------------------------------------------

void BiasedActuationHelper::print(const BiasedEyesActuation &o, const String &prefix) {
    Serial.print(prefix);

    Serial.print(" B={");
    Serial.print(o.bearing.value);
    Serial.print(", ");
    Serial.print(o.bearing.bias);
    Serial.print("}");

    Serial.print(" E={");
    Serial.print(o.elevation.value);
    Serial.print(", ");
    Serial.print(o.elevation.bias);
    Serial.print("}");

    Serial.print(" left.lid.upper={");
    Serial.print(o.left.lid.upper.value);
    Serial.print(", ");
    Serial.print(o.left.lid.upper.bias);
    Serial.print("}");

    Serial.print(" left.lid.lower={");
    Serial.print(o.left.lid.lower.value);
    Serial.print(", ");
    Serial.print(o.left.lid.lower.bias);
    Serial.print("}");

    Serial.print(" right.lid.upper={");
    Serial.print(o.right.lid.upper.value);
    Serial.print(", ");
    Serial.print(o.right.lid.upper.bias);
    Serial.print("}");

    Serial.print(" right.lid.lower={");
    Serial.print(o.right.lid.lower.value);
    Serial.print(", ");
    Serial.print(o.right.lid.lower.bias);
    Serial.print("}");
}

//--------------------------------------------------------------------------------------------------

void BiasedActuationHelper::println(const BiasedEyesActuation &o, const String &prefix) {
    print(o, prefix);
    Serial.println();
}


//--------------------------------------------------------------------------------------------------
namespace intern {

//--------------------------------------------------------------------------------------------------

void RawActuationHelper::print(const RawActuation &o, const String &prefix) {
    BiasedActuationHelper::print(o, prefix);


    Serial.print(" raw:values:{");
    for(uint8_t idx = 0; idx < RawActuation::CHANNELS_COUNT; ++idx) {
        Serial.print(" [");
        Serial.print(idx);
        Serial.print("]=");
        Serial.print(o.channels_pwm[idx]);
    }
    Serial.print(" }");
}

//--------------------------------------------------------------------------------------------------

void RawActuationHelper::println(const RawActuation &o, const String &prefix) {
    print(o, prefix);
    Serial.println();
}

} // namespace intern
} // namespace servo
} // namespace eyes