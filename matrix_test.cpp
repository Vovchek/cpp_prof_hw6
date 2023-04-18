#include <gtest/gtest.h>
#include "sparse_matrix.h"

const int def_val = -777;

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

TEST_F(SparseMatrixTest, TestVectorInsert)
{
    EXPECT_EQ(sv.size(), 0);
    for (int i = 0; i < 10; ++i)
    {
        auto idx = i * i * i * i, v = idx + 1111;
        EXPECT_EQ(sv[idx], def_val);
        sv[idx] = v;
        EXPECT_EQ(sv[idx], v);
        EXPECT_EQ(sv.size(), i + 1);
    }
}

TEST_F(SparseMatrixTest, TestVectorFreeUnused)
{
    for (int i = 0; i < 10; ++i)
    {
        auto idx = i * i * i * i, v = idx + 1111;
        sv[idx] = v;
    }

    EXPECT_EQ(sv.size(), 10);

    for (int i = 0; i < 10; ++i)
    {
        auto idx = i * i * i * i;
        sv[idx] = def_val;
        EXPECT_EQ(sv.size(), 9 - i);
    }
}

TEST_F(SparseMatrixTest, TestMatrixDiags)
{

    for (int i = 0; i <= 9; ++i)
    {
        sm[i][i] = i;
        sm[i][9 - i] = 9 - i;
    }
    for (int i = 0; i <= 9; i++)
    {
        for (int j = 0; j <= 9; j++)
        {
            if (i == j)
                EXPECT_EQ(sm[i][j], i);
            else if (j == 9 - i)
                EXPECT_EQ(sm[i][j], 9 - i);
            else
                EXPECT_EQ(sm[i][j], def_val);
        }
    }
}

TEST_F(SparseMatrixTest, TestMatrixIterator)
{
    for (int i = 0; i <= 9; ++i)
    {
        sm[i][i] = i;
        sm[i][9 - i] = 9 - i;
    }

    auto sz = sm.size();

    for (auto c : sm)
    {
        EXPECT_EQ(sm[c.i][c.j], c.v);
        --sz;
    }
    EXPECT_EQ(sz, 0);
}

TEST_F(SparseMatrixTest, TestMatrixFreeUnused)
{
    for (int i = 0; i <= 9; ++i)
    {
        sm[i][i] = i;
        sm[i][9 - i] = 9 - i;
    }

    auto sz = sm.size();
    auto nr = sm.nrows();
    for (int i = 0; i <= 9; ++i)
    {
        sm[i][i] = def_val;
        EXPECT_EQ(--sz, sm.size());
        sm[i][9 - i] = def_val;
        EXPECT_EQ(--sz, sm.size());
        EXPECT_EQ(--nr, sm.nrows());
    }
}
