      program generate_charges
      implicit none
      integer, parameter :: n = 5
      integer, parameter :: simsize = 8**n
      real*8, dimension(simsize) :: q
      real*8, dimension(3,simsize) :: xyz
      integer :: i

        call charges(n,simsize,simsize,q,xyz)

        do i = 1,simsize
          write(42,"(4(d16.9,1x))") q(i),xyz(1,i),xyz(2,i),xyz(3,i)
        end do
        
        print *,"done gernating ",simsize,"particles."

      end program generate_charges

      subroutine charges(n,maxncharges,ncharges,q,xyz)
c
      implicit none
c
      real(kind=8) a,b,x,y,z
      real(kind=8) q(*),xyz(3,*)
c
      integer n,maxncharges,ncharges,i,ix,iy,iz
c
      real(kind=8) one
      parameter(one=1.e0)
      real(kind=8) two
      parameter(two=2.e0)
c
      if(n.lt.0)  stop "error 1"
      if(maxncharges.le.0) stop "error 2"
c
      i = -n
c
      a = two**i
c
      i = i-1
c
      b = two**i
c
      i = 0
c
      ix = 0
c
      do 1 x = a,one,a
         ix = ix+1
         iy = 0
         do 2 y = a,one,a
            iy = iy+1
            iz = 0
            do 3 z = a,one,a
               iz = iz+1
               if(i.lt.maxncharges) then
                  i = i+1
                     q(i) = -one
                      if(iand(i,1).gt.0) q(i) = -q(i)
c                     q(i) = (-one)**(ix+iy+iz)
                     xyz(1,i) = x-b
                     xyz(2,i) = y-b
                     xyz(3,i) = z-b
               else
                  stop "error"
               endif
 3          continue
 2       continue
 1    continue
      ncharges = i
      return
      end subroutine charges
c
