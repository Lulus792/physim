
#include "../include/kinematics.hpp"
#include "../include/units.hpp"

void kinematics::Collision::_1D_collision(si::Particle *p1, si::Particle *p2) {
  auto new_velocity = [](si::Particle *_p1, si::Particle *_p2) {
    return 2*((_p1->mass() * _p1->velocity()) + (_p2->mass() * _p2->velocity()))/
      (_p1->mass() + _p2->mass()); 
  };
  
  p1->get<velocity_t>() = new_velocity(p1, p2);
  p2->get<velocity_t>() = new_velocity(p2, p1);

}
