#include "num_to_str.h"
#include <stdlib.h>

char* nts_u64(unsigned long long v){
    char tmp[32]; int i=0;
    if (v==0){ char* s=malloc(2); s[0]='0'; s[1]='\0'; return s; }
    while(v){ tmp[i++] = (char)('0' + (v%10)); v/=10; }
    char* s = malloc((size_t)i+1);
    for(int k=0;k<i;k++) s[k]=tmp[i-1-k];
    s[i]='\0';
    return s;
}

char* nts_i64(long long x){
    if (x>=0) return nts_u64((unsigned long long)x);
    unsigned long long v = (unsigned long long)(-(x+1)) + 1ULL;
    char* u = nts_u64(v);
    size_t n=0; while(u[n]) n++;
    char* s = malloc(n+2);
    s[0]='-'; for(size_t i=0;i<n;i++) s[1+i]=u[i]; s[1+n]='\0';
    free(u); return s;
}

char* nts_bool(int b){
    const char* t = b ? "true" : "false";
    size_t n=0; while(t[n]) n++;
    char* s=malloc(n+1);
    for(size_t i=0;i<n;i++) s[i]=t[i];
    s[n]='\0';
    return s;
}

char* nts_char(unsigned char c){
    const char* e = NULL;
    if (c=='\n') e="\\n";
    else if (c=='\r') e="\\r";
    else if (c=='\t') e="\\t";
    else if (c=='\\') e="\\\\";
    else if (c=='\"') e="\\\"";
    if (e){
        size_t n=0; while(e[n]) n++;
        char* s=malloc(n+1);
        for(size_t i=0;i<n;i++) s[i]=e[i];
        s[n]='\0';
        return s;
    }
    char* s=malloc(2); s[0]=(char)c; s[1]='\0'; return s;
}

char* nts_f64(double x, int prec){
    if (prec < 0) prec = 0; if (prec > 9) prec = 9;
    int neg = (x < 0.0); if (neg) x = -x;
    unsigned long long ip = (unsigned long long)x;
    double frac = x - (double)ip;

    double scale = 1.0; for (int i=0;i<prec;i++) scale *= 10.0;
    unsigned long long fp = (unsigned long long)(frac*scale + 0.5);

    // enteros
    char* A = nts_u64(ip);

    // fracción con padding a prec dígitos
    char* B = NULL; size_t nb=0;
    if (prec>0){
        char* Bb = nts_u64(fp);
        size_t nb2=0; while(Bb[nb2]) nb2++;
        B = malloc((size_t)prec + 1);
        int z = prec - (int)nb2, k=0;
        for(; k<z; k++) B[k]='0';
        for(size_t t=0; t<nb2; t++,k++) B[k]=Bb[t];
        B[prec]='\0';
        nb = (size_t)prec;
        free(Bb);
    }

    size_t na=0; while(A[na]) na++;
    size_t tot = (neg?1:0) + na + (prec>0?1+nb:0);
    char* s = malloc(tot+1);
    size_t p=0;
    if (neg) s[p++]='-';
    for(size_t i=0;i<na;i++) s[p++]=A[i];
    if (prec>0){
        s[p++]='.';
        for(size_t i=0;i<nb;i++) s[p++]=B[i];
    }
    s[p]='\0';
    free(A); if (B) free(B);
    return s;
}
