/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <cstddef>
#include <vector>
#include "gtest/gtest.h"
#include "test_fixture_view.hpp"

namespace xf
{
    // abscissa: { "f", "g", "h", "m", "n" }
    // ordinate: { 1, 4, 6 }
    inline coordinate_view_type build_coordinate_view(const coordinate_type& c)
    {
        using map_type = typename coordinate_view_type::map_type;
        map_type nmap;

        auto r = range("f", "n");
        auto sr = range(1, 6, 2);

        const auto& abscissa = c["abscissa"];
        const auto& ordinate = c["ordinate"];

        nmap.emplace(std::make_pair("abscissa", axis_view_type(abscissa, r.build_islice(abscissa))));
        nmap.emplace(std::make_pair("ordinate", axis_view_type(ordinate, sr.build_islice(ordinate))));

        return coordinate_view(std::move(nmap));
    }

    TEST(xcoordinate_view, size)
    {
        auto c = make_test_view_coordinate();
        auto cv = build_coordinate_view(c);

        EXPECT_EQ(2u, cv.size());
        EXPECT_FALSE(cv.empty());
    }

    TEST(xcoordinate_view, contains)
    {
        auto c = make_test_view_coordinate();
        auto cv = build_coordinate_view(c);

        EXPECT_TRUE(cv.contains("abscissa"));
        EXPECT_FALSE(cv.contains("humidity"));
    }

    TEST(xcoordinate_view, access)
    {
        auto c = make_test_view_coordinate();
        auto cv = build_coordinate_view(c);

        EXPECT_EQ(c["abscissa"]["f"], 3);
        EXPECT_EQ(c["abscissa"]["g"], 4);
        EXPECT_EQ(c["abscissa"]["h"], 5);
        EXPECT_EQ(c["abscissa"]["m"], 6);
        EXPECT_EQ(c["abscissa"]["n"], 7);
        EXPECT_EQ(c["ordinate"][1], 0);
        EXPECT_EQ(c["ordinate"][4], 2);
        EXPECT_EQ(c["ordinate"][6], 4);

        EXPECT_EQ(c[std::make_pair("abscissa", "f")], 3);
        EXPECT_EQ(c[std::make_pair("abscissa", "g")], 4);
        EXPECT_EQ(c[std::make_pair("abscissa", "h")], 5);
        EXPECT_EQ(c[std::make_pair("abscissa", "m")], 6);
        EXPECT_EQ(c[std::make_pair("abscissa", "n")], 7);
        EXPECT_EQ(c[std::make_pair("ordinate", 1)], 0);
        EXPECT_EQ(c[std::make_pair("ordinate", 4)], 2);
        EXPECT_EQ(c[std::make_pair("ordinate", 6)], 4);
    }

    TEST(xcoordinate_view, iterator)
    {
        auto c = make_test_view_coordinate();
        auto cv = build_coordinate_view(c);

        auto iter = cv.cbegin();
        EXPECT_EQ((iter->second)["f"], 3);
        ++iter;
        EXPECT_EQ((iter->second)[1], 0);
        ++iter;
        EXPECT_EQ(iter, cv.cend());
    }

    TEST(xcoordinate_view, key_iterator)
    {
        auto c = make_test_view_coordinate();
        auto cv = build_coordinate_view(c);

        auto iter = cv.key_begin();
        EXPECT_EQ(*iter, "abscissa");
        ++iter;
        EXPECT_EQ(*iter, "ordinate");
        ++iter;
        EXPECT_EQ(iter, cv.key_end());
    }
}