	jal $ra, $zero, $zero, $imm2, 0, circle			# goto cicrcle
	halt $zero, $zero, $zero, $zero, 0, 0			# halt
circle:
	add $sp, $sp, $imm2, $zero, 0, -4				# adjust stack for 4 items
	sw $zero, $sp, $imm2, $s0, 0, 3				# save $s0
	sw $zero, $sp, $imm2, $s1, 0, 2				# save return address
	sw $zero, $sp, $imm2, $s2, 0, 1				# save argument
	sw $zero, $sp, $imm2, $ra, 0, 0				# save argument

	out $zero,$imm1,$zero,$imm2,22,0			# set monitorcmd to 0
	add $t0,$imm1,$zero,$zero,0x100,0 			# t0=0x100
	lw $s0,$t0,$zero,$zero,0,0 					# s0=radius
	add $s1,$zero,$zero,$zero,0,0 				# s1=0 (i=0)
loop_i:
	 bge $zero,$s1,$imm1,$imm2,128, end_i		# if (i>=255) goto end_i
	add $s2,$zero,$zero,$zero,0,0 				# s2=0 (j=0)
loop_j: 
	bge $zero,$s2,$imm1,$imm2,128, end_j		# if (j>=255) goto end_j
	add $a0,$s1,$zero,$zero,0,0					#load y to a0
	add $a1,$s2,$zero,$zero,0,0					#load x to a1
	add $a2,$s0,$zero,$zero,0,0					#load r to a2
	jal $ra,$zero,$zero,$imm1, inside_circle,0		#jump to inside_circle
	sll $t0,$s1,$imm1,$zero,8,0					# shift i by 8
	add $t0,$t0,$s2,$zero,0,0					#t0[15:8]=i, t0[7:0]=j
	out $zero,$imm1,$zero,$t0,20,0				# set Monitoraddr
	out $zero,$imm1,$zero,$v0,21,0				# set monitordata to result of inside circle
	out $zero,$imm1,$zero,$imm2,22,1			# set monitorcmd to 1, first querter
	out $zero,$imm1,$zero,$imm2,22,0			# set monitorcmd to 0
	sub $t1,$imm1,$s1,$zero,255,0				#t1=255-i
	sll $t0,$t1,$imm1,$zero,8,0					# shift t1 by 8
	add $t0,$t0,$s2,$zero,0,0					#t0[15:8]=255-i, t0[7:0]=j
	out $zero,$imm1,$zero,$t0,20,0				# set Monitoraddr
	out $zero,$imm1,$zero,$imm2,22,1			# set monitorcmd to 1, 2 querter
	out $zero,$imm1,$zero,$imm2,22,0			# set monitorcmd to 0
	sub $t2,$imm1,$s2,$zero,255,0				#t2=255-j
	sll $t0,$t1,$imm1,$zero,8,0					# shift 255-i by 8
	add $t0,$t0,$t2,$zero,0,0					#t0[15:8]=255-i, t0[7:0]=255-j
	out $zero,$imm1,$zero,$t0,20,0				# set Monitoraddr
	out $zero,$imm1,$zero,$imm2,22,1			# set monitorcmd to 1
	out $zero,$imm1,$zero,$imm2,22,0			# set monitorcmd to 0
	sll $t0,$s1,$imm1,$zero,8,0					# shift i by 8
	add $t0,$t0,$t2,$zero,0,0					#t0[15:8]=i, t0[7:0]=255-j
	out $zero,$imm1,$zero,$t0,20,0				# set Monitoraddr
	out $zero,$imm1,$zero,$imm2,22,1			# set monitorcmd to 1
	out $zero,$imm1,$zero,$imm2,22,0			# set monitorcmd to 0
	add $s2,$s2,$imm1,$zero,1,0 				# s2=s2+1 (j++)
	beq $zero,$zero,$zero,$imm1,loop_j,0			#goto loop_j
end_j: 
	add $s1,$s1,$imm1,$zero,1,0 				# s1=s1+1 (i++)
	beq $zero,$zero,$zero,$imm1,loop_i,0			#goto loop_i
end_i: 
	lw $s0, $sp, $imm2, $zero, 0, 3				# save $s0
	lw $s1, $sp, $imm2, $zero, 0, 2				# save return address
	lw $s2, $sp, $imm2, $zero, 0, 1				# save argument
	lw $ra, $sp, $imm2, $zero, 0, 0				# save argument
	beq $zero, $zero, $zero, $ra, 0, 0				# return
inside_circle:								# v=1 if inside, else v=0
	sub $t0,$a0,$imm1,$zero,127,0				# t0 = x-127
	sub $t1,$a1,$imm1,$zero,127,0				# t1 = y-127
	mac $t0,$t0,$t0,$zero,0,0					# t0= (x-127)^2
	mac $t1,$t1,$t1,$t0,0,0						# t1= (y-127)^2 + (x-127)^2
	mac $t2,$a2,$a2,$zero,0,0					# t2= r^2
	add $v0,$imm1,$zero,$zero,0,0				# v=0
	bgt $zero,$t1,$t2,$imm1, end,0				# if (y-127)^2 + (x-127)^2 > r^2 don’t change the return value and go to end of function
	add $v0,$imm1,$zero,$zero,255,0				# v=255 (x,y) inside the circle
end:
	 beq $zero, $zero, $zero, $ra, 0, 0			# return
.word 0x100 100


	

