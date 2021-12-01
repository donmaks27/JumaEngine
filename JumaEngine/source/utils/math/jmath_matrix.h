// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "jmath_vector.h"
#include "base_types/matrix_base.h"

namespace jutils
{
    namespace math
    {
        template<vector_size_type Rows, vector_size_type Columns, typename Type1, typename Type2>
        constexpr bool isMatricesEqual(const math::matrix<Rows, Columns, Type1>& value1, const math::matrix<Rows, Columns, Type2>& value2, const float eps = EPSILON_DEFAULT)
        {
            for (vector_size_type i = 0; i < Rows; i++)
            {
                if (!isVectorsEqual(value1.rows[i], value2.rows[i], eps))
                {
                    return false;
                }
            }
            return true;
        }
    }
}
