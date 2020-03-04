#include <doctest/doctest.h>

#include <klein/klein.hpp>

using namespace kln;

TEST_CASE("multivector-gp")
{
    SUBCASE("plane*plane")
    {
        // d*e_0 + a*e_1 + b*e_2 + c*e_3
        plane p1{1.f, 2.f, 3.f, 4.f};
        plane p2{2.f, 3.f, -1.f, -2.f};
        motor p12 = p1 * p2;
        CHECK_EQ(p12.scalar(), 5.f);
        CHECK_EQ(p12.e12(), -1.f);
        CHECK_EQ(p12.e31(), 7.f);
        CHECK_EQ(p12.e23(), -11.f);
        CHECK_EQ(p12.e01(), 10.f);
        CHECK_EQ(p12.e02(), 16.f);
        CHECK_EQ(p12.e03(), 2.f);
        CHECK_EQ(p12.e0123(), 0.f);
    }

    SUBCASE("plane*point")
    {
        // d*e_0 + a*e_1 + b*e_2 + c*e_3
        plane p1{1.f, 2.f, 3.f, 4.f};
        // x*e_032 + y*e_013 + z*e_021 + e_123
        point p2{-2.f, 1.f, 4.f};

        motor p1p2 = p1 * p2;
        CHECK_EQ(p1p2.scalar(), 0.f);
        CHECK_EQ(p1p2.e01(), -5.f);
        CHECK_EQ(p1p2.e02(), 10.f);
        CHECK_EQ(p1p2.e03(), -5.f);
        CHECK_EQ(p1p2.e12(), 3.f);
        CHECK_EQ(p1p2.e31(), 2.f);
        CHECK_EQ(p1p2.e23(), 1.f);
        CHECK_EQ(p1p2.e0123(), 16.f);
    }

    SUBCASE("line*line")
    {
        // a*e01 + b*e02 + c*e03 + d*e23 + e*e31 + f*e12
        line l1{1.f, 0.f, 0.f, 3.f, 2.f, 1.f};
        line l2{0.f, 1.f, 0.f, 4.f, 1.f, -2.f};

        motor l1l2 = l1 * l2;
        CHECK_EQ(l1l2.scalar(), -12.f);
        CHECK_EQ(l1l2.e12(), 5.f);
        CHECK_EQ(l1l2.e31(), -10.f);
        CHECK_EQ(l1l2.e23(), 5.f);
        CHECK_EQ(l1l2.e01(), 1.f);
        CHECK_EQ(l1l2.e02(), -2.f);
        CHECK_EQ(l1l2.e03(), -4.f);
        CHECK_EQ(l1l2.e0123(), 6.f);
    }

    SUBCASE("point*plane")
    {
        // x*e_032 + y*e_013 + z*e_021 + e_123
        point p1{-2.f, 1.f, 4.f};
        // d*e_0 + a*e_1 + b*e_2 + c*e_3
        plane p2{1.f, 2.f, 3.f, 4.f};

        motor p1p2 = p1 * p2;
        CHECK_EQ(p1p2.scalar(), 0.f);
        CHECK_EQ(p1p2.e01(), -5.f);
        CHECK_EQ(p1p2.e02(), 10.f);
        CHECK_EQ(p1p2.e03(), -5.f);
        CHECK_EQ(p1p2.e12(), 3.f);
        CHECK_EQ(p1p2.e31(), 2.f);
        CHECK_EQ(p1p2.e23(), 1.f);
        CHECK_EQ(p1p2.e0123(), -16.f);
    }

    SUBCASE("point*point")
    {
        // x*e_032 + y*e_013 + z*e_021 + e_123
        point p1{1.f, 2.f, 3.f};
        point p2{-2.f, 1.f, 4.f};

        motor p1p2 = p1 * p2;
        CHECK_EQ(p1p2.scalar(), -1.f);
        CHECK_EQ(p1p2.e12(), 0.f);
        CHECK_EQ(p1p2.e31(), 0.f);
        CHECK_EQ(p1p2.e23(), 0.f);
        CHECK_EQ(p1p2.e0123(), 0.f);
        CHECK_EQ(p1p2.e01(), 3.f);
        CHECK_EQ(p1p2.e02(), 1.f);
        CHECK_EQ(p1p2.e03(), -1.f);
    }

    SUBCASE("rotor*translator")
    {
        rotor r;
        r.p1_ = _mm_set_ps(1.f, 0, 0, 1.f);
        translator t;
        t.p2_   = _mm_set_ps(1.f, 0, 0, 0.f);
        motor m = r * t;
        CHECK_EQ(m.scalar(), 1.f);
        CHECK_EQ(m.e01(), 0.f);
        CHECK_EQ(m.e02(), 0.f);
        CHECK_EQ(m.e03(), 1.f);
        CHECK_EQ(m.e23(), 0.f);
        CHECK_EQ(m.e31(), 0.f);
        CHECK_EQ(m.e12(), 1.f);
        CHECK_EQ(m.e0123(), 1.f);
    }

    SUBCASE("translator*rotor")
    {
        rotor r;
        r.p1_ = _mm_set_ps(1.f, 0, 0, 1.f);
        translator t;
        t.p2_   = _mm_set_ps(1.f, 0, 0, 0.f);
        motor m = t * r;
        CHECK_EQ(m.scalar(), 1.f);
        CHECK_EQ(m.e01(), 0.f);
        CHECK_EQ(m.e02(), 0.f);
        CHECK_EQ(m.e03(), 1.f);
        CHECK_EQ(m.e23(), 0.f);
        CHECK_EQ(m.e31(), 0.f);
        CHECK_EQ(m.e12(), 1.f);
        CHECK_EQ(m.e0123(), 1.f);
    }

    SUBCASE("motor*motor")
    {
        motor m1{2, 3, 4, 5, 6, 7, 8, 9};
        motor m2{6, 7, 8, 9, 10, 11, 12, 13};
        motor m3 = m1 * m2;
        CHECK_EQ(m3.scalar(), -86.f);
        CHECK_EQ(m3.e23(), 36.f);
        CHECK_EQ(m3.e31(), 32.f);
        CHECK_EQ(m3.e12(), 52.f);
        CHECK_EQ(m3.e01(), -38.f);
        CHECK_EQ(m3.e02(), -76.f);
        CHECK_EQ(m3.e03(), -66.f);
        CHECK_EQ(m3.e0123(), 384.f);
    }
}