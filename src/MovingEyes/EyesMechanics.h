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
    static void print(const EyesActuation &o);
};

//--------------------------------------------------------------------------------------------------

struct RawActuationHelper
{
    static void print(const RawActuation &o);
};

//--------------------------------------------------------------------------------------------------

struct EyesMechanics
{
    virtual ~EyesMechanics() = default;

    void setup();
    virtual void process();

protected:
    PCA9685 controller{ Wire, PCA9685_PhaseBalancer_Weaved };
    PCA9685_ServoEvaluator servo_evaluator; // (128,324,526);
    RawActuation raw_actuation_values;
};

} // namespace EyeMech
