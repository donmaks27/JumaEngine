// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "vector2_base.h"

namespace jutils
{
    namespace math
    {
        template<typename Type>
        class vector<3, Type>
        {
        public:

            using type = Type;

            static constexpr vector_size_type size = 3;

            constexpr vector() = default;
            constexpr vector(const type x, const type y, const type z)
                : x(x), y(y), z(z)
            {}
            explicit constexpr vector(const type value)
                : x(value), y(value), z(value)
            {}
            template<typename OtherType>
            constexpr vector(const vector<3, OtherType>& value)
                : x(static_cast<type>(value.x)), y(static_cast<type>(value.y)), z(static_cast<type>(value.z))
            {}
            template<vector_size_type Size, typename OtherType, TEMPLATE_ENABLE((Size > 3))>
            explicit constexpr vector(const vector<Size, OtherType>& value)
                : x(static_cast<type>(value.x)), y(static_cast<type>(value.y)), z(static_cast<type>(value.z))
            {}
            template<typename OtherType>
            explicit constexpr vector(const vector<2, OtherType>& value)
                : x(static_cast<type>(value.x)), y(static_cast<type>(value.y)), z(0)
            {}

            template<vector_size_type Size, typename OtherType, TEMPLATE_ENABLE((Size >= 3))>
            constexpr vector& operator=(const vector<Size, OtherType>& value)
            {
                if (this != &value)
                {
                    x = static_cast<type>(value.x); y = static_cast<type>(value.y); z = static_cast<type>(value.z);
                }
                return *this;
            }
            template<typename OtherType>
            constexpr vector& operator=(const vector<2, OtherType>& value)
            {
                if (this != &value)
                {
                    x = static_cast<type>(value.x); y = static_cast<type>(value.y); z = 0;
                }
                return *this;
            }

            type x = 0;
            type y = 0;
            type z = 0;

            constexpr vector<2, type> xy() const { return { x, y }; }

            constexpr type& get(const vector_size_type index)
            {
                assert((index >= 0) && (index < this->size));
                switch (index)
                {
                case 0: return x;
                case 1: return y;
                default: ;
                }
                return z;
            }
            constexpr const type& get(const vector_size_type index) const
            {
                assert((index >= 0) && (index < this->size));
                switch (index)
                {
                case 0: return x;
                case 1: return y;
                default: ;
                }
                return z;
            }
            constexpr type& operator[](const vector_size_type index) { return get(index); }
            constexpr const type& operator[](const vector_size_type index) const { return get(index); }
            
            constexpr type* getData() { return &get(0); }
            constexpr const type* getData() const { return &get(0); }

            template<typename OtherType, TEMPLATE_ENABLE(is_castable<OtherType, type>)>
            constexpr vector& operator+=(OtherType value);
            template<typename OtherType>
            constexpr vector& operator+=(const vector<3, OtherType>& value);
            template<typename OtherType, TEMPLATE_ENABLE(is_castable<OtherType, type>)>
            constexpr vector& operator-=(OtherType value);
            template<typename OtherType>
            constexpr vector& operator-=(const vector<3, OtherType>& value);
            template<typename OtherType, TEMPLATE_ENABLE(is_castable<OtherType, type>)>
            constexpr vector& operator*=(OtherType value);
            template<typename OtherType, TEMPLATE_ENABLE(is_castable<OtherType, type>)>
            constexpr vector& operator/=(OtherType value);

            constexpr vector& operator++();
            constexpr vector& operator--();
            constexpr vector operator++(int);
            constexpr vector operator--(int);

            template<typename OtherType>
            constexpr bool operator==(const vector<3, OtherType>& value) const
            {
                return (x == static_cast<type>(value.x)) && (y == static_cast<type>(value.y)) && (z == static_cast<type>(value.z));
            }
            template<typename OtherType>
            constexpr bool operator!=(const vector<3, OtherType>& value) const { return !this->operator==(value); }

            jstring toString() const { return JSTR("{ ") + TO_JSTR(x) + JSTR("; ") + TO_JSTR(y) + JSTR("; ") + TO_JSTR(z) + JSTR(" }"); }
        };

        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator+(const vector<3, Type1>& value1, Type2 value2);
        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator+(Type1 value1, const vector<3, Type2>& value2);
        template<typename Type1, typename Type2>
        constexpr vector<3, Type1> operator+(const vector<3, Type1>& value1, const vector<3, Type2>& value2);

        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator-(const vector<3, Type1>& value1, Type2 value2);
        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator-(Type1 value1, const vector<3, Type2>& value2);
        template<typename Type1, typename Type2>
        constexpr vector<3, Type1> operator-(const vector<3, Type1>& value1, const vector<3, Type2>& value2);

        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator*(const vector<3, Type1>& value1, Type2 value2);
        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator*(Type1 value1, const vector<3, Type2>& value2);

        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator/(const vector<3, Type1>& value1, Type2 value2);
        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator/(Type1 value1, const vector<3, Type2>& value2);



        template<typename Type>
        template<typename OtherType, TEMPLATE_ENABLE_IMPL(is_castable<OtherType, Type>)>
        constexpr vector<3, Type>& vector<3, Type>::operator+=(OtherType value)
        {
            x += static_cast<Type>(value); y += static_cast<Type>(value); z += static_cast<Type>(value);
            return *this;
        }
        template<typename Type>
        template<typename OtherType>
        constexpr vector<3, Type>& vector<3, Type>::operator+=(const vector<3, OtherType>& value)
        {
            x += static_cast<Type>(value.x); y += static_cast<Type>(value.y); z += static_cast<Type>(value.z);
            return *this;
        }
        template<typename Type>
        template<typename OtherType, TEMPLATE_ENABLE_IMPL(is_castable<OtherType, Type>)>
        constexpr vector<3, Type>& vector<3, Type>::operator-=(OtherType value)
        {
            x -= static_cast<Type>(value); y -= static_cast<Type>(value); z -= static_cast<Type>(value);
            return *this;
        }
        template<typename Type>
        template<typename OtherType>
        constexpr vector<3, Type>& vector<3, Type>::operator-=(const vector<3, OtherType>& value)
        {
            x -= static_cast<Type>(value.x); y -= static_cast<Type>(value.y); z -= static_cast<Type>(value.z);
            return *this;
        }
        template<typename Type>
        template<typename OtherType, TEMPLATE_ENABLE_IMPL(is_castable<OtherType, Type>)>
        constexpr vector<3, Type>& vector<3, Type>::operator*=(OtherType value)
        {
            x *= static_cast<Type>(value); y *= static_cast<Type>(value); z *= static_cast<Type>(value);
            return *this;
        }
        template<typename Type>
        template<typename OtherType, TEMPLATE_ENABLE_IMPL(is_castable<OtherType, Type>)>
        constexpr vector<3, Type>& vector<3, Type>::operator/=(OtherType value)
        {
            x /= static_cast<Type>(value); y /= static_cast<Type>(value); z /= static_cast<Type>(value);
            return *this;
        }

        template<typename Type>
        constexpr vector<3, Type>& vector<3, Type>::operator++()
        {
            ++x; ++y; ++z;
            return *this;
        }
        template<typename Type>
        constexpr vector<3, Type>& vector<3, Type>::operator--()
        {
            --x; --y; --z;
            return *this;
        }
        template<typename Type>
        constexpr vector<3, Type> vector<3, Type>::operator++(int)
        {
            vector<3, Type> temp = *this;
            ++x; ++y; ++z;
            return temp;
        }
        template<typename Type>
        constexpr vector<3, Type> vector<3, Type>::operator--(int)
        {
            vector<3, Type> temp = *this;
            --x; --y; --z;
            return temp;
        }

        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator+(const vector<3, Type1>& value1, const Type2 value2)
        {
            return { value1.x + static_cast<Type1>(value2), value1.y + static_cast<Type1>(value2), value1.z + static_cast<Type1>(value2) };
        }
        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator+(const Type1 value1, const vector<3, Type2>& value2)
        {
            return { value1 + static_cast<Type1>(value2.x), value1 + static_cast<Type1>(value2.y), value1 + static_cast<Type1>(value2.z) };
        }
        template<typename Type1, typename Type2>
        constexpr vector<3, Type1> operator+(const vector<3, Type1>& value1, const vector<3, Type2>& value2)
        {
            return { value1.x + static_cast<Type1>(value2.x), value1.y + static_cast<Type1>(value2.y), value1.z + static_cast<Type1>(value2.z) };
        }

        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator-(const vector<3, Type1>& value1, const Type2 value2)
        {
            return { value1.x - static_cast<Type1>(value2), value1.y - static_cast<Type1>(value2), value1.z - static_cast<Type1>(value2) };
        }
        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator-(const Type1 value1, const vector<3, Type2>& value2)
        {
            return { value1 - static_cast<Type1>(value2.x), value1 - static_cast<Type1>(value2.y), value1 - static_cast<Type1>(value2.z) };
        }
        template<typename Type1, typename Type2>
        constexpr vector<3, Type1> operator-(const vector<3, Type1>& value1, const vector<3, Type2>& value2)
        {
            return { value1.x - static_cast<Type1>(value2.x), value1.y - static_cast<Type1>(value2.y), value1.z - static_cast<Type1>(value2.z) };
        }

        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator*(const vector<3, Type1>& value1, const Type2 value2)
        {
            return { value1.x * static_cast<Type1>(value2), value1.y * static_cast<Type1>(value2), value1.z * static_cast<Type1>(value2) };
        }
        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator*(const Type1 value1, const vector<3, Type2>& value2)
        {
            return { value1 * static_cast<Type1>(value2.x), value1 * static_cast<Type1>(value2.y), value1 * static_cast<Type1>(value2.z) };
        }

        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator/(const vector<3, Type1>& value1, const Type2 value2)
        {
            return { value1.x / static_cast<Type1>(value2), value1.y / static_cast<Type1>(value2), value1.z / static_cast<Type1>(value2) };
        }
        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<3, Type1> operator/(const Type1 value1, const vector<3, Type2>& value2)
        {
            return { value1 / static_cast<Type1>(value2.x), value1 / static_cast<Type1>(value2.y), value1 / static_cast<Type1>(value2.z) };
        }
    }
}
