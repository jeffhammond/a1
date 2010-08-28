/* 
 * The following is a notice of limited availability of the code, and disclaimer
 * which must be included in the prologue of the code and in all source listings
 * of the code.
 *
 * Copyright (c) 2010  Argonne Leadership Computing Facility, Argonne National
 * Laboratory
 *
 * Permission is hereby granted to use, reproduce, prepare derivative works, and
 * to redistribute to others.
 *
 *
 *                          LICENSE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer listed
 *   in this license in the documentation and/or other materials
 *   provided with the distribution.
 *
 * - Neither the name of the copyright holders nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * The copyright holders provide no reassurances that the source code
 * provided does not infringe any patent, copyright, or any other
 * intellectual property rights of third parties.  The copyright holders
 * disclaim any liability to any recipient for claims brought against
 * recipient by any third party for infringement of that parties
 * intellectual property rights.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "bench.h"

void get_static_nocallback()
{

    if (myrank == 0)
    {

        DCMF_Request_t get_req[ITERATIONS + SKIP];
        DCMF_Request_t get_flush;
        DCMF_Callback_t get_done;
        DCMF_Callback_t get_flush_done;
        int flush_done_count;
        int msgsize, i;

        get_done.function = NULL;
        get_done.clientdata = NULL;

        get_flush_done.function = done;
        get_flush_done.clientdata = (void *) &flush_done_count;

        char buffer[50];
        sprintf(buffer, "%20s  %20s", "Msg Size", "Injection Rt (kbps)");
        printf("%s \n", buffer);
        fflush(stdout);

        barrier();

        for (msgsize = 1; msgsize < MAX_MSG_SIZE; msgsize *= 2)
        {

            /***********************
             * warmup               *
             ***********************/
            for (i = 0; i < SKIP; i++)
            {
                DCMF_Get(&get_protocol,
                         &get_req[i],
                         get_done,
                         DCMF_SEQUENTIAL_CONSISTENCY,
                         1,
                         msgsize,
                         &memregion[myrank],
                         &memregion[myrank + 1],
                         i * msgsize,
                         i * msgsize);
            }

            flush_done_count = 1;
            DCMF_Get(&get_protocol,
                     &get_flush,
                     get_flush_done,
                     DCMF_SEQUENTIAL_CONSISTENCY,
                     1,
                     1,
                     &memregion[myrank],
                     &memregion[myrank + 1],
                     0,
                     0);
            while (flush_done_count)
                DCMF_Messager_advance();

            /***********************
             * start timer          *
             ***********************/

            t_start = DCMF_Timebase();

            for (i = SKIP; i < ITERATIONS + SKIP; i++)
            {
                DCMF_Get(&get_protocol,
                         &get_req[i],
                         get_done,
                         DCMF_SEQUENTIAL_CONSISTENCY,
                         1,
                         msgsize,
                         &memregion[myrank],
                         &memregion[myrank + 1],
                         i * msgsize,
                         i * msgsize);
            }

            flush_done_count = 1;
            DCMF_Get(&get_protocol,
                     &get_flush,
                     get_flush_done,
                     DCMF_SEQUENTIAL_CONSISTENCY,
                     1,
                     1,
                     &memregion[myrank],
                     &memregion[myrank + 1],
                     0,
                     0);
            while (flush_done_count)
                DCMF_Messager_advance();
            t_stop = DCMF_Timebase();

            /***********************
             * stop timer          *
             ***********************/

            t_usec = ((t_stop - t_start) / clockMHz);
            printf("%20d %20.0f \n", msgsize, t_usec / (ITERATIONS));

        }

        barrier();

    }
    else
    {

        barrier();

        barrier();

    }
}

void get_static_callback()
{

    if (myrank == 0)
    {

        DCMF_Request_t get_req[ITERATIONS + SKIP];
        DCMF_Callback_t get_done;
        int done_count;
        int msgsize, i;

        get_done.function = done;
        get_done.clientdata = (void *) &done_count;

        char buffer[50];
        sprintf(buffer, "%20s  %20s", "Msg Size", "Injection Rt (kbps)");
        printf("%s \n", buffer);
        fflush(stdout);

        barrier();

        for (msgsize = 1; msgsize < MAX_MSG_SIZE; msgsize *= 2)
        {

            /***********************
             * warmup               *
             ***********************/
            done_count = SKIP;
            for (i = 0; i < SKIP; i++)
            {
                DCMF_Get(&get_protocol,
                         &get_req[i],
                         get_done,
                         DCMF_SEQUENTIAL_CONSISTENCY,
                         1,
                         msgsize,
                         &memregion[myrank],
                         &memregion[myrank + 1],
                         i * msgsize,
                         i * msgsize);
            }
            while (done_count)
                DCMF_Messager_advance();

            /***********************
             * start timer          *
             ***********************/

            t_start = DCMF_Timebase();
            done_count = ITERATIONS;

            for (i = SKIP; i < ITERATIONS + SKIP; i++)
            {
                DCMF_Get(&get_protocol,
                         &get_req[i],
                         get_done,
                         DCMF_SEQUENTIAL_CONSISTENCY,
                         1,
                         msgsize,
                         &memregion[myrank],
                         &memregion[myrank + 1],
                         i * msgsize,
                         i * msgsize);
            }

            while (done_count)
                DCMF_Messager_advance();
            t_stop = DCMF_Timebase();

            /***********************
             * stop timer          *
             ***********************/

            t_usec = ((t_stop - t_start) / clockMHz);
            printf("%20d %20.0f \n", msgsize, t_usec / (ITERATIONS));

        }

        barrier();

    }
    else
    {

        barrier();

        barrier();

    }
}

void get_dynamic_callback()
{

    if (myrank == 0)
    {

        DCMF_Request_t get_req[ITERATIONS + SKIP];
        DCMF_Callback_t get_done;
        int done_count;
        int msgsize, i;

        get_done.function = done;
        get_done.clientdata = (void *) &done_count;

        char buffer[50];
        sprintf(buffer, "%20s  %20s", "Msg Size", "Injection Rt (kbps)");
        printf("%s \n", buffer);
        fflush(stdout);

        barrier();

        for (msgsize = 1; msgsize < MAX_MSG_SIZE; msgsize *= 2)
        {

            /***********************
             * warmup               *
             ***********************/
            done_count = SKIP;
            for (i = 0; i < SKIP; i++)
            {
                DCMF_Get(&get_protocol,
                         &get_req[i],
                         get_done,
                         DCMF_RELAXED_CONSISTENCY,
                         1,
                         msgsize,
                         &memregion[myrank],
                         &memregion[myrank + 1],
                         i * msgsize,
                         i * msgsize);
            }
            while (done_count)
                DCMF_Messager_advance();

            /***********************
             * start timer          *
             ***********************/

            t_start = DCMF_Timebase();
            done_count = ITERATIONS;

            for (i = SKIP; i < ITERATIONS + SKIP; i++)
            {
                DCMF_Get(&get_protocol,
                         &get_req[i],
                         get_done,
                         DCMF_RELAXED_CONSISTENCY,
                         1,
                         msgsize,
                         &memregion[myrank],
                         &memregion[myrank + 1],
                         i * msgsize,
                         i * msgsize);
            }

            while (done_count)
                DCMF_Messager_advance();
            t_stop = DCMF_Timebase();

            /***********************
             * stop timer          *
             ***********************/

            t_usec = ((t_stop - t_start) / clockMHz);
            printf("%20d %20.0f \n", msgsize, t_usec / (ITERATIONS));

        }

        barrier();

    }
    else
    {

        barrier();

        barrier();

    }
}

int main()
{
    DCMF_Messager_initialize();

    init();

    barrier_init(DCMF_DEFAULT_GLOBALBARRIER_PROTOCOL);

    control_init(DCMF_DEFAULT_CONTROL_PROTOCOL, DCMF_DEFAULT_NETWORK);

    memregion_init(MAX_BUF_SIZE * nranks);

    get_init(DCMF_DEFAULT_GET_PROTOCOL, DCMF_TORUS_NETWORK);

    barrier();

    if (myrank == 0)
    {
        printf("Get Latency (usec) with static routing and nocallback \n");
        fflush(stdout);
    }
    get_static_nocallback();

    if (myrank == 0)
    {
        printf("Get Latency (usec) with static routing and callback \n");
        fflush(stdout);
    }
    get_static_callback();

    if (myrank == 0)
    {
        printf("Get Latency (usec) with dynamic routing and callback \n");
        fflush(stdout);
    }
    get_dynamic_callback();

    barrier();

    printf("[%d] Benchmark complete\n", myrank);
    fflush(stdout);

    memregion_finalize();

    DCMF_Messager_finalize();

    return 0;
}
