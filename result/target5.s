.text
.globl main

add_float :
addi $sp $sp -8
sw $ra 8($sp)
sw $fp 4($sp)
move $fp $sp
addi $sp $sp -8
s.s $f17 8($sp)
s.s $f18 4($sp)
l.s $f1 8($sp)
l.s $f2 4($sp)
add.s $f3 $f1 $f2
mov.s $f0 $f3
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
addi $sp $sp -20
li $t0 0
sw $t0 20($sp)
li $t0 2
sw $t0 16($sp)
li.s $f1 2.0
s.s $f1 12($sp)
li.s $f1 3.0
s.s $f1 8($sp)
l.s $f17 8($sp)
l.s $f18 12($sp)
addi $sp $sp -8
sw $ra 8($sp)
sw $fp 4($sp)
jal add_float
lw $fp 4($sp)
lw $ra 8($sp)
addi $sp $sp 8
mov.s $f1 $f0
s.s $f1 4($sp)
addi $sp $sp -0
label0 :
li $t0 1
lw $t1 20($sp)
add $t2 $t1 $t0
sw $t2 20($sp)
li $t0 3
lw $t1 20($sp)
slt $t2 $t1 $t0
bne $zero $t2 label0
label1 :
addi $sp $sp 0
li $t0 0
move $v0 $t0
addi $sp $sp 20
lw $fp 4($sp)
lw $ra 8($sp)
addi $sp $sp 8
jr $ra
li $v0 10
syscall
