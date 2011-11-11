COPT      = -g -O3
FOPT      = -g -O3

ifeq ($(TARGET),)
CC        = mpicc
FC        = mpif
MAKEJNUM  = 2
# Megatron
LIBARMCI  = -L/home/jeff/eclipse/ARMCI-MPI/trunk/install/lib -larmci
endif

ifeq ($(TARGET),XE)
CC        = cc
FC        = ftn
MAKEJNUM  = 12
# Hopper
LIBARMCI  = -L/global/homes/j/jhammond/ARMCI-MPI/armci-mpi/trunk/install/lib -larmci
endif

ifeq ($(TARGET),BGP)
COMPILER  = XL
CC        = mpixlc
FC        = mpixlf90
MAKEJNUM  = 4
# Surveyor
LIBARMCI  = -L/home/jhammond/ARMCI-MPI/armci-mpi/trunk/install -larmci
endif

ifeq ($(COMPILER),)
FPP       = -cpp
FIXED     = -ffixed-form
FREE      = -ffree-form
FINT8     = -fdefault-integer-8
FNORNGCHK = -fno-range-check
endif

ifeq ($(COMPILER),PGI)
FPP       = -Mpreprocess
FIXED     = -Mfixed
FREE      = -Mfree
FINT8     = -i8
FNORNGCHK = 
endif

ifeq ($(COMPILER),XL)
FPP       = -qsuffix=cpp=f
FIXED     = -qfixed
FREE      = -qfree=f90
FINT8     = -qintsize=8
FNORNGCHK = 
endif

#DMAPP_PATH=/opt/cray/dmapp/default
#GNIH_PATH=/opt/cray/gni-headers/default
#UGNI_PATH=/opt/cray/ugni/default
#UDREG_PATH=/opt/cray/udreg/default
#PMI_PATH=/opt/cray/pmi/default
#RCA_PATH=/opt/cray/rca/default
#HSS_PATH=/opt/cray-hss-devel/default

#INC     = -I$(DMAPP_PATH)/include -I$(UDREG_PATH)/include -I$(GNIH_PATH)/include -I$(PMI_PATH)/include -I$(RCA_PATH)/include  -I$(HSS_PATH)/include
#LIBS    = -L$(DMAPP_PATH)/lib64   -L$(UDREG_PATH)/lib64   -L$(UGNI_PATH)/lib64   -L$(PMI_PATH)/lib64   -L$(RCA_PATH)/lib64   -ldmapp -ludreg -lugni -lpmi -lrca

CFLAGS   = $(COPT) $(INC)
FFLAGS   = $(FOPT) $(INC)

LD       = $(FC)
LDFLAGS  = $(FOPT)

LIBS     = $(LIBARMCI)

OBJECTS = caladdr2.o cptrsize.o diffcpointers.o dummy_malloc.o fmm_full.o fmmgradt.o fmm_mods.o fmmmopn.o fmmoopn.o getcjpa.o getneighbors.o mp_constants.o mp_wrapper.o ptroffset.o pvlist2.o

all: test_fmm_r32.x

test_fmm_r32.x: libfm_r32.a sl/libsl.a
	$(LD) $(LDFLAGS) libfm_r32.a sl/libsl.a $(LIBS) -o $@

sl/libsl.a:
	make -C sl -j $(MAKEJNUM) CC="$(CC)" CFLAGS="$(CFLAGS)"

libfm_r32.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

myarmci_constants.mod mympi_constants.mod: mp_constants.f
	$(FC) $(FFLAGS) $(FPP)          $(FREE) -c $< 

armci_types.mod armci_wrapper.mod mpi_wrapper.mod mp_wrapper.mod: mp_wrapper.f myarmci_constants.mod mympi_constants.mod
	$(FC) $(FFLAGS) $(FPP) $(FINT8) $(FREE) -c $< 

fmmkinds.mod cal5md.mod cputime.mod fmm0.mod fmmalloc.mod fmmd7.mod fmmhybrid.mod fmmicharge1icharge2.mod fmmicharge5icharge6.mod fmmint34.mod fmmjcharge1jcharge2.mod fmmjmp.mod fmmnsqrndiv.mod mbox2int.mod mcoordinates.mod mem_info.mod mfbox2int.mod mnbox2int.mod mp_edge.mod mp_emptyboxes.mod mp_info.mod mplummer.mod mp_pass2bftrq.mod msort.mod mstatistics.mod pass2bftrpointers.mod qinfo.mod smqm.mod: fmm_mods.f mp_wrapper.mod
	$(FC) $(FFLAGS) $(FPP) $(FINT8) $(FIXED) -c $< 

fmmgradt.o: fmmgradt.f fmmkinds.mod
	$(FC) $(FFLAGS) $(FPP) $(FINT8) $(FIXED) -c $< -o $@

fmmoopn.o: fmmoopn.f fmmkinds.mod
	$(FC) $(FFLAGS) $(FPP) $(FINT8) $(FIXED) -c $< -o $@

fmmmopn.o: fmmmopn.f fmmkinds.mod
	$(FC) $(FFLAGS) $(FPP) $(FINT8) $(FIXED) -c $< -o $@

getcjpa.o getneighbors_vars.mod: getcjpa.f
	$(FC) $(FFLAGS) $(FPP) $(FINT8) $(FREE) -c $< 

getneighbors.o: getneighbors.f
	$(FC) $(FFLAGS) $(FPP) $(FINT8) $(FREE) -c $<

caladdr2.o: caladdr2.f getneighbors_vars.mod
	$(FC) $(FFLAGS) $(FPP) $(FINT8) $(FREE) -c $<

pvlist2.o pvlist.mod: pvlist2.f fmmkinds.mod
	$(FC) $(FFLAGS) $(FPP) $(FINT8) $(FREE) -c $<

fmm_full.o: fmm_full.f fmmkinds.mod pvlist.mod getneighbors_vars.mod mp_wrapper.mod mp_info.mod
	$(FC) $(FFLAGS) $(FPP) $(FINT8) $(FIXED) $(FNORNGCHK) -c $<

clean:
	$(RM) $(RMFLAGS) *.o *.mod

realclean: clean
	$(RM) $(RMFLAGS) libfm_r32.a test_fmm_r32.x
	$(RM) $(RMFLAGS) sl/libsl.a
	make -C sl clean

