# Implementation and evaluation of some branching predictors
The goal of this practical work is to evaluate the performance of several branch prediction models.

## Simulator architecture
The source codes of the different predictors have been produced in the folder `sim/` under the name `predictor_<name_of_predictor>.cc`

To compile the simulator with the desired predictor, you must create a symbolic link `predictor.cc` to said predictor. For example:

```
cd ./sim && ln -s predictor_perceptron.cc predictor.cc
```
The `predictor.h` header has been enhanced with various attributes required by other predictor implementations.

Once the symbolic link is created, we can compile the project:
```
cd ./sim && make clean && make
```

This work was done as part of the Conception and Exploration of multi-core architectures course given in the last year of the Information Systems Engineering specialization at Grenoble INP - Ensimag, UGA.
