# See LICENSE for license details

test.prg: plat_main.c plat_vim.c plat_vim.h
	@echo linking test.prg
	cl65 -o test.prg -t c64 -m test.map -I. -Cl -r -Oi plat_main.c plat_vim.c

test.prg: plat_precomp.h

test.prg: makefile

test.prg: plat_project.mak

# end-of-file: plat_project.mak
