
#include "test_support.h"
#include "kinematics.hpp"

namespace TestKinematics {

class TestCollision {
private:
  energie_t _E;
  mass_t _m;
  velocity_t _v;
  momentum_t _p;
  si::Particle p1{};
  si::Particle p2{};

public:
  TestCollision() {
    _E = energie_t{500.0};
    _m = mass_t{10.0};
    _v = velocity_t{10.0};
    _p = momentum_t{100};
    p1 = si::Particle(si::Energie(_E), si::Mass(_m), si::Velocity(_v), si::Momentum(_p));
    p2 = si::Particle(si::Energie(_E), si::Mass(_m), si::Velocity(_v), si::Momentum(_p));
  }

  void test_1D_collision() {
    kinematics::Collision::_1D_collision(&p1, &p2);
    EXPECT_NEAR(p1.momentum(), 100.0, 1e-9);
    EXPECT_NEAR(p2.momentum(), 100.0, 1e-9);
  }
};

} 

TEST_METHOD(TestKinematics::TestCollision, test_1D_collision)
