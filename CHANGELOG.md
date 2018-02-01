# minipy Changelog

## v-1.1.0 - 2017/09

### Added
- Travis-CI
- License
- Changelog
- Built-in functions
```
 1. Dict.pop(key, default_value)
 2. List.extend(list)
```
- Tool: minipy -dis [script.py] print opcodes
- tail call optimization
- Syntax
```
 1. [a,b] = v1,v2
 2. a = [1,2] * 10
```

### Changed
- Change bytecodes type from byte to TmCodeCache

### Removed
- Tm2c

### Fixed
- Fix repl OP_SET behavior
- Unhandled exception message now writes to stderr
- slice

## v-1.0.0 - 2016/10/23

### Added
- A interpreter written in C
- A compiler written in minipy itself
- Basic objects
- Simple class
- Some builtin functions
- Basic modules: sys, time, math etc.
- Mark-sweep garbage collector
- Repl tools.
