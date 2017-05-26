.data
dat: .word 10, 20
newline: .asciiz "\n"

.text #text section
.globl main #call main by SPIM

foo:
la $t0, dat
addi $sp,$sp, -8
sw $ra, 4($sp)
sw $fp, 0($sp)
addi $sp, $sp, -4
addi $a0,$0,2       # y = 2       
sw $a0, 0($sp)
lw $a0, 16($sp)     # lw i
sll $a0, $a0, 2     # i << 2
add $a0, $a0, $t0   # get index a[i]
lw $a0, 0($a0)      # x =  a[i]
addi $sp, $sp, -4
sw $a0,0($sp)
blez $a0, else
lw $a0, 16($sp)     # lw j
sll $a0, $a0, 2     # j << 2
add $a0, $a0, $t0   # get index a[j]
lw $t1, 4($sp)      # y
sw $t1, 0($a0)      # a[j] = y
add $a0, $t1,$0     # y

j exit
else:
lw $a0, 16($sp)     # lw j
sll $a0, $a0, 2     # j << 2
add $a0, $a0, $t0   # get index a[j]
lw $a0, 0($a0)      # y = a[j]
exit:
lw $t1, 0($sp)      # get a[i]
mult $t1, $a0    
mflo $a0	
addi $sp, $sp, 4
addi $sp, $sp, 4
li $v0,1
syscall
li $v0,4
la $a0, newline
syscall


lw $ra, 4($sp)
lw $fp, 0($sp)
addi $sp, $sp, 8
jr $ra


main:

addi $a0, $0, 0
addi $sp, $sp, -4
sw $a0, 0($sp)
addi $a0, $0, 1
addi $sp, $sp, -4
sw $a0, 0($sp)
jal foo 
addi $sp, $sp, 4
addi $sp, $sp, 4

li $v0,10
syscall