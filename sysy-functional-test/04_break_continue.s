main:
  addi sp, sp, -24
BB_0:
  li t1, 0
  sw t1, 20(sp)
  li t1, 0
  sw t1, 16(sp)
BB_1:
  lw t1, 16(sp)
  li t2, 20
  sub t2, t2, t1
  sgtz t2, t2
  bnez t2, BB_2
  j BB_3
BB_2:
  li t1, 0
  sw t1, 12(sp)
BB_4:
  lw t1, 12(sp)
  li t2, 10
  sub t2, t2, t1
  sgtz t2, t2
  bnez t2, BB_5
  j BB_6
BB_5:
  li t1, 0
  sw t1, 8(sp)
BB_7:
  lw t1, 8(sp)
  li t2, 5
  sub t2, t2, t1
  sgtz t2, t2
  bnez t2, BB_8
  j BB_9
BB_8:
  li t1, 0
  sw t1, 4(sp)
BB_10:
  lw t1, 4(sp)
  li t2, 3
  sub t2, t2, t1
  sgtz t2, t2
  bnez t2, BB_11
  j BB_12
BB_11:
  lw t1, 4(sp)
  addi t2, t1, 1
  li t1, 3
  sub t1, t2, t1
  seqz t3, t1
  sgtz t1, t1
  or t1, t1, t3
  bnez t1, BB_13
  j BB_14
BB_13:
  lw t1, 4(sp)
  bnez t1, BB_15
  j BB_16
BB_15:
  lw t1, 4(sp)
  lw t2, 4(sp)
  seqz t3, t2
  snez t1, t1
  snez t3, t3
  or t2, t1, t3
  bnez t2, BB_17
  j BB_18
BB_17:
  lw t1, 4(sp)
  li t2, -1
  sub t2, t1, t2
  li t1, 3
  sub t1, t2, t1
  seqz t3, t1
  sgtz t1, t1
  or t1, t1, t3
  bnez t1, BB_19
  j BB_20
BB_19:
  j BB_12
  j BB_10
  j BB_21
BB_20:
  j BB_21
BB_21:
  j BB_22
BB_18:
  j BB_22
BB_22:
  j BB_23
BB_16:
  j BB_23
BB_23:
  j BB_24
BB_14:
  j BB_24
BB_24:
  li t1, 0
  sw t1, 0(sp)
BB_25:
  lw t1, 0(sp)
  li t2, 2
  sub t2, t2, t1
  sgtz t2, t2
  bnez t2, BB_26
  j BB_27
BB_26:
  lw t1, 0(sp)
  addi t2, t1, 1
  sw t2, 0(sp)
  j BB_25
  j BB_27
  lw t1, 20(sp)
  addi t2, t1, 1
  sw t2, 20(sp)
  j BB_25
BB_27:
  lw t1, 4(sp)
  addi t2, t1, 1
  sw t2, 4(sp)
  lw t1, 20(sp)
  addi t2, t1, 1
  sw t2, 20(sp)
  j BB_10
BB_12:
  nop
BB_28:
  j BB_29
BB_29:
  nop
BB_30:
  j BB_31
BB_31:
  j BB_32
  j BB_30
BB_32:
  j BB_33
  j BB_28
BB_33:
  lw t1, 8(sp)
  addi t2, t1, 1
  sw t2, 8(sp)
  j BB_7
BB_9:
  lw t1, 12(sp)
  addi t2, t1, 1
  sw t2, 12(sp)
  j BB_4
  lw t1, 12(sp)
  addi t2, t1, 1
  sw t2, 12(sp)
  j BB_4
BB_6:
  lw t1, 16(sp)
  addi t2, t1, 1
  sw t2, 16(sp)
  j BB_1
BB_3:
  lw t1, 20(sp)
  mv a0, t1
  ret 
