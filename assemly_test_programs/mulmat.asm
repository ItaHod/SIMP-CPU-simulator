main:       
		sll $sp, $imm1, $imm2, $zero, 1, 11		# set $sp = 1 << 11 = 2048
		add $sp,   $sp,    $imm1, $zero, -16,        0      # allocate space in stack
                sw  $sp,   $zero,  $s0,   $zero,  0,         0      # store $s0 in stack
                sw  $sp,   $imm1,  $s1,   $zero,  4,         0      # store $s1 in stack
                sw  $sp,   $imm1,  $s2,   $zero,  8,         0      # store $s2 in stack
                sw  $sp,   $imm1,  $ra,   $zero,  12,        0      # store $ra in stack
                add $a0,   $imm1,  $zero,  $zero,  0,        0      #rows_i = 0 
                add $a1,   $imm1,  $zero,  $zero,  0,        0      #columns_i = 0 
                add $a2,   $imm1,  $zero,  $zero,  0x100,    0      #pointer to matrixes

loopi:          bgt $zero, $a0,    $imm1,  $imm2, 3,         end    #end of matrix multiplication if row_i>3
loopj:          bgt $zero, $a1,    $imm1,  $imm2, 3,         loopi  #end of row j
                jal $ra,   $zero,  $zero,  $imm1, vector_mul,0      #multiply vectors i,j
                add $a1,   $imm1,  $zero,  $zero,  1,        0      #column_i++
                beq $zero, $zero,  $zero  $imm1   loopj,     0 
                                                                    #end of loop j in loop i
                add $a0,   $imm1,  $zero,  $zero,  1,        0      #rows_i++
                add $a1,   $imm1,  $zero,  $zero,  0,        0      #columns_i = 0 
                beq $zero, $zero,  $zero  $imm1   loopi,     0

vector_mul:     mac $s1,   $a0,    $imm1, $a2,    4,         0      #pointer to left vector
                add $s2,   $a1,    $a2,   $imm1,  16,        0      #pointer to right vector
                add $t0,   $zero,  $zero  $zero,  0,         0      #vectors elements index - ve_i [0:3]
                add $s0,   $zero,  $zero  $zero,  0,         0      #multiplication sum initialize
L1:             bgt $zero, $t0,    $imm1, $imm2,  3,         save   #if ve_i>3 => save sum
                lw  $t1,   $s1,    $zero  $zero,  0,         0      #load element of left vector
                lw  $t2,   $s2,    $zero  $zero,  0,         0      #load element of right vector
                mac $s0,   $t1,    $t2    $s0,    0,         0      #multiply and add to $s0
                add $t0,   $imm1,  $zero  $zero,  1,         0      #ve_i ++
                add $s1,   $imm1,  $zero  $zero,  1,         0      #left vector next element
                add $s2,   $imm1,  $zero  $zero,  4,         0      #right vector next element
                beq $zero, $zero,  $zero  $imm1   L1,        0
save:           mac $t1,   $a0,    $imm1, $a1,    4,         0      #calculate memory offset for saving multiplication
                add $t1,   $t1,    $imm1, $zero,  0x12F,     0      #the exact location with matrix address
                sw  $t1,   $zero,  $s0,   $zero,  0,         0      #store result in result matrix    
                beq $zero, $zero,  $zero  $ra     0,         0      #end vector multiplication
 end:           lw  $sp,   $zero,  $s0,   $zero,  0,         0      # load $s0 from stack
                lw  $sp,   $imm1,  $s1,   $zero,  4,         0      # load $s1 from stack
                lw  $sp,   $imm1,  $s2,   $zero,  8,         0      # load $s2 from stack
                lw  $sp,   $imm1,  $ra,   $zero,  12,        0      # load $ra from stack
                add $sp,   $sp,    $imm1, $zero,  16,        0      # remove space from stack            
		halt $zero, $zero, $zero, $zero, 0, 0			# halt

 