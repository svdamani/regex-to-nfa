#include "util.h"

NFA LL_prim(char**);
NFA LL_expr(char**);
NFA LL_term(char**);

/** Compile basic regular expression to NFA */
NFA LL_expr(char **statement) {
    NFA lhs, ret, rhs;
    char ch;
    for (lhs = LL_term(statement);; lhs = ret) {
        ch = **statement;
        if (isalnum(ch) || ch == '(') { /* expr term */
            rhs = LL_term(statement);
            ret = NFA_concat(&lhs, &rhs);
        }
        else if (ch == '|') {           /* expr | term */
            *statement += 1;            /* eat '|' */
            rhs = LL_term(statement);
            ret = NFA_alternate(&lhs, &rhs);
        }
        else return lhs;                /* term  */
    }
    return ret;   /* should never reach here */
}

/* term: term * term + primary */
NFA LL_term(char **statement) {
    NFA lhs = LL_prim(statement), ret;
    char ch = **statement;
    if (ch == '*') {            /* term * */
        ret = NFA_kleene(&lhs);
        *statement += 1;        /* eat the Kleene star */
    }
    else if (ch == '+') {       /* term + */
        ret = NFA_positive(&lhs);
        *statement += 1;        /* eat the positive closure */
    }
    else if (ch == '?') {       /* term ? */
        ret = NFA_optional(&lhs);
        *statement += 1;        /* eat the optional (question) mark */
    }
    else return lhs;             /* primary */
    return ret;
}

/* primary: ALNUM ( expr )    */
NFA LL_prim(char **statement) {
    NFA ret;
    char ch = **statement;
    if (isalnum(ch)) {          /* ALNUM */
        ret = NFA_atomic(ch);
        *statement += 1;        /* eat the character */
    }
    else if (ch == '(') {       /* ( expr ) */
        *statement += 1;        /* eat '(' */
        ret = LL_expr(statement);
        if (**statement != ')') {
            fprintf(stderr, "No matching ')' found\n");
            exit(-1);
        }
        *statement += 1;         /* eat ')' */
    }
    else {
        fprintf(stderr, "Unrecognized Character \"%c\"\n", ch);
        exit(-1);
    }
    return ret;
}

/* LL parser driver/interface */
NFA reg_to_NFA(const char *regexp) {
    char **cur = (char **)(&regexp);
    NFA nfa = LL_expr(cur);
    /* creating NFA for  regexp is just like assembling building blocks */
    if (**cur != '\0') {
        fprintf(stderr, "Unexpected Character \"%c\"\n", **cur);
        exit(-1);
    }
    return nfa;
}

/* Main Function */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s 'regexp'\n", argv[0]);
        exit(-1);
    }
    FILE *fp = fopen("\u03B5-nfa.dot", "w");
    if (fp == NULL) {
        perror("fopen");
        exit(-1);
    }
    printf("Regular Expression : '%s'\n", argv[1]);
    NFA nfa = reg_to_NFA(argv[1]);      /* parse regexp and generate NFA */
    NFA_graphviz(argv[1], &nfa, fp);    /* dump NFA as graphviz code */
    fclose(fp);
#if  0
    char str[100];
    printf("Enter a string : ");
    scanf("%s", str);
    if (string_match(&nfa, str)) printf("String accepted by \u03B5-NFA\n");
    else printf("String not accepted by \u03B5-NFA\n");
#endif 
    return 0;
}