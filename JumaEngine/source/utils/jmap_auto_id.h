// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "jmap.h"
#include "id_generator.h"

namespace JumaEngine
{
    template<typename ID_Type, typename V, TEMPLATE_ENABLE(std::is_integral_v<ID_Type>)>
    class jmap_auto_id
    {
    public:
        jmap_auto_id(bool useOnlyUniqueIDs = false)
            : m_UseOnlyUniqueIDs(useOnlyUniqueIDs)
        {}
        jmap_auto_id(const jmap_auto_id& map) = default;

        typename jmap<ID_Type, V>::const_iterator begin() const { return m_Map.begin(); }
        typename jmap<ID_Type, V>::iterator begin() { return m_Map.begin(); }
        typename jmap<ID_Type, V>::const_iterator end() const { return m_Map.end(); }
        typename jmap<ID_Type, V>::iterator end() { return m_Map.end(); }

        V* find(const ID_Type id) { return m_Map.findByKey(id); }
        const V* find(const ID_Type id) const { return m_Map.findByKey(id); }
		template<typename Pred>
		jarray<ID_Type> findByPredicate(Pred predicate) const { return m_Map.findByPredicate(predicate); }

        bool contains(const ID_Type id) const { return m_Map.contains(id); }
        template<typename Pred>
		bool containsByPredicate(Pred predicate) const { return m_Map.containsByPredicate(predicate); }

        ID_Type add(const V& value)
        {
            const ID_Type id = m_UseOnlyUniqueIDs ? m_IDGenerator.generateUniqueID() : m_IDGenerator.generateID();
            m_Map.add(id, value);
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
            m_IDGenerator.reset();
        }

    private:

        bool m_UseOnlyUniqueIDs = false;
        id_generator<ID_Type> m_IDGenerator;
        jmap<ID_Type, V> m_Map;
    };
}
