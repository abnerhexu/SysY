main:
  addi sp, sp, -24
BB_0:
  li t1, 1
  sw t1, 20(sp)
  li t1, 2
  sw t1, 16(sp)
  li t1, 3
  sw t1, 12(sp)
  li t1, 4
  sw t1, 8(sp)
  li t1, 5
  sw t1, 4(sp)
  li t1, 6
  sw t1, 0(sp)
  lw t1, 20(sp)
  lw t2, 16(sp)
  mul t3, t1, t2
  lw t1, 12(sp)
  add t2, t3, t1
  li t1, 6
  sub t1, t1, t2
  sgtz t1, t1
  lw t2, 8(sp)
  li t3, 0
  sub t3, t2, t3
  snez t3, t3
  snez t1, t1
  snez t3, t3
  and t2, t1, t3
  bnez t2, BB_1
  j BB_2
BB_1:
  lw t1, 4(sp)
  lw t2, 20(sp)
  seqz t3, t2
  addi t2, t3, 0
  snez t1, t1
  snez t2, t2
  or t3, t1, t2
  bnez t3, BB_3
  j BB_4
BB_2:
  li a0, 1
  ret 
  j BB_5
BB_5:
  nop
BB_3:
  lw t1, 12(sp)
  li t2, 2
  sub t2, t1, t2
  seqz t2, t2
  lw t1, 8(sp)
  lw t3, 4(sp)
  add t4, t1, t3
  li t1, 2
  sub t1, t4, t1
  sgtz t1, t1
  snez t2, t2
  snez t1, t1
  and t3, t2, t1
  bnez t3, BB_6
  j BB_7
BB_4:
  li a0, 2
  ret 
  j BB_8
BB_8:
  j BB_5
BB_6:
  li a0, 3
  ret 
  j BB_9
BB_7:
  lw t1, 0(sp)
  lw t2, 12(sp)
  rem t3, t1, t2
  lw t1, 4(sp)
  snez t3, t3
  snez t1, t1
  and t2, t3, t1
  bnez t2, BB_10
  j BB_11
BB_9:
  j BB_8
BB_10:
  li a0, 4
  ret 
  j BB_12
BB_11:
  lw t1, 8(sp)
  lw t2, 16(sp)
  div t3, t1, t2
  lw t1, 20(sp)
  add t2, t3, t1
  li t1, 2
  sub t1, t2, t1
  seqz t3, t1
  sgtz t1, t1
  or t1, t1, t3
  bnez t1, BB_13
  j BB_14
BB_12:
  j BB_9
BB_13:
  lw t1, 4(sp)
  lw t2, 0(sp)
  sub t3, t1, t2
  li t1, 0
  sub t1, t3, t1
  seqz t2, t1
  sgtz t1, t1
  or t1, t1, t2
  lw t2, 8(sp)
  li t3, 4
  sub t3, t2, t3
  sgtz t3, t3
  snez t1, t1
  snez t3, t3
  or t2, t1, t3
  bnez t2, BB_15
  j BB_16
BB_14:
  li a0, 5
  ret 
  j BB_17
BB_17:
  j BB_12
BB_15:
  li a0, 6
  ret 
  j BB_18
BB_16:
  lw t1, 12(sp)
  lw t2, 0(sp)
  sub t3, t1, t2
  snez t3, t3
  bnez t3, BB_19
  j BB_20
BB_18:
  j BB_17
BB_19:
  lw t1, 16(sp)
  lw t2, 4(sp)
  lw t3, 8(sp)
  mul t4, t2, t3
  add t2, t1, t4
  li t1, 10
  sub t1, t2, t1
  sgtz t1, t1
  bnez t1, BB_21
  j BB_22
BB_20:
  li a0, 7
  ret 
  j BB_23
BB_23:
  j BB_18
BB_21:
  lw t1, 0(sp)
  seqz t2, t1
  bnez t2, BB_24
  j BB_25
BB_22:
  li a0, 8
  ret 
  j BB_26
BB_26:
  j BB_23
BB_24:
  li a0, 9
  ret 
  j BB_27
BB_25:
  li a0, 10
  ret 
  j BB_27
BB_27:
  j BB_26
