.text
.globl main

frac :
addi $sp $sp -8
sw $ra 8($sp)
sw $fp 4($sp)
move $fp $sp
addi $sp $sp -4
sw $a0 4($sp)
addi $sp $sp -0
li $t0 1
lw $t1 4($sp)
beq $t1 $t0 label0
j label1
label0 :
li $t0 1
move $v0 $t0
addi $sp $sp 0
addi $sp $sp 4
lw $fp 4($sp)
lw $ra 8($sp)
addi $sp $sp 8
jr $ra
j label2
label1 :
addi $sp $sp -0
li $t0 1
lw $t1 4($sp)
sub $t2 $t1 $t0
move $a0 $t2
addi $sp $sp -8
sw $ra 8($sp)
sw $fp 4($sp)
jal frac
lw $fp 4($sp)
lw $ra 8($sp)
addi $sp $sp 8
move $t0 $v0
lw $t1 4($sp)
mul $t2 $t0 $t1
move $v0 $t2
addi $sp $sp 0
addi $sp $sp 4
lw $fp 4($sp)
lw $ra 8($sp)
addi $sp $sp 8
jr $ra
label2 :
main :
addi $sp $sp -8
sw $ra 8($sp)
sw $fp 4($sp)
move $fp $sp
addi $sp $sp -4
li $t0 3
sw $t0 4($sp)
addi $sp $sp -4
label3 :
li $t0 0
lw $t1 8($sp)
slt $t2 $t0 $t1
bne $zero $t2 label4
j label5
label4 :
lw $a0 8($sp)
addi $sp $sp -8
sw $ra 8($sp)
sw $fp 4($sp)
jal frac
lw $fp 4($sp)
lw $ra 8($sp)
addi $sp $sp 8
move $t0 $v0
sw $t0 4($sp)
li $t0 1
lw $t1 8($sp)
sub $t2 $t1 $t0
sw $t2 8($sp)
j label3
label5 :
addi $sp $sp 4
li $t0 0
move $v0 $t0
addi $sp $sp 4
lw $fp 4($sp)
lw $ra 8($sp)
addi $sp $sp 8
jr $ra
li $v0 10
syscall
