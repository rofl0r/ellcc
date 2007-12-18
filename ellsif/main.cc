// main.cc            see license.txt for copyright and terms of use
// The ellsif driver.

#include <iostream.h>     // cout
#include <stdlib.h>       // exit, getenv, abort
#include <fstream.h>      // ofstream

#include "trace.h"        // traceAddSys
#include "syserr.h"       // xsyserror
#include "parssppt.h"     // ParseTreeAndTokens, treeMain
#include "srcloc.h"       // SourceLocManager
#include "ckheap.h"       // malloc_stats
#include "cc_env.h"       // Env
#include "cc_ast.h"       // C++ AST (r)
#include "cc_ast_aux.h"   // class LoweredASTVisitor
#include "cc_lang.h"      // CCLang
#include "parsetables.h"  // ParseTables
#include "cc_print.h"     // PrintEnv
#include "cc.gr.gen.h"    // CCParse
#include "nonport.h"      // getMilliseconds
#include "ptreenode.h"    // PTreeNode
#include "ptreeact.h"     // ParseTreeLexer, ParseTreeActions
#include "sprint.h"       // structurePrint
#include "strtokp.h"      // StrtokParse
#include "smregexp.h"     // regexpMatch
#include "cc_elaborate.h" // ElabVisitor
#include "integrity.h"    // IntegrityVisitor
#include "xml_file_writer.h" // XmlFileWriter
#include "xml_reader.h"   // xmlDanglingPointersAllowed
#include "xml_do_read.h"  // xmlDoRead()
#include "xml_type_writer.h" // XmlTypeWriter
#include "bpprint.h"      // bppTranslationUnit
#include "cc2c.h"         // cc_to_c

#ifdef LLVM_EXTENSION
#include "cc2llvm.h"      // cc_to_llvm

// LLVM
#include <llvm/Module.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/Assembly/PrintModulePass.h>
#endif

/*
  Recognized file extensions:
    C source files: .c
    C header files: .h
    C preprocessed files: .i
    C++ source files: .C, .cc, .cpp, .CPP, .c++, .cp, or .cxx
    C++ header files: .hh or .H
    C++ preprocessed files: .ii
    LLVM assembly files: .ll
    LLVM bytecode files: .bc
    Assembly source files: .s

  Command line options:

  Overall Options
    -c
    -S
    -E
    -o file
    -combine
    -pipe
    -pass-exit-codes  
    -x language
    -v
    -###
    --help
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
              -O  -O0  -O1  -O2  -O3  -Os

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

// true to print the tchecked C++ syntax using bpprint after
// tcheck
static bool wantBpprint = false;

// same, but after elaboration
static bool wantBpprintAfterElab = false;


// nonempty if we want to run cc2c; value of "-" means stdout
static string cc2cOutputFname;

#ifdef LLVM_EXTENSION
// nonempty if we want to run cc2llvm; value of "-" means stdout
static string cc2llvmOutputFname;
#endif

// little check: is it true that only global declarators
// ever have Declarator::type != Declarator::var->type?
// .. no, because it's true for class members too ..
// .. it's also true of arrays declared [] but then inited ..
// .. finally, it's true of parameters whose types get
//    normalized as per cppstd 8.3.5 para 3; I didn't include
//    that case below because there's no easy way to test for it ..
// Intended to be used with LoweredASTVisitor
class DeclTypeChecker : private ASTVisitor {
public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  int instances;

public:
  DeclTypeChecker()
    : loweredVisitor(this)
    , instances(0)
  {}
  virtual ~DeclTypeChecker() {}

  virtual bool visitDeclarator(Declarator *obj);
};

bool DeclTypeChecker::visitDeclarator(Declarator *obj)
{
  if (obj->type != obj->var->type &&
      !(obj->var->flags & (DF_GLOBAL | DF_MEMBER)) &&
      !obj->type->isArrayType()) {
    instances++;
    cout << toString(obj->var->loc) << ": " << obj->var->name
         << " has type != var->type, but is not global or member or array\n";
  }
  return true;
}

// this scans the AST for E_variables, and writes down the locations
// to which they resolved; it's helpful for writing tests of name and
// overload resolution
class NameChecker : public ASTVisitor {
public:
  // accumulates the results
  stringBuilder sb;

public:
  NameChecker() {}

  virtual bool visitExpression(Expression *obj)
  {
    Variable *v = NULL;
    if (obj->isE_variable()) {
      v = obj->asE_variable()->var;
    }
    else if (obj->isE_fieldAcc()) {
      v = obj->asE_fieldAcc()->field;
    }

    // this output format is designed to minimize the effect of
    // changes to unrelated details
    if (v
        && !streq("__testOverload", v->name)
        && !streq("dummy",          v->name)
        && !streq("__other",        v->name) // "__other": for inserted elaboration code
        && !streq("this",           v->name) // dsw: not sure why "this" is showing up
        && !streq("operator=",      v->name) // an implicitly defined member of every class
        && v->name[0]!='~'                   // don't print dtors
        ) {
      sb << " " << v->name << "=" << sourceLocManager->getLine(v->loc);
    }

    return true;
  }
};

void if_malloc_stats()
{
  if (tracingSys("malloc_stats")) {
    malloc_stats();
  }
}


class SectionTimer {
  long start;
  long &elapsed;

public:
  SectionTimer(long &e)
    : start(getMilliseconds()),
      elapsed(e)
  {}
  ~SectionTimer()
  {
    elapsed += getMilliseconds() - start;
  }
};

void handle_xBase(Env &env, xBase &x)
{
  // typically an assertion failure from the tchecker; catch it here
  // so we can print the errors, and something about the location
  env.errors.print(cout);
  cout << x << endl;
  cout << "Failure probably related to code near " << env.locStr() << endl;

  // print all the locations on the scope stack; this is sometimes
  // useful when the env.locStr refers to some template code that
  // was instantiated from somewhere else
  //
  // (unfortunately, env.instantiationLocStack isn't an option b/c
  // it will have been cleared by the automatic invocation of
  // destructors unwinding the stack...)
  cout << "current location stack:\n";
  cout << env.locationStackString();

  // I changed from using exit(4) here to using abort() because
  // that way the multitest.pl script can distinguish them; the
  // former is reserved for orderly exits, whereas signals (like
  // SIGABRT) mean that something went really wrong
  abort();
}

// this is a dumb way to organize argument processing...
char *myProcessArgs(int argc, char **argv, char const *additionalInfo)
{
  // remember program name
  char const *progName = argv[0];

  #define SHIFT argv++; argc-- /* user ; */

  // process args
  while (argc >= 2) {
    if (traceProcessArg(argc, argv)) {
      continue;
    }
    else if (0==strcmp(argv[1], "-xc")) {
      // treat this as a synonym for "-tr c_lang"
      traceAddSys("c_lang");
      SHIFT;
    }
    else if (0==strcmp(argv[1], "-w")) {
      // synonym for -tr nowarnings
      traceAddSys("nowarnings");
      SHIFT;
    }
    else if (0==strcmp(argv[1], "-bpprint")) {
      wantBpprint = true;
      SHIFT;
    }
    else if (0==strcmp(argv[1], "-bpprintAfterElab")) {
      wantBpprintAfterElab = true;
      SHIFT;
    }
    else if (0==strcmp(argv[1], "-cc2c")) {
      if (argc < 3) {
        cout << "-cc2c requires a file name argument\n";
        exit(2);
      }
      cc2cOutputFname = argv[2];
      SHIFT;
      SHIFT;
    }
#ifdef LLVM_EXTENSION
    else if (0==strcmp(argv[1], "-cc2llvm")) {
      if (argc < 3) {
        cout << "-cc2llvm requires a file name argument\n";
        exit(2);
      }
      cc2llvmOutputFname = argv[2];
      SHIFT;
      SHIFT;
    }
#endif
    else {
      break;     // didn't find any more options
    }
  }

  #undef SHIFT

  if (argc != 2) {
    cout << "usage: " << progName << " [options] input-file\n"
            "  options:\n"
            "    -tr <flags>:       turn on given tracing flags (comma-separated)\n"
            "    -bbprint:          print parsed C++ back out using bpprint\n"
            "    -bbprintAfterElab: bpprint after elaboration\n"
            "    -cc2c <fname>:     generate C, write to <fname>; \"-\" means stdout\n"
#ifdef LLVM_EXTENSION
            "    -cc2llvm <fname>:  generate LLVM code, write to <fname>; \"-\" means stdout\n"
#endif
         << (additionalInfo? additionalInfo : "");
    exit(argc==1? 0 : 2);    // error if any args supplied
  }

  // some -tr equivalents, which are convenient because they
  // can be passed in the TRACE environment variable
  wantBpprint = wantBpprint || tracingSys("bpprint");

  return argv[1];
}

void doit(int argc, char **argv)
{
  // I think this is more noise than signal at this point
  xBase::logExceptions = false;

  traceAddSys("progress");
  //traceAddSys("parse-tree");

  if_malloc_stats();

  SourceLocManager mgr;

  // string table for storing parse tree identifiers
  StringTable strTable;

  // parsing language options
  CCLang lang;
  lang.GNU_Cplusplus();

  // ------------- process command-line arguments ---------
  char const *inputFname = myProcessArgs
    (argc, argv,
     "\n"
     "  general behavior flags:\n"
     "    c_lang             use C language rules (default is C++)\n"
     "    nohashline         ignore #line when reporting locations\n"
     "\n"
     "  options to stop after a certain stage:\n"
     "    stopAfterParse     stop after parsing\n"
     "    stopAfterTCheck    stop after typechecking\n"
     "    stopAfterElab      stop after semantic elaboration\n"
     "\n"
     "  output options:\n"
     "    parseTree          make a parse tree and print that, only\n"
     "    printAST           print AST after parsing\n"
     "    printTypedAST      print AST with type info\n"
     "    printElabAST       print AST after semantic elaboration\n"
     "    prettyPrint        echo input as pretty-printed C++\n"
     "    xmlPrintAST        print AST as XML\n"
     "\n"
     "  debugging output:\n"
     "    malloc_stats       print malloc stats every so often\n"
     "    env                print as variables are added to the environment\n"
     "    error              print as errors are accumulated\n"
     "    overload           print details of overload resolution\n"
     "\n"
     "  (grep in source for \"trace\" to find more obscure flags)\n"
     "");

  if (tracingSys("printAsML")) {
    Type::printAsML = true;
  }

  // FIX: dsw: couldn't we put dashes or something in here to break up
  // the word?
  if (tracingSys("nohashline")) {
    sourceLocManager->useHashLines = false;
  }

  if (tracingSys("tolerateHashlineErrors")) {
    sourceLocManager->tolerateHashlineErrors = true;
  }

  if (tracingSys("no-orig-offset")) {
    sourceLocManager->useOriginalOffset = false;
  }

  if (tracingSys("ansi")) {
    lang.ANSI_Cplusplus();
  }

  if (tracingSys("ansi_c")) {
    lang.ANSI_C89();
  }

  if (tracingSys("ansi_c99")) {
    lang.ANSI_C99();
  }

  if (tracingSys("c_lang")) {
    lang.GNU_C();
  }

  if (tracingSys("gnu_c89")) {
    lang.ANSI_C89();
    lang.GNU_C_extensions();
  }

  if (tracingSys("gnu_kandr_c_lang")) {
    lang.GNU_KandR_C();
    #ifndef KANDR_EXTENSION
      xfatal("gnu_kandr_c_lang option requires the K&R module (./configure -kandr=yes)");
    #endif
  }

  if (tracingSys("gnu2_kandr_c_lang")) {
    lang.GNU2_KandR_C();
    #ifndef KANDR_EXTENSION
      xfatal("gnu2_kandr_c_lang option requires the K&R module (./configure -kandr=yes)");
    #endif
  }

  if (tracingSys("test_xfatal")) {
    xfatal("this is a test error message");
  }

  if (tracingSys("msvcBugs")) {
    lang.MSVC_bug_compatibility();
  }

  if (!tracingSys("nowarnings")) {
    lang.enableAllWarnings();
  }

  if (tracingSys("templateDebug")) {
    // predefined set of tracing flags I've been using while debugging
    // the new templates implementation
    traceAddSys("template");
    traceAddSys("error");
    traceAddSys("scope");
    traceAddSys("templateParams");
    traceAddSys("templateXfer");
    traceAddSys("prettyPrint");
    traceAddSys("xmlPrintAST");
    traceAddSys("topform");
  }

  if (tracingSys("only_works_on_32bit") &&
      sizeof(long) != 4) {
    // we are running a regression test, and the testcase is known to
    // fail due to dependence on architecture parameters, so skip it
    cout << "skipping test b/c this is not a 32-bit architecture\n";
    exit(0);
  }

  // dump out the lang settings if the user wants them
  if (tracingSys("printLang")) {
    cout << "language settings:\n";
    cout << lang.toString();
    cout << endl;
  }
  if (tracingSys("printTracers")) {
    cout << "tracing flags:\n\t";
    printTracers(std::cout, "\n\t");
    cout << endl;
  }

  // --------------- parse --------------
  TranslationUnit *unit;

  // dsw: I needed this to persist past typechecking, so I moved it
  // out here.  Feel free to refactor.
  ArrayStack<Variable*> madeUpVariables;
  ArrayStack<Variable*> builtinVars;

  int parseWarnings = 0;
  long parseTime = 0;
  if (tracingSys("parseXml")) {
    if (tracingSys("parseXml-no-danglingPointers")) {
      xmlDanglingPointersAllowed = false;
    }
    unit = xmlDoRead(strTable, inputFname);
    if (!unit) return;
  }
  else {
    SectionTimer timer(parseTime);
    SemanticValue treeTop;
    ParseTreeAndTokens tree(lang, treeTop, strTable, inputFname);

    // grab the lexer so we can check it for errors (damn this
    // 'tree' thing is stupid..)
    Lexer *lexer = dynamic_cast<Lexer*>(tree.lexer);
    xassert(lexer);

    CCParse *parseContext = new CCParse(strTable, lang);
    tree.userAct = parseContext;

    traceProgress(2) << "building parse tables from internal data\n";
    ParseTables *tables = parseContext->makeTables();
    tree.tables = tables;

    maybeUseTrivialActions(tree);

    if (tracingSys("parseTree")) {
      // make some helpful aliases
      LexerInterface *underLexer = tree.lexer;
      UserActions *underAct = parseContext;

      // replace the lexer and parser with parse-tree-building versions
      tree.lexer = new ParseTreeLexer(underLexer, underAct);
      tree.userAct = new ParseTreeActions(underAct, tables);

      // 'underLexer' and 'tree.userAct' will be leaked.. oh well
    }

    if (!toplevelParse(tree, inputFname)) {
      exit(2); // parse error
    }

    // check for parse errors detected by the context class
    if (parseContext->errors || lexer->errors) {
      exit(2);
    }
    parseWarnings = lexer->warnings + parseContext->warnings;

    if (tracingSys("parseTree")) {
      // the 'treeTop' is actually a PTreeNode pointer; print the
      // tree and bail
      PTreeNode *ptn = (PTreeNode*)treeTop;
      ptn->printTree(cout, PTreeNode::PF_EXPAND);
      return;
    }

    // treeTop is a TranslationUnit pointer
    unit = (TranslationUnit*)treeTop;

    //unit->debugPrint(cout, 0);

    delete parseContext;
    delete tables;
  }

  checkHeap();

  // print abstract syntax tree
  if (tracingSys("printAST")) {
    unit->debugPrint(cout, 0);
  }

  //if (unit) {     // when "-tr trivialActions" it's NULL...
  //  cout << "ambiguous nodes: " << numAmbiguousNodes(unit) << endl;
  //}

  if (tracingSys("stopAfterParse")) {
    return;
  }


  // ---------------- typecheck -----------------
  BasicTypeFactory tfac;
  long tcheckTime = 0;
  if (tracingSys("no-typecheck")) {
    cout << "no-typecheck" << endl;
  } else {
    SectionTimer timer(tcheckTime);
    Env env(strTable, lang, tfac, madeUpVariables, builtinVars, unit);
    try {
      env.tcheckTranslationUnit(unit);
    }
    catch (XUnimp &x) {
      HANDLER();

      // relay to handler in main()
      cout << "in code near " << env.locStr() << ":\n";
      throw;
    }
    catch (x_assert &x) {
      HANDLER();

      if (env.errors.hasFromNonDisambErrors()) {
        if (tracingSys("expect_confused_bail")) {
          cout << "got the expected confused/bail\n";
          exit(0);
        }

        // The assertion failed only after we encountered and diagnosed
        // at least one real error in the input file.  Therefore, the
        // assertion might simply have been caused by a failure of the
        // error recovery code to fully restore all invariants (which is
        // often difficult).  So, we'll admit to being "confused", but
        // not to the presence of a bug in Elsa (which is what a failed
        // assertion otherwise nominally means).
        //
        // The error message is borrowed from gcc; I wouldn't be
        // surprised to discover they use a similar technique to decide
        // when to emit the same message.
        //
        // The reason I do not put the assertion failure message into
        // this one is I don't want it showing up in the output where
        // Delta might see it.  If I am intending to minimize an assertion
        // failure, it's no good if Delta introduces an error.
        env.error("confused by earlier errors, bailing out");
        env.errors.print(cout);
        exit(4);
      }

      if (tracingSys("expect_xfailure")) {
        cout << "got the expected xfailure\n";
        exit(0);
      }

      // if we don't have a basis for reducing severity, pass this on
      // to the normal handler
      handle_xBase(env, x);
    }
    catch (xBase &x) {
      HANDLER();
      handle_xBase(env, x);
    }

    int numErrors = env.errors.numErrors();
    int numWarnings = env.errors.numWarnings() + parseWarnings;

    // do this now so that 'printTypedAST' will include CFG info
    #ifdef CFG_EXTENSION
    // A possible TODO is to do this right after each function is type
    // checked.  However, in the current design, that means physically
    // inserting code into Function::tcheck (ifdef'd of course).  A way
    // to do it better would be to have a general post-function-tcheck
    // interface that analyses could hook in to.  That could be augmented
    // by a parsing mode that parsed each function, analyzed it, and then
    // immediately discarded its AST.
    if (numErrors == 0) {
      numErrors += computeUnitCFG(unit);
    }
    #endif // CFG_EXTENSION

    // print abstract syntax tree annotated with types
    if (tracingSys("printTypedAST")) {
      unit->debugPrint(cout, 0);
    }

    // structural delta thing
    if (tracingSys("structure")) {
      structurePrint(unit);
    }

    if (numErrors==0 && tracingSys("secondTcheck")) {
      // this is useful to measure the cost of disambiguation, since
      // now the tree is entirely free of ambiguities
      traceProgress() << "beginning second tcheck...\n";
      Env env2(strTable, lang, tfac, madeUpVariables, builtinVars, unit);
      unit->tcheck(env2);
      traceProgress() << "end of second tcheck\n";
    }

    // print errors and warnings
    env.errors.print(cout);

    cout << "typechecking results:\n"
         << "  errors:   " << numErrors << "\n"
         << "  warnings: " << numWarnings << "\n";

    if (numErrors != 0) {
      exit(4);
    }

    // lookup diagnostic
    if (env.collectLookupResults.length()) {
      // scan AST
      NameChecker nc;
      nc.sb << "collectLookupResults";
      unit->traverse(nc);

      // compare to given text
      if (streq(env.collectLookupResults, nc.sb)) {
        // ok
      }
      else {
        cout << "collectLookupResults do not match:\n"
             << "  source: " << env.collectLookupResults << "\n"
             << "  tcheck: " << nc.sb << "\n"
             ;
        exit(4);
      }
    }
  }

  // do this before elaboration; I just want to see the result of type
  // checking and disambiguation
  if (wantBpprint) {
    cout << "// bpprint\n";
    bppTranslationUnit(cout, *unit);
  }

  // ---------------- integrity checking ----------------
  long integrityTime = 0;
  {
    SectionTimer timer(integrityTime);

    // check AST integrity
    IntegrityVisitor ivis;
    unit->traverse(ivis);

    // check that the AST is a tree *and* that the lowered AST is a
    // tree; only do this *after* confirming that tcheck finished
    // without errors
    if (tracingSys("treeCheck")) {
      long start = getMilliseconds();
      LoweredIsTreeVisitor treeCheckVisitor;
      unit->traverse(treeCheckVisitor.loweredVisitor);
      traceProgress() << "done with tree check 1 ("
                      << (getMilliseconds() - start)
                      << " ms)\n";
    }

    // check an expected property of the annotated AST
    if (tracingSys("declTypeCheck") || getenv("declTypeCheck")) {
      DeclTypeChecker vis;
      unit->traverse(vis.loweredVisitor);
      cout << "instances of type != var->type: " << vis.instances << endl;
    }

    if (tracingSys("stopAfterTCheck")) {
      return;
    }
  }

  // ----------------- elaboration ------------------
  long elaborationTime = 0;
  if (tracingSys("no-elaborate")) {
    cout << "no-elaborate" << endl;
  }
  else {
    SectionTimer timer(elaborationTime);

    ElabVisitor vis(strTable, tfac, unit);

    if (!lang.isCplusplus) {
      // do only the C elaboration activities
      vis.activities = EA_C_ACTIVITIES;
    }

    // if we are going to pretty print, then we need to retain defunct children
    if (tracingSys("prettyPrint")
        // dsw: I don't know if this is right, but printing the xml
        // AST kind of resembles pretty-printing the AST; fix this if
        // it is wrong
        || tracingSys("xmlPrintAST")
        || wantBpprintAfterElab
        ) {
      vis.cloneDefunctChildren = true;
    }

    // do elaboration
    unit->traverse(vis.loweredVisitor);

    // print abstract syntax tree annotated with types
    if (tracingSys("printElabAST")) {
      unit->debugPrint(cout, 0);
    }
    if (tracingSys("stopAfterElab")) {
      return;
    }
  }

  // mark "real" (non-template) variables as such
  if (!tracingSys("parseXml")) {
    // mark "real" (non-template) variables as such
    MarkRealVars markReal;
    visitVarsF(builtinVars, markReal);
    visitRealVarsF(unit, markReal);
  }

  // more integrity checking
  {
    SectionTimer timer(integrityTime);

    // check that the AST is a tree *and* that the lowered AST is a
    // tree (do this *after* elaboration!)
    if (tracingSys("treeCheck")) {
      long start = getMilliseconds();
      LoweredIsTreeVisitor treeCheckVisitor;
      unit->traverse(treeCheckVisitor.loweredVisitor);
      traceProgress() << "done with tree check 2 ("
                      << (getMilliseconds() - start)
                      << " ms)\n";
    }
  }

  // dsw: pretty printing
  if (tracingSys("prettyPrint")) {
    traceProgress() << "dsw pretty print...\n";
    cout << "---- START ----" << endl;
    cout << "// -*-c++-*-" << endl;
    prettyPrintTranslationUnit(cout, *unit);
    cout << "---- STOP ----" << endl;
    traceProgress() << "dsw pretty print... done\n";
  }

  if (wantBpprintAfterElab) {
    cout << "// bpprintAfterElab\n";
    bppTranslationUnit(cout, *unit);
  }

  // dsw: xml printing of the raw ast
  if (tracingSys("xmlPrintAST")) {
    traceProgress() << "dsw xml print...\n";
    bool indent = tracingSys("xmlPrintAST-indent");
    int depth = 0;              // shared depth counter between printers
    cout << "---- START ----" << endl;

    // serialize Files
    IdentityManager idmgr;
    XmlFileWriter fileXmlWriter(idmgr, &cout, depth, indent, NULL);
    fileXmlWriter.toXml(sourceLocManager->serializationOnly_get_files());

    // serialize AST and maybe Types
    if (tracingSys("xmlPrintAST-types")) {
      IdentityManager idmgr;
      XmlTypeWriter xmlTypeVis( idmgr, (ASTVisitor*)NULL, &cout, depth, indent, NULL );
      XmlTypeWriter_AstVisitor xmlVis_Types(xmlTypeVis, cout, depth, indent);
      xmlTypeVis.astVisitor = &xmlVis_Types;
      ASTVisitor *vis = &xmlVis_Types;
      LoweredASTVisitor loweredXmlVis(&xmlVis_Types); // might not be used
      if (tracingSys("xmlPrintAST-lowered")) {
        vis = &loweredXmlVis;
      }
      unit->traverse(*vis);
    } else {
      IdentityManager idmgr;
      XmlAstWriter_AstVisitor xmlVis(cout, idmgr, depth, indent);
      ASTVisitor *vis = &xmlVis;
      LoweredASTVisitor loweredXmlVis(&xmlVis); // might not be used
      if (tracingSys("xmlPrintAST-lowered")) {
        vis = &loweredXmlVis;
      }
      unit->traverse(*vis);
    }

    cout << endl;
    cout << "---- STOP ----" << endl;
    traceProgress() << "dsw xml print... done\n";
  }

  // test AST cloning
  if (tracingSys("testClone")) {
    TranslationUnit *u2 = unit->clone();

    if (tracingSys("cloneAST")) {
      cout << "------- cloned AST --------\n";
      u2->debugPrint(cout, 0);
    }

    if (tracingSys("cloneCheck")) {
      ArrayStack<Variable*> madeUpVariables2;
      ArrayStack<Variable*> builtinVars2;
      // dsw: I hope you intend that I should use the cloned TranslationUnit
      Env env3(strTable, lang, tfac, madeUpVariables2, builtinVars2, u2);
      u2->tcheck(env3);

      if (tracingSys("cloneTypedAST")) {
        cout << "------- cloned typed AST --------\n";
        u2->debugPrint(cout, 0);
      }

      if (tracingSys("clonePrint")) {
        OStreamOutStream out0(cout);
        CodeOutStream codeOut(out0);
        CTypePrinter typePrinter;
        PrintEnv penv(typePrinter, &codeOut);
        cout << "---- cloned pretty print ----" << endl;
        u2->print(penv);
        codeOut.finish();
      }
    }
  }

  // test debugPrint but send the output to /dev/null (basically just
  // make sure it doesn't segfault or abort)
  if (tracingSys("testDebugPrint")) {
    ofstream devnull("/dev/null");
    unit->debugPrint(devnull, 0);
  }

  cout << "parse=" << parseTime << "ms"
       << " tcheck=" << tcheckTime << "ms"
       << " integ=" << integrityTime << "ms"
       << " elab=" << elaborationTime << "ms"
       << "\n"
       ;

  if (!cc2cOutputFname.empty()) {
    TranslationUnit *lowered = cc_to_c(strTable, *unit);
    if (cc2cOutputFname == string("-")) {
      cout << "// cc2c\n";
      bppTranslationUnit(cout, *lowered);
    }
    else {
      ofstream out(cc2cOutputFname.c_str());
      if (!out) {
        xsyserror("open", stringb("write \"" << cc2cOutputFname << "\""));
      }
      out << "// cc2c\n";
      bppTranslationUnit(out, *lowered);
    }
  }

#ifdef LLVM_EXTENSION
  if (!cc2llvmOutputFname.empty()) {
    llvm::Module* mod = cc_to_llvm(cc2llvmOutputFname, strTable, *unit,
	// RICH: Target data and target triple.
        "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-s0:0:64-f80:32:32",
        "i686-pc-linux-gnu");

    // Output the module.
    llvm::PassManager PM;
    std::ostream *out = &std::cout;  // Default to printing to stdout.
    if (cc2llvmOutputFname != string("-")) {
      out = new std::ofstream(cc2llvmOutputFname.c_str());
      if (!out) {
        xsyserror("open", stringb("write \"" << cc2llvmOutputFname << "\""));
      }
    }
    llvm::OStream L(*out);
    PM.add(new llvm::PrintModulePass(&L));
    PM.run(*mod);
    delete mod;
  }
#endif

  //traceProgress() << "cleaning up...\n";

  //malloc_stats();

  // delete the tree
  // (currently this doesn't do very much because FakeLists are
  // non-owning, so I won't pretend it does)
  //delete unit;

  strTable.clear();

  //checkHeap();
  //malloc_stats();
}

int main(int argc, char **argv)
{
  try {
    doit(argc, argv);
  }
  catch (XUnimp &x) {
    HANDLER();
    cout << x << endl;

    // don't consider this the same as dying on an assertion failure;
    // I want to have tests in regrtest that are "expected" to fail
    // for the reason that they use unimplemented language features
    return 10;
  }
  catch (XFatal &x) {
    HANDLER();

    // similar to XUnimp
    cout << x << endl;
    return 10;
  }
  catch (xBase &x) {
    HANDLER();
    cout << x << endl;
    abort();
  }

  //malloc_stats();

  return 0;
}
