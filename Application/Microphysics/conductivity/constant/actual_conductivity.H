#ifndef ACTUAL_CONDUCTIVITY_H
#define ACTUAL_CONDUCTIVITY_H

#include <cmath>
#include <eos_type.H>
#include <network.H>
#include <extern_parameters.H>

const std::string cond_name = "constant";

AMREX_FORCE_INLINE
void
actual_conductivity_init()
{
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void
actual_conductivity (T& state)
{
  state.conductivity = const_conductivity;
}
#endif
