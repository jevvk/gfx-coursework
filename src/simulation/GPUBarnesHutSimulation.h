#include "universe/Universe.h"
#include "simulation/Simulation.h"

#include "simulation/engine/GPUBarnesHut.h"

class GPUBarnesHutSimulation : public Simulation {
public:
  GPUBarnesHutSimulation(Universe* universe);

  void step();

private:
  GPUBarnesHut* barneshut;
};
