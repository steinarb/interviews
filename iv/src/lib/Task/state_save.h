#ifdef mips
extern "C" {
    void sig_save(struct sigcontext *);
    void sig_restore(struct sigcontext *);
    void float_save(struct sigcontext *);
    void float_restore(struct sigcontext *);
}
#endif mips

#ifdef sparc
extern "C" {
    void sig_save(int *, char *, char *);
    void sig_restore(char *);
}
#endif sparc
