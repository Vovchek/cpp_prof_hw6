#include <gtest/gtest.h>
#include "sparse_matrix.h"

#define def_val -777

class SparseMatrixTest : public ::testing::Test
{
protected:
    SparseVector<int, def_val> sv;
    SparseMatrix<int, 0> sm;

    void SetUp() override
    {
        for (int i = 0; i <= 9; ++i)
        {
            sm[i][i] = i;
            sm[i][9 - i] = 9 - i;
        }
    }
    // void tearDown() override {}
};

TEST_F(SparseMatrixTest, TestVectorInsert)
{
    for (int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(sv.size(), i);
        auto idx = i * i * i * i, v = idx + 1111;
        EXPECT_EQ(sv[idx], def_val);
        sv[idx] = v;
        EXPECT_EQ(sv[idx], v);
    }
    EXPECT_EQ(sv.size(), 10);
}

TEST_F(SparseMatrixTest, TestVectorRemove)
{
    for (int i = 0; i < 10; ++i)
    {
        auto idx = i * i * i * i;
        auto sz = sv.size();
        sv[idx] = def_val;
        EXPECT_EQ(sv[idx], def_val);
        //EXPECT_EQ(sv.size(), sz - 1);
    }
}

TEST_F(SparseMatrixTest, TestMatrixFillDiags)
{
    for (int i = 1; i <= 8; ++i)
    {
        for (int j = 1; j <= 8; ++j)
        {
            if (j == i)
            EXPECT_EQ(sm[i][j], i);
            else if(j == 9-i)
            EXPECT_EQ(sm[i][j], j);
            else
            EXPECT_EQ(sm[i][j], 0);
        }
        std::cout << std::endl;
    }

}

TEST_F(SparseMatrixTest, TestMatrixSize)
{
    EXPECT_EQ(sm.size(), 18);
}

TEST_F(SparseMatrixTest, TestMatrixIterate)
{
}
TEST_F(SparseMatrixTest, TestMatrixRemove)
{
}
