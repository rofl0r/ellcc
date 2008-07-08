//===- ellcc.cpp - The Elsa<->LLVM driver -------------------------------===//
//
//===----------------------------------------------------------------------===//
/*
  Command line options:

  Overall Options
    -combine
    -pass-exit-codes  
    -###
    --target-help
    --version
    @file

  C Language Options
    -ansi
    -std=standard
    -fgnu89-inline 
    -aux-info filename 
    -fno-asm
    -fno-builtin
    -fno-builtin-function 
    -fhosted
    -ffreestanding
    -fopenmp
    -fms-extensions 
    -trigraphs
    -no-integrated-cpp
    -traditional
    -traditional-cpp 
    -fallow-single-precision
    -fcond-mismatch 
    -fsigned-bitfields
    -fsigned-char 
    -funsigned-bitfields
    -funsigned-char

  C++ Language Options
    -fabi-version=n
    -fno-access-control
    -fcheck-new 
    -fconserve-space
    -ffriend-injection 
    -fno-elide-constructors 
    -fno-enforce-eh-specs 
    -ffor-scope
    -fno-for-scope
    -fno-gnu-keywords 
    -fno-implicit-templates 
    -fno-implicit-inline-templates 
    -fno-implement-inlines
    -fms-extensions 
    -fno-nonansi-builtins
    -fno-operator-names 
    -fno-optional-diags
    -fpermissive 
    -frepo
    -fno-rtti
    -fstats
    -ftemplate-depth-n 
    -fno-threadsafe-statics
    -fuse-cxa-atexit
    -fno-weak
    -nostdinc++ 
    -fno-default-inline
    -fvisibility-inlines-hidden 
    -Wabi
    -Wctor-dtor-privacy 
    -Wnon-virtual-dtor
    -Wreorder 
    -Weffc++
    -Wno-deprecated
    -Wstrict-null-sentinel 
    -Wno-non-template-friend
    -Wold-style-cast 
    -Woverloaded-virtual
    -Wno-pmf-conversions 
    -Wsign-promo
         
  Language Independent Options
    -fmessage-length=n  
    -fdiagnostics-show-location=[once|every-line]  
    -fdiagnostics-show-option

  Warning Options
    -fsyntax-only
    -pedantic
    -pedantic-errors 
    -w
    -Wextra
    -Wall
    -Waddress
    -Waggregate-return
    -Wno-attributes 
    -Wc++-compat
    -Wcast-align
    -Wcast-qual
    -Wchar-subscripts
    -Wcomment 
    -Wconversion
    -Wno-deprecated-declarations 
    -Wdisabled-optimization
    -Wno-div-by-zero
    -Wno-endif-labels 
    -Werror
    -Werror=*
    -Werror-implicit-function-declaration 
    -Wfatal-errors
    -Wfloat-equal
    -Wformat
    -Wformat=2 
    -Wno-format-extra-args
    -Wformat-nonliteral 
    -Wformat-security
    -Wformat-y2k 
    -Wimplicit
    -Wimplicit-function-declaration
    -Wimplicit-int 
    -Wimport
    -Wno-import
    -Winit-self
    -Winline 
    -Wno-int-to-pointer-cast 
    -Wno-invalid-offsetof
    -Winvalid-pch 
    -Wlarger-than-len
    -Wunsafe-loop-optimizations
    -Wlong-long 
    -Wmain
    -Wmissing-braces
    -Wmissing-field-initializers 
    -Wmissing-format-attribute
    -Wmissing-include-dirs 
    -Wmissing-noreturn 
    -Wno-multichar
    -Wnonnull
    -Wno-overflow 
    -Woverlength-strings
    -Wpacked
    -Wpadded 
    -Wparentheses
    -Wpointer-arith
    -Wno-pointer-to-int-cast 
    -Wredundant-decls 
    -Wreturn-type
    -Wsequence-point
    -Wshadow 
    -Wsign-compare
    -Wstack-protector 
    -Wstrict-aliasing
    -Wstrict-aliasing=2 
    -Wstrict-overflow
    -Wstrict-overflow=n 
    -Wswitch
    -Wswitch-default
    -Wswitch-enum 
    -Wsystem-headers
    -Wtrigraphs
    -Wundef
    -Wuninitialized 
    -Wunknown-pragmas
    -Wno-pragmas
    -Wunreachable-code 
    -Wunused
    -Wunused-function
    -Wunused-label
    -Wunused-parameter 
    -Wunused-value
    -Wunused-variable
    -Wvariadic-macros 
    -Wvolatile-register-var
    -Wwrite-strings

  C-only Warning Options
    -Wbad-function-cast
    -Wmissing-declarations 
    -Wmissing-prototypes
    -Wnested-externs
    -Wold-style-definition 
    -Wstrict-prototypes
    -Wtraditional 
    -Wdeclaration-after-statement
    -Wpointer-sign

  Debugging Options
    -dletters
    -dumpspecs
    -dumpmachine
    -dumpversion 
    -fdump-noaddr
    -fdump-unnumbered
    -fdump-translation-unit[-n] 
    -fdump-class-hierarchy[-n] 
    -fdump-ipa-all
    -fdump-ipa-cgraph 
    -fdump-tree-all 
    -fdump-tree-original[-n]  
    -fdump-tree-optimized[-n] 
    -fdump-tree-inlined[-n] 
    -fdump-tree-cfg
    -fdump-tree-vcg
    -fdump-tree-alias 
    -fdump-tree-ch 
    -fdump-tree-ssa[-n]
    -fdump-tree-pre[-n] 
    -fdump-tree-ccp[-n]
    -fdump-tree-dce[-n] 
    -fdump-tree-gimple[-raw]
    -fdump-tree-mudflap[-n] 
    -fdump-tree-dom[-n] 
    -fdump-tree-dse[-n] 
    -fdump-tree-phiopt[-n] 
    -fdump-tree-forwprop[-n] 
    -fdump-tree-copyrename[-n] 
    -fdump-tree-nrv -fdump-tree-vect 
    -fdump-tree-sink 
    -fdump-tree-sra[-n] 
    -fdump-tree-salias 
    -fdump-tree-fre[-n] 
    -fdump-tree-vrp[-n] 
    -ftree-vectorizer-verbose=n 
    -fdump-tree-storeccp[-n] 
    -feliminate-dwarf2-dups
    -feliminate-unused-debug-types 
    -feliminate-unused-debug-symbols
    -femit-class-debug-always 
    -fmem-report
    -fprofile-arcs 
    -frandom-seed=string
    -fsched-verbose=n 
    -ftest-coverage
    -ftime-report
    -fvar-tracking 
    -g
    -glevel
    -gcoff
    -gdwarf-2 
    -ggdb
    -gstabs
    -gstabs+
    -gvms
    -gxcoff
    -gxcoff+ 
    -p
    -pg
    -print-file-name=library
    -print-libgcc-file-name 
    -print-multi-directory
    -print-multi-lib 
    -print-prog-name=program
    -print-search-dirs
    -Q 
    -save-temps
    -time

  Optimization Options
              -falign-functions=n  -falign-jumps=n 
              -falign-labels=n  -falign-loops=n  
              -fbounds-check -fmudflap -fmudflapth -fmudflapir 
              -fbranch-probabilities -fprofile-values -fvpt -fbranch-target-load-optimize 
              -fbranch-target-load-optimize2 -fbtr-bb-exclusive 
              -fcaller-saves  -fcprop-registers  -fcse-follow-jumps 
              -fcse-skip-blocks  -fcx-limited-range  -fdata-sections 
              -fdelayed-branch  -fdelete-null-pointer-checks -fearly-inlining 
              -fexpensive-optimizations  -ffast-math  -ffloat-store 
              -fforce-addr  -ffunction-sections 
              -fgcse  -fgcse-lm  -fgcse-sm  -fgcse-las  -fgcse-after-reload 
              -fcrossjumping  -fif-conversion  -fif-conversion2 
              -finline-functions  -finline-functions-called-once 
              -finline-limit=n  -fkeep-inline-functions 
              -fkeep-static-consts  -fmerge-constants  -fmerge-all-constants 
              -fmodulo-sched -fno-branch-count-reg 
              -fno-default-inline  -fno-defer-pop -fmove-loop-invariants 
              -fno-function-cse  -fno-guess-branch-probability 
              -fno-inline  -fno-math-errno  -fno-peephole  -fno-peephole2 
              -funsafe-math-optimizations  -funsafe-loop-optimizations  -ffinite-math-only 
              -fno-toplevel-reorder -fno-trapping-math  -fno-zero-initialized-in-bss 
              -fomit-frame-pointer  -foptimize-register-move 
              -foptimize-sibling-calls  -fprefetch-loop-arrays 
              -fprofile-generate -fprofile-use 
              -fregmove  -frename-registers 
              -freorder-blocks  -freorder-blocks-and-partition -freorder-functions 
              -frerun-cse-after-loop 
              -frounding-math -frtl-abstract-sequences 
              -fschedule-insns  -fschedule-insns2 
              -fno-sched-interblock  -fno-sched-spec  -fsched-spec-load 
              -fsched-spec-load-dangerous  
              -fsched-stalled-insns=n -fsched-stalled-insns-dep=n 
              -fsched2-use-superblocks 
              -fsched2-use-traces -fsee -freschedule-modulo-scheduled-loops 
              -fsection-anchors  -fsignaling-nans  -fsingle-precision-constant 
              -fstack-protector  -fstack-protector-all 
              -fstrict-aliasing  -fstrict-overflow  -ftracer  -fthread-jumps 
              -funroll-all-loops  -funroll-loops  -fpeel-loops 
              -fsplit-ivs-in-unroller -funswitch-loops 
              -fvariable-expansion-in-unroller 
              -ftree-pre  -ftree-ccp  -ftree-dce -ftree-loop-optimize 
              -ftree-loop-linear -ftree-loop-im -ftree-loop-ivcanon -fivopts 
              -ftree-dominator-opts -ftree-dse -ftree-copyrename -ftree-sink 
              -ftree-ch -ftree-sra -ftree-ter -ftree-lrs -ftree-fre -ftree-vectorize 
              -ftree-vect-loop-version -ftree-salias -fipa-pta -fweb 
              -ftree-copy-prop -ftree-store-ccp -ftree-store-copy-prop -fwhole-program 
              --param name=value

Preprocessor Options
              -Aquestion=answer 
              -A-question[=answer] 
              -C  -dD  -dI  -dM  -dN 
              -Dmacro[=defn]  -E  -H 
              -idirafter dir 
              -include file  -imacros file 
              -iprefix file  -iwithprefix dir 
              -iwithprefixbefore dir  -isystem dir 
              -imultilib dir -isysroot dir 
              -M  -MM  -MF  -MG  -MP  -MQ  -MT  -nostdinc  
              -P  -fworking-directory  -remap 
              -trigraphs  -undef  -Umacro  -Wp,option 
              -Xpreprocessor option

  Assembler Options
              -Wa,option  -Xassembler option
         


  Linker Options
              object-file-name  -llibrary 
              -nostartfiles  -nodefaultlibs  -nostdlib -pie -rdynamic 
              -s  -static  -static-libgcc  -shared  -shared-libgcc  -symbolic 
              -Wl,option  -Xlinker option 
              -u symbol
         


  Directory Options

              -Bprefix  -Idir  -iquotedir  -Ldir
              -specs=file  -I- --sysroot=dir
         


  Target Options

              -V version  -b machine
         


  Machine Dependent Options

    ARM Options
              -mapcs-frame  -mno-apcs-frame 
              -mabi=name 
              -mapcs-stack-check  -mno-apcs-stack-check 
              -mapcs-float  -mno-apcs-float 
              -mapcs-reentrant  -mno-apcs-reentrant 
              -msched-prolog  -mno-sched-prolog 
              -mlittle-endian  -mbig-endian  -mwords-little-endian 
              -mfloat-abi=name  -msoft-float  -mhard-float  -mfpe 
              -mthumb-interwork  -mno-thumb-interwork 
              -mcpu=name  -march=name  -mfpu=name  
              -mstructure-size-boundary=n 
              -mabort-on-noreturn 
              -mlong-calls  -mno-long-calls 
              -msingle-pic-base  -mno-single-pic-base 
              -mpic-register=reg 
              -mnop-fun-dllimport 
              -mcirrus-fix-invalid-insns -mno-cirrus-fix-invalid-insns 
              -mpoke-function-name 
              -mthumb  -marm 
              -mtpcs-frame  -mtpcs-leaf-frame 
              -mcaller-super-interworking  -mcallee-super-interworking 
              -mtp=name
         

    Darwin Options
              -all_load  -allowable_client  -arch  -arch_errors_fatal 
              -arch_only  -bind_at_load  -bundle  -bundle_loader 
              -client_name  -compatibility_version  -current_version 
              -dead_strip 
              -dependency-file  -dylib_file  -dylinker_install_name 
              -dynamic  -dynamiclib  -exported_symbols_list 
              -filelist  -flat_namespace  -force_cpusubtype_ALL 
              -force_flat_namespace  -headerpad_max_install_names 
              -image_base  -init  -install_name  -keep_private_externs 
              -multi_module  -multiply_defined  -multiply_defined_unused 
              -noall_load   -no_dead_strip_inits_and_terms 
              -nofixprebinding -nomultidefs  -noprebind  -noseglinkedit 
              -pagezero_size  -prebind  -prebind_all_twolevel_modules 
              -private_bundle  -read_only_relocs  -sectalign 
              -sectobjectsymbols  -whyload  -seg1addr 
              -sectcreate  -sectobjectsymbols  -sectorder 
              -segaddr -segs_read_only_addr -segs_read_write_addr 
              -seg_addr_table  -seg_addr_table_filename  -seglinkedit 
              -segprot  -segs_read_only_addr  -segs_read_write_addr 
              -single_module  -static  -sub_library  -sub_umbrella 
              -twolevel_namespace  -umbrella  -undefined 
              -unexported_symbols_list  -weak_reference_mismatches 
              -whatsloaded -F -gused -gfull -mmacosx-version-min=version 
              -mkernel -mone-byte-bool
         

    DEC Alpha Options
              -mno-fp-regs  -msoft-float  -malpha-as  -mgas 
              -mieee  -mieee-with-inexact  -mieee-conformant 
              -mfp-trap-mode=mode  -mfp-rounding-mode=mode 
              -mtrap-precision=mode  -mbuild-constants 
              -mcpu=cpu-type  -mtune=cpu-type 
              -mbwx  -mmax  -mfix  -mcix 
              -mfloat-vax  -mfloat-ieee 
              -mexplicit-relocs  -msmall-data  -mlarge-data 
              -msmall-text  -mlarge-text 
              -mmemory-latency=time
         
    GNU/Linux Options
              -muclibc

    i386 and x86-64 Options

              -mtune=cpu-type  -march=cpu-type 
              -mfpmath=unit 
              -masm=dialect  -mno-fancy-math-387 
              -mno-fp-ret-in-387  -msoft-float  -msvr3-shlib 
              -mno-wide-multiply  -mrtd  -malign-double 
              -mpreferred-stack-boundary=num 
              -mmmx  -msse  -msse2 -msse3 -m3dnow 
              -mthreads  -mno-align-stringops  -minline-all-stringops 
              -mpush-args  -maccumulate-outgoing-args  -m128bit-long-double 
              -m96bit-long-double  -mregparm=num  -msseregparm 
              -mstackrealign 
              -momit-leaf-frame-pointer  -mno-red-zone -mno-tls-direct-seg-refs 
              -mcmodel=code-model 
              -m32  -m64 -mlarge-data-threshold=num
         

    IA-64 Options

              -mbig-endian  -mlittle-endian  -mgnu-as  -mgnu-ld  -mno-pic 
              -mvolatile-asm-stop  -mregister-names  -mno-sdata 
              -mconstant-gp  -mauto-pic  -minline-float-divide-min-latency 
              -minline-float-divide-max-throughput 
              -minline-int-divide-min-latency 
              -minline-int-divide-max-throughput  
              -minline-sqrt-min-latency -minline-sqrt-max-throughput 
              -mno-dwarf2-asm -mearly-stop-bits 
              -mfixed-range=register-range -mtls-size=tls-size 
              -mtune=cpu-type -mt -pthread -milp32 -mlp64 
              -mno-sched-br-data-spec -msched-ar-data-spec -mno-sched-control-spec 
              -msched-br-in-data-spec -msched-ar-in-data-spec -msched-in-control-spec 
              -msched-ldc -mno-sched-control-ldc -mno-sched-spec-verbose 
              -mno-sched-prefer-non-data-spec-insns 
              -mno-sched-prefer-non-control-spec-insns 
              -mno-sched-count-spec-in-critical-path
         

    MIPS Options

              -EL  -EB  -march=arch  -mtune=arch 
              -mips1  -mips2  -mips3  -mips4  -mips32  -mips32r2  -mips64 
              -mips16  -mno-mips16  -mabi=abi  -mabicalls  -mno-abicalls 
              -mshared  -mno-shared  -mxgot  -mno-xgot  -mgp32  -mgp64  
              -mfp32  -mfp64  -mhard-float  -msoft-float  
              -msingle-float  -mdouble-float  -mdsp  -mpaired-single  -mips3d 
              -mlong64  -mlong32  -msym32  -mno-sym32 
              -Gnum  -membedded-data  -mno-embedded-data 
              -muninit-const-in-rodata  -mno-uninit-const-in-rodata 
              -msplit-addresses  -mno-split-addresses  
              -mexplicit-relocs  -mno-explicit-relocs  
              -mcheck-zero-division  -mno-check-zero-division 
              -mdivide-traps  -mdivide-breaks 
              -mmemcpy  -mno-memcpy  -mlong-calls  -mno-long-calls 
              -mmad  -mno-mad  -mfused-madd  -mno-fused-madd  -nocpp 
              -mfix-r4000  -mno-fix-r4000  -mfix-r4400  -mno-fix-r4400 
              -mfix-vr4120  -mno-fix-vr4120  -mfix-vr4130 
              -mfix-sb1  -mno-fix-sb1 
              -mflush-func=func  -mno-flush-func 
              -mbranch-likely  -mno-branch-likely 
              -mfp-exceptions -mno-fp-exceptions 
              -mvr4130-align -mno-vr4130-align

  RS/6000 and PowerPC Options
              -mcpu=cpu-type 
              -mtune=cpu-type 
              -mpower  -mno-power  -mpower2  -mno-power2 
              -mpowerpc  -mpowerpc64  -mno-powerpc 
              -maltivec  -mno-altivec 
              -mpowerpc-gpopt  -mno-powerpc-gpopt 
              -mpowerpc-gfxopt  -mno-powerpc-gfxopt 
              -mmfcrf  -mno-mfcrf  -mpopcntb  -mno-popcntb  -mfprnd  -mno-fprnd 
              -mnew-mnemonics  -mold-mnemonics 
              -mfull-toc   -mminimal-toc  -mno-fp-in-toc  -mno-sum-in-toc 
              -m64  -m32  -mxl-compat  -mno-xl-compat  -mpe 
              -malign-power  -malign-natural 
              -msoft-float  -mhard-float  -mmultiple  -mno-multiple 
              -mstring  -mno-string  -mupdate  -mno-update 
              -mfused-madd  -mno-fused-madd  -mbit-align  -mno-bit-align 
              -mstrict-align  -mno-strict-align  -mrelocatable 
              -mno-relocatable  -mrelocatable-lib  -mno-relocatable-lib 
              -mtoc  -mno-toc  -mlittle  -mlittle-endian  -mbig  -mbig-endian 
              -mdynamic-no-pic  -maltivec  -mswdiv 
              -mprioritize-restricted-insns=priority 
              -msched-costly-dep=dependence_type 
              -minsert-sched-nops=scheme 
              -mcall-sysv  -mcall-netbsd 
              -maix-struct-return  -msvr4-struct-return 
              -mabi=abi-type -msecure-plt -mbss-plt 
              -misel -mno-isel 
              -misel=yes  -misel=no 
              -mspe -mno-spe 
              -mspe=yes  -mspe=no 
              -mvrsave -mno-vrsave 
              -mmulhw -mno-mulhw 
              -mdlmzb -mno-dlmzb 
              -mfloat-gprs=yes  -mfloat-gprs=no -mfloat-gprs=single -mfloat-gprs=double 
              -mprototype  -mno-prototype 
              -msim  -mmvme  -mads  -myellowknife  -memb  -msdata 
              -msdata=opt  -mvxworks  -mwindiss  -G num  -pthread

  SPARC Options
              -mcpu=cpu-type 
              -mtune=cpu-type 
              -mcmodel=code-model 
              -m32  -m64  -mapp-regs  -mno-app-regs 
              -mfaster-structs  -mno-faster-structs 
              -mfpu  -mno-fpu  -mhard-float  -msoft-float 
              -mhard-quad-float  -msoft-quad-float 
              -mimpure-text  -mno-impure-text  -mlittle-endian 
              -mstack-bias  -mno-stack-bias 
              -munaligned-doubles  -mno-unaligned-doubles 
              -mv8plus  -mno-v8plus  -mvis  -mno-vis
              -threads -pthreads -pthread

  Code Generation Options
              -fcall-saved-reg  -fcall-used-reg 
              -ffixed-reg  -fexceptions 
              -fnon-call-exceptions  -funwind-tables 
              -fasynchronous-unwind-tables 
              -finhibit-size-directive  -finstrument-functions 
              -fno-common  -fno-ident 
              -fpcc-struct-return  -fpic  -fPIC -fpie -fPIE 
              -fno-jump-tables 
              -freg-struct-return  -fshort-enums 
              -fshort-double  -fshort-wchar 
              -fverbose-asm  -fpack-struct[=n]  -fstack-check 
              -fstack-limit-register=reg  -fstack-limit-symbol=sym 
              -fargument-alias  -fargument-noalias 
              -fargument-noalias-global  -fargument-noalias-anything
              -fleading-underscore  -ftls-model=model 
              -ftrapv  -fwrapv  -fbounds-check 
              -fvisibility
*/
//===----------------------------------------------------------------------===//

#include "llvm/Module.h"
#include "llvm/ModuleProvider.h"
#include "llvm/PassManager.h"
#include "llvm/CallGraphSCCPass.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Target/SubtargetFeature.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetMachineRegistry.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
#include "llvm/CodeGen/FileWriters.h"
#include "llvm/Support/PassNameParser.h"
#include "llvm/System/Signals.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/PluginLoader.h"
#include "llvm/Support/Streams.h"
#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/Timer.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/LinkAllVMCore.h"
#include "llvm/Linker.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>

// Elsa.
#include "elsa.h"              // Elsa interfaces.
// pwPlexer.
#include "pwPlexer.h"          // The preprocessor.
#include "pwOS.h"

#define xstr(x) #x
#define str(x) xstr(x)

#define ELLCC 0
#define ELLCC_MINOR 1
#define ELLCC_PATCHLEVEL 0
#define ELLCC_VERSION_STRING str(ELLCC) "." str(ELLCC_MINOR) "." str(ELLCC_PATCHLEVEL)
#define ELLCC_VERSION_MODIFIER "ALPHA"
#define ELLCC_VERSION ELLCC_VERSION_STRING " " ELLCC_VERSION_MODIFIER " " __DATE__

using namespace llvm;

static pw::ErrorList errors;    // The reported errors.
static std::string progname;    // The program name.        
static pw::Plexer* pconfig;     // The program configuration.
 
/** File types.
 */
enum FileTypes {
  NONE,                         ///< Not used.
  C,                            ///< C. 
  I,                            ///< Preprocessed C.
  H,                            ///< Header file.
  CC,                           ///< C++.
  II,                           ///< Preprocessed C++.
  HH,                           ///< A C++ header file.
  LL,                           ///< An LLVM assembly file.
  LLX,                          ///< An LLVM assembly file that needs preprocessing.
  UBC,                          ///< An unoptimized LLVM bitcode file.
  BC,                           ///< An LLVM bitcode file.
  LBC,                          ///< A linked LLVM bitcode file.
  S,                            ///< A native assembly source file.
  SX,                           ///< A native assembly file that needs preprocessing.
  O,                            ///< Other: An object file, linker command file, etc.
  A,                            ///< A library.
  EXE,                          ///< An executable file.
  LINKED,                       ///< A file that has been linked.
  DLL,                          ///< A dynamic library.
  CBE,                          ///< A C backend output file.
  NUM_FILE_TYPES                ///< Always last!
};

static const char* fileTypes[] = {
  "NONE",
  "a C file",
  "a preprocessed C file",
  "a header file file",
  "a C++",
  "a preprocessed C++ file",
  "a C++ header",
  "an LLVM assembly file",
  "an LLVM assembly file that needs preprocessing",
  "an unoptimized LLVM bitcode file",
  "an LLVM bitcode file",
  "a linked LLVM bitcode file",
  "an assembly source file",
  "an assembly file that needs preprocessing",
  "an object file",
  "a library",
  "an executable file",
  "a file that has been linked",
  "a dynamic library", 
  "a C backend output file",
};

/** Translation phases.
 */
enum Phases {
    PREPROCESSING,              ///< Source language combining, filtering, substitution
    TRANSLATION,                ///< Translate source -> LLVM bitcode/assembly
    OPTIMIZATION,               ///< Optimize translation result
    BCLINKING,                  ///< Link and optimize bitcode files. 
    GENERATION,                 ///< Convert .bc to ...
    ASSEMBLY,                   ///< Convert .s to .o
    LINKING,                    ///< Link and create executable
    NUM_PHASES                  ///< Always last!
};


static const struct {
    const char* name;           ///< The name of the action.
    FileTypes result;           ///< The result of this phase if it processes multiple files.
} phases[] = {
    { "Preprocessing", },
    { "Translation", },
    { "Optimization", },
    { "Bitcode linking", BC },
    { "Generating", },
    { "Assembly", },
    { "Linking", EXE }
};

static Timer* timers[NUM_PHASES];       // The phase timers.

/** File types by extension.
 */
static std::map<std::string, FileTypes> extToLang;

/** File extensions by language.
 */
static std::map<FileTypes, std::string> langToExt;

/** Actions that can be taken on a file.
 */
enum FileActions {
    PREPROCESS,                 ///> Preprocess a file.
    CCOMPILE,                   ///> Compile a C/C++ program.
    LLASSEMBLE,                 ///> Assemble an LLVM assembly file.
    OPTIMIZE,                   ///> Optimize an LLVM bitcode file.
    BCLINK,                     ///> Link LLVM bitcode files.
    GENERATE,                   ///> Convert .bc to .s.
    ASSEMBLE,                   ///> Assemble a .s file.
    LINK,                       ///> Link object files files.
    NUM_ACTIONS                 ///> Always last!
};

static const struct {
    const char* name;           ///< The name of the action.
} fileActions[] = {
    { "preprocess" },
    { "compile" },
    { "llassemble" },
    { "optimize" },
    { "bclink", },
    { "generate" },
    { "assemble" },
    { "link", },
};

/** Association of file types, actions, and phases.
 */
struct FileInfo {
    FileTypes type;
    FileActions action;
};
static FileInfo filePhases[NUM_FILE_TYPES][NUM_PHASES];

/** Set up the file type map.
 */
static void setupFileTypes()
{
    // The extension to language and language to phase mapping.
    extToLang["c"] = C;                                 // A C file becomes a preprocessed file.
    langToExt[C] = "c";
    filePhases[C][PREPROCESSING].type = I;
    filePhases[C][PREPROCESSING].action = PREPROCESS;

    extToLang["i"] = I;                                 // A preprocessed C file becomes a unoptimized bitcode file.
    langToExt[I] = "i";
    filePhases[I][TRANSLATION].type = UBC;
    filePhases[I][TRANSLATION].action = CCOMPILE;

    extToLang["h"] = H;
    langToExt[H] = "h";
    filePhases[H][PREPROCESSING].type = I;
    filePhases[H][PREPROCESSING].action = PREPROCESS;

    extToLang["cc"] = CC;
    langToExt[CC] = "cc";
    filePhases[CC][PREPROCESSING].type = II;
    filePhases[CC][PREPROCESSING].action = PREPROCESS;
    extToLang["cp"] = CC;
    extToLang["cxx"] = CC;
    extToLang["cpp"] = CC;
    extToLang["c++"] = CC;
    extToLang["C"] = CC;

    extToLang["ii"] = II;
    langToExt[II] = "ii";
    filePhases[II][TRANSLATION].type = UBC;
    filePhases[II][TRANSLATION].action = CCOMPILE;

    extToLang["hh"] = HH;
    langToExt[HH] = "hh";
    filePhases[HH][PREPROCESSING].type = II;
    filePhases[HH][PREPROCESSING].action = PREPROCESS;
    extToLang["H"] = HH;
    extToLang["hp"] = HH;
    extToLang["hxx"] = HH;
    extToLang["h++"] = HH;
    extToLang["tcc"] = HH;

    extToLang["ll"] = LL;
    langToExt[LL] = "ll";
    filePhases[LL][TRANSLATION].type = UBC;
    filePhases[LL][TRANSLATION].action = LLASSEMBLE;

    extToLang["llx"] = LLX;
    langToExt[LLX] = "llx";
    filePhases[LLX][PREPROCESSING].type = LL;
    filePhases[LLX][PREPROCESSING].action = PREPROCESS;

    extToLang["ubc"] = UBC;
    langToExt[UBC] = "ubc";
    filePhases[UBC][OPTIMIZATION].type = BC;
    filePhases[UBC][OPTIMIZATION].action = OPTIMIZE;

    extToLang["bc"] = BC;
    langToExt[BC] = "bc";
    filePhases[BC][BCLINKING].type = LINKED;
    filePhases[BC][BCLINKING].action = BCLINK;
    filePhases[BC][GENERATION].type = S;
    filePhases[BC][GENERATION].action = GENERATE;

    extToLang["s"] = S;
    langToExt[S] = "s";
    filePhases[S][ASSEMBLY].type = O;
    filePhases[S][ASSEMBLY].action = ASSEMBLE;

    extToLang["sx"] = SX;
    langToExt[SX] = "s";
    filePhases[SX][PREPROCESSING].type = S;
    filePhases[SX][PREPROCESSING].action = PREPROCESS;
    extToLang["S"] = SX;

    extToLang["o"] = O;
    langToExt[O] = "o";
    filePhases[O][LINKING].type = LINKED;
    filePhases[O][LINKING].action = LINK;
}

//===----------------------------------------------------------------------===//
//===          PHASE OPTIONS
//===----------------------------------------------------------------------===//

static cl::opt<Phases> FinalPhase(cl::Optional,
    cl::desc("Choose final phase of translation:"),
    cl::init(LINKING),
    cl::values(
        clEnumValN(PREPROCESSING,"E",
            "Stop translation after pre-processing phase"),
        clEnumValN(TRANSLATION, "t",
            "Stop translation after translation phase"),
        clEnumValN(OPTIMIZATION,"obc",
            "Stop translation after optimization phase"),
        clEnumValN(BCLINKING,"lbc",
            "Stop translation after bitcode linking phase"),
        clEnumValN(GENERATION,"S",
            "Stop translation after generation phase"),
        clEnumValN(ASSEMBLY,"c",
            "Stop translation after assembly phase"),
        clEnumValEnd
    )
);

//===----------------------------------------------------------------------===//
//===          OPTIMIZATION OPTIONS
//===----------------------------------------------------------------------===//

/** Optimization levels.
 */
enum OptimizationLevels {
    OPT_FAST_COMPILE,           ///< Optimize to make >compile< go faster
    OPT_SIMPLE,                 ///< Standard/simple optimizations
    OPT_AGGRESSIVE,             ///< Aggressive optimizations
    OPT_LINK_TIME,              ///< Aggressive + LinkTime optimizations
    OPT_AGGRESSIVE_LINK_TIME,   ///< Make it go way fast!
    OPT_NONE                    ///< No optimizations. Keep this at the end!
};

static cl::opt<OptimizationLevels> OptLevel(cl::ZeroOrMore,
  cl::desc("Choose level of optimization to apply:"),
  cl::init(OPT_FAST_COMPILE),
  cl::values(
    clEnumValN(OPT_FAST_COMPILE,"O",
      "An alias for the -O1 option"),
    clEnumValN(OPT_NONE,"O0",
      "Perform no optimizations"),
    clEnumValN(OPT_FAST_COMPILE,"O1",
      "Optimize for compilation speed, not execution speed"),
    clEnumValN(OPT_FAST_COMPILE,"std-compile-opts",
      "An alias for the -O1 option"),
    clEnumValN(OPT_SIMPLE,"O2",
      "Perform simple translation time optimizations"),
    clEnumValN(OPT_AGGRESSIVE,"O3",
      "Perform aggressive translation time optimizations"),
    clEnumValN(OPT_LINK_TIME,"O4",
      "Perform link time optimizations"),
    clEnumValN(OPT_AGGRESSIVE_LINK_TIME,"O5",
      "Perform aggressive link time optimizations"),
    clEnumValEnd
  )
);

static cl::opt<bool> DisableInline("disable-inlining",
  cl::desc("Do not run the inliner pass"));

static cl::opt<bool> DisableInternalize("disable-internalize",
  cl::desc("Do not mark all symbols as internal"));

static cl::opt<bool> VerifyEach("verify-each",
 cl::desc("Verify intermediate results of all passes"));

static cl::alias ExportDynamic("export-dynamic",
  cl::aliasopt(DisableInternalize),
  cl::desc("Alias for -disable-internalize"));

static cl::opt<bool> Strip("strip-all", 
  cl::desc("Strip all symbol info from executable"));

static cl::alias A0("s", cl::desc("Alias for --strip-all"), 
  cl::aliasopt(Strip));

static cl::opt<bool> StripDebug("strip-debug",
  cl::desc("Strip debugger symbol info from executable"));

static cl::alias A1("SD", cl::desc("Alias for --strip-debug"),
  cl::aliasopt(StripDebug));

static cl::opt<bool> KeepInlineFunctions("fkeep-inline-functions", 
  cl::desc("Keep static inline functions"));            // RICH
//
//===----------------------------------------------------------------------===//
//===          TOOL OPTIONS
//===----------------------------------------------------------------------===//

static cl::list<std::string> PreprocessorToolOpts("Tpre", cl::ZeroOrMore,
    cl::desc("Pass specific options to the pre-processor"),
    cl::value_desc("option"));

static cl::alias PreprocessorToolOptsAlias("Wp,", cl::ZeroOrMore,
    cl::desc("Alias for -Tpre"), cl::aliasopt(PreprocessorToolOpts));

static cl::list<std::string> TranslatorToolOpts("Ttrn", cl::ZeroOrMore,
    cl::desc("Pass specific options to the assembler"),
    cl::value_desc("option"));

static cl::list<std::string> AssemblerToolOpts("Tasm", cl::ZeroOrMore,
    cl::desc("Pass specific options to the assembler"),
    cl::value_desc("option"));

static cl::alias AssemblerToolOptsAlias("Wa,", cl::ZeroOrMore,
    cl::desc("Alias for -Tasm"), cl::aliasopt(AssemblerToolOpts));

static cl::list<std::string> OptimizerToolOpts("Topt", cl::ZeroOrMore,
    cl::desc("Pass specific options to the optimizer"),
    cl::value_desc("option"));

static cl::list<std::string> LinkerToolOpts("Tlnk", cl::ZeroOrMore,
    cl::desc("Pass specific options to the linker"),
    cl::value_desc("option"));

static cl::alias LinkerToolOptsAlias("Wl,", cl::ZeroOrMore,
    cl::desc("Alias for -Tlnk"), cl::aliasopt(LinkerToolOpts));

#if RICH
static cl::list<std::string> fOpts("f", cl::ZeroOrMore, cl::Prefix,
    cl::desc("Pass through -f options to compiler tools"),
    cl::value_desc("option"));
#endif

static cl::list<std::string> MOpts("M", cl::ZeroOrMore, cl::Prefix,
    cl::desc("Pass through -M options to compiler tools"),
    cl::value_desc("option"));

static cl::list<std::string> WOpts("W", cl::ZeroOrMore, cl::Prefix,
    cl::desc("Pass through -W options to compiler tools"),
    cl::value_desc("option"));

static cl::list<std::string> BOpt("B", cl::ZeroOrMore, cl::Prefix,
    cl::desc("Specify path to find ellcc sub-tools"),
    cl::value_desc("dir"));

//===----------------------------------------------------------------------===//
//===          INPUT OPTIONS
//===----------------------------------------------------------------------===//

static cl::list<std::string> LibPaths("L", cl::Prefix,
    cl::desc("Specify a library search path"), cl::value_desc("dir"));

static cl::list<std::string> Libraries("l", cl::Prefix,
    cl::desc("Specify base name of libraries to link to"), cl::value_desc("lib"));

static cl::list<std::string> Includes("I", cl::Prefix,
    cl::desc("Specify location to search for included source"),
    cl::value_desc("dir"));

static cl::list<std::string> Defines("D", cl::Prefix,
    cl::desc("Specify a pre-processor symbol to define"),
    cl::value_desc("symbol"));

//===----------------------------------------------------------------------===//
//===          OUTPUT OPTIONS
//===----------------------------------------------------------------------===//

static cl::opt<std::string> OutputFilename("o", cl::init("a.out"),
    cl::desc("Override output filename"), cl::value_desc("file"));

static cl::opt<std::string> OutputMachine("m", cl::Prefix,
    cl::desc("Specify a target machine"), cl::value_desc("machine"));

static cl::opt<bool> Native("native", cl::init(false),
    cl::desc("Generative native code instead of bitcode"));

static cl::opt<bool> DebugOutput("g", cl::init(false),
    cl::desc("Generate objects that include debug symbols"));

#if RICH
static cl::opt<bool> StripOutput("strip", cl::init(false),
    cl::desc("Strip all symbols from linked output file"));
#endif

static cl::opt<std::string> PrintFileName("print-fname", cl::Optional,
    cl::value_desc("file"),
    cl::desc("Print the full path for the option's value"));

//===----------------------------------------------------------------------===//
//===          INFORMATION OPTIONS
//===----------------------------------------------------------------------===//

static cl::opt<bool> DryRun("dry-run", cl::Optional, cl::init(false),
    cl::desc("Do everything but perform the compilation actions"));

static cl::alias DryRunAlias("y", cl::Optional,
    cl::desc("Alias for -dry-run"), cl::aliasopt(DryRun));

static cl::opt<bool> Verbose("verbose", cl::Optional, cl::init(false),
    cl::desc("Print out each action taken"));

static cl::alias VerboseAlias("v", cl::Optional,
    cl::desc("Alias for -verbose"), cl::aliasopt(Verbose));

#if RICH
static cl::opt<bool> Debug("debug", cl::Optional, cl::init(false),
    cl::Hidden, cl::desc("Print out debugging information"));

static cl::alias DebugAlias("d", cl::Optional,
    cl::desc("Alias for -debug"), cl::aliasopt(Debug));
#endif

static cl::opt<bool> TimeActions("time-actions", cl::Optional, cl::init(false),
    cl::desc("Print execution time for each action taken"));

#if RICH
static cl::opt<bool> ShowStats("stats", cl::Optional, cl::init(false),
    cl::desc("Print statistics accumulated during optimization"));
#endif

//===----------------------------------------------------------------------===//
//===          ELSA OPTIONS
//===----------------------------------------------------------------------===//

static cl::list<std::string> ElsaTraceOpts("tr", cl::ZeroOrMore,
    cl::desc("Elsa trace options"),
    cl::value_desc("option"));
static cl::opt<bool> ElsaPrettyPrint("bpprint", cl::Optional, cl::init(false),
    cl::desc("Output pretty printed source"));
static cl::opt<bool> ElsaPrettyPrintAfterElab("bpprintAfterElab", cl::Optional, cl::init(false),
    cl::desc("Output pretty printed source after elaboration"));

//===----------------------------------------------------------------------===//
//===          LINKER OPTIONS
//===----------------------------------------------------------------------===//

// Options to control the linking, optimization, and code gen processes
static cl::opt<bool> LinkAsLibrary("link-as-library",
  cl::desc("Link the .bc files together as a library, not an executable"));

static cl::alias Relink("r", cl::aliasopt(LinkAsLibrary),
  cl::desc("Alias for -link-as-library"));

static cl::opt<bool>NativeCBE("native-cbe",
  cl::desc("Generate a native binary with the C backend and GCC"));

static cl::list<std::string> PostLinkOpts("post-link-opts",
  cl::value_desc("path"),
  cl::desc("Run one or more optimization programs after linking"));

static cl::list<std::string> XLinker("Xlinker", cl::value_desc("option"),
  cl::desc("Pass options to the system linker"));

// Compatibility options that ellcc ignores but are supported for
// compatibility with LD
static cl::opt<std::string> CO3("soname", cl::Hidden,
  cl::desc("Compatibility option: ignored"));

static cl::opt<std::string> CO4("version-script", cl::Hidden,
  cl::desc("Compatibility option: ignored"));

static cl::opt<bool> CO5("eh-frame-hdr", cl::Hidden,
  cl::desc("Compatibility option: ignored"));

static  cl::opt<std::string> CO6("h", cl::Hidden,
  cl::desc("Compatibility option: ignored"));

static cl::opt<bool> CO7("start-group", cl::Hidden,
  cl::desc("Compatibility option: ignored"));

static cl::opt<bool> CO8("end-group", cl::Hidden,
  cl::desc("Compatibility option: ignored"));

//===----------------------------------------------------------------------===//
//===          TARGET CODE GENERATOR OPTIONS
//===----------------------------------------------------------------------===//

static cl::opt<std::string>
TargetTriple("mtriple", cl::desc("Override target triple for module"));

static cl::opt<const TargetMachineRegistry::entry*, false,
               TargetMachineRegistry::Parser>
MArch("march", cl::desc("Architecture to generate code for:"));

static cl::opt<std::string>
MCPU("mcpu", 
  cl::desc("Target a specific cpu type (-mcpu=help for details)"),
  cl::value_desc("cpu-name"),
  cl::init(""));

static cl::list<std::string>
MAttrs("mattr", 
  cl::CommaSeparated,
  cl::desc("Target specific attributes (-mattr=help for details)"),
  cl::value_desc("a1,+a2,-a3,..."));

cl::opt<TargetMachine::CodeGenFileType>
FileType("filetype", cl::init(TargetMachine::AssemblyFile),
  cl::desc("Choose a file type (not all types are supported by all targets):"),
  cl::values(
       clEnumValN(TargetMachine::AssemblyFile,    "asm",
                  "  Emit an assembly ('.s') file"),
       clEnumValN(TargetMachine::ObjectFile,    "obj",
                  "  Emit a native object ('.o') file [experimental]"),
       clEnumValN(TargetMachine::DynamicLibrary, "dynlib",
                  "  Emit a native dynamic library ('.so') file"
                  " [experimental]"),
       clEnumValEnd));

//===----------------------------------------------------------------------===//
//===          ADVANCED OPTIONS
//===----------------------------------------------------------------------===//

static cl::opt<std::string> ConfigDir("config-dir", cl::Optional,
    cl::desc("Specify configuration directory to override defaults"),
    cl::value_desc("dir"));

static cl::opt<bool> EmitRawCode("emit-raw-code", cl::Hidden, cl::Optional,
    cl::desc("Emit raw, unoptimized code"));

static cl::opt<bool> PipeCommands("pipe", cl::Optional,
    cl::desc("Invoke sub-commands by linking input/output with pipes"));

static cl::opt<bool> KeepTemps("keep-temps", cl::Optional,
    cl::desc("Don't delete temporary files created by ellcc"));

//===----------------------------------------------------------------------===//
//===          POSITIONAL OPTIONS
//===----------------------------------------------------------------------===//

static cl::list<std::string> Files(cl::Positional, cl::ZeroOrMore,
    cl::desc("[Sources/objects/libraries]"));

static cl::list<FileTypes> Languages("x", cl::ZeroOrMore,
    cl::desc("Specify the source language for subsequent files"),
    cl::values(
        clEnumValN(C, "c", "The C language"),
        clEnumValN(I, "c-cpp-output", "Preprocessed C language"),
        clEnumValN(H, "c-header", "A C language header"),
        clEnumValN(CC, "c++", "The C++ language"),
        clEnumValN(II, "c++-cpp-output", "Preprocessed C++ language"),
        clEnumValN(HH, "c++-header", "A C++ language header"),
        clEnumValN(LL, "llvm-as", "An LLVM assembly file"),
        clEnumValN(LLX, "llvm-as-with-cpp", "An LLVM assembly file that must be preprocessed"),
        clEnumValN(BC, "llvm-bc", "An LLVM bitcode file"),
        clEnumValN(UBC, "llvm-bc", "An unoptimized LLVM bitcode file"),
        clEnumValN(S, "assembly", "An assembly file"),
        clEnumValN(SX, "assembly-with-cpp", "An LLVM assembly file that must be preprocessed"),
        clEnumValN(O, "other", "An object file, linker command file, etc"),
        clEnumValEnd));

// The OptimizationList is automatically populated with registered Passes by the
// PassNameParser.
//
static cl::list<const PassInfo*, bool, PassNameParser>
OptimizationList(cl::desc("Optimizations available:"));

// Other command line options...
//

static cl::opt<bool>
PrintEachXForm("p", cl::desc("Print module after each transformation"));

static cl::opt<bool>
NoOutput("disable-output",
         cl::desc("Do not write result bitcode file"), cl::Hidden);

static cl::opt<bool>
NoVerify("disable-verify", cl::desc("Do not verify result module"), cl::Hidden);

static cl::opt<bool>
Quiet("q", cl::desc("Obsolete option"), cl::Hidden);

static cl::alias
QuietA("quiet", cl::desc("Alias for -q"), cl::aliasopt(Quiet));

static cl::opt<bool>
AnalyzeOnly("analyze", cl::desc("Only perform analysis, no optimization"));

/** The input file list.
 */
struct Input {
    sys::Path name;             ///< The input's name.
    FileTypes type;             ///< The input's type.
    Module* module;             ///< The module associated with the input, if any.
    bool temp;                  ///< true if this is contained in a temporary file.
    pw::Plexer* language;       ///< Non-NULL if this type has a language definition.
    Input() : type(NONE), module(NULL), temp(false) {}
    Input(std::string& name, FileTypes type = NONE, Module* module = NULL, bool temp = false, pw::Plexer* language = NULL)
        : name(name), type(type), module(module), temp(temp), language(language)  {}
    void setName(sys::Path newName)
    {
        if (temp) {
            if (!name.eraseFromDisk() && Verbose) {
                cout << "  " << name << " has been deleted\n";
            }
            temp = false;
        }
        name = newName;
    }
};
typedef std::vector<Input> InputList;

// ---------- Define Printers for module and function passes ------------
namespace {

struct CallGraphSCCPassPrinter : public CallGraphSCCPass {
  static char ID;
  const PassInfo *PassToPrint;
  CallGraphSCCPassPrinter(const PassInfo *PI) : 
    CallGraphSCCPass((intptr_t)&ID), PassToPrint(PI) {}

  virtual bool runOnSCC(const std::vector<CallGraphNode *>&SCC) {
    if (!Quiet) {
      cout << "Printing analysis '" << PassToPrint->getPassName() << "':\n";

      for (unsigned i = 0, e = SCC.size(); i != e; ++i) {
        Function *F = SCC[i]->getFunction();
        if (F) 
          getAnalysisID<Pass>(PassToPrint).print(cout, F->getParent());
      }
    }
    // Get and print pass...
    return false;
  }
  
  virtual const char *getPassName() const { return "'Pass' Printer"; }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequiredID(PassToPrint);
    AU.setPreservesAll();
  }
};

char CallGraphSCCPassPrinter::ID = 0;

struct ModulePassPrinter : public ModulePass {
  static char ID;
  const PassInfo *PassToPrint;
  ModulePassPrinter(const PassInfo *PI) : ModulePass((intptr_t)&ID),
                                          PassToPrint(PI) {}

  virtual bool runOnModule(Module &M) {
    if (!Quiet) {
      cout << "Printing analysis '" << PassToPrint->getPassName() << "':\n";
      getAnalysisID<Pass>(PassToPrint).print(cout, &M);
    }

    // Get and print pass...
    return false;
  }

  virtual const char *getPassName() const { return "'Pass' Printer"; }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequiredID(PassToPrint);
    AU.setPreservesAll();
  }
};

char ModulePassPrinter::ID = 0;
struct FunctionPassPrinter : public FunctionPass {
  const PassInfo *PassToPrint;
  static char ID;
  FunctionPassPrinter(const PassInfo *PI) : FunctionPass((intptr_t)&ID),
                                            PassToPrint(PI) {}

  virtual bool runOnFunction(Function &F) {
    if (!Quiet) { 
      cout << "Printing analysis '" << PassToPrint->getPassName()
           << "' for function '" << F.getName() << "':\n";
    }
    // Get and print pass...
    getAnalysisID<Pass>(PassToPrint).print(cout, F.getParent());
    return false;
  }

  virtual const char *getPassName() const { return "FunctionPass Printer"; }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequiredID(PassToPrint);
    AU.setPreservesAll();
  }
};

char FunctionPassPrinter::ID = 0;

struct LoopPassPrinter : public LoopPass {
  static char ID;
  const PassInfo *PassToPrint;
  LoopPassPrinter(const PassInfo *PI) : 
    LoopPass((intptr_t)&ID), PassToPrint(PI) {}

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) {
    if (!Quiet) {
      cout << "Printing analysis '" << PassToPrint->getPassName() << "':\n";
      getAnalysisID<Pass>(PassToPrint).print(cout, 
                                  L->getHeader()->getParent()->getParent());
    }
    // Get and print pass...
    return false;
  }
  
  virtual const char *getPassName() const { return "'Pass' Printer"; }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequiredID(PassToPrint);
    AU.setPreservesAll();
  }
};

char LoopPassPrinter::ID = 0;

struct BasicBlockPassPrinter : public BasicBlockPass {
  const PassInfo *PassToPrint;
  static char ID;
  BasicBlockPassPrinter(const PassInfo *PI) 
    : BasicBlockPass((intptr_t)&ID), PassToPrint(PI) {}

  virtual bool runOnBasicBlock(BasicBlock &BB) {
    if (!Quiet) {
      cout << "Printing Analysis info for BasicBlock '" << BB.getName()
           << "': Pass " << PassToPrint->getPassName() << ":\n";
    }

    // Get and print pass...
    getAnalysisID<Pass>(PassToPrint).print(cout, BB.getParent()->getParent());
    return false;
  }

  virtual const char *getPassName() const { return "BasicBlockPass Printer"; }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequiredID(PassToPrint);
    AU.setPreservesAll();
  }
};

char BasicBlockPassPrinter::ID = 0;
inline void addPass(PassManager &PM, Pass *P) {
  // Add the pass to the pass manager...
  PM.add(P);

  // If we are verifying all of the intermediate steps, add the verifier...
  if (VerifyEach) PM.add(createVerifierPass());
}

void AddStandardCompilePasses(PassManager &PM) {
  PM.add(createVerifierPass());                  // Verify that input is correct

  addPass(PM, createLowerSetJmpPass());          // Lower llvm.setjmp/.longjmp

  // If the -strip-debug command line option was specified, do it.
  if (StripDebug)
    addPass(PM, createStripSymbolsPass(true));

  if (OptLevel == OPT_NONE) return;

  addPass(PM, createRaiseAllocationsPass());     // call %malloc -> malloc inst
  addPass(PM, createCFGSimplificationPass());    // Clean up disgusting code
  addPass(PM, createPromoteMemoryToRegisterPass());// Kill useless allocas
  addPass(PM, createGlobalOptimizerPass());      // Optimize out global vars
  addPass(PM, createGlobalDCEPass());            // Remove unused fns and globs
  addPass(PM, createIPConstantPropagationPass());// IP Constant Propagation
  addPass(PM, createDeadArgEliminationPass());   // Dead argument elimination
  addPass(PM, createInstructionCombiningPass()); // Clean up after IPCP & DAE
  addPass(PM, createCFGSimplificationPass());    // Clean up after IPCP & DAE

  addPass(PM, createPruneEHPass());              // Remove dead EH info

  // RICH: If we inline here, an externally referenced function could be optimized out (bzip2).
  if (0 && !DisableInline)
    addPass(PM, createFunctionInliningPass());   // Inline small functions

  addPass(PM, createArgumentPromotionPass());    // Scalarize uninlined fn args

  addPass(PM, createTailDuplicationPass());      // Simplify cfg by copying code
  addPass(PM, createSimplifyLibCallsPass());     // Library Call Optimizations
  addPass(PM, createInstructionCombiningPass()); // Cleanup for scalarrepl.
  addPass(PM, createJumpThreadingPass());        // Thread jumps.
  addPass(PM, createCFGSimplificationPass());    // Merge & remove BBs
  addPass(PM, createScalarReplAggregatesPass()); // Break up aggregate allocas
  addPass(PM, createInstructionCombiningPass()); // Combine silly seq's
  addPass(PM, createCondPropagationPass());      // Propagate conditionals

  addPass(PM, createTailCallEliminationPass());  // Eliminate tail calls
  addPass(PM, createCFGSimplificationPass());    // Merge & remove BBs
  addPass(PM, createReassociatePass());          // Reassociate expressions
  addPass(PM, createLoopRotatePass());
  addPass(PM, createLICMPass());                 // Hoist loop invariants
  addPass(PM, createLoopUnswitchPass());         // Unswitch loops.
  addPass(PM, createLoopIndexSplitPass());       // Index split loops.
  // FIXME : Removing instcombine causes nestedloop regression.
  addPass(PM, createInstructionCombiningPass()); 
  addPass(PM, createIndVarSimplifyPass());       // Canonicalize indvars
  addPass(PM, createLoopDeletionPass());         // Delete dead loops
  addPass(PM, createLoopUnrollPass());           // Unroll small loops
  addPass(PM, createInstructionCombiningPass()); // Clean up after the unroller
  addPass(PM, createGVNPass());                  // Remove redundancies
  addPass(PM, createMemCpyOptPass());            // Remove memcpy / form memset
  addPass(PM, createSCCPPass());                 // Constant prop with SCCP

  // Run instcombine after redundancy elimination to exploit opportunities
  // opened up by them.
  addPass(PM, createInstructionCombiningPass());
  addPass(PM, createCondPropagationPass());      // Propagate conditionals

  addPass(PM, createDeadStoreEliminationPass()); // Delete dead stores
  addPass(PM, createAggressiveDCEPass());        // SSA based 'Aggressive DCE'
  addPass(PM, createCFGSimplificationPass());    // Merge & remove BBs
  addPass(PM, createStripDeadPrototypesPass());  // Get rid of dead prototypes
  addPass(PM, createDeadTypeEliminationPass());  // Eliminate dead types
  addPass(PM, createConstantMergePass());        // Merge dup global constants
}

} // anonymous namespace

//===----------------------------------------------------------------------===//
//===          GetFileType - determine type of a file
//===----------------------------------------------------------------------===//
static const FileTypes GetFileType(const std::string& fname, unsigned pos) {
    static std::vector<FileTypes>::iterator langIt = Languages.begin();
    static FileTypes CurrLang = O;

    // If a -x LANG option has been specified ..
    if (langIt != Languages.end())
        // If the -x LANG option came before the current file on command line
        if (Languages.getPosition( langIt - Languages.begin() ) < pos) {
            // use that language
            CurrLang = *langIt++;
            return CurrLang;
        }

    // If there's a current language in effect
    if (CurrLang != O)
        return CurrLang; // use that language

    // otherwise just determine lang from the filename's suffix
    std::string ext(fname.substr(fname.rfind('.', fname.size()) + 1));
    if (extToLang.find(ext) == extToLang.end()) {
        return O;
    }

    return extToLang[ext];
}

/// PrintAndExit - Prints a message to standard error and exits with an error code.
///
/// Inputs:
///  Message  - The message to print to standard error.
///
static void PrintAndExit(const std::string &Message, int errcode = 1)
{
    cerr << progname << ": " << Message << "\n";
    llvm_shutdown();
    exit(errcode);
}

/// Exit - Exit with an error code.
///
static void Exit(int errcode = 1)
{
    llvm_shutdown();
    exit(errcode);
}

/// Optimize - Perform link time optimizations. This will run the scalar
/// optimizations, any loaded plugin-optimization modules, and then the
/// inter-procedural optimizations if applicable.
static void Optimize(Module* M)
{
  // Instantiate the pass manager to organize the passes.
  PassManager Passes;

  // If we're verifying, start off with a verification pass.
  if (VerifyEach)
    Passes.add(createVerifierPass());

  // Add an appropriate TargetData instance for this module...
  addPass(Passes, new TargetData(M));

  if (OptLevel != OPT_NONE) {
    // Now that composite has been compiled, scan through the module, looking
    // for a main function.  If main is defined, mark all other functions
    // internal.
    if (!DisableInternalize)
      addPass(Passes, createInternalizePass(true));

    // Propagate constants at call sites into the functions they call.  This
    // opens opportunities for globalopt (and inlining) by substituting function
    // pointers passed as arguments to direct uses of functions.  
    addPass(Passes, createIPSCCPPass());

    // Now that we internalized some globals, see if we can hack on them!
    addPass(Passes, createGlobalOptimizerPass());

    // Linking modules together can lead to duplicated global constants, only
    // keep one copy of each constant...
    addPass(Passes, createConstantMergePass());

    // Remove unused arguments from functions...
    addPass(Passes, createDeadArgEliminationPass());

    // Reduce the code after globalopt and ipsccp.  Both can open up significant
    // simplification opportunities, and both can propagate functions through
    // function pointers.  When this happens, we often have to resolve varargs
    // calls, etc, so let instcombine do this.
    addPass(Passes, createInstructionCombiningPass());

    if (!DisableInline)
      addPass(Passes, createFunctionInliningPass()); // Inline small functions

    addPass(Passes, createPruneEHPass());            // Remove dead EH info
    addPass(Passes, createGlobalOptimizerPass());    // Optimize globals again.
    addPass(Passes, createGlobalDCEPass());          // Remove dead functions

    // If we didn't decide to inline a function, check to see if we can
    // transform it to pass arguments by value instead of by reference.
    addPass(Passes, createArgumentPromotionPass());

    // The IPO passes may leave cruft around.  Clean up after them.
    addPass(Passes, createInstructionCombiningPass());

    addPass(Passes, createScalarReplAggregatesPass()); // Break up allocas

    // Run a few AA driven optimizations here and now, to cleanup the code.
    addPass(Passes, createGlobalsModRefPass());      // IP alias analysis

    addPass(Passes, createLICMPass());               // Hoist loop invariants
    addPass(Passes, createGVNPass());                  // Remove redundancies
    addPass(Passes, createDeadStoreEliminationPass()); // Nuke dead stores

    // Cleanup and simplify the code after the scalar optimizations.
    addPass(Passes, createInstructionCombiningPass());

    // Delete basic blocks, which optimization passes may have killed...
    addPass(Passes, createCFGSimplificationPass());

    // Now that we have optimized the program, discard unreachable functions...
    addPass(Passes, createGlobalDCEPass());
  }

  // If the -s or -S command line options were specified, strip the symbols out
  // of the resulting program to make it smaller.  -s and -S are GNU ld options
  // that we are supporting; they alias -strip-all and -strip-debug.
  if (Strip || StripDebug)
    addPass(Passes, createStripSymbolsPass(StripDebug && !Strip));

  // Create a new optimization pass for each one specified on the command line
  std::auto_ptr<TargetMachine> target;
  for (unsigned i = 0; i < OptimizationList.size(); ++i) {
    const PassInfo *Opt = OptimizationList[i];
    if (Opt->getNormalCtor())
      addPass(Passes, Opt->getNormalCtor()());
    else
      std::cerr << "llvm-ld: cannot create pass: " << Opt->getPassName() 
                << "\n";
  }

  // The user's passes may leave cruft around. Clean up after them them but
  // only if we haven't got optimizations enabled
  if (OptLevel != OPT_NONE) {
    addPass(Passes, createInstructionCombiningPass());
    addPass(Passes, createCFGSimplificationPass());
    addPass(Passes, createDeadCodeEliminationPass());
    addPass(Passes, createGlobalDCEPass());
  }

  // Make sure everything is still good.
  Passes.add(createVerifierPass());

  // Run our queue of passes all at once now, efficiently.
  Passes.run(*M);
}

static void PrintCommand(const std::vector<const char*> &args) {
  std::vector<const char*>::const_iterator I = args.begin(), E = args.end(); 
  for (; I != E; ++I)
    if (*I)
      cout << *I << " ";
  cout << "\n" << std::flush;
}

/// CopyEnv - This function takes an array of environment variables and makes a
/// copy of it.  This copy can then be manipulated any way the caller likes
/// without affecting the process's real environment.
///
/// Inputs:
///  envp - An array of C strings containing an environment.
///
/// Return value:
///  NULL - An error occurred.
///
///  Otherwise, a pointer to a new array of C strings is returned.  Every string
///  in the array is a duplicate of the one in the original array (i.e. we do
///  not copy the char *'s from one array to another).
///
static char ** CopyEnv(char ** const envp) {
  // Count the number of entries in the old list;
  unsigned entries;   // The number of entries in the old environment list
  for (entries = 0; envp[entries] != NULL; entries++)
    /*empty*/;

  // Add one more entry for the NULL pointer that ends the list.
  ++entries;

  // If there are no entries at all, just return NULL.
  if (entries == 0)
    return NULL;

  // Allocate a new environment list.
  char **newenv = new char* [entries];
  if ((newenv = new char* [entries]) == NULL)
    return NULL;

  // Make a copy of the list.  Don't forget the NULL that ends the list.
  entries = 0;
  while (envp[entries] != NULL) {
    newenv[entries] = new char[strlen (envp[entries]) + 1];
    strcpy (newenv[entries], envp[entries]);
    ++entries;
  }
  newenv[entries] = NULL;

  return newenv;
}


/// RemoveEnv - Remove the specified environment variable from the environment
/// array.
///
/// Inputs:
///  name - The name of the variable to remove.  It cannot be NULL.
///  envp - The array of environment variables.  It cannot be NULL.
///
/// Notes:
///  This is mainly done because functions to remove items from the environment
///  are not available across all platforms.  In particular, Solaris does not
///  seem to have an unsetenv() function or a setenv() function (or they are
///  undocumented if they do exist).
///
static void RemoveEnv(const char * name, char ** const envp) {
  for (unsigned index=0; envp[index] != NULL; index++) {
    // Find the first equals sign in the array and make it an EOS character.
    char *p = strchr (envp[index], '=');
    if (p == NULL)
      continue;
    else
      *p = '\0';

    // Compare the two strings.  If they are equal, zap this string.
    // Otherwise, restore it.
    if (!strcmp(name, envp[index]))
      *envp[index] = '\0';
    else
      *p = '=';
  }

  return;
}

#if 0

/// Preprocess - Preprocess the given file.
///
/// Inputs:
///  InputFilename   - The name of the input source file.
///  OutputFilename  - The name of the file to generate.
///
/// Outputs:
///  None.
///
/// Returns non-zero value on error.
///
static int Preprocess(const std::string &OutputFilename,
                      const std::string &InputFilename,
                      pw::Plexer* language,
                      std::string& ErrMsg)
{
  // Remove these environment variables from the environment of the
  // programs that we will execute.  It appears that GCC sets these
  // environment variables so that the programs it uses can configure
  // themselves identically.
  //
  // However, when we invoke GCC below, we want it to use its normal
  // configuration.  Hence, we must sanitize its environment.
  // Determine the location of the gcc program.
  sys::Path gcc = FindExecutable("gcc", progname);
  if (gcc.isEmpty())
    PrintAndExit("Failed to find gcc");

  // Mark the output files for removal if we get an interrupt.
  sys::RemoveFileOnSignal(sys::Path(OutputFilename));

  extern char **environ;
  char ** clean_env = CopyEnv(environ);
  if (clean_env == NULL)
    return 1;
  RemoveEnv("LIBRARY_PATH", clean_env);
  RemoveEnv("COLLECT_GCC_OPTIONS", clean_env);
  RemoveEnv("GCC_EXEC_PREFIX", clean_env);
  RemoveEnv("COMPILER_PATH", clean_env);
  RemoveEnv("COLLECT_GCC", clean_env);

  // Run GCC to preprocess the file..
  std::vector<std::string> args;
  args.push_back(gcc.c_str());
  args.push_back("-E");
  args.push_back("-o");
  args.push_back(OutputFilename);
  args.push_back(InputFilename);

  // Now that "args" owns all the std::strings for the arguments, call the c_str
  // method to get the underlying string array.  We do this game so that the
  // std::string array is guaranteed to outlive the const char* array.
  std::vector<const char *> Args;
  for (unsigned i = 0, e = args.size(); i != e; ++i)
    Args.push_back(args[i].c_str());
  Args.push_back(0);
  if (Verbose) {
    cout << "    ";
    PrintCommand(Args);
  }

  // Run gcc to preprocess the file.
  int R = sys::Program::ExecuteAndWait(
    gcc, &Args[0], (const char**)clean_env, 0, 0, 0, &ErrMsg);
  delete [] clean_env;
  return R;
}

#else

/// Preprocess - Preprocess the given file.
///
/// Inputs:
///  InputFilename   - The name of the input source file.
///  OutputFilename  - The name of the file to generate.
///
/// Outputs:
///  None.
///
/// Returns non-zero value on error.
///
static int Preprocess(const std::string &OutputFilename,
                      const std::string &InputFilename,
                      pw::Plexer* language,
                      std::string& ErrMsg)
{
    pw::PP* pp = new pw::PP(InputFilename, errors);
    FILE* fp = NULL;

    if (pp == NULL) {
        // An error occured creating the preprocessor.
        ErrMsg = "can't create preprocessor";
        return 1;
    }

    if (!pp->setInput(fp)) {
        ErrMsg = "can't open " + InputFilename;
        delete pp;
        return 1;
    }

    FILE* ofp;
    if (FinalPhase == PREPROCESSING) {
        // Output everything to stdout (like gcc).
        ofp = stdout;
    } else {
        ofp = pw::tfopen(OutputFilename.c_str(), "w");
        if (ofp == NULL) {
            ErrMsg = "can't open " + OutputFilename + " for writing.";
            delete pp;
            return 1;
        }
    }

    if (language) {
        pw::Options options = language->options;
        pp->setOptions(&options);    		// Set pre-processor options.

        // Set pre-defined macros.
        for (int i = 0; i < language->macros.size(); ++i) {
            pp->addDefine(language->macros[i]);
        }

        // Set the user include paths.
        for (size_t i = 0; i < Includes.size(); ++i) {
            pp->addUserInclude(Includes[i]);
        }

        // Set the include paths.
        for (int i = 0; i < language->includes.size(); ++i) {
            pp->addInclude(language->includes[i]);
        }

        // Preprocess the file.
        const char* lastfile;
        lastfile = errors.file;
        fprintf(ofp, "# %d \"%s\"\n", 1, errors.file);
        pp->getToken(pw::PP::GETALL);
        for (;;) {
            if (pp->info.token == pw::PPStream::ENDOFFILE) {
                // End of file.
                break;
            }

            if (errors.file != lastfile) {
                // Output #line directive if pre-processing.
                lastfile = errors.file;
                fprintf(ofp, "# %d \"%s\"\n", pp->info.startline, errors.file);
            }

            fprintf(ofp, "%s", pp->info.string.c_str());
            pp->getToken(pw::PP::GETALL);
        }
    }

    if (FinalPhase != PREPROCESSING) {
        pw::fclose(ofp);
    }
    delete pp;
    return 0;
}

#endif

/// Link - generates a native object file from the
/// specified bitcode file.
///
/// Inputs:
///  InputFilename   - The name of the input bitcode file.
///  OutputFilename  - The name of the file to generate.
///  NativeLinkItems - The native libraries, files, code with which to link
///  LibPaths        - The list of directories in which to find libraries.
///
/// Outputs:
///  None.
///
/// Returns non-zero value on error.
///
static int Link(const std::string& OutputFilename,
                std::vector<Input*>& InputFilenames,
                const Linker::ItemList& LinkItems,
                std::string& ErrMsg)
{
  // Remove these environment variables from the environment of the
  // programs that we will execute.  It appears that GCC sets these
  // environment variables so that the programs it uses can configure
  // themselves identically.
  //
  // However, when we invoke GCC below, we want it to use its normal
  // configuration.  Hence, we must sanitize its environment.
        
  // Determine the location of the ld program.
  sys::Path ld = FindExecutable("gcc", progname);
  if (ld.isEmpty())
    PrintAndExit("Failed to find ld");

  // Mark the output files for removal if we get an interrupt.
  sys::RemoveFileOnSignal(sys::Path(OutputFilename));

  extern char **environ;
  char ** clean_env = CopyEnv(environ);
  if (clean_env == NULL)
    return 1;
  RemoveEnv("LIBRARY_PATH", clean_env);
  RemoveEnv("COLLECT_GCC_OPTIONS", clean_env);
  RemoveEnv("GCC_EXEC_PREFIX", clean_env);
  RemoveEnv("COMPILER_PATH", clean_env);
  RemoveEnv("COLLECT_GCC", clean_env);


  // RICH:
  // Run GCC to assemble and link the program into native code.
  //
  // Note:
  //  We can't just assemble and link the file with the system assembler
  //  and linker because we don't know where to put the _start symbol.
  //  GCC mysteriously knows how to do it.
  std::vector<std::string> args;
  args.push_back(ld.c_str());
  // RICH: args.push_back("-fno-strict-aliasing");
  // RICH: args.push_back("-O3");
  args.push_back("-o");
  args.push_back(OutputFilename);
  for (unsigned i = 0; i < InputFilenames.size(); ++i ) {
      args.push_back(InputFilenames[i]->name.toString());
  }
            
  // Add in the library paths
  for (unsigned index = 0; index < LibPaths.size(); index++) {
    args.push_back("-L");
    args.push_back(LibPaths[index]);
  }

  // Add the requested options
  for (unsigned index = 0; index < XLinker.size(); index++) {
    args.push_back(XLinker[index]);
    args.push_back(Libraries[index]);
  }

  // Add in the libraries to link.
  for (unsigned index = 0; index < LinkItems.size(); index++)
    if (LinkItems[index].first != "crtend") {
      if (LinkItems[index].second)
        args.push_back("-l" + LinkItems[index].first);
      else
        args.push_back(LinkItems[index].first);
    }

  // Now that "args" owns all the std::strings for the arguments, call the c_str
  // method to get the underlying string array.  We do this game so that the
  // std::string array is guaranteed to outlive the const char* array.
  std::vector<const char *> Args;
  for (unsigned i = 0, e = args.size(); i != e; ++i)
    Args.push_back(args[i].c_str());
  Args.push_back(0);

  if (Verbose) {
    cout << "    ";
    PrintCommand(Args);
  }

  // Run the compiler to assembly and link together the program.
  int R = sys::Program::ExecuteAndWait(
    ld, &Args[0], (const char**)clean_env, 0, 0, 0, &ErrMsg);
  delete [] clean_env;
  return R;
}

/// Assemble - Assemble the given file.
///
/// Inputs:
///  InputFilename   - The name of the input source file.
///  OutputFilename  - The name of the file to generate.
///
/// Outputs:
///  None.
///
/// Returns non-zero value on error.
///
static int Assemble(const std::string &OutputFilename,
                      const std::string &InputFilename,
                      std::string& ErrMsg)
{
  // Remove these environment variables from the environment of the
  // programs that we will execute.  It appears that GCC sets these
  // environment variables so that the programs it uses can configure
  // themselves identically.
  // RICH: Choose the appropriate assembler.
  sys::Path as = FindExecutable("as", progname);
  if (as.isEmpty())
    PrintAndExit("Failed to find as");

  // Mark the output files for removal if we get an interrupt.
  sys::RemoveFileOnSignal(sys::Path(OutputFilename));

  // Run as to preprocess the file..
  std::vector<std::string> args;
  args.push_back(as.c_str());
  args.push_back("-o");
  args.push_back(OutputFilename);
  args.push_back(InputFilename);

  // Now that "args" owns all the std::strings for the arguments, call the c_str
  // method to get the underlying string array.  We do this game so that the
  // std::string array is guaranteed to outlive the const char* array.
  std::vector<const char *> Args;
  for (unsigned i = 0, e = args.size(); i != e; ++i)
    Args.push_back(args[i].c_str());
  Args.push_back(0);
  if (Verbose) {
    cout << "    ";
    PrintCommand(Args);
  }

  // Run as to preprocess the file.
  int R = sys::Program::ExecuteAndWait(
    as, &Args[0], NULL, 0, 0, 0, &ErrMsg);
  return R;
}

//===----------------------------------------------------------------------===//
//===          doMulti - Handle a phase acting on multiple files.
//===----------------------------------------------------------------------===//
static void doMulti(Phases phase, std::vector<Input*>& files, InputList& result, TimerGroup& timerGroup)
{
    switch (phase) {
    case BCLINKING: {
        if (TimeActions) {
            timers[phase]->startTimer();
        }

        // Generate the output name.
        sys::Path outputName(OutputFilename);
        outputName.eraseSuffix();
        outputName.appendSuffix("bc");
        // Construct a Linker.
        Linker TheLinker(progname, outputName.toString(), Verbose);

        // Keep track of the native link items (versus the bitcode items)
        Linker::ItemList NativeLinkItems;

        // Add library paths to the linker
        TheLinker.addPaths(LibPaths);
        TheLinker.addSystemPaths();

        // Remove any consecutive duplicates of the same library...
        Libraries.erase(std::unique(Libraries.begin(), Libraries.end()),
                Libraries.end());

        std::vector<sys::Path> Files;
        for (unsigned i = 0; i < files.size(); ++i) {
            if (files[i]->module) {
                // We have this module.
                std::string ErrorMessage;
                if (TheLinker.LinkInModule(files[i]->module, &ErrorMessage)) {
                    PrintAndExit(ErrorMessage);
                }
                if (Verbose) {
                    cout << "  " << files[i]->name << " was consumed by the bitcode linker\n";
                }
                files[i]->module = NULL;         // The module has been consumed.
                files[i]->name.clear();
            } else {
                Files.push_back(sys::Path(files[i]->name));
            }
        }
            
        if (Files.size() && TheLinker.LinkInFiles(Files))
            PrintAndExit(TheLinker.getLastError());

        if (LinkAsLibrary) {
            // The libraries aren't linked in but are noted as "dependent" in the
            // module.
            for (cl::list<std::string>::const_iterator I = Libraries.begin(),
                    E = Libraries.end(); I != E ; ++I) {
                TheLinker.getModule()->addLibrary(*I);
            }
        } else {
            // Add the libraries.
            for (cl::list<std::string>::const_iterator I = Libraries.begin(),
                    E = Libraries.end(); I != E ; ++I) {
                bool isNative;  // RICH?
                if (TheLinker.LinkInLibrary(*I, isNative)) {
                    PrintAndExit(TheLinker.getLastError());
                }
            }

            // Link all the items together
            Linker::ItemList Items;
            if (TheLinker.LinkInItems(Items, NativeLinkItems) )
                PrintAndExit(TheLinker.getLastError());
        }

        Module* module = TheLinker.releaseModule();

        // Optimize the module.
        Optimize(module);

        // Add the linked bitcode to the input list.
        std::string name(outputName.toString());
        Input input(name, BC, module);
        input.temp = true;
        if (Verbose) {
            cout << "  " << fileActions[filePhases[BC][phase].action].name
                << " " << outputName << " added to the file list\n";
        }
        result.push_back(input);

        if (TimeActions) {
	    timers[phase]->stopTimer();
        }
        break;
    }
    case LINKING: {
        if (TimeActions) {
            timers[phase]->startTimer();
        }

        std::string ErrMsg;  
        // Keep track of the native link items (versus the bitcode items)
        Linker::ItemList NativeLinkItems;      // RICH
        if (Link(OutputFilename, files, NativeLinkItems, ErrMsg) != 0) {
            PrintAndExit(ErrMsg);
        }

        if (TimeActions) {
	    timers[phase]->stopTimer();
        }
        break;
    }
    default:
        // RICH: Illegal multi pass.
        break;
    }

    // Delete any consumed files.
    for (unsigned i = 0; i < files.size(); ++i ) {
        if (files[i]->temp) {
            if (!files[i]->name.eraseFromDisk() && Verbose) {
                cout << "  " << files[i]->name << " has been deleted\n";
                files[i]->name.clear();
            }
        }
    }
}

//===----------------------------------------------------------------------===//
//===          doSingle - Handle a phase acting on a single file.
//===----------------------------------------------------------------------===//
static FileTypes doSingle(Phases phase, Input& input, Elsa& elsa, FileTypes thisType)
{
    FileTypes nextType = filePhases[thisType][phase].type;

    switch (phase) {
    case PREPROCESSING: {              // Source language combining, filtering, substitution
        if (TimeActions) {
	    timers[phase]->startTimer();
        }

        if (Verbose) {
            // This file needs processing during this phase.
            cout << "  " << fileActions[filePhases[thisType][phase].action].name << " " << input.name
                << " to become " << fileTypes[nextType] << "\n";
        }

        sys::Path to(input.name.getBasename());
        to.appendSuffix(langToExt[nextType]);

        std::string ErrMsg;  
        if(Preprocess(to.toString(), input.name.toString(), input.language, ErrMsg) != 0) {
            PrintAndExit(ErrMsg);
        }

        input.setName(to);
        // Mark the file as a temporary file.
        input.temp = true;

        if (TimeActions) {
	    timers[phase]->stopTimer();
        }
        break;
    }
    case TRANSLATION: {              // Translate source -> LLVM bitcode/assembly
        // Elsa does its own timing.

        if (Verbose) {
            // This file needs processing during this phase.
            cout << "  " << fileActions[filePhases[thisType][phase].action].name << " " << input.name
                << " to become " << fileTypes[nextType] << "\n";
        }

        sys::Path to(input.name.getBasename());
        to.appendSuffix(langToExt[nextType]);
        if (filePhases[thisType][phase].action == CCOMPILE) {
            Elsa::Language lang = Elsa::GNUC;
            if (thisType == II) {
                // This is a C++ file.
                lang = Elsa::GNUCXX;
                // RICH: std: C, C++, K&R, etc.
            } else {
                // This is a C file.
                // RICH: std: C, C++, K&R, etc.
            }
            int result = elsa.parse(lang, input.name.c_str(), to.c_str(), input.module, input.language);
            if (result) {
                Exit(result);
            }
        } else {
            // RICH: Non C sources, e.g. .ll->.ubc
        }
        input.setName(to);
        break;
    }
    case OPTIMIZATION: {             // Optimize translation result
        if (TimeActions) {
	    timers[phase]->startTimer();
        }

        if (Verbose) {
            // This file needs processing during this phase.
            cout << "  " << fileActions[filePhases[thisType][phase].action].name << " " << input.name
                << " to become " << fileTypes[nextType] << "\n";
        }

        sys::Path to(input.name.getBasename());
        to.appendSuffix(langToExt[nextType]);
        std::string ErrorMessage;
        if (input.module == NULL) {
            // Load the input module...
            if (MemoryBuffer *Buffer = MemoryBuffer::getFileOrSTDIN(to.toString(), &ErrorMessage)) {
                input.module = ParseBitcodeFile(Buffer, &ErrorMessage);
                delete Buffer;
            }
        }

        if (input.module == NULL) {
            if (ErrorMessage.size()) {
                PrintAndExit(ErrorMessage);
            } else {
                PrintAndExit("bitcode didn't read correctly.");
            }
        }

        input.setName(to);

#if RICH
        // Figure out what stream we are supposed to write to...
        // FIXME: cout is not binary!
        std::ostream *Out = &std::cout;  // Default to printing to stdout...
        if (OutputFilename != "-") {
            if (!Force && std::ifstream(OutputFilename.c_str())) {
                // If force is not specified, make sure not to overwrite a file!
                cerr << argv[0] << ": error opening '" << OutputFilename
                     << "': file exists!\n"
                     << "Use -f command line argument to force output\n";
                return 1;
            }
            std::ios::openmode io_mode = std::ios::out | std::ios::trunc |
                                       std::ios::binary;
            Out = new std::ofstream(OutputFilename.c_str(), io_mode);
    
            if (!Out->good()) {
                cerr << argv[0] << ": error opening " << OutputFilename << "!\n";
              return 1;
            }

            // Make sure that the Output file gets unlinked from the disk if we get a
            // SIGINT
            sys::RemoveFileOnSignal(sys::Path(OutputFilename));
        }

        // If the output is set to be emitted to standard out, and standard out is a
        // console, print out a warning message and refuse to do it.  We don't
        // impress anyone by spewing tons of binary goo to a terminal.
        if (!Force && !NoOutput && CheckBitcodeOutputToConsole(Out,!Quiet)) {
            NoOutput = true;
        }
#endif

        // Create a PassManager to hold and optimize the collection of passes we are
        // about to build...
        //
        PassManager Passes;

        // Add an appropriate TargetData instance for this module...
        Passes.add(new TargetData(input.module));

        // If -std-compile-opts is given, add in all the standard compilation 
        // optimizations first. This will handle -strip-debug, -disable-inline,
        // and -disable-opt as well.
        if (OptLevel > OPT_FAST_COMPILE && OptLevel < OPT_NONE)
            AddStandardCompilePasses(Passes);

        // otherwise if the -strip-debug command line option was specified, add it.
        else if (StripDebug)
            addPass(Passes, createStripSymbolsPass(true));
    
        // Create a new optimization pass for each one specified on the command line
        for (unsigned i = 0; i < OptimizationList.size(); ++i) {
            const PassInfo *PassInf = OptimizationList[i];
            Pass *P = 0;
            if (PassInf->getNormalCtor())
              P = PassInf->getNormalCtor()();
            else
              PrintAndExit(std::string("cannot create pass: ") + PassInf->getPassName());
            if (P) {
                addPass(Passes, P);
            
                if (AnalyzeOnly) {
                    if (dynamic_cast<BasicBlockPass*>(P))
                        Passes.add(new BasicBlockPassPrinter(PassInf));
                    else if (dynamic_cast<LoopPass*>(P))
                        Passes.add(new  LoopPassPrinter(PassInf));
                    else if (dynamic_cast<FunctionPass*>(P))
                        Passes.add(new FunctionPassPrinter(PassInf));
                    else if (dynamic_cast<CallGraphSCCPass*>(P))
                        Passes.add(new CallGraphSCCPassPrinter(PassInf));
                    else
                        Passes.add(new ModulePassPrinter(PassInf));
                }
            }
          
            if (PrintEachXForm)
                Passes.add(new PrintModulePass(&cerr));
        }
    
        // Check that the module is well formed on completion of optimization
        if (!NoVerify && !VerifyEach)
            Passes.add(createVerifierPass());

#if RICH
        // Write bitcode out to disk or cout as the last step...
        if (!NoOutput && !AnalyzeOnly)
            Passes.add(CreateBitcodeWriterPass(*Out));
#endif
    
        // Now that we have all of the passes ready, run them.
        Passes.run(*input.module);

#if RICH
        // Delete the ofstream.
        if (Out != &std::cout) 
            delete Out;
#endif

        if (TimeActions) {
	    timers[phase]->stopTimer();
        }

        break;
    }
    case GENERATION: {               // Convert .bc to ...
        if (TimeActions) {
	    timers[phase]->startTimer();
        }

        std::string ErrorMessage;
        if (input.module == NULL) {
            // Load the input module...
            if (MemoryBuffer *Buffer = MemoryBuffer::getFileOrSTDIN(input.name.toString(), &ErrorMessage)) {
                input.module = ParseBitcodeFile(Buffer, &ErrorMessage);
                delete Buffer;
            }
        }

        if (input.module == NULL) {
            if (ErrorMessage.size()) {
                PrintAndExit(ErrorMessage);
            } else {
                PrintAndExit("bitcode didn't read correctly.");
            }
        }

        // If we are supposed to override the target triple, do so now.
        if (!TargetTriple.empty()) {
            input.module->setTargetTriple(TargetTriple);
        }

        // Allocate target machine.  First, check whether the user has
        // explicitly specified an architecture to compile for.
        if (MArch == NULL) {
            std::string Err;
            MArch = TargetMachineRegistry::getClosestStaticTargetForModule(*input.module, Err);
            if (MArch == NULL) {
                // RICH: PrintAndExitN?
                std::cerr << progname << ": error auto-selecting target for module '"
                    << Err << "'.  Please use the -march option to explicitly "
                    << "pick a target.\n";
                Exit(1);
            }
        }

        // Package up features to be passed to target/subtarget
        std::string FeaturesStr;
        if (MCPU.size() || MAttrs.size()) {
            SubtargetFeatures Features;
            Features.setCPU(MCPU);
            for (unsigned i = 0; i != MAttrs.size(); ++i)
                Features.AddFeature(MAttrs[i]);
            FeaturesStr = Features.getString();
        }

        std::auto_ptr<TargetMachine> target(MArch->CtorFn(*input.module, FeaturesStr));
        assert(target.get() && "Could not allocate target machine!");
        TargetMachine &Target = *target.get();

        sys::Path to(input.name.getBasename());

#if RICH
        // Figure out where we are going to send the output...
        std::ostream *Out = NULL;
        if (OutputFilename != "") {
            if (OutputFilename == "-")
                return &std::cout;

            // Specified an output filename?
            // Make sure that the Out file gets unlinked from the disk if we get a
            // SIGINT
            sys::RemoveFileOnSignal(sys::Path(OutputFilename));

            return new std::ofstream(OutputFilename.c_str());
        }

        if (InputFilename == "-") {
            OutputFilename = "-";
            return &std::cout;
        }

        OutputFilename = GetFileNameRoot(InputFilename);
#endif

        switch (FileType) {
            case TargetMachine::AssemblyFile:
                if (MArch->Name[0] != 'c' || MArch->Name[1] != 0) { // not CBE
                    to.appendSuffix("s");
                    nextType = S;
                } else {
                    to.appendSuffix("cbe.c");
                    nextType = CBE;
                }
                break;
            case TargetMachine::ObjectFile:
                to.appendSuffix("o");
                nextType = O;
                break;
            case TargetMachine::DynamicLibrary:
                to.appendSuffix("dll");
                nextType = DLL;
                break;
        }

        if (Verbose) {
            // This file needs processing during this phase.
            cout << "  " << fileActions[filePhases[thisType][phase].action].name << " " << input.name
                << " to become " << fileTypes[nextType] << "\n";
        }

        // Make sure that the Out file gets unlinked from the disk if we get a
        // SIGINT
        sys::RemoveFileOnSignal(to);

        std::ostream *Out = new std::ofstream(to.c_str());
        if (Out == NULL || !Out->good()) {
            std::cerr << progname << ": error opening " << OutputFilename << "!\n";
            delete Out;
            Exit(1);
        }

        // If this target requires addPassesToEmitWholeFile, do it now.  This is
        // used by strange things like the C backend.
        if (Target.WantsWholeFile()) {
            PassManager PM;
            PM.add(new TargetData(*Target.getTargetData()));
            if (!NoVerify)
                PM.add(createVerifierPass());

            // Ask the target to add backend passes as necessary.
            if (Target.addPassesToEmitWholeFile(PM, *Out, FileType, OptLevel == OPT_FAST_COMPILE)) {
                // RICH:
                std::cerr << progname << ": target does not support generation of this"
                    << " file type!\n";
                if (Out != &std::cout) delete Out;
                // And the Out file is empty and useless, so remove it now.
                sys::Path(OutputFilename).eraseFromDisk();
                Exit(1);
            }
            PM.run(*input.module);
        } else {
            // Build up all of the passes that we want to do to the module.
            FunctionPassManager Passes(new ExistingModuleProvider(input.module));
            Passes.add(new TargetData(*Target.getTargetData()));

#ifndef NDEBUG
            if (!NoVerify)
                Passes.add(createVerifierPass());
#endif

            // Ask the target to add backend passes as necessary.
            MachineCodeEmitter *MCE = 0;

            switch (Target.addPassesToEmitFile(Passes, *Out, FileType, OptLevel == OPT_FAST_COMPILE)) {
                default:
                    assert(0 && "Invalid file model!");
                    Exit(1);
                    break;
                case FileModel::Error:
                    std::cerr << progname << ": target does not support generation of this"
                        << " file type!\n";
                    if (Out != &std::cout) delete Out;
                    // And the Out file is empty and useless, so remove it now.
                    sys::Path(OutputFilename).eraseFromDisk();
                    Exit(1);
                    break;
                case FileModel::AsmFile:
                    break;
                case FileModel::MachOFile:
                    MCE = AddMachOWriter(Passes, *Out, Target);
                    break;
                case FileModel::ElfFile:
                    MCE = AddELFWriter(Passes, *Out, Target);
                    break;
            }

            if (Target.addPassesToEmitFileFinish(Passes, MCE, OptLevel == OPT_FAST_COMPILE)) {
                std::cerr << progname << ": target does not support generation of this"
                    << " file type!\n";
                if (Out != &std::cout) delete Out;
                // And the Out file is empty and useless, so remove it now.
                sys::Path(OutputFilename).eraseFromDisk();
                Exit(1);
            }

            Passes.doInitialization();

            // Run our queue of passes all at once now, efficiently.
            // TODO: this could lazily stream functions out of the module.
            for (Module::iterator I = input.module->begin(), E = input.module->end(); I != E; ++I) {
                if (!I->isDeclaration()) {
                    Passes.run(*I);
                }
            }

            Passes.doFinalization();
        }

        // Delete the ostream if it's not a stdout stream
        if (Out != &std::cout) delete Out;

        input.setName(to);
        // Mark the file as a temporary file.
        input.temp = true;

        if (TimeActions) {
	    timers[phase]->stopTimer();
        }

        break;
    }
    case ASSEMBLY: {                 // Convert .s to .o
        if (TimeActions) {
	    timers[phase]->startTimer();
        }

        if (Verbose) {
            // This file needs processing during this phase.
            cout << "  " << fileActions[filePhases[thisType][phase].action].name << " " << input.name
                << " to become " << fileTypes[nextType] << "\n";
        }

        sys::Path to(input.name.getBasename());
        to.appendSuffix(langToExt[nextType]);

        std::string ErrMsg;  
        if(Assemble(to.toString(), input.name.toString(), ErrMsg) != 0) {
            PrintAndExit(ErrMsg);
        }

        input.setName(to);
        // Mark the file as a temporary file.
        input.temp = true;

        if (TimeActions) {
	    timers[phase]->stopTimer();
        }

        break;
    }
    default:
        // RICH: Illegal single pass.
        break;
    }
    
    return nextType;
}

//===----------------------------------------------------------------------===//
// main for ellcc
//
int main(int argc, char **argv)
 {
    llvm_shutdown_obj X;  // Call llvm_shutdown() on exit.
    InputList InpList;
    int status = 0;

    try {
        // Initial global variable above for convenience printing of program name.
        progname = sys::Path(argv[0]).getBasename();
        setupFileTypes();
        TimerGroup timerGroup("... Ellcc action timing report ...");
        for (int i = 0; i < NUM_PHASES; ++i) {
            timers[i] = new Timer(phases[i].name, timerGroup);
        }

        // Parse the command line options.
        cl::ParseCommandLineOptions(argc, argv, "C/C++ compiler\n");
        sys::PrintStackTraceOnErrorSignal();

        // Allocate a full target machine description only if necessary.
        // FIXME: The choice of target should be controllable on the command line.
        std::auto_ptr<TargetMachine> target;

        // Initialize Elsa.
        Elsa elsa(timerGroup);       // Get the parsing environment.
        elsa.setup(TimeActions);
        elsa.wantBpprint = ElsaPrettyPrint;
        elsa.wantBpprintAfterElab = ElsaPrettyPrintAfterElab;
        std::vector<std::string>::iterator traceIt = ElsaTraceOpts.begin();
        for ( ; traceIt != ElsaTraceOpts.end(); ++traceIt) {
            elsa.addTrace((*traceIt).c_str());
        }

        if (Files.size() == 0 && Libraries.size() == 0) {
            // No input files present.
            if (Verbose) {
                // Just version information.
                cerr << progname << ": version " << ELLCC_VERSION << "\n";
                Exit(0);
            } else {
                PrintAndExit("no input files");
            }
        }

        // Find configuration files.
        sys::Path config(progname);
        config.appendSuffix("ecf");
        sys::Path ecf = sys::Path::GetMainExecutable(argv[0], (void*)main);;
        ecf.eraseComponent();                           // Get the executable directory.
        bool found = false;

        sys::Path file = config;
        if (file.exists()) {
            found = true;
        }

        // Set up configuration file include paths.
        pw::Plexer* configuration = pw::Plexer::Create("ecf", errors);
        if (configuration == NULL) {
            goto showerrors;
        }
        configuration->addInclude("");                 // Add the current directory.
 
        // Check for <execute directory>/../config.
        ecf.appendComponent("..");
        ecf.appendComponent("config");
        if (ecf.isDirectory()) {
            configuration->addInclude(ecf.toString());
            if (!found) {
                file = ecf;
                file.appendComponent(config.toString());
                if (file.exists()) {
                    found = true;
                }
            }
        }
        ecf.eraseComponent();                          // Remove config.
        // Check for <execute directory>/../lib/ellcc/ELLCC_VERSION_STRING/config.
        ecf.appendComponent("lib");
        ecf.appendComponent("ellcc");
        ecf.appendComponent(ELLCC_VERSION_STRING);
        ecf.appendComponent("config");
        if (ecf.isDirectory()) {
            configuration->addInclude(ecf.toString());
            if (!found) {
                file = ecf;
                file.appendComponent(config.toString());
                if (file.exists()) {
                    found = true;
                }
            }
        }
        
        configuration->addDefine("__ELLCC__",  str(ELLCC));
        configuration->addDefine("__ELLCC_MINOR__",  str(ELLCC_MINOR));
        configuration->addDefine("__ELLCC_PATCHLEVEL__",  str(ELLCC_PATCHLEVEL));
        configuration->addDefine("__ELLCC_VERSION_STRING__",  str(ELLCC_VERSION_STRING));
        configuration->addDefine("__ELLCC_VERSION__",  str(ELLCC_VERSION));
 
        // Read the program configuration file.
        pconfig = pw::Plexer::Create(file.toString().c_str(), errors);

        if (pconfig == NULL) {
            goto showerrors;
        }

        // Gather the input files and determine their types.
        std::vector<std::string>::iterator fileIt = Files.begin();
        std::vector<std::string>::iterator libIt  = Libraries.begin();
        unsigned libPos = 0, filePos = 0;
        
        for ( ;; )  {
            if (libIt != Libraries.end())
                libPos = Libraries.getPosition( libIt - Libraries.begin() );
            else
                libPos = 0;
            if (fileIt != Files.end())
                filePos = Files.getPosition(fileIt - Files.begin());
            else
                filePos = 0;

            if (filePos != 0 && (libPos == 0 || filePos < libPos)) {
                // Add a source file
                FileTypes type = GetFileType(*fileIt, filePos);
                if (Verbose) {
                    cout << "  adding " << *fileIt << " as " << fileTypes[type] << "\n";
                }
                Input input(*fileIt, type);
                // RICH: Configure the language.
                if (type == CC || type == II) {
                    input.language = pw::Plexer::Create("/home/rich/ellcc/config/cxx98.ecf", errors);
                } else {
                    input.language = pw::Plexer::Create("/home/rich/ellcc/config/c99.ecf", errors);
                }
                InpList.push_back(input);
                ++fileIt;
            } else if ( libPos != 0 && (filePos == 0 || libPos < filePos) ) {
                // Add a library
                if (Verbose) {
                    cout << "  adding " << *libIt << " as an input library\n";
                }
                Input input(*fileIt, A);
                InpList.push_back(input);
                ++libIt;
            }
            else
                break; // we're done with the list
        }

        // Go through the phases.
        InputList::iterator it;
        for(Phases phase = PREPROCESSING; phase != NUM_PHASES; phase = (Phases)(phase + 1)) {
            if (phase == BCLINKING && (FinalPhase == GENERATION || FinalPhase == ASSEMBLY)) {
                // Don't link bitcode if assembly language or object files are wanted.
                continue;
            }
            if (Verbose) {
                cout << "Phase: " << phases[phase].name << "\n";
            }

            if (phases[phase].result != NONE) {
                // This phase deals with muiltple files.
                std::vector<Input*> files;
                for (it = InpList.begin(); it != InpList.end(); ++it) {
                    if (it->name.isEmpty()) {
                        continue;
                    }
                    FileTypes nextType = filePhases[it->type][phase].type;
                    if (nextType != NONE) {
                        if (Verbose) {
                            // This file needs processing during this phase.
                            cout << "  " << fileActions[filePhases[it->type][phase].action].name
                                << " " << it->name << " to become " << fileTypes[nextType] << "\n";
                        }
                        
                        files.push_back(&*it);
                        it->type = nextType;
                    } else {
                        if (Verbose) {
                            cout << "  " << it->name << " is ignored during this phase\n";
                        }
                    }
                }

                if (files.size()) {
                    // Perform the phase on the files.
                    doMulti(phase, files, InpList, timerGroup);
                }
            } else {
                for (it = InpList.begin(); it != InpList.end(); ++it) {
                    if (it->name.isEmpty()) {
                        continue;
                    }
                    if (filePhases[it->type][phase].type != NONE) {
                        // Perform the phase on the file.
                        it->type = doSingle(phase, *it, elsa, it->type);
                    } else {
                        if (Verbose) {
                            cout << "  " << it->name << " is ignored during this phase\n";
                        }
                    }
                }
            }

            if (FinalPhase == phase || errors.hasErrors()) {
                break;
            }

        }

        // Check to see if any module files should be generated
        for (InputList::iterator it = InpList.begin(); it != InpList.end(); ++it) {
            if (it->module && !it->temp) {
                // Output the module.
                // RICH: .ll vs. .bc.
                llvm::PassManager PM;
                std::ostream *out = new std::ofstream(it->name.c_str());
                if (!out) {
                    std::cerr << progname << ": can't open " << it->name << " for writing\n";
                    Exit(1);
                }
                if (Verbose) {
                    cout << "  creating temporary file " << it->name << "\n";
                }
#if RICH
                llvm::OStream L(*out);
                PM.add(new llvm::PrintModulePass(&L));
#endif
                PM.add(llvm::CreateBitcodeWriterPass(*out));
                PM.run(*it->module);
                delete it->module;
            }
        }

        for (int i = 0; i < NUM_PHASES; ++i) {
            delete timers[i];
        }
            
        status =  0;
    } catch (const std::string& msg) {
        cerr << argv[0] << ": " << msg << "\n";
        status =  1;
    } catch (...) {
        cerr << argv[0] << ": Unexpected unknown exception occurred.\n";
        status =  1;
    }

showerrors:
    int totalerrors = 0;
    for (int j = 0; j < pw::Error::ERRORCNT; ++j) {
        // Calculate the total number of errors.
        totalerrors += errors.errorCount(j);
    }

    if (totalerrors) {
        for (int i = 0; i < pw::Error::ERRORCNT; ++i) {
            const char *name;
            const char *plural;
            int count = errors.errorCount(i);

            if (count == 0)
                continue;

            if (count == 1)
                plural = "";
            else
                plural = "s";

            name = pw::Error::modifier((pw::Error::Type)i);
            fprintf(stdout, "%d %s message%s reported\n", count, name, plural);
        }

        // Show errors.
        errors.sort();
        errors.print(stdout);
        status = errors.hasErrors();
    }

    llvm_shutdown();
    return status;
}
