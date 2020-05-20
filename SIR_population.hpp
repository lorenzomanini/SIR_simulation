#ifndef PERSON_HPP
#define PERSON_HPP

#include <vector>

struct Position
{
  double x;
  double y;
};

struct Velocity
{
  double vx;
  double vy;
};

struct Person
{
  Position position;
  Velocity velocity;
};

using People = std::vector<Person>;

struct SIR_Population
{
  People S;
  People I;
  People R;
};

SIR_Population create_sir_population(int size, int S, int I, int R);

#endif