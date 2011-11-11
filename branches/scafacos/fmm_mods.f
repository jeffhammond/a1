#include "fmm.h"
      module fmmkinds
#ifdef FMM_ISO_C_BINDING
       use iso_c_binding
#endif
       implicit none
       integer, parameter:: fmm_real=FMM_REAL
       integer, parameter:: fmm_real_extended=FMM_REAL_EXTENDED
       integer, parameter:: fmm_real_itor=FMM_REAL_ITOR
       integer, parameter:: fmm_integer=FMM_INTEGER
#ifdef FMM_ISO_C_BINDING
       integer, parameter:: fmm_c_integer=FMM_C_INTEGER
#endif
       integer, parameter:: fmm_logical=FMM_LOGICAL
       integer, parameter:: fmm_testalloc_integer=FMM_TESTALLOC_INTEGER
#ifdef FMM_PARALLEL
       integer, parameter:: mp_real_max=FMM_MP_REAL_MAX
       integer, parameter::mp_integer_processes=FMM_MP_INTEGER_PROCESSES
#endif
       type tsndibox
        integer(kind=fmm_integer), pointer:: sndibox(:)
       end type tsndibox
       type tsndomegatree
        real(kind=fmm_real), pointer:: sndomegatree(:,:)
       end type tsndomegatree
       type tsndq
        real(kind=fmm_real), pointer:: sndq(:)
       end type tsndq
       type tsndxyz
        real(kind=fmm_real), pointer:: sndxyz(:,:)
       end type tsndxyz
#ifdef FMM_LOADSORT
       type tsndiboxload
        integer(kind=fmm_integer), pointer:: sndiboxload(:)
       end type tsndiboxload
#endif
#ifdef FMM_UNIFORMGRID
       type tsnduniformgrid
        real(kind=fmm_real), pointer:: snduniformgrid(:,:)
       end type tsnduniformgrid
#endif
       type telem
        integer(kind=fmm_integer) pos,val
        type(telem), pointer:: prev,next
       end type telem
      end module fmmkinds
c
      module fmmd7
       use fmmkinds
       implicit none
       type pmu
        real(kind=fmm_real), pointer:: mu(:)
       end type pmu
      end module fmmd7
c
      module fmmint34
       use fmmkinds
       implicit none
       integer(kind=fmm_integer) int3xyzd,int3xyzd1,int3xyz
       integer(kind=fmm_integer), external:: int4x,int4y,int4z
      end module fmmint34
c
      module fmmjmp
       use fmmkinds
       implicit none
       integer(kind=fmm_integer) njmp,ajmp,bjmp,tjmp,qjmp,vjmp,cjmp,
     . gjmp,djmp,ejmp
       integer(kind=fmm_integer) ijmp,jjmp,kjmp,hjmp,fjmp,rjmp,wjmp,
     . mjmp,sjmp
       logical(kind=fmm_logical) jmp,jmpg,jmph,jmpp,jmpupd,jmpj,jmpn,
     . jmpb,jmpjp
      end module fmmjmp
c
      module fmmhybrid
       use fmmkinds
       implicit none
       real(kind=fmm_real) fhy
       integer(kind=fmm_integer) nchhy,nhy,msthy,medhy,nsthy,nedhy,mmhy,
     . nmhy,indhy,poshy
       logical(kind=fmm_logical) hybrid,dhym,dhyn
      end module fmmhybrid
c
      module mem_info
       use fmmkinds
       implicit none
       integer(kind=fmm_integer) mem_s,mmem_s,mem_l,mem_m
       integer(kind=fmm_integer) mem_s2,mmem_s2,mem_l2,mem_m2
      end module mem_info
c
      module fmmnsqrndiv
       use fmmkinds
       implicit none
       integer(kind=fmm_integer) nsqr,ndiv
      end module fmmnsqrndiv
c
      module fmm0
       use fmmkinds
       implicit none
       real(kind=fmm_real) fmmi0
      end module fmm0
c
      module smqm
       use fmmkinds
       implicit none
       real(kind=fmm_real) smqp,smqn,smqsq,smq,smaq
      end module smqm
c
      module qinfo
       use fmmkinds
       implicit none
       real(kind=fmm_real) qmin,qmax,qall
       logical(kind=fmm_logical) qscratch,qdestroyed,useqinsh
      end module qinfo
c
      module cal5md
       use fmmkinds
       implicit none
       integer(kind=fmm_integer) in(0:16)
      end module cal5md
c
      module msort
       use fmmkinds
       implicit none
#ifndef FMM_SORTHD
       integer(kind=fmm_integer), target:: mem_sizes(2)
#endif
       integer(kind=fmm_integer), target:: addr_desc(2)
      end module msort
c
#ifdef FMM_COMPRESSION
      module compression
       use fmmkinds
       implicit none
#ifdef FMM_EXTREMECOMPRESSION
#ifdef FMM_EXTREMEEXTREMECOMPRESSION
       real(kind=fmm_real) sgc(0:1)
       integer(kind=fmm_integer), parameter:: nchcompression=512
       real(kind=fmm_real), target:: xyzcompression(3,nchcompression)
#endif
#endif
       integer(kind=fmm_integer) nbib,ibm,ibd,ib01,ibm01(0:1),ibm011,
     . ibsrtws,ibsrtwm,ibsrtrs,ibsrtrm,nbibsrt,nint1,ibsrts,ibsrtm,
     . ibsrts8,n2bibsrt,ibsrtsm,ibsrtmm,ibsrtsm8
#ifdef FMM_SORTHD
       integer(kind=fmm_integer) ibm7,ibm1
#endif
#ifdef FMM_EXTREMECOMPRESSION
       integer(kind=fmm_integer) maxboxnumber,maxnp,mindp,mindpl,mindpm,
     . mindps
#ifdef FMM_EXTREMEEXTREMECOMPRESSION
       integer(kind=fmm_integer) ieo,ies,iem,jesx,jemx,jesy,jemy,jesz,
     . jemz,iea,iev,jis,jim,kis,kim
#endif
#endif
#ifdef FMM_PARALLEL
       integer(kind=fmm_integer) nbmb,nintgb1
#endif
       integer(kind=fmm_integer), allocatable:: int1m(:),mint1(:),
     . int1scr(:)
#ifdef FMM_EXTREMECOMPRESSION
       integer(kind=fmm_integer), allocatable:: pinb(:)
#endif
       logical(kind=fmm_logical) twoint
      end module compression
#endif
c
#ifdef FMM_TREETOGRAD
      module mtreetograd
       use fmmkinds
       implicit none
#ifdef FMM_EXTREMETREETOGRAD
       integer(kind=fmm_integer) ntreetograd,starttreetograd,
     . itreetograd,otreetograd,ptreetograd,dtreetograd,bftreetograd
#endif
       logical(kind=fmm_logical) allocomega
#ifdef FMM_EXTREMETREETOGRAD
       logical(kind=fmm_logical) allocmu,ltreetograd,doallocbftreetograd
#endif
      end module mtreetograd
#endif
c
#ifdef FMM_MULTIPOLEMOMENTS
      module mmultipolemoments
       use fmmkinds
       implicit none
       real(kind=fmm_real_extended) zhip
       real(kind=fmm_real_extended), allocatable:: romegahip(:),
     . iomegahip(:)
      end module mmultipolemoments
#endif
c
#if defined(FMM_CPUTIME) || defined(FMM_WALLTIME)
      module cputime
       use fmmkinds
       implicit none
       integer(kind=fmm_integer), parameter:: nsted=13
       real(kind=fmm_real_extended) start_cputime,start_walltime,
     . end_cputime,end_walltime,st(nsted),ed(nsted),edst
      end module cputime
#endif
c
      module mcoordinates
       use fmmkinds
       implicit none
       real(kind=fmm_real) sa(3),sb,sc(3)
      end module mcoordinates
c
      module mplummer
       use fmmkinds
       implicit none
       real(kind=fmm_real) a_plummer,distance_plummer,b1_plummer,
     . b0_plummer,c2_plummer,c1_plummer,c0_plummer,gr1_plummer,
     . gr0_plummer
       logical(kind=fmm_logical) plummer_potential
      end module mplummer
c
#ifdef FMM_UNIFORMGRID
      module muniformgrid
       use fmmkinds
       implicit none
       real(kind=fmm_real) uniformgridox,uniformgridoy,uniformgridoz,
     . uniformgridoalpha,uniformgridm,uniformgridx,uniformgridy,
     . uniformgridz,uniformgridalpha,uniformgridv,uniformgridb,
     . uniformgridalphab,uniformgridd1x,uniformgridd5x,uniformgridd3x,
     . uniformgridd1y,uniformgridd5y,uniformgridd3y,uniformgridd1z,
     . uniformgridd5z,uniformgridd3z,uniformgridf,uniformgridg,
     . uniformgrid1,uniformgrid9,uniformgridsx,uniformgridsy,
     . uniformgridsz,uniformgridpx,uniformgridpy,uniformgridpz,
     . uniformgridmx,uniformgridmy,uniformgridmz
       real(kind=fmm_real_itor), allocatable:: uniformgridseq(:)
       real(kind=fmm_real), allocatable:: uniformgrid(:,:)
#ifdef FMM_PARALLEL
       real(kind=fmm_real), allocatable:: uniformgridedge(:,:),
     . uniformgridseqremote(:),uniformgridremote(:,:)
#endif
       integer(kind=fmm_integer) uniformgridi,uniformgridj,uniformgridk,
     . uniformgridl
#ifdef FMM_PARALLEL
       integer(kind=fmm_integer) uniformgridqiislocal,
     . uniformgridqjislocal,uniformgridedgestart,uniformgridpos(12),
     . uniformgridkremote,uniformgridlremote
       integer(kind=fmm_integer), allocatable:: uniformgridqiaddr(:)
       logical(kind=fmm_logical) uniformgridcallfromp5bibj
#endif
      end module muniformgrid
#endif
c
#ifdef FMM_DAMPING
      module mdamping
       use fmmkinds
       implicit none
       real(kind=fmm_real_extended) efinboxa,efbibja,enfa,enfd1,enfd2,
     . enfdt
       real(kind=fmm_real_extended),
     . parameter:: zod=0.e0_fmm_real_extended
       real(kind=fmm_real) enmonodipoled,endidipoled,enmadelungd,qdd,
     . qdn,qdm,enpp,enmp,damping
#ifdef FMM_DAMPING_SHQEN
       real(kind=fmm_real) ep,enpps,enmps
#endif
       real(kind=fmm_real), allocatable:: enfdq(:)
       real(kind=fmm_real), allocatable, target:: enfdb(:,:),enfdbi(:,:)
       real(kind=fmm_real), pointer:: enfdp(:,:)
#ifdef FMM_DAMPING_PP_DIPOLEMOMENTS
       real(kind=fmm_real_extended) qppx,qppy,qppz,enppmd,enppmdtmp,
     . enppmdd
       real(kind=fmm_real) qppa,qppxa,qppya,qppza,qppb,qppxb,qppyb,qppzb
       real(kind=fmm_real), allocatable:: qppdx(:),qppdy(:),qppdz(:)
#endif
       logical(kind=fmm_logical) enfdba
      end module mdamping
#endif
c
#ifdef FMM_STATISTICS
      module mstatistics
       use fmmkinds
       implicit none
       real(kind=fmm_real_extended) tstart,tend
      end module mstatistics
#endif
c
      module fmmicharge1icharge2
       use fmmkinds
       implicit none
       integer(kind=fmm_integer) icharge1,icharge2,icharge3,icharges
      end module fmmicharge1icharge2
c
      module fmmicharge5icharge6
       use fmmkinds
       implicit none
       integer(kind=fmm_integer) icharge5,icharge6
      end module fmmicharge5icharge6
c
      module fmmjcharge1jcharge2
       use fmmkinds
       implicit none
       integer(kind=fmm_integer) jcharge1,jcharge2,jcharges
      end module fmmjcharge1jcharge2
c
      module pass2bftrpointers
       use fmmkinds
       implicit none
c_i_j: i:pairnumber, j=1: kbox, j=2: mbx
       real(kind=fmm_real), pointer:: promegatree_1_1(:,:),
     . piomegatree_1_1(:,:),prmutree_1_1(:,:),pimutree_1_1(:,:),
     . promegatree_2_1(:,:),piomegatree_2_1(:,:),prmutree_2_1(:,:),
     . pimutree_2_1(:,:),promegatree_3_1(:,:),piomegatree_3_1(:,:),
     . prmutree_3_1(:,:),pimutree_3_1(:,:),promegatree_4_1(:,:),
     . piomegatree_4_1(:,:),prmutree_4_1(:,:),pimutree_4_1(:,:)
       real(kind=fmm_real), pointer:: promegatree_1_2(:,:),
     . piomegatree_1_2(:,:),prmutree_1_2(:,:),pimutree_1_2(:,:),
     . promegatree_2_2(:,:),piomegatree_2_2(:,:),prmutree_2_2(:,:),
     . pimutree_2_2(:,:),promegatree_3_2(:,:),piomegatree_3_2(:,:),
     . prmutree_3_2(:,:),pimutree_3_2(:,:),promegatree_4_2(:,:),
     . piomegatree_4_2(:,:),prmutree_4_2(:,:),pimutree_4_2(:,:)
      end module pass2bftrpointers
c
#ifdef FMM_PARALLEL
      module mp_info
       use fmmkinds
       use mp_wrapper
       implicit none
       integer(kind=fmm_integer) me,nnodes,gbinfo3,gbinfo4,gbinfo5,
     . gbinfo7,gbinfo8,kind_integer_processes
#ifdef FMM_COMPRESSION
       integer(kind=1), allocatable:: gbinfo(:,:,:)
#else
       integer(kind=fmm_integer), allocatable:: gbinfo(:,:)
#endif
       integer(kind=mp_integer_processes), allocatable:: idp(:)
       logical(kind=fmm_logical) mp_setup
       type(c_ptr), allocatable, target:: gbpt(:),gbsndrcvol(:),
     . gbsndrcvbuffer(:),gbsndibox(:),gbsndromegatree(:),
     . gbsndiomegatree(:),gbsndq(:),gbsndxyz(:)
#ifdef FMM_LOADSORT
       type(c_ptr), allocatable, target:: gbsndiboxload(:)
#endif
#ifdef FMM_UNIFORMGRID
       type(c_ptr), allocatable, target:: gbsnduniformgrid(:)
#endif
       type(c_ptr) gbptsndrmutree,gbptsndimutree,gbptsndmutree
       type(c_ptr) gbptsndxyz,gbptsndpot,gbptsndgrad,gbptrcvgrad,
     . gbptgbinfo7
       type(c_ptr) ptscr1,ptscr2
#ifdef FMM_LOADSORT
       type(c_ptr) gbptsndiboxload,gbptrcviboxload,gbptscr
       type(c_ptr) gbpt5sndiboxload,gbpt5scr
#endif
#ifdef FMM_UNIFORMGRID
       type(c_ptr) gbptsnduniformgrid
#endif
       type(tsndibox), allocatable, volatile:: psndibox(:)
       type(tsndomegatree), allocatable, volatile:: psndromegatree(:),
     . psndiomegatree(:)
       type(tsndq), allocatable, volatile:: psndq(:)
       type(tsndxyz), allocatable, volatile:: psndxyz(:)
#ifdef FMM_LOADSORT
       type(tsndiboxload), allocatable, volatile:: psndiboxload(:)
#endif
#ifdef FMM_UNIFORMGRID
       type(tsnduniformgrid), allocatable, volatile:: psnduniformgrid(:)
#endif
#ifdef FMM_DEBUG
       integer(kind=fmm_integer),allocatable:: notifycount(:),
     . waitcount(:)
#endif
      end module mp_info
#endif
c
#ifdef FMM_PARALLEL
      module mp_emptyboxes
       use fmmkinds
       integer(kind=fmm_integer) gba
       logical(kind=fmm_logical) emptyboxes
      end module mp_emptyboxes
#endif
c
#ifdef FMM_PARALLEL
      module mp_edge
       use fmmkinds
       implicit none
       real(kind=fmm_real), pointer, volatile:: romegatreeedge(:,:),
     . iomegatreeedge(:,:),qedge(:),xyzedge(:,:)
       real(kind=fmm_real), allocatable, target:: rmutreeedge(:,:),
     . imutreeedge(:,:),gradedge(:,:)
#ifndef FMM_NOPOT
       real(kind=fmm_real), allocatable, target:: potedge(:)
#endif
#ifdef FMM_LOADSORT
       integer(kind=fmm_integer), allocatable, target:: iboxloadedge(:)
#endif
       integer(kind=fmm_integer) edgemask0,edgemask3,edgesh0,edgemk0,
     . edgesh1,edgemk1,edgesh2,edgemk2,edgesh3,edgemk3,edgesum
      end module mp_edge
#else
      module edge
       use fmmkinds
       implicit none
       integer(kind=fmm_integer) edgemask0
      end module edge
#endif
c
#ifdef FMM_PARALLEL
      module mp_pass2bftrq
       use fmmkinds
       implicit none
       integer(kind=fmm_integer) sinddb,sinddbm,sinddbmm,sind
      end module mp_pass2bftrq
#endif
c
#ifdef FMM_PARALLEL
#ifdef FMM_LOADSORT
      module mp_load
       use fmmkinds
       implicit none
       integer(kind=fmm_integer), pointer:: iboxload(:)
       logical(kind=fmm_logical) doload
      end module mp_load
#endif
#endif
c
      module fmmalloc
c
       use fmmkinds
c
       implicit none
c
       private
c
       integer(kind=fmm_integer), public:: maxallocation,corememory,
     . sortmemory
c
       integer(kind=fmm_integer), public:: nalloc,maxnalloc,nallocr
#ifdef FMM_PARALLEL
       integer(kind=fmm_integer), public:: nmp_alloc,maxnmp_alloc
#endif
       integer(kind=fmm_integer), public:: rtob,rextendedtob,ritortob,
     . itob,ltob
#ifdef FMM_PARALLEL
       integer(kind=fmm_integer), public:: iptob
#endif
#ifdef FMM_PARALLEL
       integer(kind=fmm_integer), public:: gbpttob,typeinttob,
     . typerealtob,typeqtob,typexyztob
#ifdef FMM_LOADSORT
       integer(kind=fmm_integer), public:: typeint2tob
#endif
#ifdef FMM_UNIFORMGRID
       integer(kind=fmm_integer), public:: typeuniformgridtob
#endif
       integer(kind=fmm_integer), public:: typeelemtob
#endif
c
       public:: fmmallocate,fmmdeallocate,fmmallocatept,fmmdeallocatept
#ifdef FMM_PARALLEL
       public:: mp_fmmallocate,mp_fmmdeallocate
#endif
       public:: indallocate,indallocatept,inddeallocate,inddeallocatept
       public:: dbpallocate,dbpdeallocate
       public:: srtallocate,srtdeallocate
c
       interface fmmallocate
        module procedure fmmallocdbl1d
#if FMM_REAL != FMM_REAL_EXTENDED
        module procedure fmmallocdblextended1d
#endif
#if FMM_REAL != FMM_REAL_ITOR && FFMM_REAL_EXTENDED != FMM_REAL_ITOR
        module procedure fmmallocdblitor1d
#endif
        module procedure fmmallocdbl2d,fmmallocdbl3d
        module procedure fmmallocdbl4d,fmmallocdbl5d
        module procedure fmmallocint1d,fmmallocint2d
        module procedure fmmallocint3d,fmmallocint4d
        module procedure fmmalloclog1d,fmmalloclog2d
        module procedure fmmallocbyte,fmmallocbyte2d,fmmallocbyte3d
#ifdef FMM_PARALLEL
#if FMM_INTEGER != FMM_MP_INTEGER_PROCESSES
        module procedure fmmallocintp
#endif
        module procedure fmmallocgpt,fmmalloctypeint,fmmalloctypereal
        module procedure fmmalloctypeq,fmmalloctypexyz
#ifdef FMM_LOADSORT
        module procedure fmmalloctypeint2
#endif
#ifdef FMM_UNIFORMGRID
        module procedure fmmalloctypeuniformgrid
#endif
        module procedure fmmalloctypeelem
#endif
       end interface fmmallocate
c
       interface fmmdeallocate
        module procedure fmmdeallocdbl1d
#if FMM_REAL != FMM_REAL_EXTENDED
        module procedure fmmdeallocdblextended1d
#endif
#if FMM_REAL != FMM_REAL_ITOR && FFMM_REAL_EXTENDED != FMM_REAL_ITOR
        module procedure fmmdeallocdblitor1d
#endif
        module procedure fmmdeallocdbl2d,fmmdeallocdbl3d
        module procedure fmmdeallocdbl4d,fmmdeallocdbl5d
        module procedure fmmdeallocint1d,fmmdeallocint2d
        module procedure fmmdeallocint3d,fmmdeallocint4d
        module procedure fmmdealloclog1d,fmmdealloclog2d
        module procedure fmmdeallocbyte,fmmdeallocbyte2d,
     .  fmmdeallocbyte3d
#ifdef FMM_PARALLEL
#if FMM_INTEGER != FMM_MP_INTEGER_PROCESSES
        module procedure fmmdeallocintp
#endif
        module procedure fmmdeallocgpt,fmmdealloctypeint,
     .  fmmdealloctypereal
        module procedure fmmdealloctypeq,fmmdealloctypexyz
#ifdef FMM_LOADSORT
        module procedure fmmdealloctypeint2
#endif
#ifdef FMM_UNIFORMGRID
        module procedure fmmdealloctypeuniformgrid
#endif
        module procedure fmmdealloctypeelem
#endif
       end interface fmmdeallocate
c
       interface fmmallocatept
        module procedure fmmallocdpt1d
        module procedure fmmallocipt1d
        module procedure fmmalloclpt1d
       end interface fmmallocatept
c
       interface fmmdeallocatept
        module procedure fmmdeallocdpt1d
        module procedure fmmdeallocipt1d
        module procedure fmmdealloclpt1d
       end interface fmmdeallocatept
c
#ifdef FMM_PARALLEL
       interface mp_fmmallocate
        module procedure mp_fmmalloc
       end interface mp_fmmallocate
c
       interface mp_fmmdeallocate
        module procedure mp_fmmdealloc
       end interface mp_fmmdeallocate
#endif
c
       interface indallocate
        module procedure indallocint1d
       end interface indallocate
c
       interface inddeallocate
        module procedure inddeallocint1d
       end interface inddeallocate
c
       interface indallocatept
        module procedure indallocipt1d
       end interface indallocatept
c
       interface inddeallocatept
        module procedure inddeallocipt1d
       end interface inddeallocatept
c
       interface dbpallocate
        module procedure dbpallocdbl1d
        module procedure dbpallocdbl3d
       end interface dbpallocate
c
       interface dbpdeallocate
        module procedure dbpdeallocdbl1d
        module procedure dbpdeallocdbl3d
       end interface dbpdeallocate
c
       interface srtallocate
        module procedure srtallocbyte1d
        module procedure srtallocdbl2d
       end interface srtallocate
c
       interface srtdeallocate
        module procedure srtdeallocbyte1d
        module procedure srtdeallocdbl2d
       end interface srtdeallocate
c
       contains
        subroutine fmmallocdbl1d(a,ia,ib,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real), allocatable:: a(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+rtob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocdbl1d
c
#if FMM_REAL != FMM_REAL_EXTENDED
        subroutine fmmallocdblextended1d(a,ia,ib,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real_extended), allocatable:: a(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+rextendedtob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocdblextended1d
#endif
c
#if FMM_REAL != FMM_REAL_ITOR && FFMM_REAL_EXTENDED != FMM_REAL_ITOR
        subroutine fmmallocdblitor1d(a,ia,ib,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real_itor), allocatable:: a(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+ritortob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocdblitor1d
#endif
c
        subroutine fmmallocdbl2d(a,ia,ib,ja,jb,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real), allocatable:: a(:,:)
         integer(kind=fmm_integer), intent(in):: ia,ib,ja,jb
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+rtob*(ib-ia+1)*(jb-ja+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib,ja:jb),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib,ja:jb),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocdbl2d
c
        subroutine fmmallocdbl3d(a,ia,ib,ja,jb,ka,kb,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real), allocatable:: a(:,:,:)
         integer(kind=fmm_integer), intent(in):: ia,ib,ja,jb,ka,kb
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+rtob*(ib-ia+1)*(jb-ja+1)*(kb-ka+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib,ja:jb,ka:kb),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib,ja:jb,ka:kb),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocdbl3d
c
        subroutine fmmallocdbl4d(a,ia,ib,ja,jb,ka,kb,la,lb,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real), allocatable:: a(:,:,:,:)
         integer(kind=fmm_integer), intent(in):: ia,ib,ja,jb,ka,kb,la,lb
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+rtob*(ib-ia+1)*(jb-ja+1)*(kb-ka+1)*(lb-la+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib,ja:jb,ka:kb,la:lb),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib,ja:jb,ka:kb,la:lb),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocdbl4d
c
        subroutine fmmallocdbl5d(a,ia,ib,ja,jb,ka,kb,la,lb,ma,mb,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real), allocatable:: a(:,:,:,:,:)
         integer(kind=fmm_integer), intent(in):: ia,ib,ja,jb,ka,kb,la,
     .   lb,ma,mb
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i=nalloc+rtob*(ib-ia+1)*(jb-ja+1)*(kb-ka+1)*(lb-la+1)*(mb-ma+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib,ja:jb,ka:kb,la:lb,ma:mb),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib,ja:jb,ka:kb,la:lb,ma:mb),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocdbl5d
c
        subroutine fmmallocint1d(a,ia,ib,istat)
         use fmmkinds
         implicit none
         integer(kind=fmm_integer), allocatable:: a(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+itob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocint1d
c
#ifdef FMM_PARALLEL
#if FMM_INTEGER != FMM_MP_INTEGER_PROCESSES
        subroutine fmmallocintp(a,ia,ib,istat)
         use fmmkinds
         implicit none
         integer(kind=mp_integer_processes), allocatable:: a(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+iptob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocintp
#endif
#endif
c
        subroutine fmmallocint2d(a,ia,ib,ja,jb,istat)
         use fmmkinds
         implicit none
         integer(kind=fmm_integer), allocatable:: a(:,:)
         integer(kind=fmm_integer), intent(in):: ia,ib,ja,jb
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+itob*(ib-ia+1)*(jb-ja+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib,ja:jb),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib,ja:jb),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocint2d
c
        subroutine fmmallocint3d(a,ia,ib,ja,jb,ka,kb,istat)
         use fmmkinds
         implicit none
         integer(kind=fmm_integer), allocatable:: a(:,:,:)
         integer(kind=fmm_integer), intent(in):: ia,ib,ja,jb,ka,kb
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+itob*(ib-ia+1)*(jb-ja+1)*(kb-ka+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib,ja:jb,ka:kb),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib,ja:jb,ka:kb),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocint3d
c
        subroutine fmmallocint4d(a,ia,ib,ja,jb,ka,kb,la,lb,istat)
         use fmmkinds
         implicit none
         integer(kind=fmm_integer), allocatable:: a(:,:,:,:)
         integer(kind=fmm_integer), intent(in):: ia,ib,ja,jb,ka,kb,la,lb
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+itob*(ib-ia+1)*(jb-ja+1)*(kb-ka+1)*(lb-la+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib,ja:jb,ka:kb,la:lb),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib,ja:jb,ka:kb,la:lb),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocint4d
c
        subroutine fmmalloclog1d(a,ia,ib,istat)
         use fmmkinds
         implicit none
         logical(kind=fmm_logical), allocatable:: a(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+ltob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmalloclog1d
c
        subroutine fmmalloclog2d(a,ia,ib,ja,jb,istat)
         use fmmkinds
         implicit none
         logical(kind=fmm_logical), allocatable:: a(:,:)
         integer(kind=fmm_integer), intent(in):: ia,ib,ja,jb
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+ltob*(ib-ia+1)*(jb-ja+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib,ja:jb),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib,ja:jb),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmalloclog2d
c
        subroutine fmmallocbyte(a,ia,ib,istat)
         use fmmkinds
         implicit none
         integer(kind=1), allocatable:: a(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocbyte
c
        subroutine fmmallocbyte2d(a,ia,ib,ja,jb,istat)
         use fmmkinds
         implicit none
         integer(kind=1), allocatable:: a(:,:)
         integer(kind=fmm_integer), intent(in):: ia,ib,ja,jb
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+(ib-ia+1)*(jb-ja+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib,ja:jb),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib,ja:jb),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocbyte2d
c
        subroutine fmmallocbyte3d(a,ia,ib,ja,jb,ka,kb,istat)
         use fmmkinds
         implicit none
         integer(kind=1), allocatable:: a(:,:,:)
         integer(kind=fmm_integer), intent(in):: ia,ib,ja,jb,ka,kb
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+(ib-ia+1)*(jb-ja+1)*(kb-ka+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib,ja:jb,ka:kb),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib,ja:jb,ka:kb),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocbyte3d
c
#ifdef FMM_PARALLEL
        subroutine fmmallocgpt(gpt,ia,ib,istat)
         use fmmkinds
         implicit none
         type(c_ptr), allocatable, target:: gpt(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+gbpttob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(gpt(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(gpt(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocgpt
c
        subroutine fmmalloctypeint(typeint,ia,ib,istat)
         use fmmkinds
         implicit none
         type(tsndibox), allocatable:: typeint(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+typeinttob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(typeint(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(typeint(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmalloctypeint
c
#ifdef FMM_LOADSORT
        subroutine fmmalloctypeint2(typeint2,ia,ib,istat)
         use fmmkinds
         implicit none
         type(tsndiboxload), allocatable:: typeint2(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+typeint2tob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(typeint2(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(typeint2(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmalloctypeint2
#endif
c
#ifdef FMM_UNIFORMGRID
        subroutine fmmalloctypeuniformgrid(typeuniformgrid,ia,ib,istat)
         use fmmkinds
         implicit none
         type(tsnduniformgrid), allocatable:: typeuniformgrid(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+typeuniformgridtob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(typeuniformgrid(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(typeuniformgrid(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmalloctypeuniformgrid
#endif
c
        subroutine fmmalloctypeelem(typeelem,istat)
         use fmmkinds
         implicit none
         type(telem), pointer:: typeelem
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+typeelemtob
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(typeelem,stat = istat)
          else
           istat = 1
          endif
         else
          allocate(typeelem,stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmalloctypeelem
c
        subroutine fmmalloctypereal(typereal,ia,ib,istat)
         use fmmkinds
         implicit none
         type(tsndomegatree), allocatable:: typereal(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+typerealtob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(typereal(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(typereal(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmalloctypereal
c
        subroutine fmmalloctypeq(typeq,ia,ib,istat)
         use fmmkinds
         implicit none
         type(tsndq), allocatable:: typeq(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+typeqtob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(typeq(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(typeq(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmalloctypeq
c
        subroutine fmmalloctypexyz(typexyz,ia,ib,istat)
         use fmmkinds
         implicit none
         type(tsndxyz), allocatable:: typexyz(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+typexyztob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(typexyz(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(typexyz(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmalloctypexyz
#endif
c
        subroutine fmmdeallocdbl1d(a,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real), allocatable:: a(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-rtob*i
        end subroutine fmmdeallocdbl1d
c
#if FMM_REAL != FMM_REAL_EXTENDED
        subroutine fmmdeallocdblextended1d(a,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real_extended), allocatable:: a(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-rextendedtob*i
        end subroutine fmmdeallocdblextended1d
#endif
c
#if FMM_REAL != FMM_REAL_ITOR && FFMM_REAL_EXTENDED != FMM_REAL_ITOR
        subroutine fmmdeallocdblitor1d(a,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real_itor), allocatable:: a(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-ritortob*i
        end subroutine fmmdeallocdblitor1d
#endif
c
        subroutine fmmdeallocdbl2d(a,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real), allocatable:: a(:,:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-rtob*i
        end subroutine fmmdeallocdbl2d
c
        subroutine fmmdeallocdbl3d(a,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real), allocatable:: a(:,:,:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-rtob*i
        end subroutine fmmdeallocdbl3d
c
        subroutine fmmdeallocdbl4d(a,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real), allocatable:: a(:,:,:,:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-rtob*i
        end subroutine fmmdeallocdbl4d
c
        subroutine fmmdeallocdbl5d(a,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real), allocatable:: a(:,:,:,:,:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-rtob*i
        end subroutine fmmdeallocdbl5d
c
        subroutine fmmdeallocint1d(a,istat)
         use fmmkinds
         implicit none
         integer(kind=fmm_integer), allocatable:: a(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-itob*i
        end subroutine fmmdeallocint1d
c
#ifdef FMM_PARALLEL
#if FMM_INTEGER != FMM_MP_INTEGER_PROCESSES
        subroutine fmmdeallocintp(a,istat)
         use fmmkinds
         implicit none
         integer(kind=mp_integer_processes), allocatable:: a(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-iptob*i
        end subroutine fmmdeallocintp
#endif
#endif
c
        subroutine fmmdeallocint2d(a,istat)
         use fmmkinds
         implicit none
         integer(kind=fmm_integer), allocatable:: a(:,:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-itob*i
        end subroutine fmmdeallocint2d
c
        subroutine fmmdeallocint3d(a,istat)
         use fmmkinds
         implicit none
         integer(kind=fmm_integer), allocatable:: a(:,:,:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-itob*i
        end subroutine fmmdeallocint3d
c
        subroutine fmmdeallocint4d(a,istat)
         use fmmkinds
         implicit none
         integer(kind=fmm_integer), allocatable:: a(:,:,:,:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-itob*i
        end subroutine fmmdeallocint4d
c
        subroutine fmmdealloclog1d(a,istat)
         use fmmkinds
         implicit none
         logical(kind=fmm_logical), allocatable:: a(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-ltob*i
        end subroutine fmmdealloclog1d
c
        subroutine fmmdealloclog2d(a,istat)
         use fmmkinds
         implicit none
         logical(kind=fmm_logical), allocatable:: a(:,:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-ltob*i
        end subroutine fmmdealloclog2d
c
        subroutine fmmdeallocbyte(a,istat)
         use fmmkinds
         implicit none
         integer(kind=1), allocatable:: a(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-i
        end subroutine fmmdeallocbyte
c
        subroutine fmmdeallocbyte2d(a,istat)
         use fmmkinds
         implicit none
         integer(kind=1), allocatable:: a(:,:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-i
        end subroutine fmmdeallocbyte2d
c
        subroutine fmmdeallocbyte3d(a,istat)
         use fmmkinds
         implicit none
         integer(kind=1), allocatable:: a(:,:,:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-i
        end subroutine fmmdeallocbyte3d
c
#ifdef FMM_PARALLEL
        subroutine fmmdeallocgpt(gpt,istat)
         use fmmkinds
         implicit none
         type(c_ptr), allocatable, target:: gpt(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(gpt)
         deallocate(gpt,stat = istat)
         if(istat.eq.0) nalloc = nalloc-gbpttob*i
        end subroutine fmmdeallocgpt
c
        subroutine fmmdealloctypeint(typeint,istat)
         use fmmkinds
         implicit none
         type(tsndibox), allocatable:: typeint(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(typeint)
         deallocate(typeint,stat = istat)
         if(istat.eq.0) nalloc = nalloc-typeinttob*i
        end subroutine fmmdealloctypeint
c
#ifdef FMM_LOADSORT
        subroutine fmmdealloctypeint2(typeint2,istat)
         use fmmkinds
         implicit none
         type(tsndiboxload), allocatable:: typeint2(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(typeint2)
         deallocate(typeint2,stat = istat)
         if(istat.eq.0) nalloc = nalloc-typeint2tob*i
        end subroutine fmmdealloctypeint2
#endif
c
#ifdef FMM_UNIFORMGRID
        subroutine fmmdealloctypeuniformgrid(typeuniformgrid,istat)
         use fmmkinds
         implicit none
         type(tsnduniformgrid), allocatable:: typeuniformgrid(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(typeuniformgrid)
         deallocate(typeuniformgrid,stat = istat)
         if(istat.eq.0) nalloc = nalloc-typeuniformgridtob*i
        end subroutine fmmdealloctypeuniformgrid
#endif
c
        subroutine fmmdealloctypeelem(typeelem,istat)
         use fmmkinds
         implicit none
         type(telem), pointer:: typeelem
         integer(kind=fmm_integer), intent(out):: istat
         deallocate(typeelem,stat = istat)
         if(istat.eq.0) nalloc = nalloc-typeelemtob
        end subroutine fmmdealloctypeelem
c
        subroutine fmmdealloctypereal(typereal,istat)
         use fmmkinds
         implicit none
         type(tsndomegatree), allocatable:: typereal(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(typereal)
         deallocate(typereal,stat = istat)
         if(istat.eq.0) nalloc = nalloc-typerealtob*i
        end subroutine fmmdealloctypereal
c
        subroutine fmmdealloctypeq(typeq,istat)
         use fmmkinds
         implicit none
         type(tsndq), allocatable:: typeq(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(typeq)
         deallocate(typeq,stat = istat)
         if(istat.eq.0) nalloc = nalloc-typeqtob*i
        end subroutine fmmdealloctypeq
c
        subroutine fmmdealloctypexyz(typexyz,istat)
         use fmmkinds
         implicit none
         type(tsndxyz), allocatable:: typexyz(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(typexyz)
         deallocate(typexyz,stat = istat)
         if(istat.eq.0) nalloc = nalloc-typexyztob*i
        end subroutine fmmdealloctypexyz
#endif
c
        subroutine fmmallocdpt1d(a,ia,ib,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real), pointer:: a(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+rtob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocdpt1d
c
        subroutine fmmallocipt1d(a,ia,ib,istat)
         use fmmkinds
         implicit none
         integer(kind=fmm_integer), pointer:: a(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+itob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmallocipt1d
c
        subroutine fmmalloclpt1d(a,ia,ib,istat)
         use fmmkinds
         implicit none
         logical(kind=fmm_logical), pointer:: a(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+ltob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine fmmalloclpt1d
c
        subroutine fmmdeallocdpt1d(a,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real), pointer:: a(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         a => null()
         if(istat.eq.0) nalloc = nalloc-rtob*i
        end subroutine fmmdeallocdpt1d
c
        subroutine fmmdeallocipt1d(a,istat)
         use fmmkinds
         implicit none
         integer(kind=fmm_integer), pointer:: a(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         a => null()
         if(istat.eq.0) nalloc = nalloc-itob*i
        end subroutine fmmdeallocipt1d
c
        subroutine fmmdealloclpt1d(a,istat)
         use fmmkinds
         implicit none
         logical(kind=fmm_logical), pointer:: a(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         a => null()
         if(istat.eq.0) nalloc = nalloc-ltob*i
        end subroutine fmmdealloclpt1d
c
#ifdef FMM_PARALLEL
        subroutine mp_fmmalloc(gpt,n)
         use fmmkinds
         use mp_info, only: nnodes,mp_allocate,mp_error
         implicit none
         type(c_ptr), allocatable, target:: gpt(:)
         integer(kind=fmm_integer) n,i,j,k,istat
         if(n.ge.0) then
          i = nalloc+n
          j = nmp_alloc+n
          if(maxallocation.gt.0) then
           if(maxallocation.ge.i) then
            if(n.gt.0) then
             if(nnodes.gt.1) then
              k = n
             else
              k = -n
             endif
            else
             k = n
            endif
            call mp_allocate(gpt,k,istat)
           else
            istat = 1
           endif
          else
           if(n.gt.0) then
            if(nnodes.gt.1) then
             k = n
            else
             k = -n
            endif
           else
            k = n
           endif
           call mp_allocate(gpt,k,istat)
          endif
          if(istat.eq.0) then
           nalloc = i
           maxnalloc = max(maxnalloc,nalloc)
           sortmemory = max(sortmemory,maxnalloc)
           nmp_alloc = j
           maxnmp_alloc = max(maxnmp_alloc,nmp_alloc)
          else
#ifdef FMM_INFO
           write(6,*) ' allocated memory:',nalloc,'requested memory:',i
           call mp_error(istat)
#endif
          endif
         else
          call mp_error(n)
         endif
        end subroutine mp_fmmalloc
c
        subroutine mp_fmmdealloc(gpt,n)
         use fmmkinds
         use mp_info, only: nnodes,mp_deallocate,mp_error
         implicit none
         type(c_ptr) gpt
         integer(kind=fmm_integer) n,istat
         if(n.ge.0) then
          if(n.gt.0) then
           if(nnodes.gt.1) then
            istat = n
           else
            istat = -n
           endif
          else
           istat = n
          endif
          call mp_deallocate(gpt,istat)
          if(istat.eq.0) then
           nalloc = nalloc-n
           nmp_alloc = nmp_alloc-n
          else
           call mp_error(istat)
          endif
         else
          call mp_error(n)
         endif
        end subroutine mp_fmmdealloc
#endif
c
        subroutine indallocint1d(a,ia,ib,istat)
         use fmmkinds
         implicit none
         integer(kind=fmm_integer), allocatable:: a(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+itob*(ib-ia+1)
         if(corememory.gt.0) then
          if(corememory.ge.i) then
           allocate(a(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         endif
        end subroutine indallocint1d
c
        subroutine inddeallocint1d(a,istat)
         use fmmkinds
         implicit none
         integer(kind=fmm_integer), allocatable:: a(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-itob*i
        end subroutine inddeallocint1d
c
        subroutine indallocipt1d(a,ia,ib,istat)
         use fmmkinds
         implicit none
         integer(kind=fmm_integer), pointer:: a(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+itob*(ib-ia+1)
         if(corememory.gt.0) then
          if(corememory.ge.i) then
           allocate(a(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         endif
        end subroutine indallocipt1d
c
        subroutine inddeallocipt1d(a,istat)
         use fmmkinds
         implicit none
         integer(kind=fmm_integer), pointer:: a(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         a => null()
         if(istat.eq.0) nalloc = nalloc-itob*i
        end subroutine inddeallocipt1d
c
        subroutine srtallocbyte1d(a,ia,ib,istat)
         use fmmkinds
         implicit none
         integer(kind=1), allocatable:: a(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+(ib-ia+1)
         if(sortmemory.gt.0) then
          if(sortmemory.ge.i) then
           allocate(a(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         endif
        end subroutine srtallocbyte1d
c
        subroutine srtallocdbl2d(a,ia,ib,ja,jb,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real), allocatable:: a(:,:)
         integer(kind=fmm_integer), intent(in):: ia,ib,ja,jb
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+(ib-ia+1)*(jb-ja+1)
         if(sortmemory.gt.0) then
          if(sortmemory.ge.i) then
           allocate(a(ia:ib,ja:jb),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib,ja:jb),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         endif
        end subroutine srtallocdbl2d
c
        subroutine srtdeallocbyte1d(a,istat)
         use fmmkinds
         implicit none
         integer(kind=1), allocatable:: a(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-i
        end subroutine srtdeallocbyte1d
c
        subroutine srtdeallocdbl2d(a,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real), allocatable:: a(:,:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-i
        end subroutine srtdeallocdbl2d
c
        subroutine dbpallocdbl1d(a,ia,ib,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real_extended), allocatable:: a(:)
         integer(kind=fmm_integer), intent(in):: ia,ib
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+rtob*(ib-ia+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine dbpallocdbl1d
c
        subroutine dbpallocdbl3d(a,ia,ib,ja,jb,ka,kb,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real_extended), allocatable:: a(:,:,:)
         integer(kind=fmm_integer), intent(in):: ia,ib,ja,jb,ka,kb
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = nalloc+rtob*(ib-ia+1)*(jb-ja+1)*(kb-ka+1)
         if(maxallocation.gt.0) then
          if(maxallocation.ge.i) then
           allocate(a(ia:ib,ja:jb,ka:kb),stat = istat)
          else
           istat = 1
          endif
         else
          allocate(a(ia:ib,ja:jb,ka:kb),stat = istat)
         endif
         if(istat.eq.0) then
          nalloc = i
          maxnalloc = max(maxnalloc,nalloc)
          sortmemory = max(sortmemory,maxnalloc)
         else
#ifdef FMM_INFO
          write(6,*) ' allocated memory:',nalloc,'requested memory:',i
#endif
         endif
        end subroutine dbpallocdbl3d
c
        subroutine dbpdeallocdbl1d(a,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real_extended), allocatable:: a(:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-rtob*i
        end subroutine dbpdeallocdbl1d
c
        subroutine dbpdeallocdbl3d(a,istat)
         use fmmkinds
         implicit none
         real(kind=fmm_real_extended), allocatable:: a(:,:,:)
         integer(kind=fmm_integer), intent(out):: istat
         integer(kind=fmm_integer) i
         i = size(a)
         deallocate(a,stat = istat)
         if(istat.eq.0) nalloc = nalloc-rtob*i
        end subroutine dbpdeallocdbl3d
      end module fmmalloc
c
      module mnbox2int
       use fmmkinds
       implicit none
       integer(kind=fmm_integer) nbox2int
      end module mnbox2int
c
#ifndef FMM_NOFUNCTIONPOINTER
      module mfbox2int
       abstract interface
        subroutine fbox2int(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz
c
        end subroutine fbox2int
       end interface
c
       procedure(fbox2int), pointer:: box2int,box3int
      end module mfbox2int
#endif
c
#ifndef FMM_NOFUNCTIONPOINTER
      module mbox2int
       contains
        subroutine box2intn(jbox,intx,inty,intz)
c
        use fmmkinds
        use mnbox2int
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k,l,m,n
c
        if(jbox.gt.0) then
           i = jbox-1
           j = ishft(i,-1)
           k = ishft(j,-1)
           intx = iand(i,1)
           inty = iand(j,1)
           intz = iand(k,1)
           if(nbox2int.gt.2) then
              l = 2-nbox2int
              l = l+l
              m = 1
              do 1 n = -2,l,-2
                 m = m+m
                 intx = ior(intx,iand(ishft(i,n),m))
                 inty = ior(inty,iand(ishft(j,n),m))
                 intz = ior(intz,iand(ishft(k,n),m))
 1            continue
           endif
        else
           call bummer('box2intn: error, jbox = ',jbox)
        endif
        return
        end subroutine box2intn
c
        subroutine box2int1(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz
c
        intx = 0
        inty = 0
        intz = 0
        return
        end subroutine box2int1
c
        subroutine box2int2(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i
c
        i = jbox-1
        intx = iand(i,1)
        inty = iand(ishft(i,-1),1)
        intz = iand(ishft(i,-2),1)
        return
        end subroutine box2int2
c
        subroutine box2int3(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(iand(i,1),iand(ishft(i,-2),2))
        inty = ior(iand(j,1),iand(ishft(j,-2),2))
        intz = ior(iand(k,1),iand(ishft(k,-2),2))
        return
        end subroutine box2int3
c
        subroutine box2int4(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx=ior(ior(iand(i,1),iand(ishft(i,-2),2)),iand(ishft(i,-4),4))
        inty=ior(ior(iand(j,1),iand(ishft(j,-2),2)),iand(ishft(j,-4),4))
        intz=ior(ior(iand(k,1),iand(ishft(k,-2),2)),iand(ishft(k,-4),4))
        return
        end subroutine box2int4
c
        subroutine box2int5(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(iand(i,1),iand(ishft(i,-2),2)),
     .  iand(ishft(i,-4),4)),iand(ishft(i,-6),8))
        inty = ior(ior(ior(iand(j,1),iand(ishft(j,-2),2)),
     .  iand(ishft(j,-4),4)),iand(ishft(j,-6),8))
        intz = ior(ior(ior(iand(k,1),iand(ishft(k,-2),2)),
     .  iand(ishft(k,-4),4)),iand(ishft(k,-6),8))
        return
        end subroutine box2int5
c
        subroutine box2int6(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(iand(i,1),iand(ishft(i,-2),2)),
     .  iand(ishft(i,-4),4)),iand(ishft(i,-6),8)),iand(ishft(i,-8),16))
        inty = ior(ior(ior(ior(iand(j,1),iand(ishft(j,-2),2)),
     .  iand(ishft(j,-4),4)),iand(ishft(j,-6),8)),iand(ishft(j,-8),16))
        intz = ior(ior(ior(ior(iand(k,1),iand(ishft(k,-2),2)),
     .  iand(ishft(k,-4),4)),iand(ishft(k,-6),8)),iand(ishft(k,-8),16))
        return
        end subroutine box2int6
c
        subroutine box2int7(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(ior(iand(i,1),iand(ishft(i,-2),2)),
     .  iand(ishft(i,-4),4)),iand(ishft(i,-6),8)),iand(ishft(i,-8),16)),
     .  iand(ishft(i,-10),32))
        inty = ior(ior(ior(ior(ior(iand(j,1),iand(ishft(j,-2),2)),
     .  iand(ishft(j,-4),4)),iand(ishft(j,-6),8)),iand(ishft(j,-8),16)),
     .  iand(ishft(j,-10),32))
        intz = ior(ior(ior(ior(ior(iand(k,1),iand(ishft(k,-2),2)),
     .  iand(ishft(k,-4),4)),iand(ishft(k,-6),8)),iand(ishft(k,-8),16)),
     .  iand(ishft(k,-10),32))
        return
        end subroutine box2int7
c
        subroutine box2int8(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(ior(ior(iand(i,1),iand(ishft(i,-2),2)),
     .  iand(ishft(i,-4),4)),iand(ishft(i,-6),8)),iand(ishft(i,-8),16)),
     .  iand(ishft(i,-10),32)),iand(ishft(i,-12),64))
        inty = ior(ior(ior(ior(ior(ior(iand(j,1),iand(ishft(j,-2),2)),
     .  iand(ishft(j,-4),4)),iand(ishft(j,-6),8)),iand(ishft(j,-8),16)),
     .  iand(ishft(j,-10),32)),iand(ishft(j,-12),64))
        intz = ior(ior(ior(ior(ior(ior(iand(k,1),iand(ishft(k,-2),2)),
     .  iand(ishft(k,-4),4)),iand(ishft(k,-6),8)),iand(ishft(k,-8),16)),
     .  iand(ishft(k,-10),32)),iand(ishft(k,-12),64))
        return
        end subroutine box2int8
c
        subroutine box2int9(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(ior(ior(ior(iand(i,1),
     .  iand(ishft(i,-2),2)),iand(ishft(i,-4),4)),iand(ishft(i,-6),8)),
     .  iand(ishft(i,-8),16)),iand(ishft(i,-10),32)),
     .  iand(ishft(i,-12),64)),iand(ishft(i,-14),128))
        inty = ior(ior(ior(ior(ior(ior(ior(iand(j,1),
     .  iand(ishft(j,-2),2)),iand(ishft(j,-4),4)),iand(ishft(j,-6),8)),
     .  iand(ishft(j,-8),16)),iand(ishft(j,-10),32)),
     .  iand(ishft(j,-12),64)),iand(ishft(j,-14),128))
        intz = ior(ior(ior(ior(ior(ior(ior(iand(k,1),
     .  iand(ishft(k,-2),2)),iand(ishft(k,-4),4)),iand(ishft(k,-6),8)),
     .  iand(ishft(k,-8),16)),iand(ishft(k,-10),32)),
     .  iand(ishft(k,-12),64)),iand(ishft(k,-14),128))
        return
        end subroutine box2int9
c
        subroutine box2int10(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(ior(ior(ior(ior(iand(i,1),
     .  iand(ishft(i,-2),2)),iand(ishft(i,-4),4)),iand(ishft(i,-6),8)),
     .  iand(ishft(i,-8),16)),iand(ishft(i,-10),32)),
     .  iand(ishft(i,-12),64)),iand(ishft(i,-14),128)),
     .  iand(ishft(i,-16),256))
        inty = ior(ior(ior(ior(ior(ior(ior(ior(iand(j,1),
     .  iand(ishft(j,-2),2)),iand(ishft(j,-4),4)),iand(ishft(j,-6),8)),
     .  iand(ishft(j,-8),16)),iand(ishft(j,-10),32)),
     .  iand(ishft(j,-12),64)),iand(ishft(j,-14),128)),
     .  iand(ishft(j,-16),256))
        intz = ior(ior(ior(ior(ior(ior(ior(ior(iand(k,1),
     .  iand(ishft(k,-2),2)),iand(ishft(k,-4),4)),iand(ishft(k,-6),8)),
     .  iand(ishft(k,-8),16)),iand(ishft(k,-10),32)),
     .  iand(ishft(k,-12),64)),iand(ishft(k,-14),128)),
     .  iand(ishft(k,-16),256))
        return
        end subroutine box2int10
c
        subroutine box2int11(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(ior(ior(ior(ior(ior(iand(i,1),
     .  iand(ishft(i,-2),2)),iand(ishft(i,-4),4)),iand(ishft(i,-6),8)),
     .  iand(ishft(i,-8),16)),iand(ishft(i,-10),32)),
     .  iand(ishft(i,-12),64)),iand(ishft(i,-14),128)),
     .  iand(ishft(i,-16),256)),iand(ishft(i,-18),512))
        inty = ior(ior(ior(ior(ior(ior(ior(ior(ior(iand(j,1),
     .  iand(ishft(j,-2),2)),iand(ishft(j,-4),4)),iand(ishft(j,-6),8)),
     .  iand(ishft(j,-8),16)),iand(ishft(j,-10),32)),
     .  iand(ishft(j,-12),64)),iand(ishft(j,-14),128)),
     .  iand(ishft(j,-16),256)),iand(ishft(j,-18),512))
        intz = ior(ior(ior(ior(ior(ior(ior(ior(ior(iand(k,1),
     .  iand(ishft(k,-2),2)),iand(ishft(k,-4),4)),iand(ishft(k,-6),8)),
     .  iand(ishft(k,-8),16)),iand(ishft(k,-10),32)),
     .  iand(ishft(k,-12),64)),iand(ishft(k,-14),128)),
     .  iand(ishft(k,-16),256)),iand(ishft(k,-18),512))
        return
        end subroutine box2int11
c
        subroutine box2int12(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(iand(i,1),
     .  iand(ishft(i,-2),2)),iand(ishft(i,-4),4)),iand(ishft(i,-6),8)),
     .  iand(ishft(i,-8),16)),iand(ishft(i,-10),32)),
     .  iand(ishft(i,-12),64)),iand(ishft(i,-14),128)),
     .  iand(ishft(i,-16),256)),iand(ishft(i,-18),512)),
     .  iand(ishft(i,-20),1024))
        inty = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(iand(j,1),
     .  iand(ishft(j,-2),2)),iand(ishft(j,-4),4)),iand(ishft(j,-6),8)),
     .  iand(ishft(j,-8),16)),iand(ishft(j,-10),32)),
     .  iand(ishft(j,-12),64)),iand(ishft(j,-14),128)),
     .  iand(ishft(j,-16),256)),iand(ishft(j,-18),512)),
     .  iand(ishft(j,-20),1024))
        intz = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(iand(k,1),
     .  iand(ishft(k,-2),2)),iand(ishft(k,-4),4)),iand(ishft(k,-6),8)),
     .  iand(ishft(k,-8),16)),iand(ishft(k,-10),32)),
     .  iand(ishft(k,-12),64)),iand(ishft(k,-14),128)),
     .  iand(ishft(k,-16),256)),iand(ishft(k,-18),512)),
     .  iand(ishft(k,-20),1024))
        return
        end subroutine box2int12
c
        subroutine box2int13(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(iand(i,1),
     .  iand(ishft(i,-2),2)),iand(ishft(i,-4),4)),iand(ishft(i,-6),8)),
     .  iand(ishft(i,-8),16)),iand(ishft(i,-10),32)),
     .  iand(ishft(i,-12),64)),iand(ishft(i,-14),128)),
     .  iand(ishft(i,-16),256)),iand(ishft(i,-18),512)),
     .  iand(ishft(i,-20),1024)),iand(ishft(i,-22),2048))
        inty = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(iand(j,1),
     .  iand(ishft(j,-2),2)),iand(ishft(j,-4),4)),iand(ishft(j,-6),8)),
     .  iand(ishft(j,-8),16)),iand(ishft(j,-10),32)),
     .  iand(ishft(j,-12),64)),iand(ishft(j,-14),128)),
     .  iand(ishft(j,-16),256)),iand(ishft(j,-18),512)),
     .  iand(ishft(j,-20),1024)),iand(ishft(j,-22),2048))
        intz = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(iand(k,1),
     .  iand(ishft(k,-2),2)),iand(ishft(k,-4),4)),iand(ishft(k,-6),8)),
     .  iand(ishft(k,-8),16)),iand(ishft(k,-10),32)),
     .  iand(ishft(k,-12),64)),iand(ishft(k,-14),128)),
     .  iand(ishft(k,-16),256)),iand(ishft(k,-18),512)),
     .  iand(ishft(k,-20),1024)),iand(ishft(k,-22),2048))
        return
        end subroutine box2int13
c
        subroutine box2int14(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx=ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(iand(i,1),
     .  iand(ishft(i,-2),2)),iand(ishft(i,-4),4)),iand(ishft(i,-6),8)),
     .  iand(ishft(i,-8),16)),iand(ishft(i,-10),32)),
     .  iand(ishft(i,-12),64)),iand(ishft(i,-14),128)),
     .  iand(ishft(i,-16),256)),iand(ishft(i,-18),512)),
     .  iand(ishft(i,-20),1024)),iand(ishft(i,-22),2048)),
     .  iand(ishft(i,-24),4096))
        inty=ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(iand(j,1),
     .  iand(ishft(j,-2),2)),iand(ishft(j,-4),4)),iand(ishft(j,-6),8)),
     .  iand(ishft(j,-8),16)),iand(ishft(j,-10),32)),
     .  iand(ishft(j,-12),64)),iand(ishft(j,-14),128)),
     .  iand(ishft(j,-16),256)),iand(ishft(j,-18),512)),
     .  iand(ishft(j,-20),1024)),iand(ishft(j,-22),2048)),
     .  iand(ishft(j,-24),4096))
        intz=ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(iand(k,1),
     .  iand(ishft(k,-2),2)),iand(ishft(k,-4),4)),iand(ishft(k,-6),8)),
     .  iand(ishft(k,-8),16)),iand(ishft(k,-10),32)),
     .  iand(ishft(k,-12),64)),iand(ishft(k,-14),128)),
     .  iand(ishft(k,-16),256)),iand(ishft(k,-18),512)),
     .  iand(ishft(k,-20),1024)),iand(ishft(k,-22),2048)),
     .  iand(ishft(k,-24),4096))
        return
        end subroutine box2int14
c
        subroutine box2int15(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  iand(i,1),iand(ishft(i,-2),2)),iand(ishft(i,-4),4)),
     .  iand(ishft(i,-6),8)),iand(ishft(i,-8),16)),
     .  iand(ishft(i,-10),32)),iand(ishft(i,-12),64)),
     .  iand(ishft(i,-14),128)),iand(ishft(i,-16),256)),
     .  iand(ishft(i,-18),512)),iand(ishft(i,-20),1024)),
     .  iand(ishft(i,-22),2048)),iand(ishft(i,-24),4096)),
     .  iand(ishft(i,-26),8192))
        inty = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  iand(j,1),iand(ishft(j,-2),2)),iand(ishft(j,-4),4)),
     .  iand(ishft(j,-6),8)),iand(ishft(j,-8),16)),
     .  iand(ishft(j,-10),32)),iand(ishft(j,-12),64)),
     .  iand(ishft(j,-14),128)),iand(ishft(j,-16),256)),
     .  iand(ishft(j,-18),512)),iand(ishft(j,-20),1024)),
     .  iand(ishft(j,-22),2048)),iand(ishft(j,-24),4096)),
     .  iand(ishft(j,-26),8192))
        intz = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  iand(k,1),iand(ishft(k,-2),2)),iand(ishft(k,-4),4)),
     .  iand(ishft(k,-6),8)),iand(ishft(k,-8),16)),
     .  iand(ishft(k,-10),32)),iand(ishft(k,-12),64)),
     .  iand(ishft(k,-14),128)),iand(ishft(k,-16),256)),
     .  iand(ishft(k,-18),512)),iand(ishft(k,-20),1024)),
     .  iand(ishft(k,-22),2048)),iand(ishft(k,-24),4096)),
     .  iand(ishft(k,-26),8192))
        return
        end subroutine box2int15
c
        subroutine box2int16(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  iand(i,1),iand(ishft(i,-2),2)),iand(ishft(i,-4),4)),
     .  iand(ishft(i,-6),8)),iand(ishft(i,-8),16)),
     .  iand(ishft(i,-10),32)),iand(ishft(i,-12),64)),
     .  iand(ishft(i,-14),128)),iand(ishft(i,-16),256)),
     .  iand(ishft(i,-18),512)),iand(ishft(i,-20),1024)),
     .  iand(ishft(i,-22),2048)),iand(ishft(i,-24),4096)),
     .  iand(ishft(i,-26),8192)),iand(ishft(i,-28),16384))
        inty = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  iand(j,1),iand(ishft(j,-2),2)),iand(ishft(j,-4),4)),
     .  iand(ishft(j,-6),8)),iand(ishft(j,-8),16)),
     .  iand(ishft(j,-10),32)),iand(ishft(j,-12),64)),
     .  iand(ishft(j,-14),128)),iand(ishft(j,-16),256)),
     .  iand(ishft(j,-18),512)),iand(ishft(j,-20),1024)),
     .  iand(ishft(j,-22),2048)),iand(ishft(j,-24),4096)),
     .  iand(ishft(j,-26),8192)),iand(ishft(j,-28),16384))
        intz = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  iand(k,1),iand(ishft(k,-2),2)),iand(ishft(k,-4),4)),
     .  iand(ishft(k,-6),8)),iand(ishft(k,-8),16)),
     .  iand(ishft(k,-10),32)),iand(ishft(k,-12),64)),
     .  iand(ishft(k,-14),128)),iand(ishft(k,-16),256)),
     .  iand(ishft(k,-18),512)),iand(ishft(k,-20),1024)),
     .  iand(ishft(k,-22),2048)),iand(ishft(k,-24),4096)),
     .  iand(ishft(k,-26),8192)),iand(ishft(k,-28),16384))
        return
        end subroutine box2int16
c
        subroutine box2int17(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(iand(i,1),iand(ishft(i,-2),2)),iand(ishft(i,-4),4)),
     .  iand(ishft(i,-6),8)),iand(ishft(i,-8),16)),
     .  iand(ishft(i,-10),32)),iand(ishft(i,-12),64)),
     .  iand(ishft(i,-14),128)),iand(ishft(i,-16),256)),
     .  iand(ishft(i,-18),512)),iand(ishft(i,-20),1024)),
     .  iand(ishft(i,-22),2048)),iand(ishft(i,-24),4096)),
     .  iand(ishft(i,-26),8192)),iand(ishft(i,-28),16384)),
     .  iand(ishft(i,-30),32768))
        inty = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(iand(j,1),iand(ishft(j,-2),2)),iand(ishft(j,-4),4)),
     .  iand(ishft(j,-6),8)),iand(ishft(j,-8),16)),
     .  iand(ishft(j,-10),32)),iand(ishft(j,-12),64)),
     .  iand(ishft(j,-14),128)),iand(ishft(j,-16),256)),
     .  iand(ishft(j,-18),512)),iand(ishft(j,-20),1024)),
     .  iand(ishft(j,-22),2048)),iand(ishft(j,-24),4096)),
     .  iand(ishft(j,-26),8192)),iand(ishft(j,-28),16384)),
     .  iand(ishft(j,-30),32768))
        intz = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(iand(k,1),iand(ishft(k,-2),2)),iand(ishft(k,-4),4)),
     .  iand(ishft(k,-6),8)),iand(ishft(k,-8),16)),
     .  iand(ishft(k,-10),32)),iand(ishft(k,-12),64)),
     .  iand(ishft(k,-14),128)),iand(ishft(k,-16),256)),
     .  iand(ishft(k,-18),512)),iand(ishft(k,-20),1024)),
     .  iand(ishft(k,-22),2048)),iand(ishft(k,-24),4096)),
     .  iand(ishft(k,-26),8192)),iand(ishft(k,-28),16384)),
     .  iand(ishft(k,-30),32768))
        return
        end subroutine box2int17
c
        subroutine box2int18(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(ior(iand(i,1),iand(ishft(i,-2),2)),iand(ishft(i,-4),4)),
     .  iand(ishft(i,-6),8)),iand(ishft(i,-8),16)),
     .  iand(ishft(i,-10),32)),iand(ishft(i,-12),64)),
     .  iand(ishft(i,-14),128)),iand(ishft(i,-16),256)),
     .  iand(ishft(i,-18),512)),iand(ishft(i,-20),1024)),
     .  iand(ishft(i,-22),2048)),iand(ishft(i,-24),4096)),
     .  iand(ishft(i,-26),8192)),iand(ishft(i,-28),16384)),
     .  iand(ishft(i,-30),32768)),iand(ishft(i,-32),65536))
        inty = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(ior(iand(j,1),iand(ishft(j,-2),2)),iand(ishft(j,-4),4)),
     .  iand(ishft(j,-6),8)),iand(ishft(j,-8),16)),
     .  iand(ishft(j,-10),32)),iand(ishft(j,-12),64)),
     .  iand(ishft(j,-14),128)),iand(ishft(j,-16),256)),
     .  iand(ishft(j,-18),512)),iand(ishft(j,-20),1024)),
     .  iand(ishft(j,-22),2048)),iand(ishft(j,-24),4096)),
     .  iand(ishft(j,-26),8192)),iand(ishft(j,-28),16384)),
     .  iand(ishft(j,-30),32768)),iand(ishft(j,-32),65536))
        intz = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(ior(iand(k,1),iand(ishft(k,-2),2)),iand(ishft(k,-4),4)),
     .  iand(ishft(k,-6),8)),iand(ishft(k,-8),16)),
     .  iand(ishft(k,-10),32)),iand(ishft(k,-12),64)),
     .  iand(ishft(k,-14),128)),iand(ishft(k,-16),256)),
     .  iand(ishft(k,-18),512)),iand(ishft(k,-20),1024)),
     .  iand(ishft(k,-22),2048)),iand(ishft(k,-24),4096)),
     .  iand(ishft(k,-26),8192)),iand(ishft(k,-28),16384)),
     .  iand(ishft(k,-30),32768)),iand(ishft(k,-32),65536))
        return
        end subroutine box2int18
c
        subroutine box2int19(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(ior(ior(iand(i,1),iand(ishft(i,-2),2)),iand(ishft(i,-4),4)),
     .  iand(ishft(i,-6),8)),iand(ishft(i,-8),16)),
     .  iand(ishft(i,-10),32)),iand(ishft(i,-12),64)),
     .  iand(ishft(i,-14),128)),iand(ishft(i,-16),256)),
     .  iand(ishft(i,-18),512)),iand(ishft(i,-20),1024)),
     .  iand(ishft(i,-22),2048)),iand(ishft(i,-24),4096)),
     .  iand(ishft(i,-26),8192)),iand(ishft(i,-28),16384)),
     .  iand(ishft(i,-30),32768)),iand(ishft(i,-32),65536)),
     .  iand(ishft(i,-34),131072))
        inty = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(ior(ior(iand(j,1),iand(ishft(j,-2),2)),iand(ishft(j,-4),4)),
     .  iand(ishft(j,-6),8)),iand(ishft(j,-8),16)),
     .  iand(ishft(j,-10),32)),iand(ishft(j,-12),64)),
     .  iand(ishft(j,-14),128)),iand(ishft(j,-16),256)),
     .  iand(ishft(j,-18),512)),iand(ishft(j,-20),1024)),
     .  iand(ishft(j,-22),2048)),iand(ishft(j,-24),4096)),
     .  iand(ishft(j,-26),8192)),iand(ishft(j,-28),16384)),
     .  iand(ishft(j,-30),32768)),iand(ishft(j,-32),65536)),
     .  iand(ishft(j,-34),131072))
        intz = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(ior(ior(iand(k,1),iand(ishft(k,-2),2)),iand(ishft(k,-4),4)),
     .  iand(ishft(k,-6),8)),iand(ishft(k,-8),16)),
     .  iand(ishft(k,-10),32)),iand(ishft(k,-12),64)),
     .  iand(ishft(k,-14),128)),iand(ishft(k,-16),256)),
     .  iand(ishft(k,-18),512)),iand(ishft(k,-20),1024)),
     .  iand(ishft(k,-22),2048)),iand(ishft(k,-24),4096)),
     .  iand(ishft(k,-26),8192)),iand(ishft(k,-28),16384)),
     .  iand(ishft(k,-30),32768)),iand(ishft(k,-32),65536)),
     .  iand(ishft(k,-34),131072))
        return
        end subroutine box2int19
c
        subroutine box2int20(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(ior(ior(ior(iand(i,1),iand(ishft(i,-2),2)),
     .  iand(ishft(i,-4),4)),iand(ishft(i,-6),8)),iand(ishft(i,-8),16)),
     .  iand(ishft(i,-10),32)),iand(ishft(i,-12),64)),
     .  iand(ishft(i,-14),128)),iand(ishft(i,-16),256)),
     .  iand(ishft(i,-18),512)),iand(ishft(i,-20),1024)),
     .  iand(ishft(i,-22),2048)),iand(ishft(i,-24),4096)),
     .  iand(ishft(i,-26),8192)),iand(ishft(i,-28),16384)),
     .  iand(ishft(i,-30),32768)),iand(ishft(i,-32),65536)),
     .  iand(ishft(i,-34),131072)),iand(ishft(i,-36),262144))
        inty = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(ior(ior(ior(iand(j,1),iand(ishft(j,-2),2)),
     .  iand(ishft(j,-4),4)),iand(ishft(j,-6),8)),iand(ishft(j,-8),16)),
     .  iand(ishft(j,-10),32)),iand(ishft(j,-12),64)),
     .  iand(ishft(j,-14),128)),iand(ishft(j,-16),256)),
     .  iand(ishft(j,-18),512)),iand(ishft(j,-20),1024)),
     .  iand(ishft(j,-22),2048)),iand(ishft(j,-24),4096)),
     .  iand(ishft(j,-26),8192)),iand(ishft(j,-28),16384)),
     .  iand(ishft(j,-30),32768)),iand(ishft(j,-32),65536)),
     .  iand(ishft(j,-34),131072)),iand(ishft(j,-36),262144))
        intz = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(ior(ior(ior(iand(k,1),iand(ishft(k,-2),2)),
     .  iand(ishft(k,-4),4)),iand(ishft(k,-6),8)),iand(ishft(k,-8),16)),
     .  iand(ishft(k,-10),32)),iand(ishft(k,-12),64)),
     .  iand(ishft(k,-14),128)),iand(ishft(k,-16),256)),
     .  iand(ishft(k,-18),512)),iand(ishft(k,-20),1024)),
     .  iand(ishft(k,-22),2048)),iand(ishft(k,-24),4096)),
     .  iand(ishft(k,-26),8192)),iand(ishft(k,-28),16384)),
     .  iand(ishft(k,-30),32768)),iand(ishft(k,-32),65536)),
     .  iand(ishft(k,-34),131072)),iand(ishft(k,-36),262144))
        return
        end subroutine box2int20
c
        subroutine box2int21(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(ior(ior(ior(ior(iand(i,1),iand(ishft(i,-2),2)),
     .  iand(ishft(i,-4),4)),iand(ishft(i,-6),8)),iand(ishft(i,-8),16)),
     .  iand(ishft(i,-10),32)),iand(ishft(i,-12),64)),
     .  iand(ishft(i,-14),128)),iand(ishft(i,-16),256)),
     .  iand(ishft(i,-18),512)),iand(ishft(i,-20),1024)),
     .  iand(ishft(i,-22),2048)),iand(ishft(i,-24),4096)),
     .  iand(ishft(i,-26),8192)),iand(ishft(i,-28),16384)),
     .  iand(ishft(i,-30),32768)),iand(ishft(i,-32),65536)),
     .  iand(ishft(i,-34),131072)),iand(ishft(i,-36),262144)),
     .  iand(ishft(i,-38),524288))
        inty = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(ior(ior(ior(ior(iand(j,1),iand(ishft(j,-2),2)),
     .  iand(ishft(j,-4),4)),iand(ishft(j,-6),8)),iand(ishft(j,-8),16)),
     .  iand(ishft(j,-10),32)),iand(ishft(j,-12),64)),
     .  iand(ishft(j,-14),128)),iand(ishft(j,-16),256)),
     .  iand(ishft(j,-18),512)),iand(ishft(j,-20),1024)),
     .  iand(ishft(j,-22),2048)),iand(ishft(j,-24),4096)),
     .  iand(ishft(j,-26),8192)),iand(ishft(j,-28),16384)),
     .  iand(ishft(j,-30),32768)),iand(ishft(j,-32),65536)),
     .  iand(ishft(j,-34),131072)),iand(ishft(j,-36),262144)),
     .  iand(ishft(j,-38),524288))
        intz = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(ior(ior(ior(ior(iand(k,1),iand(ishft(k,-2),2)),
     .  iand(ishft(k,-4),4)),iand(ishft(k,-6),8)),iand(ishft(k,-8),16)),
     .  iand(ishft(k,-10),32)),iand(ishft(k,-12),64)),
     .  iand(ishft(k,-14),128)),iand(ishft(k,-16),256)),
     .  iand(ishft(k,-18),512)),iand(ishft(k,-20),1024)),
     .  iand(ishft(k,-22),2048)),iand(ishft(k,-24),4096)),
     .  iand(ishft(k,-26),8192)),iand(ishft(k,-28),16384)),
     .  iand(ishft(k,-30),32768)),iand(ishft(k,-32),65536)),
     .  iand(ishft(k,-34),131072)),iand(ishft(k,-36),262144)),
     .  iand(ishft(k,-38),524288))
        return
        end subroutine box2int21
c
        subroutine box2int22(jbox,intx,inty,intz)
c
        use fmmkinds
c
        implicit none
c
        integer(kind=fmm_integer) jbox,intx,inty,intz,i,j,k
c
        i = jbox-1
        j = ishft(i,-1)
        k = ishft(j,-1)
        intx = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(ior(ior(ior(ior(ior(iand(i,1),iand(ishft(i,-2),2)),
     .  iand(ishft(i,-4),4)),iand(ishft(i,-6),8)),iand(ishft(i,-8),16)),
     .  iand(ishft(i,-10),32)),iand(ishft(i,-12),64)),
     .  iand(ishft(i,-14),128)),iand(ishft(i,-16),256)),
     .  iand(ishft(i,-18),512)),iand(ishft(i,-20),1024)),
     .  iand(ishft(i,-22),2048)),iand(ishft(i,-24),4096)),
     .  iand(ishft(i,-26),8192)),iand(ishft(i,-28),16384)),
     .  iand(ishft(i,-30),32768)),iand(ishft(i,-32),65536)),
     .  iand(ishft(i,-34),131072)),iand(ishft(i,-36),262144)),
     .  iand(ishft(i,-38),524288)),iand(ishft(i,-40),1048576))
        inty = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(ior(ior(ior(ior(ior(iand(j,1),iand(ishft(j,-2),2)),
     .  iand(ishft(j,-4),4)),iand(ishft(j,-6),8)),iand(ishft(j,-8),16)),
     .  iand(ishft(j,-10),32)),iand(ishft(j,-12),64)),
     .  iand(ishft(j,-14),128)),iand(ishft(j,-16),256)),
     .  iand(ishft(j,-18),512)),iand(ishft(j,-20),1024)),
     .  iand(ishft(j,-22),2048)),iand(ishft(j,-24),4096)),
     .  iand(ishft(j,-26),8192)),iand(ishft(j,-28),16384)),
     .  iand(ishft(j,-30),32768)),iand(ishft(j,-32),65536)),
     .  iand(ishft(j,-34),131072)),iand(ishft(j,-36),262144)),
     .  iand(ishft(j,-38),524288)),iand(ishft(j,-40),1048576))
        intz = ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(ior(
     .  ior(ior(ior(ior(ior(ior(iand(k,1),iand(ishft(k,-2),2)),
     .  iand(ishft(k,-4),4)),iand(ishft(k,-6),8)),iand(ishft(k,-8),16)),
     .  iand(ishft(k,-10),32)),iand(ishft(k,-12),64)),
     .  iand(ishft(k,-14),128)),iand(ishft(k,-16),256)),
     .  iand(ishft(k,-18),512)),iand(ishft(k,-20),1024)),
     .  iand(ishft(k,-22),2048)),iand(ishft(k,-24),4096)),
     .  iand(ishft(k,-26),8192)),iand(ishft(k,-28),16384)),
     .  iand(ishft(k,-30),32768)),iand(ishft(k,-32),65536)),
     .  iand(ishft(k,-34),131072)),iand(ishft(k,-36),262144)),
     .  iand(ishft(k,-38),524288)),iand(ishft(k,-40),1048576))
        return
        end subroutine box2int22
      end module mbox2int
#endif



