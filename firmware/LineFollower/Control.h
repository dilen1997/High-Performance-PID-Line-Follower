#ifndef LINE_FOLLOWER_CONTROL_H
#define LINE_FOLLOWER_CONTROL_H
#include <stdint.h>

namespace LineFollower {
constexpr uint16_t kCenter = 2000;
constexpr uint16_t kBlackThreshold = 500;  // Calibrated QTR scale: 0..1000.
constexpr int kBaseSpeed = 190;
constexpr int kMaxSpeed = 210;
constexpr int kRecoverySpeed = 65;
constexpr uint32_t kRecoveryTimeoutMs = 750;
constexpr float kKp = 0.363f;
constexpr float kKi = 0.0f;  // PD by default; bounded integral available for tuning.
constexpr float kKd = 0.677f;
constexpr int32_t kIntegralLimit = 10000;

struct MotorCommand { int left; int right; };
inline int clampSpeed(int value) {
    return value < -kMaxSpeed ? -kMaxSpeed : value > kMaxSpeed ? kMaxSpeed : value;
}

class Controller {
public:
    MotorCommand update(uint16_t position, bool onLine, bool allBlack, uint32_t now) {
        if (!onLine) {
            if (!lost_) { lost_ = true; lostAt_ = now; }
            integral_ = 0;
            if (lastDirection_ == 0 || uint32_t(now - lostAt_) >= kRecoveryTimeoutMs)
                return {0, 0};
            return {-lastDirection_ * kRecoverySpeed, lastDirection_ * kRecoverySpeed};
        }
        const int error = int(kCenter) - int(allBlack ? lastPosition_ : position);
        const int derivative = (!seen_ || lost_) ? 0 : error - previousError_;
        seen_ = true;
        lost_ = false;
        if (!allBlack) {
            lastPosition_ = position;
            if (error != 0) lastDirection_ = error > 0 ? 1 : -1;
        }
        if (kKi != 0.0f) {
            integral_ += error;
            if (integral_ > kIntegralLimit) integral_ = kIntegralLimit;
            if (integral_ < -kIntegralLimit) integral_ = -kIntegralLimit;
        } else integral_ = 0;
        previousError_ = error;
        const int correction = int(kKp * error + kKi * integral_ + kKd * derivative);
        return {clampSpeed(kBaseSpeed - correction), clampSpeed(kBaseSpeed + correction)};
    }
private:
    int previousError_ = 0;
    int lastDirection_ = 0;
    int32_t integral_ = 0;
    uint16_t lastPosition_ = kCenter;
    uint32_t lostAt_ = 0;
    bool lost_ = false;
    bool seen_ = false;
};
}
#endif
