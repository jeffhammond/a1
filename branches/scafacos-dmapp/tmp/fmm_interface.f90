!fmm interface for external programs
subroutine fmm_init()
use mp_wrapper, only : mp_init
implicit none
	
	call mp_init()
	
end subroutine fmm_init

subroutine fmm_finalize()
use mp_wrapper, only : mp_finalize
implicit none

	call mp_finalize()
	
end subroutine fmm_finalize
