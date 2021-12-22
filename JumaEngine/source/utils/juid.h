#pragma once

#include "type_defines.h"
#include "type_checks.h"

#include <limits>

namespace jutils
{
    template<typename IdType = uint64, TEMPLATE_ENABLE(std::is_integral_v<IdType>)>
    class juid
    {
    public:

        using id_type = IdType;
        static constexpr id_type invalidUID = 0;

        juid(const bool incrementUID = true)
            : nextUID(incrementUID ? 1 : std::numeric_limits<id_type>::max())
            , isIncrementUID(incrementUID)
        {}
        juid(const juid& value)
            : nextUID(value.nextUID)
            , isIncrementUID(value.isIncrementUID)
        {}

        juid& operator=(const juid& value)
        {
            if (this != &value)
            {
                nextUID = value.nextUID;
                isIncrementUID = value.isIncrementUID;
            }
            return *this;
        }

        id_type getUID()
        {
            if (nextUID == invalidUID)
            {
                return invalidUID;
            }
            if (isIncrementUID && (nextUID == std::numeric_limits<id_type>::max()))
            {
                nextUID = invalidUID;
                return std::numeric_limits<id_type>::max();
            }
            return isIncrementUID ? nextUID++ : nextUID--;
        }

    private:

        id_type nextUID;
        bool isIncrementUID;
    };
}
