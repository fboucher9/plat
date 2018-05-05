# See LICENSE for license details

test.prg: plat_main.c
	@echo linking test.prg
	cl65 -o test.prg -t c64 -I. -Cl -r -Oi plat_main.c

test.prg: plat_precomp.h

test.prg: makefile

test.prg: plat_project.mak

# end-of-file: plat_project.mak
