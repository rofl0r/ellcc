Description := Static runtime libraries for ellcc/Linux.

###

PWD := $(shell pwd)
override CC := $(PWD)/../../../bin/ecc
Arch := unknown
Configs :=

# Configurations which just include all the runtime functions.
Configs += arm
Arch.arm := arm

Configs += armeb
Arch.armeb := armeb

Configs += i386
Arch.i386 := i386

Configs += mips
Arch.mips := mips

Configs += ppc
Arch.ppc := ppc

Configs += ppc64
Arch.ppc64 := ppc64

Configs += x86_64
Arch.x86_64 := x86_64

# Configuration for profile runtime.
ifeq ($(call contains,i386 x86_64,$(CompilerTargetArch)),true)
Configs += profile-i386 profile-x86_64
Arch.profile-i386 := i386
Arch.profile-x86_64 := x86_64
endif

# Configuration for ASAN runtime.
ifeq ($(CompilerTargetArch),i386)
Configs += asan-i386
Arch.asan-i386 := i386
endif
ifeq ($(CompilerTargetArch),x86_64)
Configs += asan-x86_64
Arch.asan-x86_64 := x86_64
endif

# Configuration for TSAN runtime.
ifeq ($(CompilerTargetArch),x86_64)
Configs += tsan-x86_64
Arch.tsan-x86_64 := x86_64
endif

###

CFLAGS := -Wall -Werror -O3 -fomit-frame-pointer

CFLAGS.arm := $(CFLAGS) -target arm-ellcc-linux-eabi -mcpu=cortex-a9 -mfpu=neon
CFLAGS.armeb := $(CFLAGS) -target armeb-ellcc-linux-eabi -mcpu=cortex-a9 -mfpu=neon
CFLAGS.i386 := $(CFLAGS) -target i386-ellcc-linux
CFLAGS.mips := $(CFLAGS) -target mips-ellcc-linux -mcpu=mips32r2
CFLAGS.ppc := $(CFLAGS) -target ppc-ellcc-linux -mcpu=e500
CFLAGS.ppc64 := $(CFLAGS) -target ppc64-ellcc-linux -mcpu=e500
CFLAGS.x86_64 := $(CFLAGS) -target x86_64-ellcc-linux
CFLAGS.profile-i386 := $(CFLAGS) -target i386-ellcc-linux
CFLAGS.profile-x86_64 := $(CFLAGS) -target x86_64-ellcc-linux
CFLAGS.asan-i386 := $(CFLAGS) -target i386-ellcc-linux -fPIE -fno-builtin
CFLAGS.asan-x86_64 := $(CFLAGS) -target x86_64-ellcc-linux -fPIE -fno-builtin
CFLAGS.tsan-x86_64 := $(CFLAGS) -target x86_64-ellcc-linux -fPIE -fno-builtin

FUNCTIONS.arm := $(call set_difference, $(CommonFunctions), clear_cache) \
		 aeabi_ldivmod \
		 aeabi_uldivmod \
		 aeabi_memcpy
FUNCTIONS.armeb := $(call set_difference, $(CommonFunctions), clear_cache) \
		 aeabi_ldivmod \
		 aeabi_uldivmod \
		 aeabi_memcpy
FUNCTIONS.i386 := $(CommonFunctions) $(ArchFunctions.i386)
FUNCTIONS.mips := $(CommonFunctions) $(ArchFunctions.mips)
FUNCTIONS.ppc := $(CommonFunctions) $(ArchFunctions.ppc)
FUNCTIONS.ppc64 := $(CommonFunctions) $(ArchFunctions.ppc64)
FUNCTIONS.x86_64 := $(CommonFunctions) $(ArchFunctions.x86_64)
FUNCTIONS.profile-i386 := GCDAProfiling
FUNCTIONS.profile-x86_64 := GCDAProfiling
FUNCTIONS.asan-i386 := $(AsanFunctions) $(InterceptionFunctions) \
                                        $(SanitizerCommonFunctions)
FUNCTIONS.asan-x86_64 := $(AsanFunctions) $(InterceptionFunctions) \
                                          $(SanitizerCommonFunctions)
FUNCTIONS.tsan-x86_64 := $(TsanFunctions) $(InterceptionFunctions) \
                                          $(SanitizerCommonFunctions) 

# Always use optimized variants.
OPTIMIZED := 1

# We don't need to use visibility hidden on Linux.
VISIBILITY_HIDDEN := 0
