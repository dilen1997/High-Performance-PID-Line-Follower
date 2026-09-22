#include <cstdlib>
#include <iostream>
#include "Control.h"
using namespace LineFollower;
#define CHECK(x) do { if (!(x)) { std::cerr << "Failed line " << __LINE__ << ": " << #x << '\n'; return 1; } } while (0)
int main() {
    Controller c;
    auto v = c.update(2000, false, false, 0);
    CHECK(v.left == 0 && v.right == 0);
    v = c.update(2000, true, false, 1);
    CHECK(v.left == kBaseSpeed && v.right == kBaseSpeed);
    v = c.update(0, true, false, 2);
    CHECK(v.left < v.right && v.left >= -kMaxSpeed && v.right <= kMaxSpeed);
    v = c.update(0, false, false, 3);
    CHECK(v.left == -kRecoverySpeed && v.right == kRecoverySpeed);
    v = c.update(0, false, false, 3 + kRecoveryTimeoutMs);
    CHECK(v.left == 0 && v.right == 0);
    v = c.update(2000, true, false, 1000);
    CHECK(v.left == kBaseSpeed && v.right == kBaseSpeed); // no derivative kick
    for (int i = 0; i < 100000; ++i) c.update(0, true, false, 1001 + i);
    v = c.update(0, true, true, 200000);
    CHECK(v.left >= -kMaxSpeed && v.right <= kMaxSpeed);
    Controller right;
    right.update(4000, true, false, 0);
    v = right.update(4000, false, false, UINT32_MAX - 100);
    CHECK(v.left == kRecoverySpeed && v.right == -kRecoverySpeed);
    v = right.update(4000, false, false, 700); // millis rollover
    CHECK(v.left == 0 && v.right == 0);
    std::cout << "Controller regressions passed.\n";
}
