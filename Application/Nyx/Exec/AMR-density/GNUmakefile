# AMREX_HOME defines the directory in which we will find all the AMReX code
AMREX_HOME = ${CBB_HOME}/Application/amrex
# TOP defines the directory in which we will find Source, Exec, etc
TOP = ../..

# compilation options
COMP    = gnu
USE_MPI = TRUE
# Use with Async IO
MPI_THREAD_MULTIPLE = FALSE
#USE_OMP = TRUE
USE_CUDA = FALSE

USE_SUNDIALS      = TRUE
USE_FORT_ODE = FALSE
SUNDIALS_ROOT =  ${CBB_HOME}/Application/Nyx/subprojects/sundials/instdir
PROFILE       = FALSE
TRACE_PROFILE = FALSE
COMM_PROFILE  = FALSE
TINY_PROFILE  = TRUE

USE_HDF5=TRUE
HDF5_HOME=${HDF5}

#USE_HDF5_SZ3 = TRUE
#SZ3_HOME = /opt/orisz3/install
#SZ3_HOME = /opt/SZ3/install
#SZ3_HOME = /opt/SZ_SLE/install

PRECISION = DOUBLE
USE_SINGLE_PRECISION_PARTICLES = TRUE
DEBUG     = FALSE

# physics
DIM      = 3
USE_HEATCOOL = TRUE
USE_AGN = FALSE
ifeq ($(NO_HYDRO),TRUE)
USE_SDC = FALSE
USE_SUNDIALS = FALSE
USE_FUSED = FALSE
else
ifeq ($(USE_HEATCOOL),TRUE)
USE_SDC = TRUE
USE_SUNDIALS = TRUE
USE_FUSED ?= $(USE_CUDA)
else
USE_SDC = FALSE
USE_SUNDIALS = FALSE
USE_FUSED = FALSE
endif
endif
USE_CONST_SPECIES = TRUE

Bpack := ./Make.package
Blocs := .

include $(TOP)/Exec/Make.Nyx
