#!/usr/bin/env bash
set -euo pipefail

# Benchmark script for Makefile - measures build time with different -j options and with/without ccache
OUT_FILE="benchmark_results.txt"
echo "Benchmark started at $(date)" > $OUT_FILE

# N values to test; include logical values up to num cores
NCPUS=$(nproc 2>/dev/null || echo 1)
VALUES=(1 2 4 8 $NCPUS)

clean_and_build() {
    LOCAL_J=$1
    echo "\n## make -j${LOCAL_J}" >> $OUT_FILE
    make clean >/dev/null 2>&1 || true
    /usr/bin/time -f "real %e\nuser %U\nsys %S" make -j${LOCAL_J} 2>&1 | tee -a $OUT_FILE
}

echo "Benchmarking without ccache" >> $OUT_FILE
for j in "${VALUES[@]}"; do
    clean_and_build $j
done

if command -v ccache >/dev/null 2>&1; then
    echo "\nBenchmarking with ccache" >> $OUT_FILE
    # Enable ccache via env
    export CC="ccache gcc"
    export CXX="ccache g++"
    for j in "${VALUES[@]}"; do
        clean_and_build $j
    done
else
    echo "\nccache not found; skipping ccache benchmarks" >> $OUT_FILE
fi

echo "\nBenchmarks completed at $(date)" >> $OUT_FILE
echo "Results in: $OUT_FILE"
