#pragma once

#include <vector>
#include <math.h>


#ifndef M_PI
	#define M_PI 3.141592653589793f
#endif

#ifndef M_PI_X_2
	#define M_PI_X_2 (float)M_PI * 2.0f
#endif


/**
 * @addtogroup iwtween IwTween API Reference
 * @{
 */

namespace IwTween {

// Easing functions
// These sit in a namespace called "Ease" so can be copy/pasted
// from OpenQuick's QTween.h

/**
 * The 'ease' namespace is used for functions that can modify how tweens are applied.
 * Each easing function is of the form y = f(x), where x = (0..1) and y = (0..1).
 *
 * An easing function also takes a 'value' parameter which can be used to modify the easing function;
 * for example, to specify a multiplication factor or exponential power. Note that although this
 * can be given a default input value in the header definition of the function, the way the function
 * is called (from a function pointer) means this is not respected. Therefore, any user-defined easing
 * functions should assume an input easing value of 0.0f means "no input", and set the value
 * accordingly.
 */

	

namespace Ease {
    /**
     * Easing function providing a linear curve (straight line).
     * The value field is not used.
     */
    float linear(float time, float value = 1.0f);

    /**
     * Easing function providing a straight line, with fixed value of 1.
     * When combined with a mode of 'mirror', this can be used to provide a square wave.
     * The value field is not used.
     */
    float one(float time, float value = 1.0f);

    /**
     * Easing function providing a straight line, with fixed value of 0.
     * When combined with a mode of 'mirror', this can be used to provide a square wave.
     * The value field is not used.
     */
    float zero(float time, float value = 1.0f);

    /**
     * Easing function providing a curve based on raising the time value to a specified power.
     * The value field specifies the power, and defaults to 2 (i.e. a squared function).
     * The easing is "soft" at the start of the tween, and "hard" at the end.
     */
    float powIn(float time, float power = 2.0f);

    /**
     * Easing function providing a curve based on raising the time value to a specified power.
     * The value field specifies the power, and defaults to 2 (i.e. a squared function).
     * The easing is "hard" at the start of the tween, and "soft" at the end.
     */
    float powOut(float time, float power = 2.0f);

    /**
     * Easing function providing a curve based on raising the time value to a specified power.
     * The value field specifies the power, and defaults to 2 (i.e. a squared function).
     * The easing is "soft" at both the start and end of the tween.
     */
    float powInOut(float time, float power = 2.0f);

    /**
     * Easing function providing a curve based on raising a value to the power of 10 * time.
     * The value defaults to 2.
     * The easing is "soft" at the start of the tween, and "hard" at the end.
     */
    float expIn(float time, float value = 2.0f);

    /**
     * Easing function providing a curve based on raising a value to the power of 10 * time.
     * The value defaults to 2.
     * The easing is "hard" at the start of the tween, and "soft" at the end.
     */
    float expOut(float time, float value = 2.0f);

    /**
     * Easing function providing a curve based on raising a value to the power of 10 * time.
     * The value defaults to 2.
     * The easing is "soft" at both the start and end of the tween.
     */
    float expInOut(float time, float value = 2.0f);

    /**
     * Easing function providing a sine curve raised to a specified power.
     * The value field specifies the power, and defaults to 1 (i.e. a standard sine function).
     * The easing is "soft" at the start of the tween, and "hard" at the end.
     */
    float sineIn(float time, float power = 1.0f);

    /**
     * Easing function providing a sine curve raised to a specified power.
     * The value field specifies the power, and defaults to 1 (i.e. a standard sine function).
     * The easing is "hard" at the start of the tween, and "soft" at the end.
     */
    float sineOut(float time, float power = 1.0f);

    /**
     * Easing function providing a sine curve raised to a specified power.
     * The value field specifies the power, and defaults to 1 (i.e. a standard sine function).
     * The easing is "soft" at the both the start and end of the tween.
     */
    float sineInOut(float time, float power = 1.0f);

    /**
     * Easing function providing a damped oscillation curve.
     * The value field specifies the period of the oscillation wave, and defaults to 2 * PI.
     * The easing is "soft" at the start of the tween, and "hard" at the end.
     */
    float elasticIn(float time, float period = (float)M_PI * 2.0f);

    /**
     * Easing function providing a damped oscillation curve.
     * The value field specifies the period of the oscillation wave, and defaults to 2 * PI.
     * The easing is "hard" at the start of the tween, and "soft" at the end.
     */
    float elasticOut(float time, float period = (float)M_PI * 2.0f);

    /**
     * Easing function providing a damped oscillation curve.
     * The value field specifies the period of the oscillation wave, and defaults to 2 * PI.
     * The easing is "soft" at both the start and end of the tween.
     */
    float elasticInOut(float time, float period = (float)M_PI * 2.0f);

    /**
     * Easing function providing a damped bouncing curve.
     * The value field is not used.
     * The easing is "soft" at the start of the tween, and "hard" at the end.
     */
    float bounceIn(float time, float value = 1.0f);

    /**
     * Easing function providing a damped bouncing curve.
     * The value field is not used.
     * The easing is "hard" at the start of the tween, and "soft" at the end.
     */
    float bounceOut(float time, float value = 1.0f);

    /**
     * Easing function providing a damped bouncing curve.
     * The value field is not used.
     * The easing is "soft" at both the start and end of the tween.
     */
    float bounceInOut(float time, float value = 1.0f);

    /**
     * Easing function providing an "overshoot" and spring-back curve.
     * The value field is not used.
     * The easing is "soft" at the start of the tween, and "hard" at the end.
     */
    float backIn(float time, float value = 1.0f);

    /**
     * Easing function providing an "overshoot" and spring-back curve.
     * The value field is not used.
     * The easing is "hard" at the start of the tween, and "soft" at the end.
     */
    float backOut(float time, float value = 1.0f);

    /**
     * Easing function providing an "overshoot" and spring-back curve.
     * The value field is not used.
     * The easing is "soft" at both the start and end of the tween.
     */
    float backInOut(float time, float value = 1.0f);
}; // namespace Ease

/**
 * All system-defined types to be used within the CTweenManager::Tween() input array.
 * @see CTweenManager::Tween
 * @par Required Header Files
 * IwTween.h
 */
enum Type
{
    /**
     * Tween a bool value. Must be followed by a pointer to a bool variable, then the
     * target value (true or false).
     */
    eBool = 0x100,

    /**
     * Tween a (signed) byte value. Must be followed by a pointer to a (signed) byte variable, then the
     * target value [-128..127].
     */
    eByte,

    /**
     * Tween an unsigned byte value. Must be followed by a pointer to an unsigned byte variable, then the
     * target value [0..255].
     */
    eUByte,

    /**
     * Tween a (signed) short value. Must be followed by a pointer to a (signed) short variable, then the
     * target value [-32768..32767].
     */
    eShort,

    /**
     * Tween an unsigned short value. Must be followed by a pointer to an unsigned short variable, then the
     * target value [0..65535].
     */
    eUShort,

    /**
     * Tween a (signed) int value. Must be followed by a pointer to a (signed) int variable, then the
     * target value.
     */
    eInt,

    /**
     * Tween an unsigned int value. Must be followed by a pointer to an unsigned int variable, then the
     * target value.
     */
    eUInt,

    /**
     * Tween a float value. Must be followed by a pointer to a float variable, then the
     * target value.
     */
    eFloat,

    /**
     * By default, a tweened variable will interpolate FROM the current value TO the specified value.
     * However if we specify 'FROM', then the tweened variable will interpolate FROM the specified value
     * to the current value (a snapshot of which is stored when the interpolation begins).
     */
    eFrom,

    /**
     * Specifies that a tween is complete when the target value is reached. This is the default;
     * the 'CLAMP' value is used internally to differentiate from the alternative 'REPEAT' and
     * 'MIRROR' modes.
     *
     * The interpolation value moves from 0 (after any 'DELAY') to 1 (after a further 'time'), whereby
     * any 'ONCOMPLETE' callback is fired. The function IsComplete() will return true only after this
     * point. The interpolation value stays at 1 thereafter.
     */
    eClamp,

    /**
     * Specifies that a tween cycles indefinitely until cancelled.
     *
     * The interpolation value moves from 0 (after any 'DELAY') to 1 (after a further
     * 'time'), whereby any 'ONCOMPLETE' callback is fired. The interpolation value then starts
     * again at 0, and moves to 1 after a further 'time'; i.e. it continually cycles with a
     * period of 'time'. The 'ONCOMPLETE' callback fires after each period. The function
     * IsComplete() NEVER returns true.
     */
    eRepeat,

    /**
     * Specifies that a tween cycles indefinitely until cancelled.
     *
     * Like 'REPEAT' except that the interpolation value alternately ramps up to 1
     *  and back down to 0 for each pair of cycles (as opposed to ramping to 1 and then
     *  immediately jumping back to 0). For each odd-numbered cycle, the interpolation value
     *  is (1-r), where r is the value that would be generated from the corresponding 'REPEAT'
     *  mode. The 'ONCOMPLETE' callback fires after each period. The function IsComplete()
     *  NEVER returns true.
     */
    eMirror,

    /**
     * Any delay in seconds before the interpolation begins (the default value is 0.0f).
     * If this value is greater than 0, then this period must elapse before any properties
     * start tweening. The 'ONSTART' callback is called only when this period has elapsed.
     */
    eDelay,

    /**
     * Whether or not to interpret the specified values as absolute, or relative to the
     * current values. The default is false, meaning absolute values. If true, the 'current'
     * values are those at the point at which any 'DELAY' period has elapsed, i.e. when the
     * actual interpolation is about to begin.
     */

    eDelta,

    /**
     * The tween easing function (default value is Ease.linear). Easing functions
     * allow the properties to be animated in a non-linear fashion, for example to slow down at
     * the start or end of the animation period. A full list of easing functions is provided above.
     * Apps can provide their own custom easing functions; are of type float myFunction(float input, float value)
     * where both the input and return values are in the range [0..1]. The 'value' input is that
     * specified by EASINGVALUE below, and can be used to modify the function behaviour.
     */
    eEasing,

    /**
     * The tween easing value. Depending on the easing function being used,
     * this value can affect the 'strength' of the function, for example the degree to which the
     * animation speeds up or slows down at the start or end of the period. The default value depends
     * on the easing function, and must be explicitly implemented at the start of the function by
     * checking against a value of 0.0f (i.e. the function implementation should assume that a
     * value of 0.0f means 'use default value', and set the value accordingly).
     */
    eEasingValue,

    /**
     * A function to be called before the interpolation begins. The function is called
     * only once any 'DELAY' period has elapsed.
     */
//    eOnStart,

    /**
     * A function to be called after the interpolation completes. The function is called
     * at the end of each tween 'cycle': if the mode is 'CLAMP', there is only a single cycle,
     * otherwise cycles repeat indefinitely with the period specified by 'time'.
     */
//    eOnComplete,

    /**
     * Terminator for the CTweenManager::Tween() input array. This MUST be provided as the final
     * entry in the list, otherwise the Tween() function will almost certainly throw an assertion.
     */
    eEnd,
};

//------------------------------------------------------------------------------
// CTween
//------------------------------------------------------------------------------
class Tween;
class TweenRepository;


class TweenListener {
public:
    virtual void OnTweenStarted(Tween* tween) {};
    virtual void OnTweenEnded(Tween* tween) {};
};

/**
 * A tween object. A call to CTweenManager::Tween() creates a CTween object and returns
 * a pointer to it. This pointer need not be stored, as CTweenManager::Update() typically
 * does all the work to manage the interpolation and the lifecycle of the CTween object.
 * However, if the pointer is stored it can be used to get/set various properties of
 * the tween, and to explicitly cancel the tween (which destroys the object).
 *
 * You should never delete a Tween pointer; the object lifecycle is managed by the
 * CTweenManager;
 */
class Tween
{
public:
    /**
     * An easing function, as specified by the EASING value in the input array.
     */
    typedef float (*EasingFn)(float, float);

    /**
     * An callback function, as specified by the ONSTART and ONCOMPLETE values in the input array.
     */
    typedef void (*Callback)(Tween* pTween);

private:
    // A target value and type
    class ValueType
    {
    public:
        ValueType() {};
        ValueType(Type t, void* v, int i)
        {
            m_Type = t;
            m_Value = v;
            m_ValueInput = i;
        };
        void SetFromCurrentValue(void* pStart, void* pInput, bool isDelta);
        void SetFromInterp(float interp);

        Type    m_Type;         // enumerated type of target value
        void*   m_Value;        // pointer to target value
        int     m_ValueInput;   // (cast to type): value as specified by tween caller
        int     m_ValueStart;   // (cast to type): value as found when interpolation actually starts
    };

    // A tween spec
    class Spec
    {
    public:
        Spec();

        float       m_Time;         // duration of tween cycle
        float       m_Delay;        // delay before animation starts
        bool        m_Delta;        // true if the specified values should be treated as deltas from current values
        Type        m_Mode;         // CLAMP, REPEAT or MIRROR
        EasingFn    m_Easing;       // interpolation function
        float       m_EasingValue;  // default is nil, so C++ function can take default C++ input
//        Callback    m_OnStart;      // callback for when animation actually starts
//        Callback    m_OnComplete;   // callback for when each cycle completes
//		CTweenListener* m_Listener;
    };

    // Private constructor/destructor; these are managed by the CTweenManager
	Tween(Spec spec) { m_Spec = spec; };
public:
    ~Tween() {};

public:
    /**
     * Get the amount of time elapsed on the tween.
     * This begins increasing right after the tween is created.
     * @return The elapsed time, in seconds.
     */
    float GetElapsedTime() { return m_ElapsedTime; }

    /**
     * Returns true only if the tween has started animating values,
     * i.e. after any 'DELAY' has been passed.
     * @return True only after any 'DELAY' has been passed.
     */
    bool IsAnimating() { return m_InProgress; }

    /**
     * Return the number of cycles that have elapsed on the tween.
     * If the mode is 'CLAMP', this is 0 until the tween completes, then 1.
     * If 'REPEAT' or 'MIRROR', this increases by 1 for each completed cycle.
     * @return The number of completed cycles.
     */
    int GetNumCycles() { return m_NumCycles; }

    /**
     * Cancel the tween. The interpolating variables will remain at their current values
     * and the Tween object will be destroyed.
     */
    void Cancel();

    /**
     * Pause the tween. Whilst a tween is paused no time will elapse, no values will be
     * updated and no callbacks fired.
     * @see Restart
     */
    void Pause() { m_IsPaused = true; }

    /**
     * Restart the tween. Whether or not the tween was paused, this sets it back to normal
     * operation.
     * @see Pause
     */
    void Restart() { m_IsPaused = false; }

private:
    // Update the tween. Returns true only if the tween is complete and can be cancelled.
    bool Tick(float dt);

    Spec    m_Spec;         // values passed at Tween set-up
    float   m_ElapsedTime;  // time elapsed, in seconds
    bool    m_IsTo;         // true if "to", else "from"
    bool    m_IsComplete;   // true only if CLAMP and interpolation complete
    bool    m_InProgress;   // true only if DELAY has been passed
    bool    m_IsPaused;     // true only if tween is paused
    int     m_NumCycles;    // number of cycles completed

    std::vector<ValueType> m_ValueTypes; // the interpolating values
    TweenRepository* m_List;  // pointer to owner list

    friend class TweenRepository;
};

//------------------------------------------------------------------------------
// CTweenManager
//------------------------------------------------------------------------------
/**
 * The manager for a list of Tweens. Tweens are created and destroyed only through this
 * class. It is assumed that the lifecycle of this object corresponds with the lifecycle
 * of any variables being interpolated. Typically, an app creates various objects each of which
 * encapsulates a CTweenManager. The created Tweens then interpolate other variables on the object.
 *
 * Examples:
 *
 * g_Tweener = new IwTween::CTweenManager;
 *
 * // Tween float from [0..1]
 * float valFloatTo = 0.0f;
 * g_Tweener->Tween(2.0f,
 *     FLOAT, &valFloatTo, 1.0f,
 *     END);
 *
 * // Tween float from [1..0] after delay of 0.5 sec. Throw a callback when we start and complete
 * float valFloatFromDelay = 0.0f;
 * void onStart(CTween* pTween) {}
 * void onComplete(CTween* pTween) {}
 * g_Tweener->Tween(1.0f,
 *     FROM,
 *     DELAY, 1.0f,
 *     ONSTART, onStart,
 *     ONCOMPLETE, onComplete,
 *     FLOAT, &valFloatFromDelay, 0.5f,
 *     END);
 *
 * // Tween byte value from [0..255] using sin easing
 * char valCharMirrorSin = 0;
 * g_Tweener->Tween(1.0f,
 *     MIRROR,
 *     EASING, Ease::sinInOut,
 *     BYTE, &valCharMirrorSin, 1.0f,
 *     END);
 *
 * // Tween bool value between true and false, using mirrored Ease::zero to create 'square wave'
 * bool valBoolFlip = false;
 * g_Tweener->Tween(0.5f,
 *     MIRROR,
 *     EASING, Ease::zero,
 *     BOOL, &valBoolFlip, true,
 *     END);
 */
class TweenRepository
{
public:
    TweenRepository() {};
    ~TweenRepository() {};
	void OnStarted(Tween* tween);
	void OnEnded(Tween* tween);

    /**
     * Create a new tween. The first argument is the duration of the tween, in seconds.
     * A variable argument list then follows, which contains directives of type CTween::Type
     * together with appropriate following values. The argument list must be terminated with
     * the directive CTween::END.
     * @param time The duration of the tween, in seconds.
     * @param ... The variable argument list describing the tween.
     * @return Pointer to the created CTween object.
     */
    Tween* Run(double time, ...);

    /**
     * Update all tweens, specifying the time to elapse (in seconds). Any tweens that have completed
     * are destroyed.
     * @param dt The time to elapse on the tween, in seconds.
     */
    void Tick(float dt);

    /**
     * Return the number of tweens in the manager.
     * @return The number of tweens in the manager.
     */
    int GetNumTweens() { return m_Tweens.size(); }

    /**
     * Delete all tweens.
     */
    void Clear() { m_Tweens.clear(); }

	void AddListener(TweenListener* listener);
	void RemoveListener(TweenListener* listener);
	void RemoveAllListener();


private:
    std::vector<Tween> m_Tweens; // the tweens
	TArray<TweenListener*> m_Listener;
	


    friend class Tween;
};

} // namespace IwTween


