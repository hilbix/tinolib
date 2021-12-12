/* See:
 * https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms
 * https://saadahmad.ca/cc-preprocessor-metaprogramming-lists-and-for_each/
 *
 * Test with:
 * cpp -D__A_TEST__ cpptricks.h
 *
 * Note that `cpp` compresses multiple spaces/TABs to a single SPC
 */

#ifdef	__A_TEST__
#undef	__A_TEST__
#define	__A_TEST__(A,B)	if (#A != A_STR(B)) fprintf(x=stderr, "%s:%d: %s should be %s\n", __FILE__, __LINE__, #A, A_STR(B));
#define	__A_test__(...)	skip++;
#define	__AxTEST__(...)	__VA_ARGS__
#include <stdio.h>
__AxTEST__(int main() { FILE *x=NULL; int skip=0; )
#else
#define	__A_TEST__(...)
#define	__A_test__(...)
#define	__AxTEST__(...)
#endif

#define	A_STR(X)	#X

__A_TEST__((	""		),(	A_STR()			))
__A_TEST__((	""		),(	A_STR(   )		))

#define	A_HEAD(X,...)	X
#define	A_TAIL(X,...)	__VA_ARGS__
#define	A_2ND(X,Y,...)	Y

__A_TEST__((			),(	A_HEAD(   )		))
__A_TEST__((	a		),(	A_HEAD( a )		))
__A_TEST__((	a b		),(	A_HEAD( a b )		))
__A_TEST__((	( a , b )	),(	A_HEAD( ( a , b ) )	))
__A_TEST__((	a		),(	A_HEAD( a , b )		))
__A_TEST__((	a		),(	A_HEAD( a , b , c )	))
__A_TEST__((			),(	A_TAIL(   )		))
__A_TEST__((			),(	A_TAIL( a )		))
__A_TEST__((	b		),(	A_TAIL( a , b )		))
__A_TEST__((	b , c		),(	A_TAIL( a , b , c )	))
__A_TEST__((	b , c		),(	A_TAIL( a , b , c )	))
__A_TEST__((	b , c		),(	A_TAIL( a , b , c )	))

#define	A_CAT(X,...)	X ## __VA_ARGS__
#define	A_X(...)	A_CAT(__VA_ARGS__)

__A_TEST__((	a		),(	A_CAT( a )			))
__A_TEST__((	ab		),(	A_CAT( a , b )			))
__A_TEST__((	ab , c , d	),(	A_CAT( a , b , c , d )		))
__A_TEST__((	ab () , c	),(	A_CAT( a , b () , c )		))
__A_TEST__((	b , c		),(	A_CAT(   , b , c )		))
__A_test__((	b , c		),(	A_CAT( a ,   , c )		))
__A_test__((	ab , c		),(	A_CAT( __AxTEST__ (a) , b , c )	))
__A_TEST__((	ab , c		),(	A_X( __AxTEST__ (a) , b , c )	))

#define	A_BNOT(X)	A_CAT(A_BNOT_,X)
#define	A_BNOT_0	1
#define	A_BNOT_1	0

__A_TEST__((	1		),(	A_BNOT( 0 )			))
__A_TEST__((	0		),(	A_BNOT( 1 )			))
__A_test__((	/*error*/	),(	A_BNOT( 2 )			))
__A_TEST__((	0		),(	A_BNOT( __AxTEST__ (1) )	))
__A_TEST__((	1		),(	A_BNOT( __AxTEST__ (0) )	))

#define	A_BOR(X)	A_CAT(A_BOR_,X)
#define	A_BOR_0(X)	X
#define	A_BOR_1(X)	1

__A_TEST__((	1		),(	A_X(A_BOR ( 1 ) ( x ))	))
__A_TEST__((	0		),(	A_BOR ( 0 ) ( 0 )	))
__A_test__((	/*error*/	),(	A_BOR ( 2)		))
__A_TEST__((	0		),(	A_BOR ( __AxTEST__ (1))	))
__A_TEST__((	1		),(	A_BOR ( __AxTEST__ (0))	))

#define	A_BAND(X)	A_CAT(A_BAND_,X)
#define	A_BAND_0(X)	0
#define	A_BAND_1(X)	X

#define	A_CHECK(...)	A_2ND(__VA_ARGS__,0,)
#define	A_PROBE(X)	X,1,



#define	A_NOT(X)	A_2ND(A_CAT(A_NOT_,X))

#define	A_BOOL(X)	A_BNOT(A_NOT(X))

#define	A_INC(X)	A_CAT(A_IN_,X)
#define	A_DEC(X)	A_CAT(A_DE_,X)

#define	A_IN_0	1
#define	A_IN_1	2
#define	A_IN_2	3
#define	A_IN_3	4
#define	A_IN_4	5
#define	A_IN_5	6
#define	A_IN_6	7
#define	A_IN_7	8
#define	A_IN_8	9
#define	A_IN_9	10
#define	A_IN_10	11
#define	A_IN_11	12
#define	A_IN_12	13
#define	A_IN_13	14
#define	A_IN_14	15
#define	A_IN_15	16
#define	A_IN_16	17
#define	A_IN_17	18
#define	A_IN_18	19
#define	A_IN_19	20
#define	A_IN_20 A_OVERFLOW

#define	A_DE_0	A_UNDERFLOW
#define	A_DE_1	0
#define	A_DE_2	1
#define	A_DE_3	2
#define	A_DE_4	3
#define	A_DE_5	4
#define	A_DE_6	5
#define	A_DE_7	6
#define	A_DE_8	7
#define	A_DE_9	8
#define	A_DE_10	9
#define	A_DE_11	10
#define	A_DE_12	11
#define	A_DE_13	12
#define	A_DE_14	13
#define	A_DE_15	14
#define	A_DE_16	15
#define	A_DE_17	16
#define	A_DE_18	17
#define	A_DE_19	18
#define	A_DE_20 19

#define	A_IF(X)		A_CAT(A_IF_,X)
#define	A_IF_1(...)	A_HEAD(__VA_ARGS__)
#define	A_IF_0(...)	A_TAIL(__VA_ARGS__)

#define	A_EMPTY()	TRY_EXTRACT_EXISTS(A_DEFER(A_HEAD)

__AxTEST__(if(skip)fprintf(stderr,"note: %d tests skipped because they are technically impossible\n",skip); return x ? 1 : 0; }) /*test end*/

