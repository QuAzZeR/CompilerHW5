.data

newline: .asciiz "\n"

.text #text section
.globl main #call main by SPIM

Factorial:
addi $sp,$sp, -8
sw $ra, 4($sp)
sw $fp, 0($sp)
lw $a0, 8($sp)
bne		$a0, $0, else	# if $t0 != 0t1 then 
li $a0, 1
j exit
else:
addi $sp, $sp, -4
sw $a0,0($sp)
addi $sp, $sp, -4
addi $a0, $a0, -1
sw $a0,0($sp)
jal Factorial
addi $sp, $sp, 4
lw $t1,0($sp)
addi $sp, $sp, 4
mult	$a0, $t1			# $a0 * $t1 = Hi and Lo registers
mflo	$a0					# copy Lo to $a0
exit:

lw $ra, 4($sp)
lw $fp, 0($sp)
addi $sp, $sp, 8
jr $ra


main:
li $a0, 3
addi $sp, $sp, -4
sw $a0, 0($sp)
jal Factorial
addi $sp, $sp, 4

li $v0,1
syscall
li $v0,10
syscall