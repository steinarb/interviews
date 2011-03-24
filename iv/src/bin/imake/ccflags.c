/*
 * Put any special flags that will be necessary to get imake.c to compile.  Do
 * not include the value of BOOTSTRAPCFLAGS as it will be passed to imake.c
 * automatically.
 */

#ifdef hp9000s300
#define ccflags "-Wc,-Nd4000,-Ns3000 -DSYSV"
#endif /* hp9000s300 */

#ifdef hp9000s800
#define ccflags "-DSYSV"
#endif /* hp9000s800 */

#ifdef macII
#define ccflags "-DSYSV"
#endif /* macII */

#ifdef att
#define ccflags "-DSYSV -DUSG"
#endif /* att */

#ifdef aix
#define ccflags "-Daix"
#endif

#ifdef CRAY
#define ccflags "-DSYSV"
#endif /* CRAY */

#ifdef umips
#ifdef SYSTYPE_SYSV
#define ccflags "-DSYSV -I../../lib/X/mips -I/usr/include/bsd ../../lib/X/mips/mipssysvc.c -lbsd"
#endif
#endif

#ifndef ccflags
#define ccflags "-O"
#endif /* ccflags */

main()
{
	write(1, ccflags, sizeof(ccflags) - 1);
	exit(0);
}
