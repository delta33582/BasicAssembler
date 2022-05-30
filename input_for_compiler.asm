.data
string1:	.asciiz	"hello\n"	#string1
string2:	.asciiz	"ce_uit\n"	#string2
arrayA:		.word 2 4 8 12 6 4
arrayB:		.space 10

.text		#code
main:	addi	$v0, $0, 4	
	lui $1, 0xABCD		# Note: ABCD la 4 so cuoi cua MSSV
	add $a0, $1, $0
	syscall
	andi	$a0, $zero, 10		#$a0: 10
	sub	$s0, $zero, $s1#$s0 = -$s1
	#nhay toi function 1
	jal	function1
	addiu $v0, $0,   1
	syscall
	addi $v0, $0, 4
	lui $1, 0x0000bc12
	lui $s1, 31
	ori $s3, $s4, 5
	j Exit
	

function1:
	#luu dia chi sau khi thuc hien jal function1	
	addi	$sp, $sp, 4
	subu $a0, $a1, $a2
	sw	$ra, 0($sp)
	sb $s0, 8($t0)
  slt $t0, $t4, $t5
	beq $t0, $0, Exit
function_2:	###ham function 2
addi	$sp, $sp, -8
sw	$ra, 0($sp)
	sw	$s0, 4($sp)
	slti	$t1, $a0, 1
bne $t1, $0, Else
	lw	$s1, 0($sp)
	lbu	$s0, 4($s6)
	
   sltu $s4, $s5, $s6
addi $a0, $a0, -5

	jal	function_2
	
Else:	lw	$ra, 8($sp)
	jr	$ra

Exit:
#ket thuc chuong trinh

