/* ========= begin header generated by ./mkh ========= */
#ifdef __cplusplus
extern "C" {
#endif

/* === regcomp.c === */
static void p_ere(register struct parse *p, int stop);
static void p_ere_exp(register struct parse *p);
static void p_str(register struct parse *p);
static void p_bre(register struct parse *p, register int end1, register int end2);
static int p_simp_re(register struct parse *p, int starordinary);
static int p_count(register struct parse *p);
static void p_bracket(register struct parse *p);
static void p_b_term(register struct parse *p, register cset *cs);
static void p_b_cclass(register struct parse *p, register cset *cs);
static void p_b_eclass(register struct parse *p, register cset *cs);
static char p_b_symbol(register struct parse *p);
static char p_b_coll_elem(register struct parse *p, int endc);
static char othercase(int ch);
static void bothcases(register struct parse *p, int ch);
static void ordinary(register struct parse *p, register int ch);
static void nonnewline(register struct parse *p);
static void repeat(register struct parse *p, sopno start, int from, int to);
static int seterr(register struct parse *p, int e);
static cset *allocset(register struct parse *p);
static void freeset(register struct parse *p, register cset *cs);
static int freezeset(register struct parse *p, register cset *cs);
static int firstch(register struct parse *p, register cset *cs);
static int nch(register struct parse *p, register cset *cs);
static void mcadd(register struct parse *p, register cset *cs, register char *cp);
/* static void mcsub(register cset *cs, register char *cp);   */
/* static int mcin(register cset *cs, register char *cp);     */
/* static char *mcfind(register cset *cs, register char *cp); */
static void mcinvert(register struct parse *p, register cset *cs);
static void mccase(register struct parse *p, register cset *cs);
static int isinsets(register struct re_guts *g, int c);
static int samesets(register struct re_guts *g, int c1, int c2);
static void categorize(struct parse *p, register struct re_guts *g);
static sopno dupl(register struct parse *p, sopno start, sopno finish);
static void doemit(register struct parse *p, sop op, size_t opnd);
static void doinsert(register struct parse *p, sop op, size_t opnd, sopno pos);
static void dofwd(register struct parse *p, sopno pos, sop value);
static void enlarge(register struct parse *p, sopno size);
static void stripsnug(register struct parse *p, register struct re_guts *g);
static void findmust(register struct parse *p, register struct re_guts *g);
static sopno pluscount(register struct parse *p, register struct re_guts *g);

#ifdef __cplusplus
}
#endif
/* ========= end header generated by ./mkh ========= */
