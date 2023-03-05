#pragma once

/**
 * @file sparse_matrix.h
 * @brief SparseVector, SparseMatrix, Proxy classes implementations
 * @author Vladimir Chekal
 * @date March 2023
 * @details
 * SparseVector is a std::map based template class intended to store very large sparse vectors
 * while storing only cell values different from the default one.
 * Cell type and a default value are template parameters.
*/


#include <map>

template <typename V, V def_val = 0>
class SparseVector;
template <typename V, V def_val>
class Proxy;
template <typename V, V def_val = 0>
class SparseMatrix;

/**
 * @brief Proxy for SparseVector to discer cell write or read
 * 
 * @tparam V 
 * @tparam def_val 
 */
template <typename V, V def_val>
class Proxy
{
public:
    using vector_type = SparseVector<V, def_val>;
    using proxy_type = Proxy<V, def_val>;

    Proxy(vector_type *v, int i) : pv{v}, idx{i} {} ///< consructor 
    ~Proxy() = default;     ///< destructor
    V operator=(const V &v) ///< cell value assignment operator. lvalue operator[] envoked, update map if nessesery
    {   
        if (v == def_val) // putting Default value in cell
        {                 // if key 'idx' is in map, then remove it
            pv->erase(idx);
        }
        else // putting non-Default value in cell
        {    // if no key 'idx' in map, then insert (idx, v), else update
            pv->insert(idx, v);
        }
        return v;
    }

    operator V() const ///< cell value type cast operator. rvalue operator[] envoked, return the existing or Default value
    {
        return pv->get_value(idx);
    }

    proxy_type &operator=(const proxy_type &rhv) ///< 
    {
        // xvalue operator[], update & return this proxy
        if (&rhv != this)
        {
            operator=(V(rhv));
        }
        return *this;
    };

private:
    vector_type *pv{nullptr};
    int idx{-1};
};

template <typename V, V def_val>
class SparseVector
{
public:
    using vector_data_type = typename std::map<int, V>;
    V operator[](int i) const
    {
        return get_value(i);
    };
    Proxy<V, def_val> operator[](int i)
    {
        // At this point, we don't know whether operator[] was called, so we return
        // a proxy object and defer the decision until later
        return Proxy<V, def_val>(this, i);
    };
    V get_value(int i) const
    {
        auto it = data.find(i);
        return (it != data.end()) ? it->second : def_val;
    }
    int size() const { return data.size(); }
    int max_index() const { return data.crbegin().first; }
    bool empty() const { return data.empty(); }
    void clear()
    {
        data.clear();
    }
    size_t erase(int idx)
    {
        auto it = data.find(idx);
        if (it != data.end())
        {
            data.erase(it);
            return 1;
        }
        return 0;
    }
    void insert(int i, const V &v) { data[i] = v; }
    typename vector_data_type::iterator begin() { return data.begin(); }
    typename vector_data_type::iterator end() { return data.end(); }
    typename vector_data_type::iterator find(int i) { return data.find(i); }

private:
    vector_data_type data;
};

template <typename V, V def_val>
class SparseMatrix
{
    std::map<int, SparseVector<V, def_val>> data;

public:
    using matrix_data_type = std::map<int, SparseVector<V, def_val>>;
    using vector_data_type = std::map<int, V>;

    int size() const
    {
        int s = 0;
        for (auto it = data.cbegin(); it != data.cend(); ++it)
        {
            s += it->second.size();
        }
        return s;
    }
    int nrows() const
    {
        return data.size();
    }
    SparseVector<V, def_val> &operator[](int i)
    {
        return data[i];
    }
    void clear()
    {
        data.clear();
    }
    void pack()
    {
        for (auto it = data.begin(); it != data.end();)
        {
            if (it->second.empty())
                it = data.erase(it);
            else
                ++it;
        }
    }

    struct pos_type
    {
        int i, j;
    };
    struct ret_type
    {
        int i, j;
        V v;
    };

    class iterator
    {

        matrix_data_type *data_ptr{nullptr};
        pos_type cur_pos{-1, -1};

    public:
        using value_type = ret_type;
        using pointer = ret_type *;
        using reference = ret_type &;
        using iterator_category = std::forward_iterator_tag;

        iterator(matrix_data_type *pm, const pos_type &&p) : data_ptr{pm}, cur_pos{p} {}
        iterator(matrix_data_type *pm, int row, int col) : data_ptr{pm}, cur_pos{row, col} {}
        // iterator(const pos_type &p) : cur_pos(p) {}
        // iterator(const matrix_data_type::iterator  &it_row, const vector_data_type::iterator &it_col) : cur_pos{it_row, it_col} {}

        iterator(const iterator &it) : data_ptr{it.data_ptr}, cur_pos{it.cur_pos} {}
        bool operator!=(const iterator &other)
        {
            return data_ptr != other.data_ptr || cur_pos.i != other.cur_pos.i || cur_pos.j != other.cur_pos.j;
        }
        bool operator==(const iterator &other)
        {
            return !(*this != other);
        }
        ret_type operator*() const
        {
            ret_type r{cur_pos.i, cur_pos.j, def_val};
            if (data_ptr)
            {
                r.v = (*data_ptr)[cur_pos.i][cur_pos.j];
            }
            return r;
        }
        iterator &operator++()
        {
            auto it_i = data_ptr->find(cur_pos.i);
            if (it_i != data_ptr->end())
            {

                auto v = it_i->second;
                auto it_j = v.find(cur_pos.j);
                if (it_j != v.end() && ++it_j != v.end())
                { // andvance to next column
                    cur_pos.j = it_j->first;
                    return *this;
                }
                // try the next rows
                while (++it_i != data_ptr->end())
                { 
                    auto v = it_i->second;
                    auto it_j = v.begin();
                    if (it_j != v.end()) // check if it's not a garbage
                    {
                        cur_pos.i = it_i->first;
                        cur_pos.j = it_j->first;
                        return *this;
                    }
                } // while(++it_i)
            }
            cur_pos.i = cur_pos.j = -1;
            return *this;
        }
    }; // iterator

    iterator begin() {
        pack(); // remove garbage
        auto it = data.begin();
        if(it == data.end())
            return end();
        // there should be no empty SparseVectors
        return iterator(&data, it->first, it->second.begin()->first);
    }
    iterator end() { return iterator(&data, -1, -1); }
};
