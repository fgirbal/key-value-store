# Key Value store
Final project for the class of Systems Programming, consists of a simple key-value store with a client API and fault tolerance implemented.

**Language:** Portuguese

**Authors:** Francisco Eiras and Miguel Cardoso

## Compiling

By running `make` in `../Code`, it will generate the `front-server`, `data-server` and `client` files. You can use the API by including the file `psiskv.h` in your application. It is possible to compile only the data or front server by running `make data-server` or `make front-server`. After compiling, run `make clean` to clear all the `.o` created files.

## Running

To run the servers, just execute:

```
./front-server
./data-server
```

## Implementation details

Implementation details can be found in the file `Report.pdf`.
