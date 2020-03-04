#pragma once

#include "detail/sandwich.hpp"
#include "detail/sse.hpp"

#include "line.hpp"
#include "point.hpp"

namespace kln
{
/// \defgroup plane Planes
///
/// In projective geometry, planes are the fundamental element through which all
/// other entities are constructed. Lines are the meet of two planes, and points
/// are the meet of three planes (equivalently, a line and a plane).
///
/// The plane multivector in PGA looks like
/// $d\mathbf{e}_0 + a\mathbf{e}_1 + b\mathbf{e}_2 + c\mathbf{e}_3$. Points
/// that reside on the plane satisfy the familiar equation
/// $d + ax + by + cz = 0$.

/// \addtogroup plane
/// @{
class plane final
{
public:
    plane() noexcept = default;

    plane(__m128 xmm) noexcept
        : p0_{xmm}
    {}

    /// The constructor performs the rearrangement so the plane can be specified
    /// in the familiar form: ax + by + cz + d
    plane(float a, float b, float c, float d) noexcept
    {
        p0_ = _mm_set_ps(c, b, a, d);
    }

    /// Data should point to four floats with memory layout `(d, a, b, c)` where
    /// `d` occupies the lowest address in memory.
    explicit plane(float* data) noexcept
    {
        p0_ = _mm_loadu_ps(data);
    }

    /// Unaligned load of data. The `data` argument should point to 4 floats
    /// corresponding to the
    /// `(d, a, b, c)` components of the plane multivector where `d` occupies
    /// the lowest address in memory.
    ///
    /// !!! tip
    ///
    ///     This is a faster mechanism for setting data compared to setting
    ///     components one at a time.
    void load(float* data) noexcept
    {
        p0_ = _mm_loadu_ps(data);
    }

    /// Normalize this plane $p$ such that $p \cdot p = 1$.
    ///
    /// In order to compute the cosine of the angle between planes via the
    /// inner product operator `|`, the planes must be normalized. Producing a
    /// normalized rotor between two planes with the geometric product `*` also
    /// requires that the planes are normalized.
    ///
    /// !!! tip
    ///
    ///     Normalization here is done using the `rsqrtps`
    ///     instruction with a maximum relative error of $1.5\times 2^{-12}$.
    void normalize() noexcept
    {
        __m128 inv_norm = _mm_rsqrt_ps(detail::hi_dp_bc(p0_, p0_));
#ifdef KLEIN_SSE_4_1
        inv_norm = _mm_blend_ps(inv_norm, _mm_set_ss(1.f), 1);
#else
        inv_norm = _mm_add_ps(inv_norm, _mm_set_ss(1.f));
#endif
        p0_ = _mm_mul_ps(inv_norm, p0_);
    }

    /// Compute the plane norm, which is often used to compute distances
    /// between points and lines.
    ///
    /// Given a normalized point $P$ and normalized line $\ell$, the plane
    /// $P\vee\ell$ containing both $\ell$ and $P$ will have a norm equivalent
    /// to the distance between $P$ and $\ell$.
    [[nodiscard]] float norm() const noexcept
    {
        float out;
        _mm_store_ss(&out, _mm_rcp_ss(_mm_rsqrt_ss(detail::hi_dp(p0_, p0_))));
        return out;
    }

    /// Reflect another plane $p_2$ through this plane $p_1$. The operation
    /// performed via this call operator is an optimized routine equivalent to
    /// the expression $p_1 p_2 p_1$.
    [[nodiscard]] plane KLN_VEC_CALL operator()(plane const& p) const noexcept
    {
        plane out;
        detail::sw00(p0_, p.p0_, out.p0_);
        return out;
    }

    /// Reflect line $\ell$ through this plane $p$. The operation
    /// performed via this call operator is an optimized routine equivalent to
    /// the expression $p \ell p$.
    [[nodiscard]] line KLN_VEC_CALL operator()(line const& l) const noexcept
    {
        line out;
        detail::sw10(p0_, l.p1_, out.p1_, out.p2_);
        __m128 p2_tmp;
        detail::sw20(p0_, l.p2_, p2_tmp);
        out.p2_ = _mm_add_ps(out.p2_, p2_tmp);
        return out;
    }

    /// Reflect the point $P$ through this plane $p$. The operation
    /// performed via this call operator is an optimized routine equivalent to
    /// the expression $p P p$.
    [[nodiscard]] point KLN_VEC_CALL operator()(point const& p) const noexcept
    {
        point out;
        detail::sw30(p0_, p.p3_, out.p3_);
        return out;
    }

    /// Plane addition
    plane& KLN_VEC_CALL operator+=(plane b) noexcept
    {
        p0_ = _mm_add_ps(p0_, b.p0_);
        return *this;
    }

    /// Plane subtraction
    plane& KLN_VEC_CALL operator-=(plane b) noexcept
    {
        p0_ = _mm_sub_ps(p0_, b.p0_);
        return *this;
    }

    /// Plane uniform scale
    plane& operator*=(float s) noexcept
    {
        p0_ = _mm_mul_ps(p0_, _mm_set1_ps(s));
        return *this;
    }

    /// Plane uniform scale
    plane& operator*=(int s) noexcept
    {
        p0_ = _mm_mul_ps(p0_, _mm_set1_ps(static_cast<float>(s)));
        return *this;
    }

    /// Plane uniform inverse scale
    plane& operator/=(float s) noexcept
    {
        p0_ = _mm_mul_ps(p0_, _mm_rcp_ps(_mm_set1_ps(s)));
        return *this;
    }

    /// Plane uniform inverse scale
    plane& operator/=(int s) noexcept
    {
        p0_ = _mm_mul_ps(p0_, _mm_rcp_ps(_mm_set1_ps(static_cast<float>(s))));
        return *this;
    }

    [[nodiscard]] float x() const noexcept
    {
        float out[4];
        _mm_store_ps(out, p0_);
        return out[1];
    }

    [[nodiscard]] float e1() const noexcept
    {
        return x();
    }

    [[nodiscard]] float y() const noexcept
    {
        float out[4];
        _mm_store_ps(out, p0_);
        return out[2];
    }

    [[nodiscard]] float e2() const noexcept
    {
        return y();
    }

    [[nodiscard]] float z() const noexcept
    {
        float out[4];
        _mm_store_ps(out, p0_);
        return out[3];
    }

    [[nodiscard]] float e3() const noexcept
    {
        return z();
    }

    [[nodiscard]] float d() const noexcept
    {
        float out;
        _mm_store_ss(&out, p0_);
        return out;
    }

    [[nodiscard]] float e0() const noexcept
    {
        return d();
    }

    __m128 p0_;
};

/// Plane addition
[[nodiscard]] inline plane KLN_VEC_CALL operator+(plane a, plane b) noexcept
{
    plane c;
    c.p0_ = _mm_add_ps(a.p0_, b.p0_);
    return c;
}

/// Plane subtraction
[[nodiscard]] inline plane KLN_VEC_CALL operator-(plane a, plane b) noexcept
{
    plane c;
    c.p0_ = _mm_sub_ps(a.p0_, b.p0_);
    return c;
}

/// Plane uniform scale
[[nodiscard]] inline plane KLN_VEC_CALL operator*(plane p, float s) noexcept
{
    plane c;
    c.p0_ = _mm_mul_ps(p.p0_, _mm_set1_ps(s));
    return c;
}

/// Plane uniform scale
[[nodiscard]] inline plane KLN_VEC_CALL operator*(float s, plane p) noexcept
{
    return p * s;
}

/// Plane uniform scale
[[nodiscard]] inline plane KLN_VEC_CALL operator*(plane p, int s) noexcept
{
    return p * static_cast<float>(s);
}

/// Plane uniform scale
[[nodiscard]] inline plane KLN_VEC_CALL operator*(int s, plane p) noexcept
{
    return p * static_cast<float>(s);
}

/// Plane uniform inverse scale
[[nodiscard]] inline plane KLN_VEC_CALL operator/(plane p, float s) noexcept
{
    plane c;
    c.p0_ = _mm_mul_ps(p.p0_, _mm_rcp_ps(_mm_set1_ps(s)));
    return c;
}

/// Plane uniform inverse scale
[[nodiscard]] inline plane KLN_VEC_CALL operator/(plane p, int s) noexcept
{
    return p / static_cast<float>(s);
}

/// Unary minus
[[nodiscard]] inline plane operator-(plane p) noexcept
{
    return {_mm_xor_ps(p.p0_, _mm_set1_ps(-0.f))};
}
} // namespace kln
/// @}
