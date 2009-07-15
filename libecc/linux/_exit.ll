; ModuleID = '_exit.bc'
target datalayout = "e-p:32:32-i1:8:0-i8:8:0-i16:16:0-i32:32:0-i32:32:0-i64:64:0-f32:32:0-f64:64:0-f128:128:0-v64:64:64-v128:128:0-a0:0"
target triple = "arm-elf"

define void @_exit(i32) nounwind {
entry:
	%status = alloca i32		; <i32*> [#uses=3]
	store i32 %0, i32* %status
	%_sys_result = alloca i32		; <i32*> [#uses=2]
	%_a1 = alloca i32		; <i32*> [#uses=4]
	%_sys_result1 = alloca i32		; <i32*> [#uses=5]
	%_sys_result3 = alloca i32		; <i32*> [#uses=2]
	%_a14 = alloca i32		; <i32*> [#uses=4]
	%_sys_result5 = alloca i32		; <i32*> [#uses=5]
	br label %test

test:		; preds = %continue, %entry
	br i1 true, label %body, label %next

body:		; preds = %test
	%1 = load i32* %status		; <i32> [#uses=1]
	store i32 %1, i32* %_a1
	%2 = load i32* %_a1		; <i32> [#uses=1]
	%3 = call i32 asm "swi\09$1\09@ syscall SYS_ify(exit_group)", "=r,i,r"(i32 231, i32 %2) nounwind		; <i32> [#uses=1]
	store i32 %3, i32* %_a1
	%4 = load i32* %_a1		; <i32> [#uses=1]
	store i32 %4, i32* %_sys_result
	%5 = load i32* %_sys_result		; <i32> [#uses=1]
	store i32 %5, i32* %_sys_result1
	%6 = load i32* %_sys_result1		; <i32> [#uses=1]
	%7 = icmp uge i32 %6, -4095		; <i1> [#uses=1]
	br i1 %7, label %ifTrue, label %ifFalse

continue:		; preds = %next8
	br label %test

next:		; preds = %test
	br label %return

ifTrue:		; preds = %body
	%8 = load i32* %_sys_result1		; <i32> [#uses=1]
	%9 = sub i32 0, %8		; <i32> [#uses=1]
	%10 = call i32 (...)* @__set_errno(i32 %9)		; <i32> [#uses=0]
	store i32 -1, i32* %_sys_result1
	br label %next2

ifFalse:		; preds = %body
	br label %next2

next2:		; preds = %ifFalse, %ifTrue
	%11 = load i32* %_sys_result1		; <i32> [#uses=0]
	%12 = load i32* %status		; <i32> [#uses=1]
	store i32 %12, i32* %_a14
	%13 = load i32* %_a14		; <i32> [#uses=1]
	%14 = call i32 asm "swi\09$1\09@ syscall SYS_ify(exit)", "=r,i,r"(i32 60, i32 %13) nounwind		; <i32> [#uses=1]
	store i32 %14, i32* %_a14
	%15 = load i32* %_a14		; <i32> [#uses=1]
	store i32 %15, i32* %_sys_result3
	%16 = load i32* %_sys_result3		; <i32> [#uses=1]
	store i32 %16, i32* %_sys_result5
	%17 = load i32* %_sys_result5		; <i32> [#uses=1]
	%18 = icmp uge i32 %17, -4095		; <i1> [#uses=1]
	br i1 %18, label %ifTrue6, label %ifFalse7

ifTrue6:		; preds = %next2
	%19 = load i32* %_sys_result5		; <i32> [#uses=1]
	%20 = sub i32 0, %19		; <i32> [#uses=1]
	%21 = call i32 (...)* @__set_errno(i32 %20)		; <i32> [#uses=0]
	store i32 -1, i32* %_sys_result5
	br label %next8

ifFalse7:		; preds = %next2
	br label %next8

next8:		; preds = %ifFalse7, %ifTrue6
	%22 = load i32* %_sys_result5		; <i32> [#uses=0]
	br label %continue

return:		; preds = %next
	ret void
}

declare i32 @__set_errno(...) nounwind

declare void @_Exit(i32) nounwind
