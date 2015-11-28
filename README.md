Origin:  https://github.com/hilbix/tinolib/

# How to fork and use the fork in case of a git submodule?

You do not need to edit `.gitmodules`.  Instead, please get used to the `insteadOf` rule in `git-config`.

```
git config --global url.https://github.com/YOURGITHUBUSERHERE/tinolib.git.insteadOf https://github.com/hilbix/tinolib.git
```

- You can use any URL for `https://github.com/YOURGITHUBUSERHERE/tinolib.git` like `git@git.example.com:repository.git`.  Yes, that's the power of `git`!
- Note that you must change `https://github.com/hilbix/tinolib.git` to what is found in `.gitmodules`.


# Is `tinolib` meant to be portable?

Yes and no.  Instead this here is a very big and ugly mess.  ;)

I'm sorry, but this comes from the fact that I do not like `autoconf` and `./configure`.  I want the main code to be free from system dependent code.  All the system dependencies shall be handled in this library.  It is shared by many of my tools and most things that I write in C.  And it shall work across everything I have.  All I want to do to checking it out with `git submodule add` and I am ready to go.

Hence, this library is meant to be portable to things I come across.  This is mainly Linux and sometimes CygWin.  I do not own OS-X (yet).

# If you have trouble compiling something which uses this

- Be sure to have the GNU toolchain (`awk`, `sed`, etc.) and a Unix like environment.
- If you have system include problems, have a look into `sysfix.h`.  Try to add stick to some thing like `sysfix_linux.h`, `sysfix_cygwin.h` and `sysfix_diet.h` to adapt your system.
- I recently changed `sysfix.h` to use the standard names again and moved all the Linux stuff into `sysfix_linux.h`.  This was in preparation to `sysfix_osx.h`.
- I also added a stub `sysfix_osx.h`.  However I do not have OS-X, so please fix the conditional 


# Why is it organized this way?

`tinolib` is meant to be C89 and POSIX compliant.  However more and
more C99 features will be utilized, but these features shall be kept
optional if possible.  If you find something which is not in
compliance you probably found a bug or something I am not aware of.
In this case, please inform me on GitHub (https://github.com/hilbix/).

To be C89 compliant with long names tinolib has internal linkage.
Therefor it is based on includes and nearly everything is kept static.
A normal program with tinolib is one single file which is pulled
together through the C-Preprocessor.  However Makefile.tino knows a
little bit about dependencies, too, such that you can compile and link
serveral object files, too.  But in this case you often loose C89
compatibility as the link might work on your system but not somewhere
else (routine1 and routine2 unter C89 both define the object ROUTIN).

tinolib shall be modular and optional.  That is, you do not need all
the contents.  However currently a proper helper is missing to make
programs independently of tinolib.  Note that developing/using tinolib
needs a huge environment while compiling the result (after make dist)
shall work on all minimal environments.

When it comes to additional external resources which need downloads,
configure, automake you probably need a full Linux environment (like
CygWin).


# Naming convention

I now start to change the lib to comply to C99 for external names,
that is a NEW NAMEING CONVENTION for names up to 31 characters.

- `tino_module_function_args` schema is retained, however function names
  must not exceed 25 characters this way.

- `functionA()`	Automatic internal error handling (via `tino_err`).
- `functionE()`	Error: returns errors but EINTR handled internally
- `functionI()`	Interruptable: As before, but returns on EINTR
- `functionO()`	always OK Routine, cannot fail (like returning void)
- `functionN()`	always OK Routine, but can return NULL or negative
- `functionU()`	Usage not recommended: No more use this one!
- `function()`	Old version, unspecified, deprecated

Also after the capitalized letter there can be lower case letters for:
- `functionXb()`	Bad sideffects
- `functionXi()`	Marker that function has changed a lot.  Multiple ok!
- `functionXl()`	Internal library routine, do not use outside!
- `functionXn()`	Non threadsafe or non reentrant
- `functionXp()`	Printf() style arguments
- `functionXs()`	Sideffect warning

Multiple lowercase letters are sorted ABC.

- Type `s` functions shall not be called more than once at a time, as
  they might return internal static buffers.

- Type `p` functions take %v as a varargs parameter.  This option has no
  sideffect on varargs parameter.  That's why a `v` type can be missing.

In case you wonder: `tinolib` isn't fixed, so such a rename is possible.
I think, this naming convention will hold in future, but `tinolib` is
not a set of fixed work like a standard library which upholds some old
compatibility just to be compatible.  It is thought to make life
easier.  So from time to time you might need to rename functions or
change some things you conform to a standard way which previously
needed manual crafting.  This also is why `tinolib` is always part of
all distributions and not independent of it.  Also I need not to obey
to this convention for any reason.


# How to use (small intro)

You have following:

- Linux
- A copy of `tinolib` at `/path/to/library/tino/`
- A source file `!NAME!.c` or `!NAME!.cc`
  where `!NAME!` is `basename "$PWD"`
- Now to start from fresh, just do:
	`/path/to/library/tino/setuptino.sh`
  this will create the initial `Makefile`.
- To change the parameters etc. just do
	`vi Makefile.tino`
- If you need to recreate the `Makefile`, just do
	`make -f Makefile.tino`
  However, most times the `Makefile` will update itself from
  `Makefile.tino` automagically.
- It's quirky, sometimes make must be called several times until
  things settle.  If not, don't panic, the first `make` was ok!

Note that the basic functions of the `Makefile` then can live without
the subdirectory `tino/` - some errors will show up, but everything
still compiles if the code does not include from the library.


# What's the dark matter with all this here?

I want a hassle free method of keeping everything updated for me.  I
want `make` to make it all, so no `./configure; make`.  `make` must be
enough, as `make` already is insane (ask non-programmers how to
compile software, they will not tell you "to use make", so `make` is
insane and `configure` is even more insane).  I want to be able to
build everything without GNU extended tools.  This does not mean to not
use GNU extensions at all.  However it shall be easy to comment them
out and still have a successful build.  I want it easy.

Additionally a minimum of standard Unix utilities shall be needed to
run this all.  Note that I already think it needs too many of them.
However `make all` shall still do, I hope.

A third thing is, to be able to concentrate on the source, not on
incompatibilities.  So the library shall handle it all.  And moreover
it shall conform to the C89 standard, such that you can expect that
you are able to compile on any system for which the source is anyhow
meaningful.


# Prerequisites

`make all` in a distribution currently needs:

1) `make`  (not neccessarily GNU make)
2) `gawk`  (you need GNU awk, sorry)
3) `touch` (could not circumvent this, sorry)
4a) An C89 compliant ANSI-C compiler which make knows about.
4b) Dito an ANSI-CC compiler, in case of C++
5) The proper set of includes and libraries (possibly all GNU).

Nothing else (not even a bash, perl etc.) shall be needed,
and not a compiler with support for -M or -MM.


# Thoughts

At least, the above is the idea behind it, even that I think I failed
completely to make it usable for others.
However the basic needs are hopefully relatively moderate.

My long term goal for this here is (not joking): All you need is a
suitable ANSI-C compiler with libraries and such to compile my
distributions.  For this there perhaps will be a little
`Makefile.tino.run.c` which can be compiled to a program which calls the
compiler for you (if you find a way to even get rid of the compiler, I
would be glad.  Well, ok, that was a joke now *eg*).

However, to use all features of `tinolib`, you probably need a huge
Linux environment and some undefined and unclear setups (like `CVS`) in
place.  But still it's the idea for the distribution to have an easy
`make all` process.  And that the distribution can be independent of
`tinolib` if the sources do not depend on this library.

All portability issues shall be handled in `tinolib` in future, such
that you do not need a single `#if` in your source except for compile
time features.  You get the idea, that's what I want.

Well, I'm far away from my goal, but it's evolving in slow motion.


# License

Most of the files are still still GPL v2.
However I want to change most to my own license which is called CLL, which is very similar to Public Domain.

Each time I am on it and find the time, I will drop GPL in favor of CLL, as the CLL is much more .

This `README.md` and some of the library files are:

This Works is placed under the terms of the Copyright Less License,
see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.

