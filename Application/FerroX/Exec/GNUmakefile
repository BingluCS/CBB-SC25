# AMREX_HOME defines the directory in which we will find all the AMReX code.
AMREX_HOME  = ${CBB_HOME}/Application/amrex
USE_HDF5=TRUE
HDF5_HOME=${HDF5}

CODE_HOME := ..

DEBUG        = FALSE
USE_MPI      = TRUE
USE_OMP      = FALSE
USE_CUDA     = FALSE
COMP         = gnu
DIM          = 3
CXXSTD       = c++17
TINY_PROFILE = TRUE

USE_EB = FALSE
USE_SUNDIALS = FALSE

ifeq ($(USE_CUDA), TRUE)
 USE_OMP= FALSE
endif

ifeq ($(USE_SUNDIALS),TRUE)
  ifeq ($(USE_CUDA),TRUE)
    SUNDIALS_ROOT ?= $(TOP)../../sundials/instdir_cuda
  else
    SUNDIALS_ROOT ?= $(TOP)../../sundials/instdir
  endif
  ifeq ($(NERSC_HOST),perlmutter)
    SUNDIALS_LIB_DIR ?= $(SUNDIALS_ROOT)/lib64
  else
    SUNDIALS_LIB_DIR ?= $(SUNDIALS_ROOT)/lib
  endif

  USE_CVODE_LIBS ?= TRUE
  USE_ARKODE_LIBS ?= TRUE

  DEFINES += -DAMREX_USE_SUNDIALS
  INCLUDE_LOCATIONS += $(SUNDIALS_ROOT)/include
  LIBRARY_LOCATIONS += $(SUNDIALS_LIB_DIR)

  LIBRARIES += -L$(SUNDIALS_LIB_DIR) -lsundials_cvode
  LIBRARIES += -L$(SUNDIALS_LIB_DIR) -lsundials_arkode
  LIBRARIES += -L$(SUNDIALS_LIB_DIR) -lsundials_nvecmanyvector
  LIBRARIES += -L$(SUNDIALS_LIB_DIR) -lsundials_core

  ifeq ($(USE_CUDA),TRUE)
    LIBRARIES += -L$(SUNDIALS_LIB_DIR) -lsundials_nveccuda
  endif

endif


PRINT_NAME   = FALSE
PRINT_LOW   = FALSE
PRINT_HIGH   = FALSE
TIME_DEPENDENT = FALSE

include $(CODE_HOME)/Source/Make.FerroX

