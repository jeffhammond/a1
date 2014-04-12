A1 stands for Argonne 1-sided.

# Summary

A1 is a completely rewritten implementation of ARMCI-like one-sided communication, 
i.e. an alternative runtime system for Global Arrays.

# Goals of the project

* low-latency/high-bandwidth
* scalability to at least one million processes
* thread-safety
* modular design ala MPICH
* interoperability with MPI
* software quality (readable, documented source code and robust build system)
* productive interaction with vendors

# Progress

The Blue Gene/P implementation over DCMF was essentially complete 
when we transitioned away from this project in favor of more
adventurous projects like MPI-3 and OSPRI.

# Developers

Jeff Hammond, Sreeram Potluri and Pavan Balaji worked on this project from 2010 to 2012.
