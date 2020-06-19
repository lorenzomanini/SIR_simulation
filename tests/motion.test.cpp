#include "doctest.h"
#include "population.hpp"
#include "random_motion.hpp"

int constexpr size = 100;
double constexpr sd_motion = 0.;

People people{Person{Position{0., 0.}, Velocity{0., 0.}}};
Population population{people, People(), People()};
Simulation_State state(size, population);
Random_Motion motion{sd_motion};

TEST_CASE("Testing motion with sd = 0")
{
  CHECK(state.population.S[0].position.x == doctest::Approx(0.));
  CHECK(state.population.S[0].position.y == doctest::Approx(0.));

  motion.update(state.population, state.size);
  CHECK(state.population.S[0].position.x == doctest::Approx(0.));
  CHECK(state.population.S[0].position.y == doctest::Approx(0.));

  state.population.S[0].velocity.vx = 10.;
  state.population.S[0].velocity.vy = 20.;
  motion.update(state.population, state.size);

  CHECK(state.population.S[0].velocity.vx ==
        doctest::Approx(10.).epsilon(0.02));
  CHECK(state.population.S[0].velocity.vy ==
        doctest::Approx(20.).epsilon(0.02));
  CHECK(state.population.S[0].position.x == doctest::Approx(10.).epsilon(0.02));
  CHECK(state.population.S[0].position.y == doctest::Approx(20.).epsilon(0.02));
}

TEST_CASE("Testing friction")
{
  // v -= |v| * v / 10000

  state.population.S[0].position.x = 0.;
  state.population.S[0].position.y = 0.;
  state.population.S[0].velocity.vx = 100.;
  state.population.S[0].velocity.vy = 200.;
  motion.update(state.population, state.size);

  CHECK(std::abs(state.population.S[0].velocity.vx) ==
        doctest::Approx(99.).epsilon(0.01));
  CHECK(std::abs(state.population.S[0].velocity.vy) ==
        doctest::Approx(196.).epsilon(0.01));
}

TEST_CASE("Testing bouncing")
{
  state.population.S[0].position.x = 0.;
  state.population.S[0].position.y = 0.;
  state.population.S[0].velocity.vx = -10.;
  state.population.S[0].velocity.vy = -20.;
  motion.update(state.population, state.size);

  CHECK(state.population.S[0].velocity.vx ==
        doctest::Approx(10.).epsilon(0.02));
  CHECK(state.population.S[0].velocity.vy ==
        doctest::Approx(20.).epsilon(0.02));
  CHECK(state.population.S[0].position.x == doctest::Approx(0.).epsilon(0.02));
  CHECK(state.population.S[0].position.y == doctest::Approx(0.).epsilon(0.02));

  state.population.S[0].position.x = size;
  state.population.S[0].position.y = size;
  state.population.S[0].velocity.vx = 10.;
  state.population.S[0].velocity.vy = 20.;
  motion.update(state.population, state.size);

  CHECK(state.population.S[0].velocity.vx ==
        doctest::Approx(-10.).epsilon(0.02));
  CHECK(state.population.S[0].velocity.vy ==
        doctest::Approx(-20.).epsilon(0.02));
  CHECK(state.population.S[0].position.x ==
        doctest::Approx(size).epsilon(0.02));
  CHECK(state.population.S[0].position.y ==
        doctest::Approx(size).epsilon(0.02));
}

int constexpr S = 200;
int constexpr I = 0;
int constexpr R = 0;
int constexpr sd_motion_2 = 1.;

Simulation_State state_2(size, S, I, R);
Random_Motion motion_2{sd_motion_2};

double mean()
{
  double sum = 0.;
  for (Person const& person : state_2.population.S) {
    sum += person.position.x;
    sum += person.position.y;
  }
  int N = state_2.population.S.size() * 2;
  return (sum / N);
}

TEST_CASE("Testing position, velocity and acceleration mean")
{
  CHECK(mean() == doctest::Approx(size / 2).epsilon(0.1));
  for (int i = 0; i != 10; ++i) {
    Simulation_State previous_state = state_2;
    motion_2.update(state_2.population, state_2.size);
    CHECK(mean() == doctest::Approx(size / 2).epsilon(0.1));

    double displacement_sum = 0.;
    double velocity_sum = 0.;
    double acceleration_sum = 0.;

    for (int i = 0; i != S; ++i) {
      displacement_sum += state_2.population.S[i].position.x -
                          previous_state.population.S[i].position.x;
      displacement_sum += state_2.population.S[i].position.y -
                          previous_state.population.S[i].position.y;

      velocity_sum += state_2.population.S[i].velocity.vx;
      velocity_sum += state_2.population.S[i].velocity.vy;

      acceleration_sum += state_2.population.S[i].velocity.vx -
                          previous_state.population.S[i].velocity.vx;
      acceleration_sum += state_2.population.S[i].velocity.vy -
                          previous_state.population.S[i].velocity.vy;
    }

    CHECK(displacement_sum / (2 * S) == doctest::Approx(0.).epsilon(1.));
    CHECK(velocity_sum / (2 * S) == doctest::Approx(0.).epsilon(1.));
    CHECK(acceleration_sum / (2 * S) == doctest::Approx(0.).epsilon(1.));
  }
}