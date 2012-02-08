// RUN: %clang_cc1 -triple i686-pc-win32 -fms-extensions -fblocks -Dnil=0 -rewrite-objc -fobjc-fragile-abi   -o - %s
int main() {
        __weak __block id foo = nil;
        __block id foo2 = nil;
        id foo3 = nil;
        
        void (^myblock)() = ^{
                foo = nil;
                foo2 = nil;
                [foo3 bar];
                id foo4 = foo3;
        };
}
