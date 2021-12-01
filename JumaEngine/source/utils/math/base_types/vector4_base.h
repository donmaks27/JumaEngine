// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "vector3_base.h"

namespace jutils
{
    namespace math
    {
        template<typename Type>
        class vector<4, Type>
        {
        public:

            using type = Type;

            static constexpr vector_size_type size = 4;

            constexpr vector() = default;
            constexpr vector(const type x, const type y, const type z, const type w)
                : x(x), y(y), z(z), w(w)
            {}
            explicit constexpr vector(const type value)
                : x(value), y(value), z(value), w(value)
            {}
            template<typename OtherType>
            constexpr vector(const vector<4, OtherType>& value)
                : x(static_cast<type>(value.x)), y(static_cast<type>(value.y)), z(static_cast<type>(value.z))
            {}
            template<typename OtherType>
            explicit constexpr vector(const vector<3, OtherType>& value)
                : x(static_cast<type>(value.x)), y(static_cast<type>(value.y)), z(static_cast<type>(value.z)), w(0)
            {}
            template<typename OtherType>
            explicit constexpr vector(const vector<2, OtherType>& value)
                : x(static_cast<type>(value.x)), y(static_cast<type>(value.y)), z(0), w(0)
            {}

            template<typename OtherType>
            constexpr vector& operator=(const vector<4, OtherType>& value)
            {
                if (this != &value)
                {
                    x = static_cast<type>(value.x); y = static_cast<type>(value.y); z = static_cast<type>(value.z); w = static_cast<type>(value.w);
                }
                return *this;
            }
            template<typename OtherType>
            constexpr vector& operator=(const vector<3, OtherType>& value)
            {
                if (this != &value)
                {
                    x = static_cast<type>(value.x); y = static_cast<type>(value.y); z = static_cast<type>(value.z); w = 0;
                }
                return *this;
            }
            template<typename OtherType>
            constexpr vector& operator=(const vector<2, OtherType>& value)
            {
                if (this != &value)
                {
                    x = static_cast<type>(value.x); y = static_cast<type>(value.y); z = 0; w = 0;
                }
                return *this;
            }

            type x = 0;
            type y = 0;
            type z = 0;
            type w = 0;

            constexpr vector<2, type> xy() const { return { x, y }; }
            constexpr vector<3, type> xyz() const { return { x, y, z }; }

            constexpr type& get(const vector_size_type index)
            {
                assert((index >= 0) && (index < this->size));
                switch (index)
                {
                case 0: return x;
                case 1: return y;
                case 2: return z;
                default: ;
                }
                return w;
            }
            constexpr const type& get(const vector_size_type index) const
            {
                assert((index >= 0) && (index < this->size));
                switch (index)
                {
                case 0: return x;
                case 1: return y;
                case 2: return z;
                default: ;
                }
                return w;
            }
            constexpr type& operator[](const vector_size_type index) { return get(index); }
            constexpr const type& operator[](const vector_size_type index) const { return get(index); }
            
            constexpr type* getData() { return &get(0); }
            constexpr const type* getData() const { return &get(0); }

            template<typename OtherType, TEMPLATE_ENABLE(is_castable<OtherType, type>)>
            constexpr vector& operator+=(OtherType value);
            template<typename OtherType>
            constexpr vector& operator+=(const vector<4, OtherType>& value);
            template<typename OtherType, TEMPLATE_ENABLE(is_castable<OtherType, type>)>
            constexpr vector& operator-=(OtherType value);
            template<typename OtherType>
            constexpr vector& operator-=(const vector<4, OtherType>& value);
            template<typename OtherType, TEMPLATE_ENABLE(is_castable<OtherType, type>)>
            constexpr vector& operator*=(OtherType value);
            template<typename OtherType, TEMPLATE_ENABLE(is_castable<OtherType, type>)>
            constexpr vector& operator/=(OtherType value);

            constexpr vector& operator++();
            constexpr vector& operator--();
            constexpr vector operator++(int);
            constexpr vector operator--(int);

            template<typename OtherType>
            constexpr bool operator==(const vector<4, OtherType>& value) const
            {
                return (x == static_cast<type>(value.x)) && (y == static_cast<type>(value.y)) && (z == static_cast<type>(value.z)) && (w == static_cast<type>(value.w));
            }
            template<typename OtherType>
            constexpr bool operator!=(const vector<4, OtherType>& value) const { return !this->operator==(value); }

            jstring toString() const { return JSTR("{ ") + TO_JSTR(x) + JSTR("; ") + TO_JSTR(y) + JSTR("; ") + TO_JSTR(z) + JSTR("; ") + TO_JSTR(w) + JSTR(" }"); }
        };

        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator+(const vector<4, Type1>& value1, Type2 value2);
        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator+(Type1 value1, const vector<4, Type2>& value2);
        template<typename Type1, typename Type2>
        constexpr vector<4, Type1> operator+(const vector<4, Type1>& value1, const vector<4, Type2>& value2);

        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator-(const vector<4, Type1>& value1, Type2 value2);
        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator-(Type1 value1, const vector<4, Type2>& value2);
        template<typename Type1, typename Type2>
        constexpr vector<4, Type1> operator-(const vector<4, Type1>& value1, const vector<4, Type2>& value2);

        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator*(const vector<4, Type1>& value1, Type2 value2);
        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator*(Type1 value1, const vector<4, Type2>& value2);

        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator/(const vector<4, Type1>& value1, Type2 value2);
        template<typename Type1, typename Type2, TEMPLATE_ENABLE(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator/(Type1 value1, const vector<4, Type2>& value2);



        template<typename Type>
        template<typename OtherType, TEMPLATE_ENABLE_IMPL(is_castable<OtherType, Type>)>
        constexpr vector<4, Type>& vector<4, Type>::operator+=(OtherType value)
        {
            x += static_cast<Type>(value); y += static_cast<Type>(value); z += static_cast<Type>(value); w += static_cast<Type>(value);
            return *this;
        }
        template<typename Type>
        template<typename OtherType>
        constexpr vector<4, Type>& vector<4, Type>::operator+=(const vector<4, OtherType>& value)
        {
            x += static_cast<Type>(value.x); y += static_cast<Type>(value.y); z += static_cast<Type>(value.z); w += static_cast<Type>(value.w);
            return *this;
        }
        template<typename Type>
        template<typename OtherType, TEMPLATE_ENABLE_IMPL(is_castable<OtherType, Type>)>
        constexpr vector<4, Type>& vector<4, Type>::operator-=(OtherType value)
        {
            x -= static_cast<Type>(value); y -= static_cast<Type>(value); z -= static_cast<Type>(value); w -= static_cast<Type>(value);
            return *this;
        }
        template<typename Type>
        template<typename OtherType>
        constexpr vector<4, Type>& vector<4, Type>::operator-=(const vector<4, OtherType>& value)
        {
            x -= static_cast<Type>(value.x); y -= static_cast<Type>(value.y); z -= static_cast<Type>(value.z); w -= static_cast<Type>(value.w);
            return *this;
        }
        template<typename Type>
        template<typename OtherType, TEMPLATE_ENABLE_IMPL(is_castable<OtherType, Type>)>
        constexpr vector<4, Type>& vector<4, Type>::operator*=(OtherType value)
        {
            x *= static_cast<Type>(value); y *= static_cast<Type>(value); z *= static_cast<Type>(value); w *= static_cast<Type>(value);
            return *this;
        }
        template<typename Type>
        template<typename OtherType, TEMPLATE_ENABLE_IMPL(is_castable<OtherType, Type>)>
        constexpr vector<4, Type>& vector<4, Type>::operator/=(OtherType value)
        {
            x /= static_cast<Type>(value); y /= static_cast<Type>(value); z /= static_cast<Type>(value); w /= static_cast<Type>(value);
            return *this;
        }

        template<typename Type>
        constexpr vector<4, Type>& vector<4, Type>::operator++()
        {
            ++x; ++y; ++z; ++w;
            return *this;
        }
        template<typename Type>
        constexpr vector<4, Type>& vector<4, Type>::operator--()
        {
            --x; --y; --z; --w;
            return *this;
        }
        template<typename Type>
        constexpr vector<4, Type> vector<4, Type>::operator++(int)
        {
            vector<4, Type> temp = *this;
            ++x; ++y; ++z; ++w;
            return temp;
        }
        template<typename Type>
        constexpr vector<4, Type> vector<4, Type>::operator--(int)
        {
            vector<4, Type> temp = *this;
            --x; --y; --z; --w;
            return temp;
        }

        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator+(const vector<4, Type1>& value1, const Type2 value2)
        {
            return { value1.x + static_cast<Type1>(value2), value1.y + static_cast<Type1>(value2), value1.z + static_cast<Type1>(value2), value1.w + static_cast<Type1>(value2) };
        }
        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator+(const Type1 value1, const vector<4, Type2>& value2)
        {
            return { value1 + static_cast<Type1>(value2.x), value1 + static_cast<Type1>(value2.y), value1 + static_cast<Type1>(value2.z), value1 + static_cast<Type1>(value2.w) };
        }
        template<typename Type1, typename Type2>
        constexpr vector<4, Type1> operator+(const vector<4, Type1>& value1, const vector<4, Type2>& value2)
        {
            return { value1.x + static_cast<Type1>(value2.x), value1.y + static_cast<Type1>(value2.y), value1.z + static_cast<Type1>(value2.z), value1.w + static_cast<Type1>(value2.w) };
        }

        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator-(const vector<4, Type1>& value1, const Type2 value2)
        {
            return { value1.x - static_cast<Type1>(value2), value1.y - static_cast<Type1>(value2), value1.z - static_cast<Type1>(value2), value1.w - static_cast<Type1>(value2) };
        }
        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator-(const Type1 value1, const vector<4, Type2>& value2)
        {
            return { value1 - static_cast<Type1>(value2.x), value1 - static_cast<Type1>(value2.y), value1 - static_cast<Type1>(value2.z), value1 - static_cast<Type1>(value2.w) };
        }
        template<typename Type1, typename Type2>
        constexpr vector<4, Type1> operator-(const vector<4, Type1>& value1, const vector<4, Type2>& value2)
        {
            return { value1.x - static_cast<Type1>(value2.x), value1.y - static_cast<Type1>(value2.y), value1.z - static_cast<Type1>(value2.z), value1.w - static_cast<Type1>(value2.w) };
        }

        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator*(const vector<4, Type1>& value1, const Type2 value2)
        {
            return { value1.x * static_cast<Type1>(value2), value1.y * static_cast<Type1>(value2), value1.z * static_cast<Type1>(value2), value1.w * static_cast<Type1>(value2) };
        }
        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator*(const Type1 value1, const vector<4, Type2>& value2)
        {
            return { value1 * static_cast<Type1>(value2.x), value1 * static_cast<Type1>(value2.y), value1 * static_cast<Type1>(value2.z), value1 * static_cast<Type1>(value2.w) };
        }

        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator/(const vector<4, Type1>& value1, const Type2 value2)
        {
            return { value1.x / static_cast<Type1>(value2), value1.y / static_cast<Type1>(value2), value1.z / static_cast<Type1>(value2), value1.w / static_cast<Type1>(value2) };
        }
        template<typename Type1, typename Type2, TEMPLATE_ENABLE_IMPL(is_castable<Type2, Type1>)>
        constexpr vector<4, Type1> operator/(const Type1 value1, const vector<4, Type2>& value2)
        {
            return { value1 / static_cast<Type1>(value2.x), value1 / static_cast<Type1>(value2.y), value1 / static_cast<Type1>(value2.z), value1 / static_cast<Type1>(value2.w) };
        }
    }
}
