.text
.globl main

func :
addi $sp $sp -8
sw $ra 8($sp)
sw $fp 4($sp)
move $fp $sp
addi $sp $sp -12
sw $a0 12($sp)
sw $a1 8($sp)
lw $t0 12($sp)
lw $t1 8($sp)
add $t2 $t0 $t1
sw $t2 4($sp)
lw $v0 4($sp)
addi $sp $sp 12
lw $fp 4($sp)
lw $ra 8($sp)
addi $sp $sp 8
jr $ra
main :
addi $sp $sp -8
sw $ra 8($sp)
sw $fp 4($sp)
move $fp $sp
addi $sp $sp -32
li $t0 846
sw $t0 32($sp)
li $t0 57
sw $t0 28($sp)
li $t0 0
sw $t0 24($sp)
li $t0 0
sw $t0 20($sp)
addi $sp $sp -12
label0 :
li $t0 4
lw $t1 36($sp)
slt $t2 $t1 $t0
bne $zero $t2 label1
j label2
label1 :
li $t0 4
lw $t1 36($sp)
mul $t2 $t1 $t0
addi $t0 $zero 28
sub $t2 $t0 $t2
add $t2 $sp $t2
lw $t0 36($sp)
sw $t0 0($t2)
li.s $f1 3.0
s.s $f1 12($sp)
li $t0 0
sw $t0 8($sp)
lw $a0 8($sp)
lw $a1 36($sp)
addi $sp $sp -8
sw $ra 8($sp)
sw $fp 4($sp)
jal func
lw $fp 4($sp)
lw $ra 8($sp)
addi $sp $sp 8
move $t0 $v0
sw $t0 4($sp)
addi $sp $sp -4
lw $t0 8($sp)
lw $t1 40($sp)
slt $t2 $t0 $t1
bne $zero $t2 label3
j label4
label3 :
li $t0 2
sw $t0 4($sp)
j label2
label4 :
addi $sp $sp 4
li $t0 1
lw $t1 36($sp)
add $t2 $t1 $t0
sw $t2 36($sp)
j label0
label2 :
addi $sp $sp 12
li $t0 0
move $v0 $t0
addi $sp $sp 32
lw $fp 4($sp)
lw $ra 8($sp)
addi $sp $sp 8
jr $ra
li $v0 10
syscall
