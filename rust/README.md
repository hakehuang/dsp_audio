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
source ~/.cargo/env
cargo build

# Release build (optimized)
cargo build --release

# run application
cargo run

# Run tests TBD
cargo test

# Run benchmark TBD
cargo bench