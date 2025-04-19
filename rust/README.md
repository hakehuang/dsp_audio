# Noise Cancellation DSP (Rust Implementation)

A real-time noise cancellation system implemented in Rust with C FFI compatibility.

## Features
- LMS adaptive filtering algorithm
- C-compatible FFI interface
- Cross-platform support
- SIMD-optimized processing

## Build Instructions

### Prerequisites
- Rust toolchain (stable)
- Cargo package manager
- For cross-compiling: appropriate target toolchain

### Building
```bash
# Debug build
cargo build

# Release build (optimized)
cargo build --release

# Run tests
cargo test

# Run benchmark
cargo bench