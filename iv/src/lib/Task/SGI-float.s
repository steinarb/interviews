	#.file "mips_float.s"

#include <sys/regdef.h>

	framesize = 0

.globl float_save
	.ent float_save
float_save:
	cfc1 $30,$31	  # fcr31 -> r30
	sw $30, 0x118(a0) # r30 -> sc_fpc_csr

	swc1 $f0,0x98(a0) # sc_fpregs[0]
	swc1 $f1,0x9c(a0) # sc_fpregs[1]
	swc1 $f2,0xa0(a0) # sc_fpregs[2]
	swc1 $f3,0xa4(a0) # sc_fpregs[3]
	swc1 $f4,0xa8(a0) # sc_fpregs[4]
	swc1 $f5,0xac(a0) # sc_fpregs[5]
	swc1 $f6,0xb0(a0) # sc_fpregs[6]
	swc1 $f7,0xb4(a0) # sc_fpregs[7]
	swc1 $f8,0xb8(a0) # sc_fpregs[8]
	swc1 $f9,0xbc(a0) # sc_fpregs[9]
	swc1 $f10,0xc0(a0) # sc_fpregs[10]
	swc1 $f11,0xc4(a0) # sc_fpregs[11]
	swc1 $f12,0xc8(a0) # sc_fpregs[12]
	swc1 $f13,0xcc(a0) # sc_fpregs[13]
	swc1 $f14,0xd0(a0) # sc_fpregs[14]
	swc1 $f15,0xd4(a0) # sc_fpregs[15]
	swc1 $f16,0xd8(a0) # sc_fpregs[16]
	swc1 $f17,0xdc(a0) # sc_fpregs[17]
	swc1 $f18,0xe0(a0) # sc_fpregs[18]
	swc1 $f19,0xe4(a0) # sc_fpregs[19]
	swc1 $f20,0xe8(a0) # sc_fpregs[20]
	swc1 $f21,0xec(a0) # sc_fpregs[21]
	swc1 $f22,0xf0(a0) # sc_fpregs[22]
	swc1 $f23,0xf4(a0) # sc_fpregs[23]
	swc1 $f24,0xf8(a0) # sc_fpregs[24]
	swc1 $f25,0xfc(a0) # sc_fpregs[25]
	swc1 $f26,0x100(a0) # sc_fpregs[26]
	swc1 $f27,0x104(a0) # sc_fpregs[27]
	swc1 $f28,0x108(a0) # sc_fpregs[28]
	swc1 $f29,0x10c(a0) # sc_fpregs[29]
	swc1 $f30,0x110(a0) # sc_fpregs[30]
	swc1 $f31,0x114(a0) # sc_fpregs[31]

 	j ra	# return
	nop	# delay branch
.end float_save

.globl float_restore
	.ent float_restore
float_restore:

	lwc1 $f0,0x98(a0) # sc_fpregs[0]
	lwc1 $f1,0x9c(a0) # sc_fpregs[1]
	lwc1 $f2,0xa0(a0) # sc_fpregs[2]
	lwc1 $f3,0xa4(a0) # sc_fpregs[3]
	lwc1 $f4,0xa8(a0) # sc_fpregs[4]
	lwc1 $f5,0xac(a0) # sc_fpregs[5]
	lwc1 $f6,0xb0(a0) # sc_fpregs[6]
	lwc1 $f7,0xb4(a0) # sc_fpregs[7]
	lwc1 $f8,0xb8(a0) # sc_fpregs[8]
	lwc1 $f9,0xbc(a0) # sc_fpregs[9]
	lwc1 $f10,0xc0(a0) # sc_fpregs[10]
	lwc1 $f11,0xc4(a0) # sc_fpregs[11]
	lwc1 $f12,0xc8(a0) # sc_fpregs[12]
	lwc1 $f13,0xcc(a0) # sc_fpregs[13]
	lwc1 $f14,0xd0(a0) # sc_fpregs[14]
	lwc1 $f15,0xd4(a0) # sc_fpregs[15]
	lwc1 $f16,0xd8(a0) # sc_fpregs[16]
	lwc1 $f17,0xdc(a0) # sc_fpregs[17]
	lwc1 $f18,0xe0(a0) # sc_fpregs[18]
	lwc1 $f19,0xe4(a0) # sc_fpregs[19]
	lwc1 $f20,0xe8(a0) # sc_fpregs[20]
	lwc1 $f21,0xec(a0) # sc_fpregs[21]
	lwc1 $f22,0xf0(a0) # sc_fpregs[22]
	lwc1 $f23,0xf4(a0) # sc_fpregs[23]
	lwc1 $f24,0xf8(a0) # sc_fpregs[24]
	lwc1 $f25,0xfc(a0) # sc_fpregs[25]
	lwc1 $f26,0x100(a0) # sc_fpregs[26]
	lwc1 $f27,0x104(a0) # sc_fpregs[27]
	lwc1 $f28,0x108(a0) # sc_fpregs[28]
	lwc1 $f29,0x10c(a0) # sc_fpregs[29]
	lwc1 $f30,0x110(a0) # sc_fpregs[30]
	lwc1 $f31,0x114(a0) # sc_fpregs[31]

	lw $30, 0x118(a0) # r30 -> sc_fpc_csr
	ctc1 $30,$31	   # fcr31 -> r30 (r30 will be trashed later)
 	j ra	# return
	nop	# delay branch

.end float_restore

