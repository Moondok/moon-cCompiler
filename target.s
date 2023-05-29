.text
.globl main

func :
addi $sp $sp -8
sw $ra 8($sp)
sw $fp 4($sp)
move $fp $sp
addi $sp $sp -8
sw $a0 8($sp)
sw $a1 4($sp)
lw $t0 8($sp)
lw $t1 4($sp)
add $t2 $t0 $t1
move $v0 $t2
addi $sp $sp 8
lw $fp 4($sp)
lw $ra 8($sp)
addi $sp $sp 8
jr $ra
main :
addi $sp $sp -8
sw $ra 8($sp)
sw $fp 4($sp)
move $fp $sp
addi $sp $sp -56
li.s $f1 5.3
s.s $f1 16($sp)
li $t0 1
sw $t0 12($sp)
li $t0 99
sw $t0 8($sp)
lw $a0 8($sp)
lw $a1 12($sp)
addi $sp $sp -8
sw $ra 8($sp)
sw $fp 4($sp)
jal func
lw $fp 4($sp)
lw $ra 8($sp)
addi $sp $sp 8
move $t0 $v0
sw $t0 4($sp)
li $t0 0
move $v0 $t0
addi $sp $sp 56
lw $fp 4($sp)
lw $ra 8($sp)
addi $sp $sp 8
jr $ra
li $v0 10
syscall
