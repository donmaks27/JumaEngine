// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include <string>

#include "int_defines.h"
#include "jmath.h"

namespace jutils
{
    template<typename Allocator = std::allocator<char>>
    class jstring_base : std::basic_string<char, std::char_traits<char>, Allocator>
    {
        using base_class = std::basic_string<char, std::char_traits<char>, Allocator>;

    public:

        using type = char;
        using traits_type = std::char_traits<char>;
        using allocator_type = Allocator;
        using iterator = typename base_class::iterator;
        using const_iterator = typename base_class::const_iterator;

        jstring_base()
            : base_class()
        {}
        explicit jstring_base(const allocator_type& alloc)
            : base_class(alloc)
        {}
        jstring_base(const type* const str, const allocator_type& alloc = allocator_type())
            : base_class(str, alloc)
        {}
        jstring_base(const type* const str, const int32 count, const allocator_type& alloc = allocator_type())
            : base_class(str, math::max(0, count), alloc)
        {}
        jstring_base(const int32 count, const type character, const allocator_type& alloc = allocator_type())
            : base_class(math::max(0, count), character, alloc)
        {}
        jstring_base(const jstring_base& value)
            : base_class(value)
        {}
        jstring_base(const jstring_base& value, const allocator_type& alloc)
            : base_class(value, alloc)
        {}
        jstring_base(jstring_base&& value) noexcept
            : base_class(std::move(value))
        {}
        jstring_base(jstring_base&& value, const allocator_type& alloc)
            : base_class(std::move(value), alloc)
        {}
        
        jstring_base& operator=(const type* const str)
        {
            this->base_class::operator=(str);
            return *this;
        }
        jstring_base& operator=(const type character)
        {
            this->base_class::operator=(character);
            return *this;
        }
        jstring_base& operator=(const jstring_base& value)
        {
            this->base_class::operator=(value);
            return *this;
        }
        jstring_base& operator=(jstring_base&& value) noexcept
        {
            this->base_class::operator=(std::move(value));
            return *this;
        }

        jstring_base& operator+=(const type character)
        {
            this->base_class::operator+=(character);
            return *this;
        }
        jstring_base& operator+=(const type* const value)
        {
            this->base_class::operator+=(value);
            return *this;
        }
        jstring_base& operator+=(const jstring_base& value)
        {
            this->base_class::operator+=(value);
            return *this;
        }

        std::basic_istream<type, traits_type>& readLine(std::basic_istream<type, traits_type>& stream)
        {
            base_class temp;
            getline(stream, temp);
            assign(temp.c_str());
            return stream;
        }

        int32 getSize() const { return static_cast<int32>(this->base_class::length()); }
        bool isValidIndex(const int32 index) const { return (index >= 0) && (index < getSize()); }
        bool isEmpty() const { return this->base_class::empty(); }

        const type* getString() const noexcept { return this->base_class::c_str(); }
        const type* operator*() const noexcept { return getString(); }

        type& get(const int32 index)
        {
            if (index < 0)
            {
                throw_out_of_range();
            }
            return this->base_class::at(index);
        }
        const type& get(const int32 index) const
        {
            if (index < 0)
            {
                throw_out_of_range();
            }
            return this->base_class::at(index);
        }
        type& operator[](const int32 index) { return get(index); }
        const type& operator[](const int32 index) const { return get(index); }

        int32 indexOf(const type character, const int32 startIndex = 0, const int32 finishIndex = -1) const
        {
            const size_t index = this->base_class::find(character, math::max(0, startIndex));
            return (index != base_class::npos) && ((finishIndex == -1) || (index <= finishIndex)) ? static_cast<int32>(index) : -1;
        }
        int32 indexOf(const type* const str, const int32 startIndex = 0, const int32 finishIndex = -1) const
        {
            const size_t index = this->base_class::find(str, math::max(0, startIndex));
            return (index != base_class::npos) && ((finishIndex == -1) || (index <= finishIndex)) ? static_cast<int32>(index) : -1;
        }
        int32 indexOf(const type* const str, const int32 strLength, const int32 startIndex, const int32 finishIndex) const
        {
            if (strLength <= 0)
            {
                return -1;
            }
            const size_t index = this->base_class::find(str, math::max(0, startIndex), strLength);
            return (index != base_class::npos) && ((finishIndex == -1) || (index <= finishIndex)) ? static_cast<int32>(index) : -1;
        }
        int32 indexOf(const jstring_base& value, const int32 startIndex = 0, const int32 finishIndex = -1) { return indexOf(value.getString(), value.getSize(), startIndex, finishIndex); }

        bool contains(const type character, const int32 startIndex = 0, const int32 finishIndex = -1) const { return indexOf(character, startIndex, finishIndex) != -1; }
        bool contains(const type* const str, const int32 startIndex = 0, const int32 finishIndex = -1) const { return indexOf(str, startIndex, finishIndex) != -1; }
        bool contains(const type* const str, const int32 strLength, const int32 startIndex, const int32 finishIndex) const { return indexOf(str, strLength, startIndex, finishIndex) != -1; }
        bool contains(const jstring_base& value, const int32 startIndex = 0, const int32 finishIndex = -1) { return indexOf(value.getString(), value.getSize(), startIndex, finishIndex); }

        jstring_base substr(const int32 startIndex = 0, const int32 length = -1) const
        {
            if (length == 0)
            {
                return "";
            }
            return this->base_class::substr(startIndex, length > 0 ? length : base_class::npos);
        }

        int32 compare(const type* const str) const { return this->base_class::compare(str); }
        int32 compare(const jstring_base& value) const { return this->base_class::compare(value); }

        jstring_base& assign(const type character) { return this->operator=(character); }
        jstring_base& assign(const type* const str) { return this->operator=(str); }
        jstring_base& assign(const type* const str, const int32 strLength)
        {
            if (strLength <= 0)
            {
                clear();
            }
            else
            {
                this->base_class::assign(str, strLength);
            }
            return *this;
        }
        jstring_base& assign(const jstring_base& value) { return this->operator=(value); }
        jstring_base& assign(jstring_base&& value) { return this->operator=(value); }

        jstring_base& add(const type character) { return this->operator+=(character); }
        jstring_base& add(const type* const str) { return this->operator+=(str); }
        jstring_base& add(const type* const str, const int32 strLength)
        {
            if (strLength > 0)
            {
                this->base_class::append(str, strLength);
            }
            return *this;
        }
        jstring_base& add(const jstring_base& value) { return this->operator+=(value); }

        jstring_base& addAt(const int32 index, const type character)
        {
            if (index < 0)
            {
                throw_out_of_range();
            }
            return this->base_class::insert(index, 1, character);
        }
        jstring_base& addAt(const int32 index, const type* const str)
        {
            if (index < 0)
            {
                throw_out_of_range();
            }
            return this->base_class::insert(index, str);
        }
        jstring_base& addAt(const int32 index, const type* const str, const int32 strLength)
        {
            if (index < 0)
            {
                throw_out_of_range();
            }
            if (strLength > 0)
            {
                this->base_class::insert(index, str, strLength);
            }
            return *this;
        }
        jstring_base& addAt(const int32 index, const jstring_base& value)
        {
            if (index < 0)
            {
                throw_out_of_range();
            }
            this->base_class::insert(index, value);
            return *this;
        }

        void reserve(const int32 size) { this->base_class::reserve(static_cast<size_t>(size)); }
        void resize(const int32 size, const type character = type()) { this->base_class::resize(static_cast<size_t>(size), character); }

        void removeAt(const int32 index, const int32 count = 1)
        {
            if (isValidIndex(index) && (count > 0))
            {
                this->base_class::erase(index);
            }
        }

        void clear() { this->base_class::clear(); }

        iterator begin() { return this->base_class::begin(); }
        iterator end() { return this->base_class::end(); }

        const_iterator begin() const { return this->base_class::begin(); }
        const_iterator end() const { return this->base_class::end(); }

    private:

        static void throw_out_of_range()
        {
            throw std::out_of_range("Invalid jstring_base<T> index!");
        }
    };

    template<typename Allocator>
    jstring_base<Allocator> operator+(const jstring_base<Allocator>& str1, const jstring_base<Allocator>& str2) { return jstring_base<Allocator>(str1).add(str2); }
    template<typename Allocator>
    jstring_base<Allocator> operator+(const jstring_base<Allocator>& str1, const typename jstring_base<Allocator>::type* str2) { return jstring_base<Allocator>(str1).add(str2); }
    template<typename Allocator>
    jstring_base<Allocator> operator+(const typename jstring_base<Allocator>::type* const str1, const jstring_base<Allocator>& str2) { return jstring_base<Allocator>(str1) + str2; }
    
    template<typename Allocator>
    bool operator==(const jstring_base<Allocator>& str1, const jstring_base<Allocator>& str2) { return str1.compare(str2) == 0; }
    template<typename Allocator>
    bool operator==(const jstring_base<Allocator>& str1, const typename jstring_base<Allocator>::type* const str2) { return str1.compare(str2) == 0; }
    template<typename Allocator>
    bool operator==(const typename jstring_base<Allocator>::type* const str1, const jstring_base<Allocator>& str2) { return str2 == str1; }
    
    template<typename Allocator>
    bool operator!=(const jstring_base<Allocator>& str1, const jstring_base<Allocator>& str2) { return !(str1 == str2); }
    template<typename Allocator>
    bool operator!=(const jstring_base<Allocator>& str1, const typename jstring_base<Allocator>::type* const str2) { return !(str1 == str2); }
    template<typename Allocator>
    bool operator!=(const typename jstring_base<Allocator>::type* const str1, const jstring_base<Allocator>& str2) { return !(str1 == str2); }

    template<typename Allocator>
    bool operator<(const jstring_base<Allocator>& str1, const jstring_base<Allocator>& str2) { return str1.compare(str2) < 0; }
    template<typename Allocator>
    bool operator<(const jstring_base<Allocator>& str1, const typename jstring_base<Allocator>::type* const str2) { return str1.compare(str2) < 0; }
    template<typename Allocator>
    bool operator<(const typename jstring_base<Allocator>::type* const str1, const jstring_base<Allocator>& str2) { return str2.compare(str1) > 0; }
    
    template<typename Allocator>
    bool operator>(const jstring_base<Allocator>& str1, const jstring_base<Allocator>& str2) { return str1.compare(str2) > 0; }
    template<typename Allocator>
    bool operator>(const jstring_base<Allocator>& str1, const typename jstring_base<Allocator>::type* const str2) { return str1.compare(str2) > 0; }
    template<typename Allocator>
    bool operator>(const typename jstring_base<Allocator>::type* const str1, const jstring_base<Allocator>& str2) { return str2.compare(str1) < 0; }

    template<typename Allocator>
    std::basic_ostream<typename jstring_base<Allocator>::type, typename jstring_base<Allocator>::traits_type>& operator<<(
        std::basic_ostream<typename jstring_base<Allocator>::type, typename jstring_base<Allocator>::traits_type>& stream, 
        const jstring_base<Allocator>& str
    ) { return stream << str.getString(); }

    template<typename Allocator>
    std::basic_istream<typename jstring_base<Allocator>::type, typename jstring_base<Allocator>::traits_type>& getline(
        std::basic_istream<typename jstring_base<Allocator>::type, typename jstring_base<Allocator>::traits_type>& stream,
        jstring_base<Allocator>& str
    ) { return str.readLine(stream); }

    using jstring = jstring_base<>;

    template<typename T>
    jstring to_jstring(T value) { return std::to_string(value).c_str(); }
}

#define JSTR(str) u8 ## str
#define TO_JSTR(value) jutils::to_jstring(value)
