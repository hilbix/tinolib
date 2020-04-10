> make3.8 bash3.2 C18(ISO/IEC 9899:2018) POSIX
>
> **This is PRE-ALPHA** for now

# MiniLib variant of tinolib

Just `make love`

## Usage

	cd
	git init workdir
	cd workdir

	git clone --recursive -b minilib https://github.com/hilbix/tinolib.git minilib
	make -C minilib

	yes n | ln -is minilib/test/* minilib/test/.*[^.]* .
	make


## About

This is a completely fresh restart of tinolib.
There are many things I want to change,
and many design decisions became clumsy or obsolete
over the time.

- Kept: No linking, everything is directly included.
- Kept: Must be used as a `git submodule`
- Kept: No `./configure` or similar complex things.
- Improved: Completely CLLed, so entirely without any Copyright, so it is Public Domain
- Changed: Needs the full power of GNU `make` v3.8 and pure GNU `bash` v3.2
- Added: Requires `git`
- Removed: No more needs `awk`
- Removed: No more `Makefile.tino`
- Removed: Function suffixes/flags, function variants are just numbered
- Removed: Prefix `tino_` or similar as this now assumes you start to use MiniLib from scratch
- Added: Generators (scripts you can call to generate things)
- Dropped: CygWin/Windows support (instead use WSL)
- Dropped: ANSI-C, this now uses C11 features
- Dropped: Wrappers for standard routines.  Only functions which provide something are added.
- Focus on correctness instead of features
- Ease of use, as long as this does not promote commonly done errors

Notes:

- `make` v3.8 and `bash` v3.2 are those versions on MacOS on my side.
- However MacOS is a 2nd class citizen at my side, sorry.
- If you find any bugs or incompatibilities, please open an Issue at GitHub

Things I definitively want to get rid of:

- `printf()`.  This is error prone, complex and weird.  Instead use `.h`
- `FILE`.  `FILE` is a pointer and thus dangerous.  `int` and low level IO work best

Contents: (Note: Not everything is in place today.)

- `#include "minilib/err.h"`: Standard way of error processing and debugging
- `#include "minilib/co.h"`: Stackless coroutines based on CPP
- `#include "minilib/io.h"`: IO subsystem using `int`
- `#include "minilib/out.h"`: Output formatting
- `#include "minilib/in.h"`: Input parsing
- `#include "minilib/time.h"`: Time and Date calculations
- `#include "minilib/alarm.h"`: Alarm and Clock management

Features can be enabled/disabled/queried using MACRO `#defines`:

- `isXY`: feature is available
- `doXY`: feature is needed
- `noXY`: feature is not needed
- `X` is the module in Caps.  `Err` for `err.h`, `Io` for `io.h` and so on.
- `Y` is the Feature in CamelCase

Common Features:

- `All` enable all stable features and is enabled by default.  Use `noXAll` to override
- `doXExp` enables all experimental features


## FAQ

License?

- Free as in free beer, free speech and free baby
- Use at own risk, absolutely no warranty

Contact?  Bug?

- Open issue on GitHub, eventually I listen

Contrib?  Patches?

- Open PR on GitHub, eventually I listen
- Your patches must fulfill the the CLL (read: Drop your Copyright!)

WTF why?

- Because I need it

