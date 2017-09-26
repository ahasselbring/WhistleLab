# System Requirements

 * Linux
 * \>=5GiB RAM (for the current sample database)
 * CMake >= 3.5
 * Qt5
 * libsndfile
 * fftw3
 * fann

# How to build

```bash
mkdir Build
cd Build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j9
```

# How to run

```bash
cd Build
./whistle
```
