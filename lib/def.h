/* MiniLib some definitions
 */

#define _LARGEFILE64_SOURCE

#define	LIKELY(X)	__builtin_expect(!!(X), 1)
#define	UNLIKELY(X)	__builtin_expect(!!(X), 0)

