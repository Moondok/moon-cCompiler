.text
.globl main

main :
addi $sp $sp -8
sw $ra 8($sp)
sw $fp 4($sp)
move $fp $sp
addi $sp $sp -20
addi $sp $sp -4
li $t0 0
sw $t0 4($sp)
label0 :
li $t0 5
lw $t1 4($sp)
slt $t2 $t1 $t0
bne $zero $t2 label1
j label2
label1 :
addi $sp $sp -4
li $t0 5
lw $t1 8($sp)
slt $t2 $t0 $t1
bne $zero $t2 label3
j label4
label3 :
li $t0 0
sw $t0 4($sp)
j label2
label4 :
addi $sp $sp 4
li $t0 4
lw $t1 4($sp)
mul $t2 $t1 $t0
addi $t0 $zero 24
sub $t2 $t0 $t2
add $t2 $sp $t2
li $t0 5
lw $t1 4($sp)
sub $t3 $t0 $t1
sw $t3 0($t2)
li $t0 1
lw $t1 4($sp)
add $t2 $t1 $t0
sw $t2 4($sp)
j label0
label2 :
addi $sp $sp 4
addi $sp $sp -8
li $t0 0
sw $t0 8($sp)
label5 :
li $t0 5
lw $t1 8($sp)
slt $t2 $t1 $t0
bne $zero $t2 label6
j label7
label6 :
li $t0 0
sw $t0 4($sp)
addi $sp $sp -0
label8 :
li $t0 5
lw $t1 8($sp)
sub $t2 $t0 $t1
li $t0 1
sub $t1 $t2 $t0
lw $t0 4($sp)
slt $t2 $t0 $t1
bne $zero $t2 label9
j label10
label9 :
addi $sp $sp -4
li $t0 4
lw $t1 8($sp)
mul $t2 $t1 $t0
addi $t0 $zero 32
sub $t2 $t0 $t2
add $t2 $sp $t2
li $t0 1
lw $t1 8($sp)
add $t3 $t1 $t0
li $t0 4
mul $t1 $t3 $t0
addi $t0 $zero 32
sub $t1 $t0 $t1
add $t1 $sp $t1
lw $t0 0($t2)
lw $t2 0($t1)
slt $t1 $t2 $t0
bne $zero $t1 label11
j label12
label11 :
li $t0 4
lw $t1 8($sp)
mul $t2 $t1 $t0
addi $t0 $zero 32
sub $t2 $t0 $t2
add $t2 $sp $t2
lw $t0 0($t2)
sw $t0 4($sp)
li $t0 4
lw $t1 8($sp)
mul $t2 $t1 $t0
addi $t0 $zero 32
sub $t2 $t0 $t2
add $t2 $sp $t2
li $t0 1
lw $t1 8($sp)
add $t3 $t1 $t0
li $t0 4
mul $t1 $t3 $t0
addi $t0 $zero 32
sub $t1 $t0 $t1
add $t1 $sp $t1
lw $t0 0($t1)
sw $t0 0($t2)
li $t0 1
lw $t1 8($sp)
add $t2 $t1 $t0
li $t0 4
mul $t1 $t2 $t0
addi $t0 $zero 32
sub $t1 $t0 $t1
add $t1 $sp $t1
lw $t0 4($sp)
sw $t0 0($t1)
label12 :
addi $sp $sp 4
li $t0 1
lw $t1 4($sp)
add $t2 $t1 $t0
sw $t2 4($sp)
j label8
label10 :
addi $sp $sp 0
li $t0 1
lw $t1 8($sp)
add $t2 $t1 $t0
sw $t2 8($sp)
j label5
label7 :
addi $sp $sp 8
li $t0 0
move $v0 $t0
addi $sp $sp 20
lw $fp 4($sp)
lw $ra 8($sp)
addi $sp $sp 8
jr $ra
li $v0 10
syscall
