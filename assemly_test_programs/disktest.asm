pre_code:
	sll $sp, $imm1, $imm2, $zero, 1, 11		# set $sp = 1 << 11 = 2048
	add $sp, $sp, $imm1, $zero, -2 ,0		# open stack
	sw $s0, $sp, $imm1, $zero, 0, 0			# save $s0
	sw $s1, $sp, $imm1, $zero, 1, 0			# save $s1
	add $s0, $imm1, $zero, $zero, 0, 0		# i = 0 
	add $s1, $imm1, $zero, $zero, 0, 0		# mem_address = 0 

read_loop:
	in $t1, $imm1, $zero,$zero, 17,0		# t1 = $diskstatus
	bne $zero, $t1, $zero, $imm1 ,read_loop,0	# if $diskstatus != 0 wait
	out $zero, $imm1, $zero, $s0, 15,0		# disksector = i
	out $zero, $imm1, $zero, $s1, 16,0		# diskbuffer = mem_adder
	out $zero, $imm1, $zero, $imm2, 14 ,1	        # diskcmd = read

	add $s0, $s0, $imm1,$zero,1,0			# i = i +1
	add $s1, $s1, $imm1 ,$zero, 128 ,0		# mem_address = mem_address + 128
	blt $zero, $s0, $imm1, $imm2 ,8 ,read_loop	# if i<8 read again

write_loop:						
	in $t1,$imm1, $zero,$zero, 17,0		        # t1 = $diskstatus
	bne $zero, $t1, $zero, $imm1 , write_loop, 0	# if $diskstatus != 0 then wait
	add $s1, $s1, $imm1 ,$zero, -128 ,0    		# mem_address = mem_address - 128
	out $zero, $imm1, $zero, $s0, 15,0		# disksector = i
	out $zero, $imm1, $zero, $s1 , 16,0		# diskbuffer = mem_address
	out $zero, $imm1, $zero, $imm2, 14 ,2	 	# diskcmd = write

	add $s0, $s0, $imm1, $zero, -1,0		# i = i-1
	bgt $zero, $s0, $zero, $imm1, write_loop, 0	# if i>0 write again

end_code:
	lw $s0, $sp, $imm1, $zero, 0, 0			# load $s0
	lw $s1, $sp, $imm1, $zero, 1, 0			# load $s1
	add $sp, $sp, $imm1, $zero, 2 ,0		# close stack
	halt $zero, $zero, $zero, $zero, 0, 0							
							
