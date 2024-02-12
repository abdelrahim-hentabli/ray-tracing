# ray-tracing

A raytracing project created for the purpose of learning

## Downloading and Building the Project

This project can be downloaded by using the 
```
git clone --recurse-submodules https://github.com/abdelrahim-hentabli/ray-tracing.git
```
or
```
git clone --recurse-submodules git@github.com:abdelrahim-hentabli/ray-tracing.git
```


The library/executables can be built by calling (from within the project directory): 
```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --parallel <# of cores available for build>
```

### Running the tests/benchmarks

Tests can be run by calling the executable at
```
./build/tools/test/tests
```

And benchmarks can be run by calling
```
./build/tools/benchmark/benchmarks
```