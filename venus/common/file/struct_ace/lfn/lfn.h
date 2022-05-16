#ifndef _BASE_LFN_H_INCLUDED
#define _BASE_LFN_H_INCLUDED

#define BASE_LFN_MAXLEN           320
#define BASE_LFN_DOUBLEMAXLEN     (2 * BASE_LFN_MAXLEN)

typedef char tLFN[BASE_LFN_MAXLEN];
typedef tLFN *pLFN;
typedef char tBASE_LFN_DBL[BASE_LFN_DOUBLEMAXLEN];
typedef tBASE_LFN_DBL *pBASE_LFN_DBL;


#endif
