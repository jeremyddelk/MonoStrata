# MonoStrata

A lightweight runtime method-patching system for Mono-based applications.
MonoStrata is a minimal, direct-to-metal patching layer designed to reroute managed methods at runtime. It operates by unwrapping hashed method pointers, validating memory protections, and injecting a hand-assembled 64-bit jump stub to redirect execution flow.
This project exists for one purpose: runtime control. No bloat, no extra layers, just clean pointer manipulation and predictable behavior.

## How It Works

MonoStrata stores encoded method pointers, unmasks them, verifies they point to executable pages, and writes a small trampoline directly over the original method.

The patch applied is:
mov rax, <hook>
jmp rax

That’s it — a 14-byte overwrite that forces the original code to jump into your replacement logic.
See MonoPatchingService.cpp for details. The core routine unwraps the addresses, checks memory protections, and applies the patch safely using VirtualQuery + VirtualProtect. 

## Features

Simple XOR-masked pointer scheme
Executable region validation before patching
14-byte 64-bit jump stub
Patch + restore support
Minimal dependencies
Easy to embed into existing engines or tooling

## Why It Exists

MonoStrata was built as a small internal engine component — a clean alternative to bloated hooking libraries when you only need deterministic method redirection in Mono runtimes. 
It’s intentionally small so it can be understood, audited, and extended.

## Use Cases

Custom game engine experimentation
Runtime debugging or instrumentation
Modding frameworks
Scripting/runtime extension layers
Educational runtime-patching examples

## Status

This is not a polished product — it’s an internal tool made public so others can study, fork, or adapt it. If it helps someone build something smarter (or decides they want to hire me), that’s just a bonus.
