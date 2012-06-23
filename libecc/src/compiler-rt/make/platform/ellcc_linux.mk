Description := Static runtime libraries for clang/Linux.

###

PWD := $(shell pwd)
override CC := $(PWD)/../../../bin/ecc
Arch := unknown
Configs :=

# Configurations which just include all the runtime functions.
Configs += full-arm
Arch.full-arm := arm

Configs += full-armeb
Arch.full-armeb := armeb

Configs += full-i386
Arch.full-i386 := i386

Configs += full-mips
Arch.full-mips := mips

Configs += full-ppc
Arch.full-ppc := ppc

Configs += full-ppc64
Arch.full-ppc64 := ppc64

Configs += full-x86_64
Arch.full-x86_64 := x86_64

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

CFLAGS.full-arm := $(CFLAGS) -target arm-ellcc-linux -mcpu=cortex-a9 -mfpu=neon
CFLAGS.full-armeb := $(CFLAGS) -target armeb-ellcc-linux -mcpu=cortex-a9 -mfpu=neon
CFLAGS.full-i386 := $(CFLAGS) -target i386-ellcc-linux
CFLAGS.full-mips := $(CFLAGS) -target mips-ellcc-linux -mcpu=mips32r2
CFLAGS.full-ppc := $(CFLAGS) -target ppc-ellcc-linux -mcpu=e500
CFLAGS.full-ppc64 := $(CFLAGS) -target ppc64-ellcc-linux -mcpu=e500
CFLAGS.full-x86_64 := $(CFLAGS) -target x86_64-ellcc-linux
CFLAGS.profile-i386 := $(CFLAGS) -target i386-ellcc-linux
CFLAGS.profile-x86_64 := $(CFLAGS) -target x86_64-ellcc-linux
CFLAGS.asan-i386 := $(CFLAGS) -target i386-ellcc-linux -fPIE -fno-builtin
CFLAGS.asan-x86_64 := $(CFLAGS) -target x86_64-ellcc-linux -fPIE -fno-builtin
CFLAGS.tsan-x86_64 := $(CFLAGS) -target x86_64-ellcc-linux -fPIE -fno-builtin

# Use our stub SDK as the sysroot to support more portable building. For now we
# just do this for the non-ASAN modules, because the stub SDK doesn't have
# enough support to build ASAN.
CFLAGS.full-i386 += --sysroot=$(ProjSrcRoot)/SDKs/linux
CFLAGS.full-x86_64 += --sysroot=$(ProjSrcRoot)/SDKs/linux
CFLAGS.profile-i386 += --sysroot=$(ProjSrcRoot)/SDKs/linux
CFLAGS.profile-x86_64 += --sysroot=$(ProjSrcRoot)/SDKs/linux

FUNCTIONS.full-arm := $(CommonFunctions) $(ArchFunctions.arm)
FUNCTIONS.full-armeb := $(CommonFunctions) $(ArchFunctions.armeb)
FUNCTIONS.full-i386 := $(CommonFunctions) $(ArchFunctions.i386)
FUNCTIONS.full-mips := $(CommonFunctions) $(ArchFunctions.mips)
FUNCTIONS.full-ppc := $(CommonFunctions) $(ArchFunctions.ppc)
FUNCTIONS.full-ppc64 := $(CommonFunctions) $(ArchFunctions.ppc64)
FUNCTIONS.full-x86_64 := $(CommonFunctions) $(ArchFunctions.x86_64)
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
