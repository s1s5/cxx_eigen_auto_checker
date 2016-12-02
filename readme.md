# Eigen C++ auto checker
This tool checks Eigen c++ pitfalls.
- https://eigen.tuxfamily.org/dox/TopicPitfalls.html

```sh
$ scons
$ clang++ -Xclang -load -Xclang libeigen_cxx_auto_checker_plugin.so -Xclang -plugin -Xclang eigen_cxx_auto_checker -std=c++11 -c <srcfile.cpp>
```

- to suppress
```c++
auto v = (A * B).eval();  // use eval
Eigen::MatrixXd v = A * B;  // specify type
```
