// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "matrix_base.h"
#include "matrix_operations.h"
#include "../../jstring.h"
#include "vector4_base.h"

#include <cassert>

namespace jutils
{
    namespace math
    {
        template<typename Type>
        class matrix<3, 4, Type>
        {
        public:

            static constexpr vector_size_type rows_count = 3;
            static constexpr vector_size_type columns_count = 4;

            using type = Type;
            using column_type = vector<rows_count, type>;
            using row_type = vector<columns_count, type>;
            using transpose_type = matrix<columns_count, rows_count, type>;

            constexpr matrix() = default;
            constexpr matrix(const type x1, const type y1, const type z1, const type w1, const type x2, const type y2, const type z2, const type w2, const type x3, const type y3, const type z3, const type w3)
                : rows{ row_type(x1, y1, z1, w1), row_type(x2, y2, z2, w2), row_type(x3, y3, z3, w3) }
            {}
            constexpr matrix(const row_type& row1, const row_type& row2, const row_type& row3)
                : rows{ row1, row2, row3 }
            {}
            template<typename OtherType>
            constexpr matrix(const matrix<3, 4, OtherType>& value)
                : rows{ value.rows[0], value.rows[1], value.rows[2] }
            {}
            template<vector_size_type ColumnsCount, typename OtherType>
            explicit constexpr matrix(const matrix<2, ColumnsCount, OtherType>& value)
                : rows{ row_type(value.rows[0]), row_type(value.rows[1]), row_type(0) }
            {}
            template<vector_size_type RowsCount, vector_size_type ColumnsCount, typename OtherType, TEMPLATE_ENABLE((RowsCount > 2) && ((RowsCount > 3) || (ColumnsCount != 4)))>
            explicit constexpr matrix(const matrix<RowsCount, ColumnsCount, OtherType>& value)
                : rows{ row_type(value.rows[0]), row_type(value.rows[1]), row_type(value.rows[2]) }
            {}

            template<vector_size_type ColumnsCount, typename OtherType>
            constexpr matrix& operator=(const matrix<2, ColumnsCount, OtherType>& value)
            {
                rows[0] = value.rows[0];
                rows[1] = value.rows[1];
                rows[2] = row_type(0);
                return *this;
            }
            template<vector_size_type RowsCount, vector_size_type ColumnsCount, typename OtherType, TEMPLATE_ENABLE((RowsCount >= 3))>
            constexpr matrix& operator=(const matrix<RowsCount, ColumnsCount, OtherType>& value)
            {
                if (this != &value)
                {
                    rows[0] = value.rows[0];
                    rows[1] = value.rows[1];
                    rows[2] = value.rows[2];
                }
                return *this;
            }

            row_type rows[rows_count];

            constexpr row_type& get(const vector_size_type index)
            {
                assert((index >= 0) && (index < rows_count));
                return rows[index];
            }
            constexpr const row_type& get(const vector_size_type index) const
            {
                assert((index >= 0) && (index < rows_count));
                return rows[index];
            }
            constexpr row_type& operator[](const vector_size_type index) { return get(index); }
            constexpr const row_type& operator[](const vector_size_type index) const { return get(index); }

            constexpr column_type getColumn(const vector_size_type index) const
            {
                assert((index >= 0) && (index < columns_count));
                switch (index)
                {
                case 0: return column_type(rows[0].x, rows[1].x, rows[2].x, rows[3].x);
                case 1: return column_type(rows[0].y, rows[1].y, rows[2].y, rows[3].y);
                case 2: return column_type(rows[0].z, rows[1].z, rows[2].z, rows[3].z);
                default: ;
                }
                return column_type(rows[0].w, rows[1].w, rows[2].w, rows[3].w);
            }

            template<typename OtherType>
            constexpr matrix& operator+=(const matrix<3, 4, OtherType>& value)
            {
                rows[0] += value[0];
                rows[1] += value[1];
                rows[2] += value[2];
                return *this;
            }
            template<typename OtherType>
            constexpr matrix& operator-=(const matrix<3, 4, OtherType>& value)
            {
                rows[0] -= value[0];
                rows[1] -= value[1];
                rows[2] -= value[2];
                return *this;
            }
            template<typename OtherType, TEMPLATE_ENABLE(is_castable<OtherType, type>)>
            constexpr matrix& operator*=(const OtherType& value)
            {
                rows[0] *= value;
                rows[1] *= value;
                rows[2] *= value;
                return *this;
            }

            template<typename OtherType>
            constexpr bool operator==(const matrix<3, 4, OtherType>& value) const
            {
                return (rows[0] == value.rows[0]) && (rows[1] == value.rows[1]) && (rows[2] == value.rows[2]);
            }
            template<typename OtherType>
            constexpr bool operator!=(const matrix<3, 4, OtherType>& value) const { return !this->operator==(value); }

            jstring toString() const { return JSTR("{ ") + rows[0].toString() + JSTR("; ") + rows[1].toString() + JSTR("; ") + rows[2].toString() + JSTR(" }"); }

            constexpr transpose_type transpose() const { return { rows[0].x, rows[1].x, rows[2].x, rows[0].y, rows[1].y, rows[2].y, rows[0].z, rows[1].z, rows[2].z, rows[0].w, rows[1].w, rows[2].w }; }
        };
    }
}
