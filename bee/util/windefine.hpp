/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifdef _WIN32

// Undefine windows.h macros
#ifdef DELETE
	#undef DELETE
#endif

#ifdef WARNING
	#undef WARNING
#endif

#ifdef ERROR
	#undef ERROR
#endif

#ifdef OPTIONAL
	#undef OPTIONAL
#endif

#define NOMINMAX
#ifdef max
	#undef max
#endif
#ifdef min
	#undef min
#endif

#endif // _WIN32
