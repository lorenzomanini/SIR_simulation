#ifndef SIR_RANDOM_MOTION_HPP
#define SIR_RANDOM_MOTION_HPP

#include <TRandom.h>
#include <algorithm>
#include "motion.hpp"
#include "population.hpp"

namespace sir {

class Random_Motion : public G_Motion
{
  double const sd_;
  TRandom random_generator_;

 public:
  Random_Motion(double sd = 1) : sd_{sd}, random_generator_{}
  {
    random_generator_.SetSeed();
  }
  void update(Population& population, int size) override
  {
    auto move = [&](Person& person) {
      person.velocity.vx += random_generator_.Gaus(0., sd_);
      person.velocity.vy += random_generator_.Gaus(0., sd_);

      // Friction
      person.velocity.vx -=
          std::abs(person.velocity.vx) * person.velocity.vx / 10000;
      person.velocity.vy -=
          std::abs(person.velocity.vy) * person.velocity.vy / 10000;

      person.position.x += person.velocity.vx;
      person.position.y += person.velocity.vy;

      if (person.position.x > size || person.position.x < 0) {
        person.velocity.vx = -person.velocity.vx;
        person.position.x += person.velocity.vx;
      }
      if (person.position.y > size || person.position.y < 0) {
        person.velocity.vy = -person.velocity.vy;
        person.position.y += person.velocity.vy;
      }
    };

    std::for_each(population.S.begin(), population.S.end(), move);
    std::for_each(population.I.begin(), population.I.end(), move);
    std::for_each(population.R.begin(), population.R.end(), move);
  }
};

}  // namespace sir

#endif