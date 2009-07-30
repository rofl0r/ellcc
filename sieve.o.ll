; ModuleID = 'sieve.o'
target datalayout = "e-p:32:32-i1:8:0-i8:8:0-i16:16:0-i32:32:0-i32:32:0-i64:32:0-f32:32:0-f64:32:0-f96:32:0-v64:64:64-v128:128:0-a0:0"
target triple = "i386-pc-linux-gnu"
	type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %1*, %0*, i32, i32, i32, i16, i8, [1 x i8], i8*, i64, i8*, i8*, i8*, i8*, i32, i32, [40 x i8] }		; type %0
	type { %1*, %0*, i32 }		; type %1
	type { i32, %3 }		; type %2
	type { i32, %4 }		; type %3
	type { i32, [4 x i8] }		; type %4
@_IO_2_1_stdin_ = external global { }		; <{ }*> [#uses=0]
@_IO_2_1_stdout_ = external global { }		; <{ }*> [#uses=0]
@_IO_2_1_stderr_ = external global { }		; <{ }*> [#uses=0]
@stdin = external global %0*		; <%0**> [#uses=0]
@stdout = external global %0*		; <%0**> [#uses=0]
@stderr = external global %0*		; <%0**> [#uses=0]
@sys_nerr = external global i32		; <i32*> [#uses=0]
@sys_errlist = external global [0 x i8*]		; <[0 x i8*]*> [#uses=0]
@flags = global [8191 x i8] zeroinitializer		; <[8191 x i8]*> [#uses=3]
@.str = internal constant [18 x i8] c"10000 iterations\0A\00"		; <[18 x i8]*> [#uses=1]
@.str1 = internal constant [11 x i8] c"%d primes\0A\00"		; <[11 x i8]*> [#uses=1]

declare i32 @__underflow(%0*) nounwind

declare i32 @__uflow(%0*) nounwind

declare i32 @__overflow(%0*, i32) nounwind

declare i32 @_IO_getc(%0*) nounwind

declare i32 @_IO_putc(i32, %0*) nounwind

declare i32 @_IO_feof(%0*) nounwind

declare i32 @_IO_ferror(%0*) nounwind

declare i32 @_IO_peekc_locked(%0*) nounwind

declare void @_IO_flockfile(%0*) nounwind

declare void @_IO_funlockfile(%0*) nounwind

declare i32 @_IO_ftrylockfile(%0*) nounwind

declare i32 @_IO_vfscanf(%0*, i8*, i8*, i32*) nounwind

declare i32 @_IO_vfprintf(%0*, i8*, i8*) nounwind

declare i32 @_IO_padn(%0*, i32, i32) nounwind

declare i32 @_IO_sgetn(%0*, i8*, i32) nounwind

declare i64 @_IO_seekoff(%0*, i64, i32, i32) nounwind

declare i64 @_IO_seekpos(%0*, i64, i32) nounwind

declare void @_IO_free_backup_area(%0*) nounwind

declare i32 @remove(i8*) nounwind

declare i32 @rename(i8*, i8*) nounwind

declare %0* @tmpfile() nounwind

declare i8* @tmpnam(i8*) nounwind

declare i8* @tmpnam_r(i8*) nounwind

declare i8* @tempnam(i8*, i8*) nounwind

declare i32 @fclose(%0*) nounwind

declare i32 @fflush(%0*) nounwind

declare i32 @fflush_unlocked(%0*) nounwind

declare %0* @fopen(i8*, i8*) nounwind

declare %0* @freopen(i8*, i8*, %0*) nounwind

declare %0* @fdopen(i32, i8*) nounwind

declare void @setbuf(%0*, i8*) nounwind

declare i32 @setvbuf(%0*, i8*, i32, i32) nounwind

declare void @setbuffer(%0*, i8*, i32) nounwind

declare void @setlinebuf(%0*) nounwind

declare i32 @fprintf(%0*, i8*, ...) nounwind

declare i32 @printf(i8*, ...) nounwind

declare i32 @sprintf(i8*, i8*, ...) nounwind

declare i32 @vfprintf(%0*, i8*, i8*) nounwind

declare i32 @vprintf(i8*, i8*) nounwind

declare i32 @vsprintf(i8*, i8*, i8*) nounwind

declare i32 @snprintf(i8*, i32, i8*, ...) nounwind

declare i32 @vsnprintf(i8*, i32, i8*, i8*) nounwind

declare i32 @__isoc99_fscanf(%0*, i8*, ...) nounwind

declare i32 @__isoc99_scanf(i8*, ...) nounwind

declare i32 @__isoc99_sscanf(i8*, i8*, ...) nounwind

declare i32 @__isoc99_vfscanf(%0*, i8*, i8*) nounwind

declare i32 @__isoc99_vscanf(i8*, i8*) nounwind

declare i32 @__isoc99_vsscanf(i8*, i8*, i8*) nounwind

declare i32 @fgetc(%0*) nounwind

declare i32 @getc(%0*) nounwind

declare i32 @getchar() nounwind

declare i32 @getc_unlocked(%0*) nounwind

declare i32 @getchar_unlocked() nounwind

declare i32 @fgetc_unlocked(%0*) nounwind

declare i32 @fputc(i32, %0*) nounwind

declare i32 @putc(i32, %0*) nounwind

declare i32 @putchar(i32) nounwind

declare i32 @fputc_unlocked(i32, %0*) nounwind

declare i32 @putc_unlocked(i32, %0*) nounwind

declare i32 @putchar_unlocked(i32) nounwind

declare i32 @getw(%0*) nounwind

declare i32 @putw(i32, %0*) nounwind

declare i8* @fgets(i8*, i32, %0*) nounwind

declare i8* @gets(i8*) nounwind

declare i32 @fputs(i8*, %0*) nounwind

declare i32 @puts(i8*) nounwind

declare i32 @ungetc(i32, %0*) nounwind

declare i32 @fread(i8*, i32, i32, %0*) nounwind

declare i32 @fwrite(i8*, i32, i32, %0*) nounwind

declare i32 @fread_unlocked(i8*, i32, i32, %0*) nounwind

declare i32 @fwrite_unlocked(i8*, i32, i32, %0*) nounwind

declare i32 @fseek(%0*, i32, i32) nounwind

declare i32 @ftell(%0*) nounwind

declare void @rewind(%0*) nounwind

declare i32 @fseeko(%0*, i32, i32) nounwind

declare i32 @ftello(%0*) nounwind

declare i32 @fgetpos(%0*, %2*) nounwind

declare i32 @fsetpos(%0*, %2*) nounwind

declare void @clearerr(%0*) nounwind

declare i32 @feof(%0*) nounwind

declare i32 @ferror(%0*) nounwind

declare void @clearerr_unlocked(%0*) nounwind

declare i32 @feof_unlocked(%0*) nounwind

declare i32 @ferror_unlocked(%0*) nounwind

declare void @perror(i8*) nounwind

declare i32 @fileno(%0*) nounwind

declare i32 @fileno_unlocked(%0*) nounwind

declare %0* @popen(i8*, i8*) nounwind

declare i32 @pclose(%0*) nounwind

declare i8* @ctermid(i8*) nounwind

declare void @flockfile(%0*) nounwind

declare i32 @ftrylockfile(%0*) nounwind

declare void @funlockfile(%0*) nounwind

define i32 @main(i32, i8**, ...) nounwind {
entry:
	%ac = alloca i32		; <i32*> [#uses=1]
	store i32 %0, i32* %ac
	%av = alloca i8**		; <i8***> [#uses=1]
	store i8** %1, i8*** %av
	%retval = alloca i32		; <i32*> [#uses=2]
	store i32 0, i32* %retval
	%i = alloca i32		; <i32*> [#uses=15]
	%prime = alloca i32		; <i32*> [#uses=3]
	%k = alloca i32		; <i32*> [#uses=5]
	%count = alloca i32		; <i32*> [#uses=4]
	%iter = alloca i32		; <i32*> [#uses=5]
	%2 = call i32 (i8*, ...)* @printf(i8* getelementptr ([18 x i8]* @.str, i32 0, i32 0))		; <i32> [#uses=0]
	store i32 1, i32* %iter
	br label %test

test:		; preds = %continue, %entry
	%3 = load i32* %iter		; <i32> [#uses=1]
	%4 = icmp sle i32 %3, 10000		; <i1> [#uses=1]
	br i1 %4, label %body, label %next

body:		; preds = %test
	store i32 0, i32* %count
	store i32 0, i32* %i
	br label %test1

continue:		; preds = %next8
	%5 = load i32* %iter		; <i32> [#uses=0]
	%6 = load i32* %iter		; <i32> [#uses=1]
	%7 = add i32 %6, 1		; <i32> [#uses=1]
	store i32 %7, i32* %iter
	br label %test

next:		; preds = %test
	%8 = load i32* %count		; <i32> [#uses=1]
	%9 = call i32 (i8*, ...)* @printf(i8* getelementptr ([11 x i8]* @.str1, i32 0, i32 0), i32 %8)		; <i32> [#uses=0]
	br label %return

test1:		; preds = %continue3, %body
	%10 = load i32* %i		; <i32> [#uses=1]
	%11 = icmp sle i32 %10, 8190		; <i1> [#uses=1]
	br i1 %11, label %body2, label %next4

body2:		; preds = %test1
	%12 = load i32* %i		; <i32> [#uses=1]
	%13 = getelementptr [8191 x i8]* @flags, i32 0, i32 %12		; <i8*> [#uses=1]
	store i8 1, i8* %13
	br label %continue3

continue3:		; preds = %body2
	%14 = load i32* %i		; <i32> [#uses=0]
	%15 = load i32* %i		; <i32> [#uses=1]
	%16 = add i32 %15, 1		; <i32> [#uses=1]
	store i32 %16, i32* %i
	br label %test1

next4:		; preds = %test1
	store i32 0, i32* %i
	br label %test5

test5:		; preds = %continue7, %next4
	%17 = load i32* %i		; <i32> [#uses=1]
	%18 = icmp sle i32 %17, 8190		; <i1> [#uses=1]
	br i1 %18, label %body6, label %next8

body6:		; preds = %test5
	%19 = load i32* %i		; <i32> [#uses=1]
	%20 = getelementptr [8191 x i8]* @flags, i32 0, i32 %19		; <i8*> [#uses=1]
	%21 = load i8* %20		; <i8> [#uses=1]
	%22 = icmp ne i8 %21, 0		; <i1> [#uses=1]
	br i1 %22, label %ifTrue, label %ifFalse

continue7:		; preds = %next9
	%23 = load i32* %i		; <i32> [#uses=0]
	%24 = load i32* %i		; <i32> [#uses=1]
	%25 = add i32 %24, 1		; <i32> [#uses=1]
	store i32 %25, i32* %i
	br label %test5

next8:		; preds = %test5
	br label %continue

ifTrue:		; preds = %body6
	%26 = load i32* %i		; <i32> [#uses=1]
	%27 = load i32* %i		; <i32> [#uses=1]
	%28 = add i32 %26, %27		; <i32> [#uses=1]
	%29 = add i32 %28, 3		; <i32> [#uses=1]
	store i32 %29, i32* %prime
	%30 = load i32* %i		; <i32> [#uses=1]
	%31 = load i32* %prime		; <i32> [#uses=1]
	%32 = add i32 %30, %31		; <i32> [#uses=1]
	store i32 %32, i32* %k
	br label %continue10

ifFalse:		; preds = %body6
	br label %next9

next9:		; preds = %next12, %ifFalse
	br label %continue7

continue10:		; preds = %body11, %ifTrue
	%33 = load i32* %k		; <i32> [#uses=1]
	%34 = icmp sle i32 %33, 8190		; <i1> [#uses=1]
	br i1 %34, label %body11, label %next12

body11:		; preds = %continue10
	%35 = load i32* %k		; <i32> [#uses=1]
	%36 = getelementptr [8191 x i8]* @flags, i32 0, i32 %35		; <i8*> [#uses=1]
	store i8 0, i8* %36
	%37 = load i32* %k		; <i32> [#uses=1]
	%38 = load i32* %prime		; <i32> [#uses=1]
	%39 = add i32 %37, %38		; <i32> [#uses=1]
	store i32 %39, i32* %k
	br label %continue10

next12:		; preds = %continue10
	%40 = load i32* %count		; <i32> [#uses=1]
	%41 = add i32 %40, 1		; <i32> [#uses=1]
	store i32 %41, i32* %count
	br label %next9

return:		; preds = %next
	%42 = load i32* %retval		; <i32> [#uses=1]
	ret i32 %42
}
