# minipy Changelog

## [1.1.0] - 2017-XX-XX

### Added
- Travis-CI
- License
- Changelog
- Dict.pop(key, default_value)
- Tool: minipy -dis [script.py] print opcodes

### Changed
- Change bytecodes type from byte to TmCodeCache

### Removed
- Tm2c

### Fixed
- Fix repl OP_SET behavior
- Unhandled exception message now writes to stderr

## [1.0.0] - 2016-10-23

### Added
- A interpreter written in C
- A compiler written in minipy itself
- Basic objects
- Simple class
- Some builtin functions
- Basic modules: sys, time, math etc.
- Mark-sweep garbage collector
- Some tools: repl, dis etc.