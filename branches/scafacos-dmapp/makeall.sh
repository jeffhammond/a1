#!/bin/bash

export CC=cc
export F90=ftn
export F77=ftn
export CFLAGS="-c -g -O2"

# GNU
export FFLAGS="-c -g -O2 -cpp -fno-range-check"
export I8FLAG="-fdefault-integer-8"
export FREEFLAG="-ffree-form"
export FIXEDFLAG="-ffixed-form"

# PGI
#export FFLAGS="-c -g -O2 -Mpreprocess"
#export I8FLAG="-i8"
#export FREEFLAG="-Mfree"
#export FIXEDFLAG="-Mfixed"

#export LIBARMCI="-L/software/ga/ga-trunk/install/lib -larmci"
export LIBARMCI="-L/global/homes/j/jhammond/ARMCI-MPI/armci-mpi/trunk/install/lib -larmci"

#${CC} ${CFLAGS} cptrsize.c
#${CC} ${CFLAGS} diffcpointers.c
#${CC} ${CFLAGS} ptroffset.c
#${CC} ${CFLAGS} dummy_malloc.c 
make cptrsize.o diffcpointers.o dummy_malloc.o ptroffset.o

#${F90} ${FFLAGS}           ${FREEFLAG} mp_constants.f90
#${F90} ${FFLAGS} ${I8FLAG} ${FREEFLAG} mp_wrapper.f90
make myarmci_constants.mod mympi_constants.mod
make armci_types.mod armci_wrapper.mod mpi_wrapper.mod mp_wrapper.mod

#${F77} ${FFLAGS} ${I8FLAG} fmm_mods.f
make fmmkinds.mod

#${F77} ${FFLAGS} ${I8FLAG} fmmgradt.f
#${F77} ${FFLAGS} ${I8FLAG} fmmoopn.f
#${F77} ${FFLAGS} ${I8FLAG} fmmmopn.f
make fmmgradt.o fmmoopn.o fmmmopn.o

#${F90} ${FFLAGS} ${I8FLAG} ${FREEFLAG} getcjpa.f90
#${F90} ${FFLAGS} ${I8FLAG} ${FREEFLAG} getneighbors.f90
#${F90} ${FFLAGS} ${I8FLAG} ${FREEFLAG} caladdr2.f90
make getcjpa.o getneighbors.o caladdr2.o

#${F90} ${FFLAGS} ${I8FLAG} ${FREEFLAG} pvlist2.f90
make pvlist2.o

#${F90} ${FFLAGS} ${I8FLAG} ${FIXEDFLAG} fmm_full.f
make fmm_full.o

#rm -f libfm_r32.a
#ar -r libfm_r32.a *.o
make libfm_r32.a

${F90} libfm_r32.a sl/libsl.a ${LIBARMCI} -o test_fmm_r32.x
#make test_fmm_r32.x
