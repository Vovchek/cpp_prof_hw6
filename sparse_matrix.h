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
 * @tparam V cell type
 * @tparam def_val default value for cells
 * 
 * @details
 * SparseVector returns this Proxy when operator [] is envoked.\n
 * If the caller needs write access (like v[i] = value) it employes operator = ,\n
 * otherwise (like var = v[i]) const typecast V() operator reurns cell value (or default).
 */
template <typename V, V def_val>
class Proxy
{
public:
    using vector_type = SparseVector<V, def_val>;
    using proxy_type = Proxy<V, def_val>;

    /**
     *  @brief Consructor. 
     * @param v Pointer to SparseVector that returned this Proxy. 
     * @param i Index of a cell.
     */
    Proxy(vector_type *v, int i) : pv{v}, idx{i} {}     
    
    /** @brief Destructor. @details default implementation. */
    ~Proxy() = default;                                 
    
/** @brief Cell value assignment operator for lvalue operator[].
 *  @details The assignment of a (default or non-default) value
 * to the cell addressed as vector[index] triggers this assignment operator.\n
 * Underlying SparseVector map container may need to be updated.
 * @param v - Cell value to be assingned.
 * @returns Cell value - the same that was passed as a parameter.
 **/
    V operator=(const V &v)
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

/**
 * @brief Casting Proxy type to cell value type operator.
 * @details Casting Proxy to V type denotes that rvalue operator[] is envoked. 
 * @returns The existing or Default cell value.
 **/ 
    operator V() const 
    {
        return pv->get_value(idx);
    }

/** @brief xvalue operator[], update & return this proxy.
 *  @details The assignment from other Proxy object as it may occur in a chain of assingnments- \n
 * vec1[i1] = vec2[i2] = ...
 *  @param rhv - Right-hand Proxy object
 *  @returns Reference to **this** Proxy object.
 **/
    proxy_type &operator=(const proxy_type &rhv) 
    {
        if (&rhv != this)
        {
            operator=(V(rhv));
        }
        return *this;
    };

private:
/** Pointer to owner SparseVector that called Proxy() constructor. **/
    vector_type *pv{nullptr}; 
/** Cell index passed to constructor by owner SparseVector  **/
    int idx{-1};
};

/**
 * @brief SparseVector container class that stores only non-default cell values of a huge (MAX_INT size) vector.
 * 
 * @tparam V cell type.
 * @tparam def_val default value for cells.
 * 
 * @details
 * SparseVector stores cell values that are not default in std::map container
 * with the key equal to the cell index and data storing corresponding cell value.\n
 * When default value is written to the cell that contained non-default value beforehand,
 * then the corresponding element is erased from the map.\n
 * In contrary, when non-default value is written to empty cell, then new element is
 * inserted into map.\n
 * Index ranges from 0 to MAX_INT.
*/
template <typename V, V def_val>
class SparseVector
{
public:
    using vector_data_type = typename std::map<int, V>;

    /** @brief const indexing operator [].
     * @param i - Index of a cell being accessed.
     * @returns Cell value (or default value) of type V
    */
    V operator[](int i) const
    {
        return get_value(i);
    };

    /** @brief Indexing operator [] both for getting value from or assigning it to a cell.
     * @param i - Index of a cell being accessed.
     * @returns Proxy object that will supply read or write access later on.
     * @details At this point, we don't know whether operator[] was called, so we return
     * a proxy object and defer the decision until later.
    */
    Proxy<V, def_val> operator[](int i)
    {
        // At this point, we don't know whether operator[] was called, so we return
        // a proxy object and defer the decision until later
        return Proxy<V, def_val>(this, i);
    };

    /** @brief Simple getter as an alternative to indexing operator []
     * @param i - Index of a cell being accessed.
     * @returns Cell value (or default value) of type V
    */
    V get_value(int i) const
    {
        auto it = data.find(i);
        return (it != data.end()) ? it->second : def_val;
    }

    /** @brief Number of cells actually engaged.
     * @returns Map size, storing non-default cell values.
    */
    int size() const { return data.size(); }

    /** @brief Index of a last cell, containing non-default value.
     * @returns Index of a last cell, containing non-default value.
    */
    int max_index() const { return data.crbegin().first; }
    /** @brief Denotes the empty status of a SparseVector object
     * @returns `true` if map is empty (all values are default), `false` otherwise.
    */
    bool empty() const { return data.empty(); }

    /** @brief Erases contents of a SparseVector object.
     * @details All elements of a map containter are being deleted.
    */
    void clear()
    {
        data.clear();
    }

    /** @brief Deletes (resets to default) one cell of a SparseVector object.
     * @param idx - Index of an cell being reset to default value.
     * @returns Number of elements erased (1 or 0 depending on initial cell content).
     * @details Erases from a map element with a key equal to idx parameter if the cell was not empty beforehand, or does nothing otherwise.\n
     * The default value for an erased cell is restored.
    */
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

    /** 
     * @brief Inters a cell at a given index
     * @param i The cell index to insert value.
     * @param v [in] The value to be inserted.
    */
    void insert(int i, const V &v) { 
        if(v != def_val) data[i] = v; 
    }

    /**
     * @brief Returns `std::map` iterator addressing the first element in the map.
     * @returns `std::map` iterator addressing the first element in the map or the location succeeding an empty map.
    */
    typename vector_data_type::iterator begin() { return data.begin(); }

    /**
     * @brief Returns `std::map` past-the-end iterator.
     * @returns `std::map` past-the-end iterator. If the map is empty, then `end() == begin()`.
    */
    typename vector_data_type::iterator end() { return data.end(); }

    /**
     * @brief Returns `std::map` iterator that refers to the location of the cell with the specified index.
     * @param i - Index of a cell to find.
     * @returns `std::map` iterator that refers to the location of an element with a specified index, or the location succeeding the last
     * element in the map (`map::end()`) if no match is found for the index.
    */
    typename vector_data_type::iterator find(int i) { return data.find(i); }

private:
    /**
     * @brief `std::map` container, storing cells with non-dfault values. Key (type int) equals to a cell index.
    */
    vector_data_type data;
};

/**
 * @brief SparseMatrix container class that stores only non-default cell values of a huge (MAX_INT x MAX_INT size) 2D matrix.
 * 
 * @tparam V cell type.
 * @tparam def_val default value for cells.
 * 
 * @details
 * SparseMatrix stores pairs in std::map container
 * with the key equal to the matrix row index and data storing corresponding SparseVector object.\n
 * When default value is written to the cell so the SparseVector object for this row becomes empty,
 * then the corresponding element **should be erased** from the map.\n
 * In contrary, when non-default value is written to a cell of an empty SparseVector, then new row is
 * inserted into map.\n
 * Row index ranges from 0 to MAX_INT.
 * 
 * @bug When row becomes empty after default value is written to a cell, this empty SparseVector
 * element is not erased automatically.\n It is nessesery to call pack() explicitly to clear out empty rows
 * or to call begin() method which also calls pack() from itself.
*/
template <typename V, V def_val>
class SparseMatrix
{
public:
    using matrix_data_type = std::map<int, SparseVector<V, def_val>>;

    /**
     * @brief Returns number of non-empty cells.
     * @returns Number of non-empty cells in a matrix.
    */
    int size() const
    {
        int s = 0;
        for (auto it = data.cbegin(); it != data.cend(); ++it)
        {
            s += it->second.size();
        }
        return s;
    }

    /**
     * @brief Returns number of non-empty rows.
     * @returns Number of non-empty cells in a matrix.
    */
    int nrows() const
    {
        return data.size();
    }

    /**
     * @brief Returns reference to SparseVector object for a given row number.
     * @param i - Row number.
     * @details Returns refference to existing of newly inserted SparseVector object, representing requested row index.
    */
    SparseVector<V, def_val> &operator[](int i)
    {
        return data[i];
    }
    
    /**
     * @brief Erase all the data.
    */
    void clear()
    {
        data.clear();
    }

    /**
     * @brief Temporary stub method to remove empty map entries.
    */
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

    /**
     * @brief Structure for storing position in a SparseMatrix object.
    */
    struct pos_type
    {
        int i, j;
    };

    /**
     * @brief Structure for storing matrix cell indexes and value.
    */
    struct ret_type
    {
        int i, j;
        V v;
    };

    /**
     * @brief Forward iterator class to iterate over non-default cells of a SparseMatrix.
     * @details SparseMatrix::iterator is intended to traverse through all non-empty cells.
    */
    class iterator
    {

        matrix_data_type *data_ptr{nullptr}; ///< pointer to the map data of the SparseMatrix to iterate
        pos_type cur_pos{-1, -1};   ///< current position of the iterator

    public:
        
        /** @name Iterator traits: */
        ///@{ 
        using value_type = ret_type;
        using pointer = ret_type *;
        using reference = ret_type &;
        using iterator_category = std::forward_iterator_tag;
        ///@}

        ///@{ 
        /**
         * @brief Constructor.
         * @param pm Pinter to SparseMatrix object to iterate over.
         * @param p Start position to iterate from.
        */
        iterator(matrix_data_type *pm, const pos_type &&p) : data_ptr{pm}, cur_pos{p} {}
        /**
         * @brief Constructor.
         * @param pm Pinter to SparseMatrix object to iterate over.
         * @param row, col Starting row and col indexies to iterate from.
        */
        iterator(matrix_data_type *pm, int row, int col) : data_ptr{pm}, cur_pos{row, col} {}
        /**
         * @brief Copy constructor.
         * @param it Iterator that is being copied.
        */
        iterator(const iterator &it) : data_ptr{it.data_ptr}, cur_pos{it.cur_pos} {}
        ///@}

        /**
         * @brief Iterator comparison, not equal.
         * @param other Iterator to compare with `this`.
         * @returns `true` if `this` iterator is not equal to `other`, `false` otherwise.
        */
        bool operator!=(const iterator &other)
        {
            return data_ptr != other.data_ptr || cur_pos.i != other.cur_pos.i || cur_pos.j != other.cur_pos.j;
        }

        /**
         * @brief Iterator comparison, equal.
         * @param other Iterator to compare with `this`.
         * @returns `true` if `this` iterator is equal to `other`, `false` otherwise.
        */
        bool operator==(const iterator &other)
        {
            return !(*this != other);
        }

        /**
         * @brief Indirection operator.
         * @returns Structure ret_type, contating row index (i), column index (j) and value (v) for the iterator-addressed cell.
        */
        ret_type operator*() const
        {
            ret_type r{cur_pos.i, cur_pos.j, def_val};
            if (data_ptr)
            {
                r.v = (*data_ptr)[cur_pos.i][cur_pos.j];
            }
            return r;
        }

        /**
         * @brief Prefix increment operator.
         * @details Advances iterator to the next non-empty matrix cell and returns reference to this iterator.\n
         * If there are no more busy cells, returns past-the-end iterator, that is equal to `end()`. 
        */
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

    /**
     * @brief Returns iterator addressing the first non-empty cell.
     * @returns Iterator addressing the first element in the matrix or the location succeeding an empty matrix.
    */
    iterator begin() {
        pack(); // remove garbage
        auto it = data.begin();
        if(it == data.end())
            return end();
        // there should be no empty SparseVectors
        return iterator(&data, it->first, it->second.begin()->first);
    }

    /**
     * @brief Returns iterator addressing past the end of matrix.
     * @returns Past-the-end iterator. If the matrix is empty, then `end() == begin()`.
    */
    iterator end() { return iterator(&data, -1, -1); }

private:
    /**
     * @brief `std::map` container, storing non-empty rows (with non-dfault values). Key (type int) equals to a row index.
    */
    matrix_data_type data;

};
