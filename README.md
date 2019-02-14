# STOP! This project has moved! :-)

We are porting this code into the Clang compiler proper instead of developing this as a plugin.

**We're over here now: https://github.com/connorkuehl/llvm-project**

# Clang randstruct
[![Build Status](https://travis-ci.org/connorkuehl/clang_randstruct.svg?branch=master)](https://travis-ci.org/connorkuehl/clang_randstruct)

By: Jordan Cantrell, Nikk Forbus, James Foster, Connor Kuehl, Cole Nixon, Tim Pugh, Jeff Takahashi

## Details

`randstruct` is a GCC compiler plugin that was ported from grsecurity to upstream.

* This randomizes the layout of manually/automatically selected C structures.

* This makes flaw exploitation less deterministic, requiring significantly more flaws before an attacker can detect and target the layout of sensitive kernel structures in memory. Kees Cook, our sponsor, wants this functionality to be made usable in Clang/LLVM.

## Goals

* Develop full randomization. All structures marked with `__randomize_layout` have field positions randomized, including bit fields.

* Develop a 'performance-sensitive' mode. Best-effort limited randomization to cache-line (64-byte) size region, keeping adjacent bit fields together.

* Develop an automatic structure selection method (e.g. all functions pointers). Disabled with `__no_randomize_layout`.

* Develop regression tests.

* Randomization seed needs to be externally created or known before building.

* Ultimate goal to be upstreamed into LLVM and Clang.
