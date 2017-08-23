# Eigen C++ auto checker
This tool checks Eigen c++ pitfalls.
- https://eigen.tuxfamily.org/dox/TopicPitfalls.html

```sh
$ cd build
$ cmake ../cxx_eigen_auto_checker/
$ clang++ -Xclang -load -Xclang CXXEigenAutoChecker.so  -Xclang -plugin -Xclang eigen_cxx_auto_checker -std=c++11 -c ../cxx_eigen_auto_checker/test/example.cpp
../cxx_eigen_auto_checker/test/example.cpp:13 Eigen::Product
../cxx_eigen_auto_checker/test/example.cpp:23 Eigen::Transpose
../cxx_eigen_auto_checker/test/example.cpp:32 Eigen::CwiseBinaryOp
```

- to suppress
```c++
auto v = (A * B).eval();  // use eval
Eigen::MatrixXd v = A * B;  // specify type
```
