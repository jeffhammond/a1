
#include <stdio.h>
#include <math.h>

#ifdef SL_USE_MPI
# include <mpi.h>
#endif

#include "test_fmm.h"


void fmm_run(fmm_info_t *nfo)
{
  fmm_data_t fmm;
  
  pint_t subx = 0;
  
  double frac = 1.0;
  
  double t;
  double ts[1 + 2 * nfo->rounds];

  INTEGER_C *addr;

  pint_t i, r;

#if defined(JUGENE) && defined(SL_USE_MPI)
  unsigned bg_last[4];
#endif

  for (i = 0; i < 1 + 2 * nfo->rounds; ++i) ts[i] = 0;

#if defined(JUGENE) && defined(SL_USE_MPI)
  MPIX_rank2torus(comm_size - 1, &bg_last[0], &bg_last[1], &bg_last[2], &bg_last[3]);
  z_printf("test-" MODSTR "-fmm, BG: %ux%ux%ux%u\n", bg_last[0] + 1, bg_last[1] + 1, bg_last[2] + 1, bg_last[3] + 1);
#else
  z_printf("test-" MODSTR "-fmm\n");
#endif

  fmm_create(&fmm, nfo);

  z_printf("init: %" PARAM_INTEGER_FMT " particles\n", fmm.nglobal);
  fmm_init_particles(&fmm);
  z_printf("init: done\n");

  r = 0;
  
  while (1)
  {
/*    fmm_print(&fmm, NULL);*/

    if (nfo->sort_front == 1 || nfo->sort_front == 3)
    {
      z_printf("ibox: depth 2\n");
      fmm_inc_depth(&fmm);
      fmm_inc_depth(&fmm);
      z_printf("ibox: done\n");

      z_printf("sort green\n");
      t = z_wtime();
      SELECTSORT(fmm_sort_front_mem)(fmm.mem0, fmm.mem1, fmm.mem_sizes, &fmm.depth, &subx, &fmm.nlocal, fmm.ibox, fmm.xyz, fmm.q, fmm.addr_desc, fmm.addr, fmm.scr, NULL);
      t = z_wtime() - t;
      z_printf("sort green: %f\n", t);

      ts[2 * r + 0] += t;

      while (fmm.depth < nfo->max_depth)
      {
        z_printf("ibox: depth %" PARAM_INTEGER_FMT ":\n", fmm.depth + 1);
        fmm_inc_depth(&fmm);
        z_printf("ibox: done\n");

        z_printf("sort green 3bit\n");
        t = z_wtime();
        SELECTSORT(fmm_sort_front_3bit_mem)(fmm.mem0, fmm.mem1, fmm.mem_sizes, &subx, &fmm.nlocal, fmm.ibox, fmm.xyz, fmm.q, fmm.addr_desc, fmm.addr, fmm.scr, NULL);
        t = z_wtime() - t;
        z_printf("sort green 3bit: %f\n", t);

        ts[2 * r + 0] += t;
      }

/*      fmm_print(&fmm, NULL);*/
    }

    if (nfo->sort_front == 2 || nfo->sort_front == 3)
    {
      frac = nfo->frac;

      z_printf("scale + ibox: depth %" PARAM_INTEGER_FMT "\n", nfo->depth);
      fmm_scalecoordinates(&fmm, nfo->frac, nfo->depth);
      z_printf("scale + ibox: done\n");

      z_printf("sort blue\n");
      t = z_wtime();
      SELECTSORT(fmm_sort_front_mem)(fmm.mem0, fmm.mem1, fmm.mem_sizes, &fmm.depth, &subx, &fmm.nlocal, fmm.ibox, fmm.xyz, fmm.q, fmm.addr_desc, fmm.addr, NULL, NULL);
      t = z_wtime() - t;
      z_printf("sort blue: %f\n", t);

      ts[2 * r + 1] += t;

/*      fmm_print(&fmm, NULL);*/
    }

    z_printf("calculate\n");
    fmm_calculate(&fmm);
    z_printf("calculate: done\n");

    ++r;

    if (r >= nfo->rounds) break;
    
    z_printf("unscale\n");
    fmm_unscalecoordinates(&fmm, frac);
    z_printf("unscale: done\n");
    
    z_printf("move\n");
    fmm_move_particles(&fmm, fmm.nlocal * nfo->move_rate);
    z_printf("move: done\n");
    
    nfo->move_rate = 0;
  }

  r *= 2;

  if (nfo->sort_back && nfo->results)
  {
    z_printf("restore\n");
    fmm_restore(&fmm, nfo->frac, &addr);
    z_printf("restore: done\n");

    z_printf("sort back\n");
    t = z_wtime();
    SELECTSORT(fmm_sort_back_mem)(fmm.mem0, fmm.mem1, fmm.mem_sizes, NTOTALN(&fmm.nglobal, &fmm.nlocal), addr, fmm.q, fmm.xyz, fmm.pot, fmm.grad, NULL);
    t = z_wtime() - t;
    z_printf("sort back: %f\n", t);

    ts[r] += t;
  }

  ++r;

/*  fmm_print(&fmm, addr);*/

  if (comm_rank == 0)
  {
    printf("%d  %" PARAM_INTEGER_FMT, comm_size, fmm.nglobal);
    for (i = 0; i < r; ++i) printf("  %f", ts[i]);
    printf("\n");
  }

  fmm_destroy(&fmm);
}


void test_fmm(int argc, char *argv[])
{
#ifdef SL_USE_MPI

  fmm_info_t nfo;

  nfo.nparticles = -1;
  if (argc > 0) nfo.nparticles_per_process = strtoll(argv[0], NULL, 10);
  else nfo.nparticles_per_process = 100000;
  nfo.max_depth = nfo.depth = ceil(log(nfo.nparticles_per_process / 10 * comm_size) / log(8.0));
  nfo.frac = 1.0;
  nfo.addr_byte = 2;
  nfo.addr_iboxbits = ceil(log(nfo.nparticles_per_process * comm_size) / log(2.0)) - (nfo.addr_byte * 8); if (nfo.addr_iboxbits < 0) nfo.addr_iboxbits = 0;
  nfo.max_mem = 460*1000*1000;
  nfo.grad_mem = 1;

  nfo.scratch = 0;
  nfo.results = 1;
  nfo.sort_front = 2;
  nfo.sort_back = 1;

  nfo.move_rate = 0.01;
  nfo.rounds = 3;

#else

  fmm_info_t nfo;

  if (argc > 0) nfo.nparticles = strtoll(argv[0], NULL, 10);
  else nfo.nparticles = 10*1000*1000;
  nfo.nparticles_per_process = -1;
  nfo.max_depth = nfo.depth = ceil(log(nfo.nparticles_per_process / 10 * comm_size) / log(8.0));
/*  nfo.max_depth = 20;
  nfo.depth = 20;*/
  nfo.frac = 1.0;
  nfo.addr_byte = 8;
  nfo.addr_iboxbits = 0;
/*  nfo.max_mem = 1*1000*1000;*/
  nfo.max_mem = 0;
  nfo.grad_mem = 0;

  nfo.scratch = 0;
  nfo.results = 0;
  nfo.sort_front = 2;
  nfo.sort_back = 1;

  nfo.move_rate = 0.0;
  nfo.rounds = 1;

#endif

  fmm_run(&nfo);
}


int main(int argc, char *argv[])
{
#ifdef SL_USE_MPI
  MPI_Init(&argc, &argv);

  comm = MPI_COMM_WORLD;
  MPI_Comm_size(comm, &comm_size);
  MPI_Comm_rank(comm, &comm_rank);
#endif

  --argc; ++argv;

  test_fmm(argc, argv);

#ifdef SL_USE_MPI
  MPI_Finalize();
#endif

  return 0;
}
