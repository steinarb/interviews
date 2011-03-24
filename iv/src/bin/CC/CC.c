/*
 * Copyright (c) 1987, 1988, 1989 Stanford University
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Stanford not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Stanford makes no representations about
 * the suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * STANFORD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL STANFORD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* 
 * C++ compiler driver
 *
 * Exit code meanings:
 *	 2  too many errors
 *	 1  internal error
 *	 0  all went well
 *	-1  nothing done
 */

#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/wait.h>

#define nil 0

typedef char *String;
typedef char **StringVec;
typedef int FileDesc;
typedef enum { false, true } boolean;

#define BADEXEC -1

#define IsChild(pid) ((pid)==0)

String CC_cpp = "/lib/cpp";
String CC_include = "-I/usr/include/CC";
String CC_library = STDLIB;

#if defined(g_plusplus)

boolean CC_gnu = true;
boolean CC_version2 = true;

String CC_compiler = "cc1plus";
String CC_translator = nil;
String CC_munch = "nmunch";
int CC_munch_relink = 0;

#else

boolean CC_gnu = false;

String CC_compiler = nil;
String CC_translator = "/usr/local/bin/cfront";
String CC_munch = "/usr/local/bin/munch";

#if defined(cfront2_0)
boolean CC_version2 = true;
int CC_munch_relink = 0;
#else
boolean CC_version2 = false;
int CC_munch_relink = 1;
#endif

#endif

#if defined(sgi)

String CC_cc = "/usr/bin/cc";
String CC_as = "/usr/bin/as";
String CC_ld = "/usr/bin/cc";

String CC_mv = "/bin/mv";
String CC_cp = "/bin/cp";

#else

String CC_cc = "/bin/cc";
String CC_as = "/bin/as";
String CC_ld = "/bin/cc";

String CC_mv = "/bin/mv";
String CC_cp = "/bin/cp";

#endif

String CC_grep = "grep";

#define ARGSIZE		2048
#define FILENAMESIZE	256
#define MAXARGS		1024
#define MAXTMPFILES	64
#define PATHLEN		1024
#define SET		true

boolean Cflag = false;
boolean Eflag = false;
boolean Fflag = false;
boolean FcFlag = false;
boolean plusIflag = false;
boolean suffixFlag = false;
boolean verboseFlag = false;
boolean fakeFlag = false;
boolean objFlag = false;
boolean ignErrFlag = false;
boolean skipAsmFlag = false;
boolean cFlag = false;
boolean noMunchFlag = false;
boolean gprofFlag = false;
boolean profFlag = false;
boolean genRelocInfoFlag = false;
boolean keepctdtFlag = false;

String cppArgs[MAXARGS];
StringVec cppArgv = cppArgs;

String compilerArgs[MAXARGS];
StringVec compilerArgv = compilerArgs;

String frontEndArgs[MAXARGS];
StringVec frontEndArgv = frontEndArgs;

String ccArgs[MAXARGS];
StringVec ccArgv = &ccArgs[0];

String asmArgs[MAXARGS];
StringVec asmArgv = &asmArgs[0];

String linkArgs[MAXARGS];
StringVec linkArgv = linkArgs;

String linkFlagList[MAXARGS];
StringVec linkFlags = linkFlagList;

String cppPathArgs[MAXARGS];
StringVec cppPathv = cppPathArgs;

String commandv[MAXARGS];
StringVec command;

String objSpaceArgs[MAXARGS];
StringVec objSpaceArgv = &objSpaceArgs[0];

char tmpFilesList[MAXTMPFILES][FILENAMESIZE];
char xBuff[FILENAMESIZE];		/* for +x file flag */
int tmpFileCount = 0;

extern String getenv();

char fileName[FILENAMESIZE], filterName[FILENAMESIZE];
char cppName[FILENAMESIZE], frontEndName[FILENAMESIZE];
char destname[FILENAMESIZE];
char objName[FILENAMESIZE];
char ccPath[PATHLEN];
String myname, linkFile;
String suffix;

int cppDesc, dest;
int numErrors = 0;

void Error (msg1, msg2)
String msg1, msg2;
{
    if (msg2 == nil || *msg2 == '\0') {
	fprintf(stderr, "%s : %s\n", myname, msg1);
    } else {
	fprintf(stderr, "%s : %s %s\n", myname, msg1, msg2);
    }
    exit(1);
}

void Warning (msg1, msg2)
String msg1, msg2;
{
    if (msg2 == nil || *msg2 == '\0') {
	fprintf(stderr, "%s : %s\n", myname, msg1);
    } else {
	fprintf(stderr, "%s : %s %s\n", myname, msg1, msg2);
    }
}

void Remove (file)
String file; 
{
    if (verboseFlag) {
	fprintf(stderr, "rm %s\n", file);
    }
    if (!fakeFlag && file != nil && *file != '\0') {
	unlink(file);
    }
}

void RemoveTmpFiles ()
{
    int i;
    for (i=0; i<tmpFileCount; i++) {
	Remove(tmpFilesList[i]);
	tmpFilesList[i][0] = '\0';
    }
}

void Quit (exitStatus)
int exitStatus;
{
    RemoveTmpFiles();
    exit(exitStatus);
}

void myclose (desc)
FileDesc desc;
{
    if (!fakeFlag && close(desc) != 0) {
	perror("oh darnit.");
	fprintf(stderr, "Close failed: %d\n", desc);
    }
}

FileDesc myopen (filename, flags, mode)
String filename;
int flags, mode;
{
    int f;
    if (!fakeFlag) {
	f = open(filename, flags, mode);
	if (f < 0) {
	    fprintf(stderr, "(%s)  ", filename);
	    perror("CC : Open failed - internal error");
	    Quit(1);
	}
    } else
	f = -1;
    return f;
}

StringVec AddArg (l, s)
StringVec l;
String s;
{
    if (s != 0 && *s != '\0') {
	*l++ = s;
    }
    return l;
}

StringVec AddList (v, list)
StringVec v;
StringVec list;
{
    while (*list != nil) {
	*v++ = *list++;
    }
    return v;
}

String newString (s)
String s;
{
     String p;
     p = (char*) malloc(FILENAMESIZE);
     strcpy(p, s);
     return p;
}

void RegisterTmpFile (filename)
String filename;
{
    int i;

    if (tmpFileCount >= MAXTMPFILES) {
	for (i = 0; i<MAXTMPFILES ; i++) {
	    unlink(tmpFilesList[i]);
	    tmpFilesList[i][0] = '\0';
	}
	tmpFileCount = 0;
    }
    strcpy(tmpFilesList[tmpFileCount], filename);
    tmpFileCount++;
}

FileDesc MakeTmpFile (tmpname, characteristic)
String tmpname;
String characteristic;
{
    int t;
    sprintf(tmpname, "/tmp/C++%d%s", getpid(), characteristic);
    if (!fakeFlag) {
	t = open(tmpname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (t < 1) {
	    perror("Can't open /tmp file");
	} else {
	    RegisterTmpFile(tmpname);
	}
    } else {
	t = fileno(stdout);
    }
    return t;
}

void PrintArgList (prog, argv)
String prog;
StringVec argv;
{
    int i;
    
    fprintf(stderr, "%s: ", prog);
    for (i=0; argv[i] != 0; i++) {
	fprintf(stderr, "%s ", argv[i]);
    }
    fprintf(stderr,"\n");
}

void initexec (progname)
String progname;
{
    command = commandv;
    *command++ = progname;
}

void RedirectIn (in)
FileDesc in;
{
    if (in != fileno(stdin)) {
	dup2(in, fileno(stdin));
	close(in);
    }
}

void RedirectOut (out)
FileDesc out;
{
    if (out != fileno(stdout)) {
	dup2(out, fileno(stdout));
	close(out);
    }
}

/* return pid of child */
int NoWaitExec (name, argv, in, out)
String name;
StringVec argv;
FileDesc in;
FileDesc out;
{
    int pid;
    
    pid = fork();
    if (pid == -1) {
	fprintf(stderr, "%s: can't fork %s\n", myname, name);
	_exit(-1);
    }
    if (IsChild(pid)) {
	RedirectIn(in);
	RedirectOut(out);
	execvp(name, argv);
	fprintf(stderr, "%s: can't execute %s\n", myname, name);
	_exit(BADEXEC);
    }
    return pid;
}

union wait mywait (pid)
int pid;
{
    union wait status;

    while (wait(&status) != pid);
    return status;
}

union wait myexec(name, argv, in, out)
String name;
StringVec argv;
FileDesc in;
FileDesc out;
{
    int pid;

    pid = NoWaitExec(name, argv, in, out);
    return mywait(pid);
}

/* 
 * if 0 is passed as in or out, use the defaults, stdin and stdout. 
 */
int execute (progfile, argv, in, out) 
String progfile;
StringVec argv;
FileDesc in, out;
{
    union wait status;

    *command++ = nil;
    if (verboseFlag) {
	PrintArgList(progfile, commandv);
    }
    if (fakeFlag) {
	return 0;
    }
    status = myexec(progfile, commandv, in, out);
    if (status.w_T.w_Termsig != 0) {
	fprintf(
	    stderr, "%s: %s terminated with signal %d\n",
	    myname, progfile, status.w_T.w_Termsig
	);
	return status.w_T.w_Termsig;
    }
    if (status.w_T.w_Retcode != 0) {
	numErrors++;
    }
    if (verboseFlag) {
	fprintf(
	    stderr, "%s: %s exited with %d\n",
	    myname, progfile, status.w_T.w_Retcode
	);
    }
    return status.w_T.w_Retcode;
}

void NoMoreOpt (opt, arg)
char opt;
String arg;
{
    if (opt != '\0') {
	fprintf(stderr,
	    "%s: %c not understood in option %s\n", myname, opt, arg
	);
	Quit(1);
    }
}

/* 
 * dir, tail, and ext are out parameters
 */
char DetermineFileType (filename, dir, tail, ext)
String filename;
String *dir;
String *tail;
String *ext;		/* var parameters */
{
    String p;
    char kind;

    *dir = nil;				/* defaults */
    *ext = nil;
    *tail = fileName;
    
    p = strlen(filename) + filename;
    while (p != fileName && *tail == fileName) {
	if (*p == '.' && *ext == nil) {
	    *ext = p;
	} else if (*p == '/' && *tail == fileName) {
	    *tail = p + 1;
	    *p = '\0';
	    *dir = fileName;
	}
	p--;
    }

    /* determine kind of file it is */
    if (*ext == nil) {
	/* assume lib file */
	kind = 'a';
    } else {
	kind = *(*ext + 1);
	if ((*(*ext + 2) != '\0' || strchr("cCsoa", kind) == nil) &&
	    strcmp(*ext, ".cc") != 0
	) {
	    kind = '?';
	}
    }
    return kind;
}

void SetUpHandler (sig, handler)
int sig;
void (*handler)();
{
    void (*signalHandler)();		/* pointer to integer function */

    signalHandler = signal(sig, handler);
    if (signalHandler == SIG_IGN) {
	(void) signal(sig, SIG_IGN);
    } else if ((int)signalHandler == -1) {
	perror("Couldn't set up signal handler");
    }
}

void interrupt ()
{
    RemoveTmpFiles();
    exit(3);
}

void HandleArgs (argc, argv)
int argc;
StringVec argv;
{
#define opt(s) (strcmp(argv[i], s) == 0)

    register int i;
    for (i = 1; i < argc; i++) {
	if (argv[i][0] == '-') {
	    if (opt("-c")) {
		cFlag = SET;
	    } else if (opt("-S")) {
		skipAsmFlag = SET;
		ccArgv = AddArg(ccArgv, argv[i]);
	    } else if (
		argv[i][1] == 'D' || argv[i][1] == 'I' || argv[i][1] == 'U'
	    ) {
		cppArgv = AddArg(cppArgv, argv[i]);
	    } else if (opt("-CC_cfront1.2")) {
		CC_translator = "cfront";
		CC_gnu = false;
		CC_version2 = false;
		CC_library = "-lC";
		CC_munch = "munch";
		CC_munch_relink = 1;
	    } else if (opt("-CC_cfront2.0")) {
		CC_translator = "cfront";
		CC_gnu = false;
		CC_version2 = true;
		CC_library = "-lC";
		CC_munch = "munch";
		CC_munch_relink = 0;
	    } else if (opt("-CC_g++")) {
		CC_compiler = "cc1plus";
		CC_gnu = true;
		CC_version2 = true;
		CC_library = "-lgnulib";
		CC_translator = nil;
		CC_munch = nil;
	    } else if (argv[i][1] == 'C' && argv[i][2] == 'C') {
		char* v = argv[i+1];
		if (v == nil) {
		    Warning("expected value for %s", &argv[i][1]);
		} else {
		    if (strcmp(v, "nil") == 0) {
			v = nil;
		    }
		    if (opt("-CC_cpp")) {
			CC_cpp = v;
		    } else if (opt("-CC_include")) {
			CC_include = v;
		    } else if (opt("-CC_compiler")) {
			CC_compiler = v;
			CC_translator = nil;
			CC_munch = nil;
		    } else if (opt("-CC_translator")) {
			CC_translator = v;
			CC_library = "-lC";
		    } else if (opt("-CC_library")) {
			CC_library = v;
		    } else if (opt("-CC_cc")) {
			CC_cc = v;
		    } else if (opt("-CC_as")) {
			CC_as = v;
		    } else if (opt("-CC_ld")) {
			CC_ld = v;
		    } else if (opt("-CC_munch")) {
			CC_munch = v;
		    } else if (opt("-CC_mv")) {
			CC_mv = v;
		    } else if (opt("-CC_cp")) {
			CC_cp = v;
		    } else if (opt("-CC_grep")) {
			CC_grep = v;
		    }
		    ++i;
		}
	    } else if (opt("-C")) {
		frontEndArgv = AddArg(frontEndArgv, argv[i]);
	    } else if (opt("-v")) {
		verboseFlag = SET;
	    } else if (opt("-n")) {
		verboseFlag = SET;
		fakeFlag = SET;
	    } else if (opt("-o")) {
		if (argv[i] == nil) {
		    Warning("No object specified after -o, using a.out", nil);
		} else {
		    objFlag = SET;
		    linkArgv = AddArg(linkArgv, argv[i]);
		    ++i;
		    linkArgv = AddArg(linkArgv, argv[i]);
		    strcpy(objName, argv[i]);
		}
	    } else if (opt("-G")) {
		if (argv[i+1] == nil) {
		    Warning("No size specified after -G, ignoring", nil);
		} else {
		    ccArgv = AddArg(ccArgv, argv[i]);
		    asmArgv = AddArg(asmArgv, argv[i]);
		    ++i;
		    ccArgv = AddArg(ccArgv, argv[i]);
		    asmArgv = AddArg(asmArgv, argv[i]);
		}
	    } else if (opt("-p")) {
		profFlag = SET;
		gprofFlag = false;
		ccArgv = AddArg(ccArgv, argv[i]);
	    } else if (opt("-pg")) {
		gprofFlag = SET;
		profFlag = false;
		ccArgv = AddArg(ccArgv, argv[i]);
	    } else if (opt("-d")) {
		ccArgv = AddArg(ccArgv, argv[i]);
	    } else if (opt("-E")) {
		Eflag = SET;
	    } else if (opt("-F")) {
		Fflag = SET;
	    } else if (opt("-Fc")) {
		Fflag = SET;
		FcFlag = SET;
	    } else if (opt("-i")) {
		ignErrFlag = SET;
	    } else if (opt("-k")) {
		keepctdtFlag = SET;
	    } else if (argv[i][1] == 'l') {
		linkArgv = AddArg(linkArgv, argv[i]);
	    } else if (opt("-N")) {
		ccArgv = AddArg(ccArgv, argv[i]);
	    } else if (opt("-NOMUNCH")) {
		noMunchFlag = SET;
	    } else if (opt("-r")) {
		genRelocInfoFlag = SET;
		ccArgv = AddArg(ccArgv, argv[i]);
	    } else if (opt("-inclinkCC")) {
		ccArgv = AddArg(ccArgv, argv[i]);
	    } else if (argv[i][1] == '.') {
		if (suffixFlag) {
		    Error("not allowed to specify more than suffix", nil);
		}
		suffixFlag = SET;
		suffix = &argv[i][1];
	    } else {
		ccArgv = AddArg(ccArgv, argv[i]);
		compilerArgv = AddArg(compilerArgv, argv[i]);
	    }
	} else if (argv[i][0] == '+') {
	    if (opt("+d")) {
		frontEndArgv = AddArg(frontEndArgv, argv[i]);
	    } else if (argv[i][1] == 'e') {
		frontEndArgv = AddArg(frontEndArgv, argv[i]);
	    } else if (opt("+V")) {
		CC_include = "-I/usr/include";
	    } else if (opt("+L")) {
		/* default */
	    } else if (argv[i][1] == 'x') {
		if (argv[i][2] != '\0') {
		    frontEndArgv = AddArg(frontEndArgv, argv[i]);
		} else {
		    i++;
		    if (argv[i] == '\0') {
			Error("No arg specified after +x", nil);
		    } else {
			strcpy(xBuff, "+x");
			strcat(xBuff, argv[i]);
			frontEndArgv = AddArg(frontEndArgv, xBuff);
		    }
		}
	    } else if (opt("+S")) {
		frontEndArgv = AddArg(frontEndArgv, argv[i]);
	    } else if (opt("+I")) {
		plusIflag = SET;
	    } else {
		Warning("Unknown flag", argv[i]);
	    }
	} else if (DoFile(argv[i]) != 0) {
	    Quit(1);
	}
    }
}

void Init ()
{
    int argc;
    char* argv[MAXARGS];
    char* p;

    p = getenv("CC_args");
    if (p != nil) {
	argc = 1;
	for (; isspace(*p); p++);
	while (*p != '\0') {
	    argv[argc++] = p;
	    for (; *p != '\0'; p++) {
		if (isspace(*p)) {
		    *p++ = '\0';
		    break;
		}
	    }
	}
	argv[argc] = nil;
	if (argc > 0) {
	    HandleArgs(argc, argv);
	}
    }
}

int main (argc, argv)
int argc;
StringVec argv;
{
    int i;
	
    myname = argv[0];
    if (argc <= 1) {
	fprintf(stderr,"usage: %s [options] [ file.{c,o,s} ] \n", myname);
	exit(-1);
    }
    SetUpHandler(SIGINT, interrupt);
    SetUpHandler(SIGQUIT, interrupt);
    SetUpHandler(SIGHUP, interrupt);
    Init();
    HandleArgs(argc, argv);
    if (!objFlag) {
	strcpy(objName, "a.out");
    }
    if (numErrors != 0) {
	Quit(2);
    }
    if (Eflag || Fflag || FcFlag || skipAsmFlag) {
        Quit(0);
    }
    if (cFlag) {
        Quit(0);
    }
    Quit(LinkPhase());
}

int Move (filename, oldsuffix, newsuffix)
String filename, oldsuffix, newsuffix;
{
    char aBuffer[FILENAMESIZE], aBuffer2[FILENAMESIZE];

    initexec("mv");
    strcpy(aBuffer, filename);
    strcat(aBuffer, oldsuffix);
    command = AddArg(command, aBuffer);
    strcpy(aBuffer2, filename);
    strcat(aBuffer2, newsuffix);
    command = AddArg(command, aBuffer2);
    return execute(CC_mv, commandv, fileno(stdin), fileno(stdout));
}

int DoFile (fullPathName)
String fullPathName;
{
    String ext, dir, fileTail;
    char fileType;

    linkFile = nil;
    strcpy(fileName, fullPathName);
    fileType = DetermineFileType(fileName, &dir, &fileTail, &ext);
    if (fileType == 'c' || fileType == 's' || 
      fileType == 'C' || fileType == 'i') {
	*(ext + 1) = 'o';		/* change xxx.c to xxx.o */
	linkFile = newString(fileTail);
	if ((fileType == 'c' || fileType == 'C') && !cFlag) {
	    RegisterTmpFile(linkFile);
	}
    } else if (fileType == 'o') {
	linkFile = newString(fullPathName);
    } else if (fileType == 'a' || fileType == 'i' || fileType == '?') {
	linkFile = newString(fullPathName);
    }
    linkArgv = AddArg(linkArgv, linkFile);
    if (ext != nil) {
	*ext = '\0';
    }
    if (fileType == 'o' || fileType == 'a' || fileType == '?') {
	return 0;
    }
    if (fileType == 'c' || fileType == 'C' || fileType == 'i') {
	int status = CPPphase(fullPathName, fileTail);
	if (Eflag || status != 0) {
	    return status;
	}
	cppDesc = myopen(cppName, O_RDONLY, 0644);
	status = Compile(fullPathName, fileTail, cppDesc);
	myclose(cppDesc);
	return status;
    } else if (fileType == 's' && !skipAsmFlag) {
	return Asm(fullPathName, fileTail);
    }
    return 0;
}

int LinkPhase () {
    int status;

    status = PassOne();
    if (status == 0 && CC_munch != nil) {
	status = PassTwo();
    }
    return status;
}

int PassOne ()
{
    initexec(CC_ld);
    command = AddList(command, ccArgs);
    command = AddList(command, linkArgs);
    return DoLink();
}

int DoLink ()
{
    if (CC_library != nil) {
	command = AddArg(command, CC_library);
    }
    return execute(CC_ld, commandv, fileno(stdin), fileno(stdout));
}

/*
 * If we are using munch, then run nm on the a.out and look
 * for static constructor functions.
 */

int PassTwo () {
    int status;
    int nmDesc, munchDesc;
    char nmName[FILENAMESIZE];

    if (!genRelocInfoFlag && !noMunchFlag) {
	nmDesc = MakeTmpFile(nmName, "nm");
	initexec("nm");
	command = AddArg(command, objName);
	status = execute("nm", commandv, fileno(stdin), nmDesc);
	if (status != 0) {
	    return status;
	}
	close(nmDesc);	
	close(fileno(stdout));
	nmDesc = myopen(nmName, O_RDONLY, 0644);
	munchDesc = myopen("__ctdt.c", O_WRONLY | O_CREAT | O_TRUNC, 0644); 
	if (!keepctdtFlag) {
	    RegisterTmpFile("__ctdt.c");    
	}

	initexec("munch");
	if (profFlag) {
	    command = AddArg(command, "-p");
	} else if (gprofFlag) {
            command = AddArg(command, "-pg");
        }
	status = execute(CC_munch, commandv, nmDesc, munchDesc);
	myclose(nmDesc);
	myclose(munchDesc);

	if (status == CC_munch_relink) {
	    status = RecompileWithMunchPhase();
	    if (status != 0) {
		return status;
	    }
	}
    }
    return 0;
}

int CPPphase (fullPathName, tail)
String fullPathName;
String tail;
{
    int status;

    initexec(CC_cpp);
#if !defined(hpux)
    command = AddArg(command, "-B");
#endif
    command = AddList(command, cppArgs);
    command = AddArg(command, "-Dc_plusplus=1");
    if (CC_version2) {
	command = AddArg(command, "-D__cplusplus=1");
    }
    if (CC_gnu) {
	command = AddArg(command, "-D__GNUG__=1");
    }
#if defined(mips)
    command = AddArg(command, "-DLANGUAGE_C");
#endif
    if (CC_include != nil) {
	command = AddArg(command, CC_include);
    }
    command = AddList(command, cppPathArgs);
    command = AddArg(command, fullPathName);
    if (Eflag) {
	if (suffixFlag) {
	    strcpy(destname, tail);
	    strcat(destname, suffix);
	    dest = myopen(
		destname, O_WRONLY | O_CREAT | O_TRUNC, 0644
	    );
	    status = execute(CC_cpp, commandv, fileno(stdin), dest);
	    myclose(dest);
	} else {
	    dest = fileno(stdout);
	    status = execute(CC_cpp, commandv, fileno(stdin), dest);
	}
    } else {
	cppDesc = MakeTmpFile(cppName, "cpp");
	status = execute(CC_cpp, commandv, fileno(stdin), cppDesc);
	myclose(cppDesc);
    }
    return status;
}

int Compile (fullPathName, tail, in)
String fullPathName;
String tail;
FileDesc in;
{
    if (CC_translator == nil) {
	return Native(fullPathName, tail, in);
    } else {
	return Translator(fullPathName, tail, in);
    }
}

int Native (fullPathName, tail, in)
String fullPathName, tail;
FileDesc in;
{
    int status;
    char asmfilename[FILENAMESIZE];
    int asmfd;
    
    initexec(CC_compiler);
    command = AddList(command, compilerArgs);
    command = AddArg(command, "-quiet");
    if (skipAsmFlag) {
	sprintf(asmfilename, "%s.s", tail);
	asmfd = myopen(asmfilename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    } else {
	asmfd = MakeTmpFile(asmfilename, "as");
    }
    status = execute(CC_compiler, commandv, in, asmfd);
    myclose(asmfd);
    if (!skipAsmFlag) {
	if (status == 0) {
	    status = Asm(asmfilename, tail);
	}
	Remove(asmfilename);
    }
    return status;
}

int Translator (fullPathName, tail, in)
String fullPathName, tail;
FileDesc in;
{
    int status;
    int frontEndDesc;
    char aBuffer[FILENAMESIZE];

    status = Cfront(fullPathName, tail, in);
    if (status != 0) {
	return status;
    }
    if (plusIflag && (!Fflag || suffixFlag)) {
	status = FilterPhase(frontEndName, tail);
	if (status != 0) {
	    return status;
	}
    }
    if (Fflag) {
	return status;
    }

    initexec(CC_cc);
    command = AddList(command, ccArgs);
    command = AddArg(command, "-c");
    command = AddArg(command, frontEndName);
    status = execute(CC_cc, commandv, fileno(stdin), fileno(stdout));
    if (status != 0) {
	Remove(linkFile);
	return status;
    }
    if (!plusIflag) {
	Remove(frontEndName);
    }
    if (skipAsmFlag) {
	return Move(tail, "..s", ".s");
    } else {
	return Move(tail, "..o", ".o");
    }
}

int Cfront (fullPathName, tail, in)
String fullPathName, tail;
FileDesc in;
{
    int status;
    int frontEndDesc;
    char nameflag[FILENAMESIZE];

    initexec(CC_translator);
    command = AddList(command, frontEndArgs);
    command = AddArg(command, "+L");
    sprintf(nameflag, "+f%s", fullPathName);
    command = AddArg(command, nameflag);
    if (Fflag) {
	if (suffixFlag) {
	    sprintf(frontEndName, "%s%s", tail, suffix);
	    dest = myopen(
		frontEndName, O_WRONLY | O_CREAT | O_TRUNC, 0644
	    );
	    status = execute(CC_translator, commandv, in, dest);
	    myclose(dest);
	} else {
	    status = execute(CC_translator, commandv, in, fileno(stdout));
	}
    } else {
	sprintf(frontEndName, "%s..c", tail);
	frontEndDesc = myopen(
	    frontEndName, O_WRONLY | O_CREAT | O_TRUNC, 0644
	);
	status = execute(CC_translator, commandv, in, frontEndDesc);
	myclose(frontEndDesc);
    }
    return status;
}

int FilterPhase (fullPathName, tail)
String fullPathName;
String tail;
{
    int status;
    int filterDesc;

    if (plusIflag) {
	filterDesc = MakeTmpFile(filterName, "grep");
	initexec("grep");
	command = AddArg(command, "-v");
	command = AddArg(command, "^#");
	command = AddArg(command, fullPathName);
	status = execute(CC_grep, commandv, fileno(stdin), filterDesc);
	if (status != 0) {
	    Warning("+I option error {grep}", nil);
	} else {
	    myclose(filterDesc);
	    initexec("mv");
	    command = AddArg(command, filterName);
	    command = AddArg(command, frontEndName);
	    status = execute(CC_mv, commandv, fileno(stdin), fileno(stdout));
	    if (status != 0) {
		perror(status);
	    }
	}
    }
    return status;
}

int Asm (fullPathName, tail)
String fullPathName, tail;
{
    char objfile[FILENAMESIZE];

    initexec("as");
    command = AddList(command, asmArgs);
    command = AddArg(command, "-o");
    sprintf(objfile, "%s.o", tail);
    command = AddArg(command, objfile);
    command = AddArg(command, fullPathName);
    return execute(CC_as, commandv, fileno(stdin), fileno(stdout));
}

/*
 * Compile ctdt and re-link.
 */

int RecompileWithMunchPhase ()
{
    int devnull;

    initexec(CC_cc);
    command = AddArg(command, "-c");
    command = AddArg(command, "__ctdt.c");
    if (!keepctdtFlag) {
	RegisterTmpFile("__ctdt.o");
    }
    devnull = myopen("/dev/null", O_WRONLY, 666);
    if (devnull == -1) {
	devnull = fileno(stdout);
    }
    execute(CC_cc, commandv, fileno(stdin), devnull);
    if (devnull != fileno(stdout)) {
	myclose(devnull);
    }
    
    initexec(CC_ld);
    command = AddList(command, ccArgs);
    command = AddArg(command, "__ctdt.o");
    command = AddList(command, linkArgs);
    return DoLink();
}
