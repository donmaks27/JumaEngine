// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "matrix_base.h"
#include "../../type_checks.h"
#include "../jmath_vector.h"

namespace jutils
{
    namespace math
    {
        template<vector_size_type RowsCount, vector_size_type ColumnsCount, typename Type, typename OtherType>
        constexpr matrix<RowsCount, ColumnsCount, Type> operator+(const matrix<RowsCount, ColumnsCount, Type>& value1, const matrix<RowsCount, ColumnsCount, OtherType>& value2);
        template<vector_size_type RowsCount, vector_size_type ColumnsCount, typename Type, typename OtherType>
        constexpr matrix<RowsCount, ColumnsCount, Type> operator-(const matrix<RowsCount, ColumnsCount, Type>& value1, const matrix<RowsCount, ColumnsCount, OtherType>& value2);
        template<vector_size_type RowsCount, vector_size_type ColumnsCount, typename Type, typename OtherType, TEMPLATE_ENABLE(is_castable<OtherType, Type>)>
        constexpr matrix<RowsCount, ColumnsCount, Type> operator*(const matrix<RowsCount, ColumnsCount, Type>& value1, const OtherType& value2);
        template<vector_size_type RowsCount, vector_size_type ColumnsCount, typename Type, typename OtherType, TEMPLATE_ENABLE(is_castable<OtherType, Type>)>
        constexpr matrix<RowsCount, ColumnsCount, Type> operator*(const Type& value1, const matrix<RowsCount, ColumnsCount, OtherType>& value2);

        template<typename Type, typename OtherType> constexpr matrix<2, 2, Type> operator*(const matrix<2, 2, Type>& value1, const matrix<2, 2, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<2, 3, Type> operator*(const matrix<2, 2, Type>& value1, const matrix<2, 3, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<2, 4, Type> operator*(const matrix<2, 2, Type>& value1, const matrix<2, 4, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<2, 2, Type> operator*(const matrix<2, 3, Type>& value1, const matrix<3, 2, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<2, 3, Type> operator*(const matrix<2, 3, Type>& value1, const matrix<3, 3, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<2, 4, Type> operator*(const matrix<2, 3, Type>& value1, const matrix<3, 4, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<2, 2, Type> operator*(const matrix<2, 4, Type>& value1, const matrix<4, 2, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<2, 3, Type> operator*(const matrix<2, 4, Type>& value1, const matrix<4, 3, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<2, 4, Type> operator*(const matrix<2, 4, Type>& value1, const matrix<4, 4, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<3, 2, Type> operator*(const matrix<3, 2, Type>& value1, const matrix<2, 2, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<3, 3, Type> operator*(const matrix<3, 2, Type>& value1, const matrix<2, 3, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<3, 4, Type> operator*(const matrix<3, 2, Type>& value1, const matrix<2, 4, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<3, 2, Type> operator*(const matrix<3, 3, Type>& value1, const matrix<3, 2, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<3, 3, Type> operator*(const matrix<3, 3, Type>& value1, const matrix<3, 3, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<3, 4, Type> operator*(const matrix<3, 3, Type>& value1, const matrix<3, 4, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<3, 2, Type> operator*(const matrix<3, 4, Type>& value1, const matrix<4, 2, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<3, 3, Type> operator*(const matrix<3, 4, Type>& value1, const matrix<4, 3, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<3, 4, Type> operator*(const matrix<3, 4, Type>& value1, const matrix<4, 4, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<4, 2, Type> operator*(const matrix<4, 2, Type>& value1, const matrix<2, 2, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<4, 3, Type> operator*(const matrix<4, 2, Type>& value1, const matrix<2, 3, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<4, 4, Type> operator*(const matrix<4, 2, Type>& value1, const matrix<2, 4, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<4, 2, Type> operator*(const matrix<4, 3, Type>& value1, const matrix<3, 2, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<4, 3, Type> operator*(const matrix<4, 3, Type>& value1, const matrix<3, 3, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<4, 4, Type> operator*(const matrix<4, 3, Type>& value1, const matrix<3, 4, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<4, 2, Type> operator*(const matrix<4, 4, Type>& value1, const matrix<4, 2, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<4, 3, Type> operator*(const matrix<4, 4, Type>& value1, const matrix<4, 3, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr matrix<4, 4, Type> operator*(const matrix<4, 4, Type>& value1, const matrix<4, 4, OtherType>& value2);

        template<vector_size_type ColumnsCount, typename Type, typename OtherType> constexpr vector<2, Type> operator*(const matrix<2, ColumnsCount, Type>& value1, const vector<ColumnsCount, OtherType>& value2);
        template<vector_size_type ColumnsCount, typename Type, typename OtherType> constexpr vector<3, Type> operator*(const matrix<3, ColumnsCount, Type>& value1, const vector<ColumnsCount, OtherType>& value2);
        template<vector_size_type ColumnsCount, typename Type, typename OtherType> constexpr vector<4, Type> operator*(const matrix<4, ColumnsCount, Type>& value1, const vector<ColumnsCount, OtherType>& value2);

        template<typename Type, typename OtherType> constexpr vector<2, Type> operator*(const vector<2, Type>& value1, const matrix<2, 2, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr vector<2, Type> operator*(const vector<3, Type>& value1, const matrix<3, 2, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr vector<2, Type> operator*(const vector<4, Type>& value1, const matrix<4, 2, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr vector<3, Type> operator*(const vector<2, Type>& value1, const matrix<2, 3, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr vector<3, Type> operator*(const vector<3, Type>& value1, const matrix<3, 3, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr vector<3, Type> operator*(const vector<4, Type>& value1, const matrix<4, 3, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr vector<4, Type> operator*(const vector<2, Type>& value1, const matrix<2, 4, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr vector<4, Type> operator*(const vector<3, Type>& value1, const matrix<3, 4, OtherType>& value2);
        template<typename Type, typename OtherType> constexpr vector<4, Type> operator*(const vector<4, Type>& value1, const matrix<4, 4, OtherType>& value2);


        
        template<vector_size_type RowsCount, vector_size_type ColumnsCount, typename Type, typename OtherType>
        constexpr matrix<RowsCount, ColumnsCount, Type> operator+(const matrix<RowsCount, ColumnsCount, Type>& value1, const matrix<RowsCount, ColumnsCount, OtherType>& value2)
        {
            return matrix<RowsCount, ColumnsCount, Type>(value1) += value2;
        }
        template<vector_size_type RowsCount, vector_size_type ColumnsCount, typename Type, typename OtherType>
        constexpr matrix<RowsCount, ColumnsCount, Type> operator-(const matrix<RowsCount, ColumnsCount, Type>& value1, const matrix<RowsCount, ColumnsCount, OtherType>& value2)
        {
            return matrix<RowsCount, ColumnsCount, Type>(value1) -= value2;
        }
        template<vector_size_type RowsCount, vector_size_type ColumnsCount, typename Type, typename OtherType, TEMPLATE_ENABLE_IMPL(is_castable<OtherType, Type>)>
        constexpr matrix<RowsCount, ColumnsCount, Type> operator*(const matrix<RowsCount, ColumnsCount, Type>& value1, const OtherType& value2)
        {
            return matrix<RowsCount, ColumnsCount, Type>(value1) *= value2;
        }
        template<vector_size_type RowsCount, vector_size_type ColumnsCount, typename Type, typename OtherType, TEMPLATE_ENABLE_IMPL(is_castable<OtherType, Type>)>
        constexpr matrix<RowsCount, ColumnsCount, Type> operator*(const Type& value1, const matrix<RowsCount, ColumnsCount, OtherType>& value2)
        {
            return matrix<RowsCount, ColumnsCount, Type>(value2) *= value1;
        }

        template<typename Type, typename OtherType>
        constexpr matrix<2, 2, Type> operator*(const matrix<2, 2, Type>& value1, const matrix<2, 2, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<2, 3, Type> operator*(const matrix<2, 2, Type>& value1, const matrix<2, 3, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<2, 4, Type> operator*(const matrix<2, 2, Type>& value1, const matrix<2, 4, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].w) + value1.rows[0].y * static_cast<Type>(value2.rows[1].w),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].w) + value1.rows[1].y * static_cast<Type>(value2.rows[1].w)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<2, 2, Type> operator*(const matrix<2, 3, Type>& value1, const matrix<3, 2, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<2, 3, Type> operator*(const matrix<2, 3, Type>& value1, const matrix<3, 3, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z) + value1.rows[0].z * static_cast<Type>(value2.rows[2].z),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z) + value1.rows[1].z * static_cast<Type>(value2.rows[2].z)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<2, 4, Type> operator*(const matrix<2, 3, Type>& value1, const matrix<3, 4, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z) + value1.rows[0].z * static_cast<Type>(value2.rows[2].z),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].w) + value1.rows[0].y * static_cast<Type>(value2.rows[1].w) + value1.rows[0].z * static_cast<Type>(value2.rows[2].w),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z) + value1.rows[1].z * static_cast<Type>(value2.rows[2].z),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].w) + value1.rows[1].y * static_cast<Type>(value2.rows[1].w) + value1.rows[1].z * static_cast<Type>(value2.rows[2].w)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<2, 2, Type> operator*(const matrix<2, 4, Type>& value1, const matrix<4, 2, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x) + value1.rows[0].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y) + value1.rows[0].w * static_cast<Type>(value2.rows[3].y),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x) + value1.rows[1].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y) + value1.rows[1].w * static_cast<Type>(value2.rows[3].y)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<2, 3, Type> operator*(const matrix<2, 4, Type>& value1, const matrix<4, 3, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x) + value1.rows[0].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y) + value1.rows[0].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z) + value1.rows[0].z * static_cast<Type>(value2.rows[2].z) + value1.rows[0].w * static_cast<Type>(value2.rows[3].z),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x) + value1.rows[1].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y) + value1.rows[1].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z) + value1.rows[1].z * static_cast<Type>(value2.rows[2].z) + value1.rows[1].w * static_cast<Type>(value2.rows[3].z)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<2, 4, Type> operator*(const matrix<2, 4, Type>& value1, const matrix<4, 4, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x) + value1.rows[0].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y) + value1.rows[0].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z) + value1.rows[0].z * static_cast<Type>(value2.rows[2].z) + value1.rows[0].w * static_cast<Type>(value2.rows[3].z),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].w) + value1.rows[0].y * static_cast<Type>(value2.rows[1].w) + value1.rows[0].z * static_cast<Type>(value2.rows[2].w) + value1.rows[0].w * static_cast<Type>(value2.rows[3].w),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x) + value1.rows[1].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y) + value1.rows[1].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z) + value1.rows[1].z * static_cast<Type>(value2.rows[2].z) + value1.rows[1].w * static_cast<Type>(value2.rows[3].z),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].w) + value1.rows[1].y * static_cast<Type>(value2.rows[1].w) + value1.rows[1].z * static_cast<Type>(value2.rows[2].w) + value1.rows[1].w * static_cast<Type>(value2.rows[3].w)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<3, 2, Type> operator*(const matrix<3, 2, Type>& value1, const matrix<2, 2, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<3, 3, Type> operator*(const matrix<3, 2, Type>& value1, const matrix<2, 3, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].z) + value1.rows[2].y * static_cast<Type>(value2.rows[1].z)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<3, 4, Type> operator*(const matrix<3, 2, Type>& value1, const matrix<2, 4, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].w) + value1.rows[0].y * static_cast<Type>(value2.rows[1].w),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].w) + value1.rows[1].y * static_cast<Type>(value2.rows[1].w),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].z) + value1.rows[2].y * static_cast<Type>(value2.rows[1].z),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].w) + value1.rows[2].y * static_cast<Type>(value2.rows[1].w)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<3, 2, Type> operator*(const matrix<3, 3, Type>& value1, const matrix<3, 2, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x) + value1.rows[2].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y) + value1.rows[2].z * static_cast<Type>(value2.rows[2].y)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<3, 3, Type> operator*(const matrix<3, 3, Type>& value1, const matrix<3, 3, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z) + value1.rows[0].z * static_cast<Type>(value2.rows[2].z),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z) + value1.rows[1].z * static_cast<Type>(value2.rows[2].z),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x) + value1.rows[2].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y) + value1.rows[2].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].z) + value1.rows[2].y * static_cast<Type>(value2.rows[1].z) + value1.rows[2].z * static_cast<Type>(value2.rows[2].z)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<3, 4, Type> operator*(const matrix<3, 3, Type>& value1, const matrix<3, 4, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z) + value1.rows[0].z * static_cast<Type>(value2.rows[2].z),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].w) + value1.rows[0].y * static_cast<Type>(value2.rows[1].w) + value1.rows[0].z * static_cast<Type>(value2.rows[2].w),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z) + value1.rows[1].z * static_cast<Type>(value2.rows[2].z),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].w) + value1.rows[1].y * static_cast<Type>(value2.rows[1].w) + value1.rows[1].z * static_cast<Type>(value2.rows[2].w),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x) + value1.rows[2].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y) + value1.rows[2].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].z) + value1.rows[2].y * static_cast<Type>(value2.rows[1].z) + value1.rows[2].z * static_cast<Type>(value2.rows[2].z),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].w) + value1.rows[2].y * static_cast<Type>(value2.rows[1].w) + value1.rows[2].z * static_cast<Type>(value2.rows[2].w)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<3, 2, Type> operator*(const matrix<3, 4, Type>& value1, const matrix<4, 2, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x) + value1.rows[0].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y) + value1.rows[0].w * static_cast<Type>(value2.rows[3].y),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x) + value1.rows[1].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y) + value1.rows[1].w * static_cast<Type>(value2.rows[3].y),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x) + value1.rows[2].z * static_cast<Type>(value2.rows[2].x) + value1.rows[2].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y) + value1.rows[2].z * static_cast<Type>(value2.rows[2].y) + value1.rows[2].w * static_cast<Type>(value2.rows[3].y)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<3, 3, Type> operator*(const matrix<3, 4, Type>& value1, const matrix<4, 3, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x) + value1.rows[0].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y) + value1.rows[0].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z) + value1.rows[0].z * static_cast<Type>(value2.rows[2].z) + value1.rows[0].w * static_cast<Type>(value2.rows[3].z),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x) + value1.rows[1].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y) + value1.rows[1].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z) + value1.rows[1].z * static_cast<Type>(value2.rows[2].z) + value1.rows[1].w * static_cast<Type>(value2.rows[3].z),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x) + value1.rows[2].z * static_cast<Type>(value2.rows[2].x) + value1.rows[2].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y) + value1.rows[2].z * static_cast<Type>(value2.rows[2].y) + value1.rows[2].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].z) + value1.rows[2].y * static_cast<Type>(value2.rows[1].z) + value1.rows[2].z * static_cast<Type>(value2.rows[2].z) + value1.rows[2].w * static_cast<Type>(value2.rows[3].z)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<3, 4, Type> operator*(const matrix<3, 4, Type>& value1, const matrix<4, 4, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x) + value1.rows[0].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y) + value1.rows[0].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z) + value1.rows[0].z * static_cast<Type>(value2.rows[2].z) + value1.rows[0].w * static_cast<Type>(value2.rows[3].z),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].w) + value1.rows[0].y * static_cast<Type>(value2.rows[1].w) + value1.rows[0].z * static_cast<Type>(value2.rows[2].w) + value1.rows[0].w * static_cast<Type>(value2.rows[3].w),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x) + value1.rows[1].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y) + value1.rows[1].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z) + value1.rows[1].z * static_cast<Type>(value2.rows[2].z) + value1.rows[1].w * static_cast<Type>(value2.rows[3].z),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].w) + value1.rows[1].y * static_cast<Type>(value2.rows[1].w) + value1.rows[1].z * static_cast<Type>(value2.rows[2].w) + value1.rows[1].w * static_cast<Type>(value2.rows[3].w),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x) + value1.rows[2].z * static_cast<Type>(value2.rows[2].x) + value1.rows[2].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y) + value1.rows[2].z * static_cast<Type>(value2.rows[2].y) + value1.rows[2].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].z) + value1.rows[2].y * static_cast<Type>(value2.rows[1].z) + value1.rows[2].z * static_cast<Type>(value2.rows[2].z) + value1.rows[2].w * static_cast<Type>(value2.rows[3].z),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].w) + value1.rows[2].y * static_cast<Type>(value2.rows[1].w) + value1.rows[2].z * static_cast<Type>(value2.rows[2].w) + value1.rows[2].w * static_cast<Type>(value2.rows[3].w)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<4, 2, Type> operator*(const matrix<4, 2, Type>& value1, const matrix<2, 2, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y),

                value1.rows[3].x * static_cast<Type>(value2.rows[0].x) + value1.rows[3].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].y) + value1.rows[3].y * static_cast<Type>(value2.rows[1].y)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<4, 3, Type> operator*(const matrix<4, 2, Type>& value1, const matrix<2, 3, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].z) + value1.rows[2].y * static_cast<Type>(value2.rows[1].z),

                value1.rows[3].x * static_cast<Type>(value2.rows[0].x) + value1.rows[3].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].y) + value1.rows[3].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].z) + value1.rows[3].y * static_cast<Type>(value2.rows[1].z)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<4, 4, Type> operator*(const matrix<4, 2, Type>& value1, const matrix<2, 4, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].w) + value1.rows[0].y * static_cast<Type>(value2.rows[1].w),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].w) + value1.rows[1].y * static_cast<Type>(value2.rows[1].w),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].z) + value1.rows[2].y * static_cast<Type>(value2.rows[1].z),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].w) + value1.rows[2].y * static_cast<Type>(value2.rows[1].w),

                value1.rows[3].x * static_cast<Type>(value2.rows[0].x) + value1.rows[3].y * static_cast<Type>(value2.rows[1].x),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].y) + value1.rows[3].y * static_cast<Type>(value2.rows[1].y),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].z) + value1.rows[3].y * static_cast<Type>(value2.rows[1].z),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].w) + value1.rows[3].y * static_cast<Type>(value2.rows[1].w)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<4, 2, Type> operator*(const matrix<4, 3, Type>& value1, const matrix<3, 2, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x) + value1.rows[2].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y) + value1.rows[2].z * static_cast<Type>(value2.rows[2].y),

                value1.rows[3].x * static_cast<Type>(value2.rows[0].x) + value1.rows[3].y * static_cast<Type>(value2.rows[1].x) + value1.rows[3].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].y) + value1.rows[3].y * static_cast<Type>(value2.rows[1].y) + value1.rows[3].z * static_cast<Type>(value2.rows[2].y)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<4, 3, Type> operator*(const matrix<4, 3, Type>& value1, const matrix<3, 3, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z) + value1.rows[0].z * static_cast<Type>(value2.rows[2].z),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z) + value1.rows[1].z * static_cast<Type>(value2.rows[2].z),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x) + value1.rows[2].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y) + value1.rows[2].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].z) + value1.rows[2].y * static_cast<Type>(value2.rows[1].z) + value1.rows[2].z * static_cast<Type>(value2.rows[2].z),

                value1.rows[3].x * static_cast<Type>(value2.rows[0].x) + value1.rows[3].y * static_cast<Type>(value2.rows[1].x) + value1.rows[3].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].y) + value1.rows[3].y * static_cast<Type>(value2.rows[1].y) + value1.rows[3].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].z) + value1.rows[3].y * static_cast<Type>(value2.rows[1].z) + value1.rows[3].z * static_cast<Type>(value2.rows[2].z)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<4, 4, Type> operator*(const matrix<4, 3, Type>& value1, const matrix<3, 4, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z) + value1.rows[0].z * static_cast<Type>(value2.rows[2].z),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].w) + value1.rows[0].y * static_cast<Type>(value2.rows[1].w) + value1.rows[0].z * static_cast<Type>(value2.rows[2].w),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z) + value1.rows[1].z * static_cast<Type>(value2.rows[2].z),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].w) + value1.rows[1].y * static_cast<Type>(value2.rows[1].w) + value1.rows[1].z * static_cast<Type>(value2.rows[2].w),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x) + value1.rows[2].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y) + value1.rows[2].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].z) + value1.rows[2].y * static_cast<Type>(value2.rows[1].z) + value1.rows[2].z * static_cast<Type>(value2.rows[2].z),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].w) + value1.rows[2].y * static_cast<Type>(value2.rows[1].w) + value1.rows[2].z * static_cast<Type>(value2.rows[2].w),

                value1.rows[3].x * static_cast<Type>(value2.rows[0].x) + value1.rows[3].y * static_cast<Type>(value2.rows[1].x) + value1.rows[3].z * static_cast<Type>(value2.rows[2].x),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].y) + value1.rows[3].y * static_cast<Type>(value2.rows[1].y) + value1.rows[3].z * static_cast<Type>(value2.rows[2].y),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].z) + value1.rows[3].y * static_cast<Type>(value2.rows[1].z) + value1.rows[3].z * static_cast<Type>(value2.rows[2].z),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].w) + value1.rows[3].y * static_cast<Type>(value2.rows[1].w) + value1.rows[3].z * static_cast<Type>(value2.rows[2].w)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<4, 2, Type> operator*(const matrix<4, 4, Type>& value1, const matrix<4, 2, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x) + value1.rows[0].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y) + value1.rows[0].w * static_cast<Type>(value2.rows[3].y),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x) + value1.rows[1].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y) + value1.rows[1].w * static_cast<Type>(value2.rows[3].y),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x) + value1.rows[2].z * static_cast<Type>(value2.rows[2].x) + value1.rows[2].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y) + value1.rows[2].z * static_cast<Type>(value2.rows[2].y) + value1.rows[2].w * static_cast<Type>(value2.rows[3].y),

                value1.rows[3].x * static_cast<Type>(value2.rows[0].x) + value1.rows[3].y * static_cast<Type>(value2.rows[1].x) + value1.rows[3].z * static_cast<Type>(value2.rows[2].x) + value1.rows[3].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].y) + value1.rows[3].y * static_cast<Type>(value2.rows[1].y) + value1.rows[3].z * static_cast<Type>(value2.rows[2].y) + value1.rows[3].w * static_cast<Type>(value2.rows[3].y)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<4, 3, Type> operator*(const matrix<4, 4, Type>& value1, const matrix<4, 3, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x) + value1.rows[0].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y) + value1.rows[0].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z) + value1.rows[0].z * static_cast<Type>(value2.rows[2].z) + value1.rows[0].w * static_cast<Type>(value2.rows[3].z),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x) + value1.rows[1].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y) + value1.rows[1].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z) + value1.rows[1].z * static_cast<Type>(value2.rows[2].z) + value1.rows[1].w * static_cast<Type>(value2.rows[3].z),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x) + value1.rows[2].z * static_cast<Type>(value2.rows[2].x) + value1.rows[2].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y) + value1.rows[2].z * static_cast<Type>(value2.rows[2].y) + value1.rows[2].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].z) + value1.rows[2].y * static_cast<Type>(value2.rows[1].z) + value1.rows[2].z * static_cast<Type>(value2.rows[2].z) + value1.rows[2].w * static_cast<Type>(value2.rows[3].z),

                value1.rows[3].x * static_cast<Type>(value2.rows[0].x) + value1.rows[3].y * static_cast<Type>(value2.rows[1].x) + value1.rows[3].z * static_cast<Type>(value2.rows[2].x) + value1.rows[3].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].y) + value1.rows[3].y * static_cast<Type>(value2.rows[1].y) + value1.rows[3].z * static_cast<Type>(value2.rows[2].y) + value1.rows[3].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].z) + value1.rows[3].y * static_cast<Type>(value2.rows[1].z) + value1.rows[3].z * static_cast<Type>(value2.rows[2].z) + value1.rows[3].w * static_cast<Type>(value2.rows[3].z)
            };
        }
        template<typename Type, typename OtherType>
        constexpr matrix<4, 4, Type> operator*(const matrix<4, 4, Type>& value1, const matrix<4, 4, OtherType>& value2)
        {
            return {
                value1.rows[0].x * static_cast<Type>(value2.rows[0].x) + value1.rows[0].y * static_cast<Type>(value2.rows[1].x) + value1.rows[0].z * static_cast<Type>(value2.rows[2].x) + value1.rows[0].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].y) + value1.rows[0].y * static_cast<Type>(value2.rows[1].y) + value1.rows[0].z * static_cast<Type>(value2.rows[2].y) + value1.rows[0].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].z) + value1.rows[0].y * static_cast<Type>(value2.rows[1].z) + value1.rows[0].z * static_cast<Type>(value2.rows[2].z) + value1.rows[0].w * static_cast<Type>(value2.rows[3].z),
                value1.rows[0].x * static_cast<Type>(value2.rows[0].w) + value1.rows[0].y * static_cast<Type>(value2.rows[1].w) + value1.rows[0].z * static_cast<Type>(value2.rows[2].w) + value1.rows[0].w * static_cast<Type>(value2.rows[3].w),

                value1.rows[1].x * static_cast<Type>(value2.rows[0].x) + value1.rows[1].y * static_cast<Type>(value2.rows[1].x) + value1.rows[1].z * static_cast<Type>(value2.rows[2].x) + value1.rows[1].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].y) + value1.rows[1].y * static_cast<Type>(value2.rows[1].y) + value1.rows[1].z * static_cast<Type>(value2.rows[2].y) + value1.rows[1].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].z) + value1.rows[1].y * static_cast<Type>(value2.rows[1].z) + value1.rows[1].z * static_cast<Type>(value2.rows[2].z) + value1.rows[1].w * static_cast<Type>(value2.rows[3].z),
                value1.rows[1].x * static_cast<Type>(value2.rows[0].w) + value1.rows[1].y * static_cast<Type>(value2.rows[1].w) + value1.rows[1].z * static_cast<Type>(value2.rows[2].w) + value1.rows[1].w * static_cast<Type>(value2.rows[3].w),

                value1.rows[2].x * static_cast<Type>(value2.rows[0].x) + value1.rows[2].y * static_cast<Type>(value2.rows[1].x) + value1.rows[2].z * static_cast<Type>(value2.rows[2].x) + value1.rows[2].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].y) + value1.rows[2].y * static_cast<Type>(value2.rows[1].y) + value1.rows[2].z * static_cast<Type>(value2.rows[2].y) + value1.rows[2].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].z) + value1.rows[2].y * static_cast<Type>(value2.rows[1].z) + value1.rows[2].z * static_cast<Type>(value2.rows[2].z) + value1.rows[2].w * static_cast<Type>(value2.rows[3].z),
                value1.rows[2].x * static_cast<Type>(value2.rows[0].w) + value1.rows[2].y * static_cast<Type>(value2.rows[1].w) + value1.rows[2].z * static_cast<Type>(value2.rows[2].w) + value1.rows[2].w * static_cast<Type>(value2.rows[3].w),

                value1.rows[3].x * static_cast<Type>(value2.rows[0].x) + value1.rows[3].y * static_cast<Type>(value2.rows[1].x) + value1.rows[3].z * static_cast<Type>(value2.rows[2].x) + value1.rows[3].w * static_cast<Type>(value2.rows[3].x),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].y) + value1.rows[3].y * static_cast<Type>(value2.rows[1].y) + value1.rows[3].z * static_cast<Type>(value2.rows[2].y) + value1.rows[3].w * static_cast<Type>(value2.rows[3].y),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].z) + value1.rows[3].y * static_cast<Type>(value2.rows[1].z) + value1.rows[3].z * static_cast<Type>(value2.rows[2].z) + value1.rows[3].w * static_cast<Type>(value2.rows[3].z),
                value1.rows[3].x * static_cast<Type>(value2.rows[0].w) + value1.rows[3].y * static_cast<Type>(value2.rows[1].w) + value1.rows[3].z * static_cast<Type>(value2.rows[2].w) + value1.rows[3].w * static_cast<Type>(value2.rows[3].w)
            };
        }
        
        template<vector_size_type ColumnsCount, typename Type, typename OtherType>
        constexpr vector<2, Type> operator*(const matrix<2, ColumnsCount, Type>& value1, const vector<ColumnsCount, OtherType>& value2)
        {
            return { math::dot(value1.rows[0], value2), math::dot(value1.rows[1], value2) };
        }
        template<vector_size_type ColumnsCount, typename Type, typename OtherType>
        constexpr vector<3, Type> operator*(const matrix<3, ColumnsCount, Type>& value1, const vector<ColumnsCount, OtherType>& value2)
        {
            return { math::dot(value1.rows[0], value2), math::dot(value1.rows[1], value2), math::dot(value1.rows[2], value2) };
        }
        template<vector_size_type ColumnsCount, typename Type, typename OtherType>
        constexpr vector<4, Type> operator*(const matrix<4, ColumnsCount, Type>& value1, const vector<ColumnsCount, OtherType>& value2)
        {
            return { math::dot(value1.rows[0], value2), math::dot(value1.rows[1], value2), math::dot(value1.rows[2], value2), math::dot(value1.rows[3], value2) };
        }

        template<typename Type, typename OtherType>
        constexpr vector<2, Type> operator*(const vector<2, Type>& value1, const matrix<2, 2, OtherType>& value2)
        {
            return {
                value1.x * static_cast<Type>(value2.rows[0].x) + value1.y * static_cast<Type>(value2.rows[1].x),
                value1.x * static_cast<Type>(value2.rows[0].y) + value1.y * static_cast<Type>(value2.rows[1].y)
            };
        }
        template<typename Type, typename OtherType>
        constexpr vector<2, Type> operator*(const vector<3, Type>& value1, const matrix<3, 2, OtherType>& value2)
        {
            return {
                value1.x * static_cast<Type>(value2.rows[0].x) + value1.y * static_cast<Type>(value2.rows[1].x) + value1.z * static_cast<Type>(value2.rows[2].x),
                value1.x * static_cast<Type>(value2.rows[0].y) + value1.y * static_cast<Type>(value2.rows[1].y) + value1.z * static_cast<Type>(value2.rows[2].y)
            };
        }
        template<typename Type, typename OtherType>
        constexpr vector<2, Type> operator*(const vector<4, Type>& value1, const matrix<4, 2, OtherType>& value2)
        {
            return {
                value1.x * static_cast<Type>(value2.rows[0].x) + value1.y * static_cast<Type>(value2.rows[1].x) + value1.z * static_cast<Type>(value2.rows[2].x) + value1.w * static_cast<Type>(value2.rows[3].x),
                value1.x * static_cast<Type>(value2.rows[0].y) + value1.y * static_cast<Type>(value2.rows[1].y) + value1.z * static_cast<Type>(value2.rows[2].y) + value1.w * static_cast<Type>(value2.rows[3].y)
            };
        }
        template<typename Type, typename OtherType>
        constexpr vector<3, Type> operator*(const vector<2, Type>& value1, const matrix<2, 3, OtherType>& value2)
        {
            return {
                value1.x * static_cast<Type>(value2.rows[0].x) + value1.y * static_cast<Type>(value2.rows[1].x),
                value1.x * static_cast<Type>(value2.rows[0].y) + value1.y * static_cast<Type>(value2.rows[1].y),
                value1.x * static_cast<Type>(value2.rows[0].z) + value1.y * static_cast<Type>(value2.rows[1].z)
            };
        }
        template<typename Type, typename OtherType>
        constexpr vector<3, Type> operator*(const vector<3, Type>& value1, const matrix<3, 3, OtherType>& value2)
        {
            return {
                value1.x * static_cast<Type>(value2.rows[0].x) + value1.y * static_cast<Type>(value2.rows[1].x) + value1.z * static_cast<Type>(value2.rows[2].x),
                value1.x * static_cast<Type>(value2.rows[0].y) + value1.y * static_cast<Type>(value2.rows[1].y) + value1.z * static_cast<Type>(value2.rows[2].y),
                value1.x * static_cast<Type>(value2.rows[0].z) + value1.y * static_cast<Type>(value2.rows[1].z) + value1.z * static_cast<Type>(value2.rows[2].z)
            };
        }
        template<typename Type, typename OtherType>
        constexpr vector<3, Type> operator*(const vector<4, Type>& value1, const matrix<4, 3, OtherType>& value2)
        {
            return {
                value1.x * static_cast<Type>(value2.rows[0].x) + value1.y * static_cast<Type>(value2.rows[1].x) + value1.z * static_cast<Type>(value2.rows[2].x) + value1.w * static_cast<Type>(value2.rows[3].x),
                value1.x * static_cast<Type>(value2.rows[0].y) + value1.y * static_cast<Type>(value2.rows[1].y) + value1.z * static_cast<Type>(value2.rows[2].y) + value1.w * static_cast<Type>(value2.rows[3].y),
                value1.x * static_cast<Type>(value2.rows[0].z) + value1.y * static_cast<Type>(value2.rows[1].z) + value1.z * static_cast<Type>(value2.rows[2].z) + value1.w * static_cast<Type>(value2.rows[3].z)
            };
        }
        template<typename Type, typename OtherType>
        constexpr vector<4, Type> operator*(const vector<2, Type>& value1, const matrix<2, 4, OtherType>& value2)
        {
            return {
                value1.x * static_cast<Type>(value2.rows[0].x) + value1.y * static_cast<Type>(value2.rows[1].x),
                value1.x * static_cast<Type>(value2.rows[0].y) + value1.y * static_cast<Type>(value2.rows[1].y),
                value1.x * static_cast<Type>(value2.rows[0].z) + value1.y * static_cast<Type>(value2.rows[1].z),
                value1.x * static_cast<Type>(value2.rows[0].w) + value1.y * static_cast<Type>(value2.rows[1].w)
            };
        }
        template<typename Type, typename OtherType>
        constexpr vector<4, Type> operator*(const vector<3, Type>& value1, const matrix<3, 4, OtherType>& value2)
        {
            return {
                value1.x * static_cast<Type>(value2.rows[0].x) + value1.y * static_cast<Type>(value2.rows[1].x) + value1.z * static_cast<Type>(value2.rows[2].x),
                value1.x * static_cast<Type>(value2.rows[0].y) + value1.y * static_cast<Type>(value2.rows[1].y) + value1.z * static_cast<Type>(value2.rows[2].y),
                value1.x * static_cast<Type>(value2.rows[0].z) + value1.y * static_cast<Type>(value2.rows[1].z) + value1.z * static_cast<Type>(value2.rows[2].z),
                value1.x * static_cast<Type>(value2.rows[0].w) + value1.y * static_cast<Type>(value2.rows[1].w) + value1.z * static_cast<Type>(value2.rows[2].w)
            };
        }
        template<typename Type, typename OtherType>
        constexpr vector<4, Type> operator*(const vector<4, Type>& value1, const matrix<4, 4, OtherType>& value2)
        {
            return {
                value1.x * static_cast<Type>(value2.rows[0].x) + value1.y * static_cast<Type>(value2.rows[1].x) + value1.z * static_cast<Type>(value2.rows[2].x) + value1.w * static_cast<Type>(value2.rows[3].x),
                value1.x * static_cast<Type>(value2.rows[0].y) + value1.y * static_cast<Type>(value2.rows[1].y) + value1.z * static_cast<Type>(value2.rows[2].y) + value1.w * static_cast<Type>(value2.rows[3].y),
                value1.x * static_cast<Type>(value2.rows[0].z) + value1.y * static_cast<Type>(value2.rows[1].z) + value1.z * static_cast<Type>(value2.rows[2].z) + value1.w * static_cast<Type>(value2.rows[3].z),
                value1.x * static_cast<Type>(value2.rows[0].w) + value1.y * static_cast<Type>(value2.rows[1].w) + value1.z * static_cast<Type>(value2.rows[2].w) + value1.w * static_cast<Type>(value2.rows[3].w)
            };
        }
    }
}
