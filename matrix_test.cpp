#include <gtest/gtest.h>
#include "sparse_matrix.h"

#define def_val -777

class SparseMatrixTest : public ::testing::Test
{
protected:
    SparseVector<int, def_val> sv;
    SparseMatrix<int, def_val> sm;
    
    void SetUp() override
    {

    }
    // void tearDown() override {}
 };

TEST_F(SparseMatrixTest, TestVector)
{
    EXPECT_EQ(sv.size(), 0);
    for (int i = 0; i < 10; ++i)
    {
        auto idx = i*i*i*i, v = idx + 1111;
        EXPECT_EQ(sv[idx], def_val);
        sv[idx] = v;
        EXPECT_EQ(sv[idx], v);
    }
}
