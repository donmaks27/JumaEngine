// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "jmap.h"
#include "id_generator.h"

namespace jutils
{
    template<typename ID_Type, typename V, TEMPLATE_ENABLE(std::is_integral_v<ID_Type>)>
    class jmap_auto_id
    {
    public:
        using iterator = typename jmap<ID_Type, V>::iterator;
        using const_iterator = typename jmap<ID_Type, V>::const_iterator;

        jmap_auto_id(bool useOnlyUniqueIDs = false)
            : m_UseOnlyUniqueIDs(useOnlyUniqueIDs)
        {}
        jmap_auto_id(const jmap_auto_id& map) = default;

        jarray<ID_Type> getIDs() const { return m_Map.getKeys(); }

        V& get(const ID_Type id) { return m_Map.get(id); }
        const V& get(const ID_Type id) const { return m_Map.get(id); }
        V& operator[](const ID_Type id) { return get(id); }
        const V& operator[](const ID_Type id) const { return get(id); }

        V* find(const ID_Type id) { return m_Map.find(id); }
        const V* find(const ID_Type id) const { return m_Map.find(id); }
		template<typename Pred>
		jarray<ID_Type> findByPredicate(Pred predicate) const { return m_Map.findByPredicate(predicate); }
		template<typename Pred>
		ID_Type findFirstByPredicate(Pred predicate) const
        {
            const ID_Type* id = m_Map.findFirstByPredicate(predicate);
            return id != nullptr ? *id : 0;
        }

        bool contains(const ID_Type id) const { return m_Map.contains(id); }
        template<typename Pred>
		bool containsByPredicate(Pred predicate) const { return m_Map.containsByPredicate(predicate); }

        ID_Type add(const V& value)
        {
            const ID_Type id = m_UseOnlyUniqueIDs ? m_IDGenerator.generateUniqueID() : m_IDGenerator.generateID();
            m_Map.set(id, value);
            return id;
        }

        void remove(const ID_Type id)
        {
            if (contains(id))
            {
                m_Map.remove(id);
                m_IDGenerator.makeIDUnused(id);
            }
        }
		template<typename Pred>
		void removeByPredicate(Pred predicate)
        {
            jarray<ID_Type> ids = findByPredicate(predicate);
            for (auto& id : ids)
            {
                remove(id);
            }
        }

        void clear()
        {
            m_Map.clear();
            m_IDGenerator.clear();
        }

        iterator begin() { return m_Map.begin(); }
        iterator end() { return m_Map.end(); }

        const_iterator begin() const { return m_Map.begin(); }
        const_iterator end() const { return m_Map.end(); }

    private:

        bool m_UseOnlyUniqueIDs = false;
        id_generator<ID_Type> m_IDGenerator;
        jmap<ID_Type, V> m_Map;
    };
}
