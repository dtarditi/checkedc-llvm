; RUN: opt < %s -simplifycfg -S | FileCheck %s

define i32 @test(i32 %x) {
; CHECK-LABEL: @test
entry:
  %y = alloca i32, align 4
  %tobool = icmp ne i32 %x, 0
  br i1 %tobool, label %if.then, label %if.else

if.then:
; CHECK-LABEL: if.then:
; CHECK: [[ASM1:%.*]] = call i32 asm "mov $0, #1", "=r"()
  %tmp1 = call i32 asm "mov $0, #1", "=r"() nounwind readnone
  store i32 %tmp1, i32* %y, align 4
  br label %if.end

if.else:
; CHECK-LABEL: if.else:
; CHECK: [[ASM2:%.*]] = call i32 asm "mov $0, #2", "=r"()
  %tmp2 = call i32 asm "mov $0, #2", "=r"() nounwind readnone
  store i32 %tmp2, i32* %y, align 4
  br label %if.end

if.end:
; CHECK-LABEL: if.end:
; CHECK: {{%.*}} = phi i32 [ [[ASM2]], %if.else ], [ [[ASM1]], %if.then ]
  %tmp3 = load i32, i32* %y, align 4
  ret i32 %tmp3
}
