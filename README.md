# C Async Lib

This lib reserves the uppercase letter `A` (as in Async) as the prefix.
The prefix can be prefixed by an unlimited number of underscores (`_`).
In particular following is reserved:

	A
	_A
	__A

This lib is based on following key concepts:

- asynchronous
- nonblocking
- stackless coroutines
- mostly reentrant (non-reentrant routines are prefixed with `AG`, `G` as in Global)

This lib is not meant to support multithreading.


## Programming model

Coroutines look very similar to normal routines.  However they are very different:

- You cannot use local variables the normal way.  (Local variables need a stack, but we are stackless.)
- You cannot pass parameters to the coroutine the normal way.  (Coroutines are stackless, hence cannot keep the state.)
- You cannot directly call other coroutines.  (Coroutines are stackless, so no call stack either.)
- You cannot directly access results of coroutines.  (Use return parameters instead.)

The coroutine paradigm is:

	ACO(name,type,parm...)
	{
	  VARs;	/* like: int i;	*/
	  Abegin();

	  /* Code
	   * AV(name) to access VAR "name"
	   * AP(parm) to access parm
	   * AG(glob) to access global
	   * ACALL(co, parm..) to call another coroutine from within coroutines
	   */

	  Aend();
	}

	Arun(name,parm,..);

## Implementation details

This library massively uses the C preprocessor to change how things are interpreted.

For example coroutines look like a C routine, but the local variables are stuffed into a `struct` instead.


## Helpers

- `ameta` implements the C preprocessor metaprogramming which is needed to implement coroutines etc.

