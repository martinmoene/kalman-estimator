#include "num/fixed-point.hpp"
#include "num/matrix.hpp"

template< typename T, int S >
struct kalman
{
    //num::matrix<T,S,S>
    auto eye()
    {
        return num::eye<T,S>();
    }
};

using fp15_t = num::fixed_point<int,15>;

auto eye1 = num::eye<double,2>();
auto eye3 = num::eye<num::fixed_point<int,15>,2>();

num::matrix<int, 2, 2> MatrixA = { 1, 2, 3, 4, };

num::matrix<fp15_t, 2, 2> MatrixILA( std::initializer_list<fp15_t>({ 1, 2, 3, 4, }) );

const num::matrix<fp15_t, 2, 2> MatrixILB({ 1, 2, 3, 4, });

//auto MatrixILC = num::matrix<fp15_t, 2, 2>({ 1, 2, 3, 4, });
//
num::matrix<fp15_t, 2, 2> MatrixILD = { 1, 2, 3, 4, };

// --------------


int main()
{
//    auto eye1 = num::eye<double,2>();
//    auto eye2 = num::eye<fixedp<int,15>,2>();
//    auto eye3 = num::eye<num::fixed_point<int,15>,2>();

//    kalman<double,2> ke1; ke1.eye();
//    kalman<fixedp<int,15>,2> ke2; ke2.eye();
//    kalman<num::fixed_point<int,15>,2> ke3; ke3.eye();


//    num::matrix<fp15_t, 2, 2> LocalMatrixIL( std::initializer_list<fp15_t>({ 11, 22, 33, 44, }) );
//
//    return LocalMatrixIL(1,1).underlying_value();
}

// g++ -std=c++17 -O2 -c -I../include/ avr-issue-tpl-fun.b.cpp
// ..\script\avr.bat -std=c++17  -O2 -c -DAVR -I../include/ avr-issue-tpl-fun.b.cpp
