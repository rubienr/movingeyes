#pragma once

#include <PCA9685.h>
#include <WString.h>

class TwoWire;

namespace eyes {
namespace servo {

struct ServoTier;
namespace intern {
struct RawActuationHelper;
}

//--------------------------------------------------------------------------------------------------

/**
 * Structure for eyes actuation (elevation, bearing, lids).
 */

struct EyesActuation {
    struct Lid {
        int8_t upper{ 0 };
        int8_t lower{ 0 };
    };

    int8_t elevation{ 0 };
    int8_t bearing{ 0 };

    struct {
        Lid lid;
    } left, right;
};

//--------------------------------------------------------------------------------------------------

struct EyesActuationHelper {
    static void print(const EyesActuation &o, const String &prefix = "EyesActuationHelper::print: ");
    static void println(const EyesActuation &o, const String &prefix = "EyesActuationHelper::print: ");
};

//--------------------------------------------------------------------------------------------------

/**
 * Structure for internal eyes actuation (elevation, bearing, lids).
 */
struct BiasedEyesActuation {
    struct BiasedValue {
        int8_t value;
        int8_t bias;

        int8_t biasedValue() { return value + bias; }
    };

    struct Lid {
        BiasedValue upper{ 0, 0 };
        BiasedValue lower{ 0, 0 };
    };

    BiasedValue elevation{ 0, 0 };
    BiasedValue bearing{ 0, 0 };

    struct {
        Lid lid;
    } left, right;
};

//--------------------------------------------------------------------------------------------------

struct BiasedActuationHelper {
    static void print(const BiasedEyesActuation &o, const String &prefix = "BiasedEyesActuationHelper::print: ");
    static void println(const BiasedEyesActuation &o, const String &prefix = "BiasedEyesActuationHelper::print: ");
};

//--------------------------------------------------------------------------------------------------

namespace intern {

//--------------------------------------------------------------------------------------------------

struct RawActuation : public BiasedEyesActuation {
    friend RawActuationHelper;
    friend ServoTier;

private:
    static constexpr uint8_t CHANNELS_COUNT{ 12 };
    uint16_t channels_pwm[CHANNELS_COUNT]{ 0 };
};

//--------------------------------------------------------------------------------------------------

struct RawActuationHelper {
    static void print(const RawActuation &o, const String &prefix = "RawActuationHelper::print: ");
    static void println(const RawActuation &o, const String &prefix = "RawActuationHelper::print: ");
};

} // namespace intern

//--------------------------------------------------------------------------------------------------

/**
 * Implementation to interface with the eye mechanics.
 * Takes actuation values and projects them to the mechanics (move) without respecting any servo
 * limits or avoiding collisions.
 */
struct ServoTier {
    /**
     * @param wire I2C communication wire
     * @param servo_evaluator to trim the servo min/max and zero position
     * @param balancer
     */
    ServoTier(TwoWire &wire, PCA9685_ServoEvaluator &servo_evaluator, PCA9685_PhaseBalancer balancer);
    virtual ~ServoTier() = default;

    virtual void setup();
    virtual void process();

protected:
    TwoWire &wire;
    PCA9685 controller;
    // TODO
    PCA9685_ServoEvaluator &servo_evaluator;
    intern::RawActuation raw_actuation_values;
};

//--------------------------------------------------------------------------------------------------

PCA9685_ServoEvaluator servoEvaluatorDefault();

//--------------------------------------------------------------------------------------------------

PCA9685_ServoEvaluator servoEvaluatorMg90sMicroservo();

} // namespace servo
} // namespace eyes