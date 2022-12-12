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

## Report
To compile the report, run the command:
```
cd ./todo/rendu && make clean && make
```
The report is then available in the `todo/rendu/` under the name `tp-branch-res.pdf`.


This work was done as part of the Conception and Exploitation of multi-core architectures course in the last year of engineering courses at Grenoble INP - Ensimag, UGA.