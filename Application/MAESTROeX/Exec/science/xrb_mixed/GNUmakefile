DEBUG      = FALSE
DIM        = 2
COMP	   = gnu
USE_MPI    = TRUE
USE_OMP    = FALSE
USE_REACT  = TRUE

TINY_PROFILE = FALSE
PROFILE      = FALSE # TRUE overrides TINY_PROFILE


# define the location of the MAESTROEX home directory
MAESTROEX_HOME  := ../../..


# Set the EOS, conductivity, and network directories
# We first check if these exist in $(MAESTROEX_HOME)/Microphysics/(EOS/conductivity/networks)
# If not we use the version in $(MICROPHYSICS_HOME)/Microphysics/(EOS/conductivity/networks)
EOS_DIR          := helmholtz
CONDUCTIVITY_DIR := stellar
NETWORK_DIR      := rprox
INTEGRATOR_DIR   := VODE

Bpack   := ./Make.package
Blocs   := .
PROBIN_PARAMETER_DIRS := .

include $(MAESTROEX_HOME)/Exec/Make.Maestro
