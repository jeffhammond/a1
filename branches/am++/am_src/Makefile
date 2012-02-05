all: am_helloworld am_ring

am_helloworld: am_helloworld.cpp
	mpicxx $<  -o $@

am_ring: am_ring.cpp
	mpicxx $<  -o $@

clean:
	rm -f am_helloworld am_ring
