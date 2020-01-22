#pragma once

namespace ien
{
    template<typename T>
    struct rect
    {
        static_assert(
            std::is_integral_v<T> || std::is_floating_point_v<T>,
            "rect only admits integral or floating point template types"
        );

        T x = static_cast<T>(0);
        T y = static_cast<T>(0);
        T w = static_cast<T>(0);
        T h = static_cast<T>(0);

        constexpr rect() { }
        
        constexpr rect(T vx, T vy, T vw, T vh) 
            : x(vx)
            , y(vy)
            , w(vw)
            , h(vh)
        { }

        constexpr T area()
        {
            return w * h;
        }
    };
}