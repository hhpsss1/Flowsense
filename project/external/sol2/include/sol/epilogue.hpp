





#pragma once


#if !defined(SOL_PROLOGUE_I_)
	#error "[sol2] Library Prologue is missing from this translation unit."
#else
	#undef SOL_PROLOGUE_I_
#endif

#if SOL_IS_ON(SOL_BUILD_CXX_MODE)
	#undef _FWD
	#undef _MOVE
#endif

