#pragma once

// -------------------------------------------------------------------------------------------------

#include <MovingEyes/TranslationTier.h>
class PCA9685_ServoEvaluator;

namespace eyes {
namespace translation {

struct Limits;
struct CompensationAngles;

} // namespace translation
namespace servo {}
struct Constraints;
} // namespace eyes

namespace funduino {

struct ArduinoPinConfiguration;

} // namespace funduino
// -------------------------------------------------------------------------------------------------

namespace config {
PCA9685_ServoEvaluator getConfiguredServoEvaluator();
eyes::translation::Limits getConfiguredLimits();
eyes::translation::CompensationAngles getConfiguredCompensationAngles();
eyes::Constraints getConfiguredConstraints();
funduino::ArduinoPinConfiguration getConfiguredJoystickShieldPinout();
} // namespace config

// -------------------------------------------------------------------------------------------------
// ------------------------------------- SECTION OUTPUT --------------------------------------------
// -------------------------------------------------------------------------------------------------

#if defined(ARDUINO_AVR_LEONARDO)
#define SERIAL_BAUD_RATE 230400
#elif defined(ARDUINO_AVR_NANO)
#define SERIAL_BAUD_RATE 57600
#endif

/**
 * enable servo tier (servo channel PWM handling) serial debug output
 */
//#define DEBUG_SERVO_TIER

/**
 * Servo evaluator for all servo channels.
 * Units: PWM value
 */

#define SERVO_EVALUATOR_POSITION_MIN_PWM 102
#define SERVO_EVALUATOR_POSITION_MAX_PWM 512
#define SERVO_EVALUATOR_POSITION_CENTER_PWM \
    (((SERVO_EVALUATOR_POSITION_MAX_PWM - SERVO_EVALUATOR_POSITION_MIN_PWM) / 2) + SERVO_EVALUATOR_POSITION_MIN_PWM)

#define SERVO_EVALUATOR_POSITION_MIN_BIAS_PWM (-7)
#define SERVO_EVALUATOR_POSITION_MAX_BIAS_PWM (+18)
#define SERVO_EVALUATOR_POSITION_CENTER_BIAS_PWM (+5)

#define SERVO_EVALUATOR_CONFIG                                                            \
    {                                                                                     \
        (SERVO_EVALUATOR_POSITION_MIN_PWM + SERVO_EVALUATOR_POSITION_MIN_BIAS_PWM),       \
        (SERVO_EVALUATOR_POSITION_CENTER_PWM + SERVO_EVALUATOR_POSITION_CENTER_BIAS_PWM), \
        (SERVO_EVALUATOR_POSITION_MAX_PWM + SERVO_EVALUATOR_POSITION_MAX_BIAS_PWM)        \
    }

// -------------------------------------------------------------------------------------------------

/**
 * enable translation tier (degrees to PWM) serial debug output
 */
//#define DEBUG_TRANSLATION_TIER


/**
 * Physical limits for movements (lids, eye bearing/elevation).
 * Units: degree
 */
#define PHYSICAL_LIMITITS_BEARING_DEGREE \
    { -55 /* min */, 55 /* max */ }

#define PHYSICAL_LIMITITS_ELEVATION_DEGREE \
    { -55 /* min */, 50 /* max */ }

#define PHYSICAL_LIMITITS_LID_LOWER_DEGREE \
    { -63 /* min */, 75 /* max */ }

#define PHYSICAL_LIMITITS_LID_UPPER_DEGREE \
    { -80 /* min */, 63 /* max */ }

#define PHYSICAL_LIMITS_DEGREE            \
    { {                                   \
      /* lids left */                     \
      PHYSICAL_LIMITITS_LID_UPPER_DEGREE, \
      PHYSICAL_LIMITITS_LID_LOWER_DEGREE, \
      },                                  \
      {                                   \
      /* lids right */                    \
      PHYSICAL_LIMITITS_LID_UPPER_DEGREE, \
      PHYSICAL_LIMITITS_LID_LOWER_DEGREE, \
      } },                                \
    PHYSICAL_LIMITITS_BEARING_DEGREE, PHYSICAL_LIMITITS_ELEVATION_DEGREE

// -------------------------------------------------------------------------------------------------

/**
 * enable moving eyes tier (collision avoidance of mechanics) serial debug output
 */
//#define DEBUG_MOVING_EYES_TIER

// -------------------------------------------------------------------------------------------------
// ------------------------------------- SECTION INPUT ---------------------------------------------
// -------------------------------------------------------------------------------------------------
/**
 * JoyStick Shield V1.A:
 * https://www.aliexpress.com/item/33008086326.html?spm=a2g0o.productlist.0.0.7fe21468B21xQK&algo_pvid=36e81b6f-ad46-424e-a638-59b3e6fbfe01&algo_expid=36e81b6f-ad46-424e-a638-59b3e6fbfe01-0&btsid=0bb0622d16053784580961544eadaa&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_
 */

/**
 * enable joystick readings serial debug output
 */
//#define DEBUG_JOYSTICK_SHIELD

/**
 * Arduino Leonardo:
 * Set switch to 5V.
 * Shield button A/B pinJOYSTICK_SHIELD_PINS_NANOs are moved to D9/D10 to leave D2/D3 (SDA/SCL)
 * free. Needs removal of the respective pins (interrupt, unsolder) and reconnecting to D9/D10
 *
 * Arduino Nano:
 * Set switch to 5V.
 *
 * { { { Ad, Bd, Cd, Dd, Ed, Fd }, { Xa, Ya, Zd } } }
 * [A-FZ] ... digital input
 * [XY]   ... analog input
 */
#if defined(ARDUINO_AVR_LEONARDO)
#define JOYSTICK_SHIELD_PINS                     \
    {                                            \
        {                                        \
            { 9, 10, 4, 5, 6, 7 }, { A0, A1, 8 } \
        }                                        \
    }
#elif defined(ARDUINO_AVR_NANO)
#define JOYSTICK_SHIELD_PINS                    \
    {                                           \
        {                                       \
            { 2, 3, 4, 5, 6, 7 }, { A0, A1, 8 } \
        }                                       \
    }
#endif


// -------------------------------------------------------------------------------------------------
// ------------------------------------- SECTION GENERAL -------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * enable very high level serial debug output
 */
//#define DEBUG_MAIN