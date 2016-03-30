//
// Created by john on 3/29/16.
//

#ifndef CPFA_ARGOS_CPFA_LOOP_FUNCTIONS_TEST_H
#define CPFA_ARGOS_CPFA_LOOP_FUNCTIONS_TEST_H


#include "gtest/gtest.h"

class CPFA_loop_functions_test : public ::testing::Test {

protected:

    // You can do set-up work for each test here.
    CPFA_loop_functions_test();

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~CPFA_loop_functions_test();

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();
};


#endif //CPFA_ARGOS_CPFA_LOOP_FUNCTIONS_TEST_H
