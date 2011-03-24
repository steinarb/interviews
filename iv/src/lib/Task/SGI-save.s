	#.file "sig_save.s"

#include <sys/regdef.h>

	framesize = 0

	.set noat
	.set noreorder

.globl sig_save
	.ent sig_save
sig_save:
       #subu $sp,framesize	# move stack pointer down
	#.frame	sp,framesize,ra
	#sw ?,0x0(a0)		# sc_onstack
	#sw ?,0x4(a0)		# sc_mask
	sw ra,0x8(a0)		# sc_pc
	sw $0,0xc(a0)		# sc_regs[0]
	sw $1,0x10(a0)	# sc_regs[1]
	sw $2,0x14(a0)	# sc_regs[2]
	sw $3,0x18(a0)	# sc_regs[3]
	sw $4,0x1c(a0)	# sc_regs[4]
	sw $5,0x20(a0)	# sc_regs[5]
	sw $6,0x24(a0)	# sc_regs[6]
	sw $7,0x28(a0)	# sc_regs[7]
	sw $8,0x2c(a0)	# sc_regs[8]
	sw $9,0x30(a0)	# sc_regs[9]
	sw $10,0x34(a0)	# sc_regs[10]
	sw $11,0x38(a0)	# sc_regs[11]
	sw $12,0x3c(a0)	# sc_regs[12]
	sw $13,0x40(a0)	# sc_regs[13]
	sw $14,0x44(a0)	# sc_regs[14]
	sw $15,0x48(a0)	# sc_regs[15]
	sw $16,0x4c(a0)	# sc_regs[16]
	sw $17,0x50(a0)	# sc_regs[17]
	sw $18,0x54(a0)	# sc_regs[18]
	sw $19,0x58(a0)	# sc_regs[19]
	sw $20,0x5c(a0)	# sc_regs[20]
	sw $21,0x60(a0)	# sc_regs[21]
	sw $22,0x64(a0)	# sc_regs[22]
	sw $23,0x68(a0)	# sc_regs[23]
	sw $24,0x6c(a0)	# sc_regs[24]
	sw $25,0x70(a0)	# sc_regs[25]
	sw $26,0x74(a0)	# sc_regs[26]
	sw $27,0x78(a0)	# sc_regs[27]
	sw $28,0x7c(a0)	# sc_regs[28]
	sw $29,0x80(a0)	# sc_regs[29]
	sw $30,0x84(a0)	# sc_regs[30]
	sw $31,0x88(a0)	# sc_regs[31]

	mflo AT		# mdlo -> AT
	sw AT,0x8c(a0) 	# AT -> sc_mdlo
	mfhi AT		# mdhi -> AT
	sw AT,0x90(a0) 	# AT -> sc_mdhi		

 	j ra	# return
	nop	# delay branch
.end sig_save

.globl sig_restore
	.ent sig_restore
sig_restore:
        #subu $sp,framesize	# move stack pointer down
	#.frame	sp,framesize,ra
	#lw ?, 0x0(a0)		# sc_onstack
	#lw ?, 0x4(a0)		# sc_mask
	#lw ?, 0x8(a0)		# sc_pc (used in final jump)

	lw $0,0xc(a0)	# sc_regs[0]
	lw $1,0x10(a0)	# sc_regs[1] assembler temp, used later (not restored)
	lw $2,0x14(a0)	# sc_regs[2]
	lw $3,0x18(a0)	# sc_regs[3]
	#lw $4,0x1c(a0)	# sc_regs[4] a0 used (restored later)
	lw $5,0x20(a0)	# sc_regs[5]
	lw $6,0x24(a0)	# sc_regs[6]
	lw $7,0x28(a0)	# sc_regs[7]
	lw $8,0x2c(a0)	# sc_regs[8]
	lw $9,0x30(a0)	# sc_regs[9]
	lw $10,0x34(a0)	# sc_regs[10]
	lw $11,0x38(a0)	# sc_regs[11]
	lw $12,0x3c(a0)	# sc_regs[12]
	lw $13,0x40(a0)	# sc_regs[13]
	lw $14,0x44(a0)	# sc_regs[14]
	lw $15,0x48(a0)	# sc_regs[15]
	lw $16,0x4c(a0)	# sc_regs[16]
	lw $17,0x50(a0)	# sc_regs[17]
	lw $18,0x54(a0)	# sc_regs[18]
	lw $19,0x58(a0)	# sc_regs[19]
	lw $20,0x5c(a0)	# sc_regs[20]
	lw $21,0x60(a0)	# sc_regs[21]
	lw $22,0x64(a0)	# sc_regs[22]
	lw $23,0x68(a0)	# sc_regs[23]
	lw $24,0x6c(a0)	# sc_regs[24]
	lw $25,0x70(a0) # sc_regs[25]
	lw $26,0x74(a0)	# sc_regs[26]
	lw $27,0x78(a0)	# sc_regs[27]
	lw $28,0x7c(a0)	# sc_regs[28]
	lw $29,0x80(a0)	# sc_regs[29]
	lw $30,0x84(a0)	# sc_regs[30]
	lw $31,0x88(a0)	# sc_regs[31]

	lw $30,0x8c(a0) 	# sc_mdlo -> $30  Take care of HI, LO
	nop
	mtlo $30		# $30 -> mtlo
	lw $30,0x90(a0) 	# sc_mdhi -> $30
	nop
	mthi $30		# $30 -> mdhi

	# store a0 in $30 (maybe should be stored on stack?)
	move $30,a0	# a0 -> $30

	# restore a0 interrupted process
	lw $4,0x1c(a0)	# sc_regs[4] -> a0

	lw $30,0x8($30)	# $30 <- sc_pc
	nop
	j $30		# continue different thread
	nop		# delay branch 

.end sig_restore
