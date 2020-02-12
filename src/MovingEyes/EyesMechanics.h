#pragma once

#include <PCA9685.h>

namespace EyeMech
{

struct EyesMechanics;

//--------------------------------------------------------------------------------------------------

/**
 * Structure for eyes actuation (elevation, bearing, lids).
 */

struct EyesActuation
{
    int8_t elevation{ 0 };
    int8_t bearing{ 0 };

    struct Lid
    {
        int8_t upper{ 0 };
        int8_t lower{ 0 };
    };

    struct
    {
        Lid lid;
    } left, right;
};

//--------------------------------------------------------------------------------------------------

struct RawActuation : public EyesActuation
{
    friend EyesMechanics;

private:
    static constexpr uint8_t CHANNELS_COUNT{ 12 };
    uint16_t channels_pwm[CHANNELS_COUNT]{ 0 };
};

//--------------------------------------------------------------------------------------------------

struct EyesActuationHelper
{
    static void print(const EyesActuation &o, const String &prefix = "EyesActuationHelper::Print: ");
    static void println(const EyesActuation &o, const String &prefix = "EyesActuationHelper::Print: ");
};

//--------------------------------------------------------------------------------------------------

struct RawActuationHelper
{
    static void print(const RawActuation &o, const String &prefix);
    static void println(const RawActuation &o, const String &prefix);
};

//--------------------------------------------------------------------------------------------------

struct EyesMechanics
{
    explicit EyesMechanics(PCA9685_ServoEvaluator &servo_evaluator);
    virtual ~EyesMechanics() = default;

    virtual void setup();
    virtual void process();

protected:
    PCA9685 controller{ Wire, PCA9685_PhaseBalancer_Weaved };
    PCA9685_ServoEvaluator &servo_evaluator;
    RawActuation raw_actuation_values;
};

} // namespace EyeMech
