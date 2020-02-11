#include "EyesMechanics.h"

namespace EyeMech
{

//--------------------------------------------------------------------------------------------------

void EyesMechanics::setup()
{
    Wire.begin();
    Wire.setClock(400000);

    controller.resetDevices();
    controller.init(0x0);
    controller.setPWMFrequency(50);
}

//--------------------------------------------------------------------------------------------------

void EyesMechanics::process()
{
    // Serial.println("EyesMechanics::process");
    // RawActuationHelper::print(raw_actuation_values);

    raw_actuation_values.channels_pwm[0] = servo_evaluator.pwmForAngle(raw_actuation_values.elevation);
    raw_actuation_values.channels_pwm[1] = servo_evaluator.pwmForAngle(raw_actuation_values.bearing);
    raw_actuation_values.channels_pwm[2] = servo_evaluator.pwmForAngle(raw_actuation_values.left.lid.upper);
    raw_actuation_values.channels_pwm[3] = servo_evaluator.pwmForAngle(raw_actuation_values.left.lid.lower);
    raw_actuation_values.channels_pwm[4] =
    servo_evaluator.pwmForAngle(raw_actuation_values.right.lid.upper);
    raw_actuation_values.channels_pwm[5] =
    servo_evaluator.pwmForAngle(raw_actuation_values.right.lid.lower);

    controller.setChannelsPWM(0, PCA9685_CHANNEL_COUNT, raw_actuation_values.channels_pwm);
}


//--------------------------------------------------------------------------------------------------

void EyesActuationHelper::print(const EyesActuation &o)
{
    Serial.print("EyesActuationHelper::print");

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

    Serial.println();
}

//--------------------------------------------------------------------------------------------------

void RawActuationHelper::print(const RawActuation &o)
{
    Serial.print("RawActuationHelper::print");

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

    Serial.println();
}

} // namespace EyeMech