message(STATUS "Available architectures (default = X64):\n")

message("   ARCH=          No specific architecture (disable some features).")
message("   ARCH=AVR       Atmel AVR ATMega128 8-bit architecture.")
message("   ARCH=MSP       TI MSP430 16-bit architecture.")
message("   ARCH=ARM       ARM 32-bit architecture.")
message("   ARCH=X86       Intel x86-compatible 32-bit architecture.")
message("   ARCH=X64       AMD x86_64-compatible 64-bit architecture.\n")

message(STATUS "Available word sizes (default = 64):\n")

message("   WORD=8         Build a 8-bit library.")
message("   WORD=16        Build a 16-bit library.")
message("   WORD=32        Build a 32-bit library.")
message("   WORD=64        Build a 64-bit library.\n")

message(STATUS "Byte boundary to align digit vectors (default = 1):\n")

message("   ALIGN=1        Do not align digit vectors.")
message("   ALIGN=2        Align digit vectors into 16-bit boundaries.")
message("   ALIGN=8        Align digit vectors into 64-bit boundaries.")
message("   ALIGN=16       Align digit vectors into 128-bit boundaries.\n")

# Architecture and memory layout.
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86")
	set(ARCH "X86" CACHE STRING "Architecture")
endif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86")
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
	set(ARCH "X64" CACHE STRING "Architecture")
endif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")

if(ARCH STREQUAL X86)
	set(AFLAGS "-m32")
	set(WORD 32)
endif(ARCH STREQUAL X86)
if(ARCH STREQUAL X64)
	set(AFLAGS "-m64")
	if (MSVC)
		# visual studio does not support 128 bit integers which is required if WORD = 64
		set(WORD 32)
	else (MSVC)
		set(WORD 64)
	endif (MSVC)
endif(ARCH STREQUAL X64)

if(NOT WORD)
	if (MSVC)
		# visual studio does not support 128 bit integers which is required if WORD = 64
		set(WORD 32)
	else (MSVC)
		set(WORD 64)
	endif (MSVC)
endif(NOT WORD)
set(WORD ${WORD} CACHE INTEGER "Processor word size")

if(NOT ALIGN)
	set(ALIGN 1)
endif(NOT ALIGN)
set(ALIGN ${ALIGN} CACHE INTEGER "Boundary to align digit vectors")