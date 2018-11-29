#include "universe/Universe.h"
#include "simulation/Simulation.h"

class GPUBarnesHutSimulation : public Simulation {
public:
  GPUBarnesHutSimulation(Universe* universe) : Simulation(universe) {};

private:
  Particle* particles;

  float gravity_const = 6.67e-11;
  float n_samples = 10;
  float time_step = 1e-2;
};
