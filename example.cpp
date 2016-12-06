/**
 * Copyright Shogo Sawai
 * @file example6.cpp
 * @brief
 * @author Shogo Sawai
 * @date 2016-12-02 00:02:00
 */
// https://eigen.tuxfamily.org/dox/TopicPitfalls.html
#include "Eigen/Eigen"

void func() {
    Eigen::MatrixXd A, B;
    auto C = A * B;
    Eigen::MatrixXd C_ok = A * B;
    // for(...) { ... w = C * v;  ...}

    // In this example, the type of C is not a MatrixXd but an abstract expression representing a matrix product and
    // storing references to A and B. Therefore, the product of A*B will be carried out multiple times, once per
    // iteration of the for loop. Moreover, if the coefficients of A or B change during the iteration, then C will
    // evaluate to different values.

    // Here is another example leading to a segfault :
    auto D = ((A + B).eval()).transpose();
    Eigen::MatrixXd D_ok = ((A + B).eval()).transpose();
    // do something with C

    // The problem is that eval() returns a temporary object (in this case a MatrixXd) which is then referenced by the
    // Transpose<> expression. However, this temporary is deleted right after the first line, and there the C expression
    // reference a dead object. The same issue might occur when sub expressions are automatically evaluated by Eigen as
    // in the following example:
    Eigen::VectorXd u, v;
    auto E = u + (A * v).normalized();
    Eigen::VectorXd E_ok = u + (A * v).normalized();
    // do something with C
}
