; ModuleID = 'a.bc'
target datalayout = "e-p:32:32-i1:8:0-i8:8:0-i16:16:0-i32:32:0-i32:32:0-i64:32:0-f32:32:0-f64:32:0-f96:32:0-v64:64:64-v128:128:0-a0:0"
target triple = "i386-pc-linux-gnu"
@flags = internal global [8191 x i8] zeroinitializer		; <[8191 x i8]*> [#uses=3]
@.str = internal constant [18 x i8] c"10000 iterations\0A\00"		; <[18 x i8]*> [#uses=1]
@.str1 = internal constant [11 x i8] c"%d primes\0A\00"		; <[11 x i8]*> [#uses=1]

declare i32 @printf(i8*, ...) nounwind

define i32 @main(i32, i8**, ...) nounwind {
entry:
	%2 = call i32 (i8*, ...)* @printf(i8* getelementptr ([18 x i8]* @.str, i32 0, i32 0))		; <i32> [#uses=0]
	br label %test

test:		; preds = %continue, %entry
	%count.0 = phi i32 [ undef, %entry ], [ %count.2, %continue ]		; <i32> [#uses=1]
	%storemerge4 = phi i32 [ %4, %continue ], [ 1, %entry ]		; <i32> [#uses=2]
	%3 = icmp sgt i32 %storemerge4, 10000		; <i1> [#uses=1]
	br i1 %3, label %return, label %test1

continue:		; preds = %test5
	%4 = add i32 %storemerge4, 1		; <i32> [#uses=1]
	br label %test

test1:		; preds = %continue3, %test
	%storemerge = phi i32 [ %7, %continue3 ], [ 0, %test ]		; <i32> [#uses=3]
	%5 = icmp sgt i32 %storemerge, 8190		; <i1> [#uses=1]
	br i1 %5, label %test5, label %continue3

continue3:		; preds = %test1
	%6 = getelementptr [8191 x i8]* @flags, i32 0, i32 %storemerge		; <i8*> [#uses=1]
	store i8 1, i8* %6
	%7 = add i32 %storemerge, 1		; <i32> [#uses=1]
	br label %test1

test5:		; preds = %continue7, %test1
	%count.2 = phi i32 [ %count.1, %continue7 ], [ 0, %test1 ]		; <i32> [#uses=3]
	%storemerge3 = phi i32 [ %12, %continue7 ], [ 0, %test1 ]		; <i32> [#uses=5]
	%8 = icmp sgt i32 %storemerge3, 8190		; <i1> [#uses=1]
	br i1 %8, label %continue, label %body6

body6:		; preds = %test5
	%9 = getelementptr [8191 x i8]* @flags, i32 0, i32 %storemerge3		; <i8*> [#uses=1]
	%10 = load i8* %9		; <i8> [#uses=1]
	%11 = icmp eq i8 %10, 0		; <i1> [#uses=1]
	br i1 %11, label %continue7, label %ifTrue

continue7:		; preds = %next12, %body6
	%count.1 = phi i32 [ %17, %next12 ], [ %count.2, %body6 ]		; <i32> [#uses=1]
	%12 = add i32 %storemerge3, 1		; <i32> [#uses=1]
	br label %test5

ifTrue:		; preds = %body6
	%13 = shl i32 %storemerge3, 1		; <i32> [#uses=1]
	%14 = add i32 %13, 3		; <i32> [#uses=1]
	br label %continue10

continue10:		; preds = %body11, %ifTrue
	%.pn = phi i32 [ %storemerge1, %body11 ], [ %storemerge3, %ifTrue ]		; <i32> [#uses=1]
	%storemerge1 = add i32 %.pn, %14		; <i32> [#uses=3]
	%15 = icmp sgt i32 %storemerge1, 8190		; <i1> [#uses=1]
	br i1 %15, label %next12, label %body11

body11:		; preds = %continue10
	%16 = getelementptr [8191 x i8]* @flags, i32 0, i32 %storemerge1		; <i8*> [#uses=1]
	store i8 0, i8* %16
	br label %continue10

next12:		; preds = %continue10
	%17 = add i32 %count.2, 1		; <i32> [#uses=1]
	br label %continue7

return:		; preds = %test
	%18 = call i32 (i8*, ...)* @printf(i8* getelementptr ([11 x i8]* @.str1, i32 0, i32 0), i32 %count.0)		; <i32> [#uses=0]
	ret i32 0
}
