/* MiniLib some definitions
 */

#define	LIKELY(X)	__builtin_expect(!!(X), 1)
#define	UNLIKELY(X)	__builtin_expect(!!(X), 0)

