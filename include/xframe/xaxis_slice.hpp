/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XAXIS_SLICE_HPP
#define XFRAME_XAXIS_SLICE_HPP

#include <cmath>
#include <xtl/xvariant.hpp>

#include "xframe_config.hpp"

namespace xf
{
    // TODO: move additional features to slices of xtensor
    // and remove xaxis_iXXX

    /****************
     * xaxis_irange *
     ****************/

    template <class T>
    class xaxis_irange
    {
    public:

        using size_type = T;

        xaxis_irange() = default;
        xaxis_irange(size_type min_val, size_type max_val) noexcept;

        size_type size() const noexcept;
        bool contains(size_type i) const noexcept;

        size_type operator()(size_type i) const noexcept;
        size_type step_size(size_type i, size_type n = 1) const noexcept;

        size_type revert_index(size_type i) const noexcept;

    private:

        size_type m_min;
        size_type m_size;
    };

    /***********************
     * xaxis_istepped_range *
     ***********************/

    template <class T>
    class xaxis_istepped_range
    {
    public:

        using size_type = T;

        xaxis_istepped_range() = default;
        xaxis_istepped_range(size_type min_val, size_type max_val, size_type step) noexcept;

        size_type size() const noexcept;
        bool contains(size_type i) const noexcept;

        size_type operator()(size_type i) const noexcept;
        size_type step_size(size_type i, size_type n = 1) const noexcept;

        size_type revert_index(size_type i) const noexcept;

    private:

        size_type m_min;
        size_type m_step;
        size_type m_size;
    };

    /**************
     * xaxis_iall *
     **************/

    template <class T>
    class xaxis_iall
    {
    public:

        using size_type = T;

        xaxis_iall() = default;
        explicit xaxis_iall(size_type size) noexcept;

        size_type size() const noexcept;
        bool contains(size_type i) const noexcept;

        size_type operator()(size_type i) const noexcept;
        size_type step_size(size_type i, size_type n = 1) const noexcept;

        size_type revert_index(size_type i) const noexcept;

    private:

        size_type m_size;
    };

    /****************
     * xaxis_islice *
     ****************/

    template <class T>
    class xaxis_islice
    {
    public:

        using slice_type = xtl::variant<xaxis_irange<T>, xaxis_istepped_range<T>, xaxis_iall<T>>;
        using size_type = T;

        xaxis_islice() = default;
        template <class S>
        xaxis_islice(S&& slice) noexcept;

        size_type size() const noexcept;
        bool contains(size_type i) const noexcept;

        size_type operator()(size_type i) const noexcept;
        size_type step_size(size_type i, size_type n = 1) const noexcept;

        size_type revert_index(size_type i) const noexcept;

    private:

        slice_type m_slice;
    };

    /*******************
     * slice_variant_t *
     *******************/

    template <class L>
    using slice_variant_t = xtl::mpl::cast_t<L, xtl::variant>;

    /***************
     * xaxis_range *
     ***************/
    
    template <class L>
    class xaxis_range
    {
    public:

        using value_type = slice_variant_t<L>;

        xaxis_range(const value_type& first, const value_type& last) noexcept;
        xaxis_range(value_type&& first, value_type&& last) noexcept;

        template <class A>
        using slice_type = xaxis_irange<typename A::mapped_type>;

        template <class A>
        slice_type<A> build_islice(const A& axis) const;

    private:

        value_type m_first;
        value_type m_last;
    };

    /***********************
     * xaxis_stepped_range *
     ***********************/

    template <class L>
    class xaxis_stepped_range
    {
    public:

        using value_type = slice_variant_t<L>;
        using size_type = std::size_t;

        xaxis_stepped_range(const value_type& first, const value_type& last, size_type step) noexcept;
        xaxis_stepped_range(value_type&& first, value_type&& last, size_type step) noexcept;

        template <class A>
        using slice_type = xaxis_istepped_range<typename A::mapped_type>;

        template <class A>
        slice_type<A> build_islice(const A& axis) const;

    private:

        value_type m_first;
        value_type m_last;
        size_type m_step;
    };

    /***************
     * xaxis_slice *
     ***************/

    template <class L = DEFAULT_LABEL_LIST>
    class xaxis_slice
    {
    public:

        using storage_type = xtl::variant<xaxis_range<L>, xaxis_stepped_range<L>>;

        xaxis_slice() = default;
        template <class V>
        xaxis_slice(const V& slice);
        template <class V>
        xaxis_slice(V&& slice);

        template <class A>
        using slice_type = xaxis_islice<typename A::mapped_type>;

        template <class A>
        slice_type<A> build_islice(const A& axis) const;

    private:

        storage_type m_data;
    };

    /********************
     * helper functions *
     ********************/

    template <class L = DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(const slice_variant_t<L>& first, const slice_variant_t<L>& last);

    template <class L = DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(slice_variant_t<L>&& first, slice_variant_t<L>&& last);

    template <class S, class L = DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(const slice_variant_t<L>& first, const slice_variant_t<L>& last, S step);

    template <class S, class L = DEFAULT_LABEL_LIST>
    xaxis_slice<L> range(slice_variant_t<L>&& first, slice_variant_t<L>&& last, S step);

    /*******************************
     * xaxis_irange implementation *
     *******************************/

    template <class T>
    inline xaxis_irange<T>::xaxis_irange(size_type min_val, size_type max_val) noexcept
        : m_min(min_val), m_size(max_val - min_val)
    {
    }

    template <class T>
    inline auto xaxis_irange<T>::size() const noexcept -> size_type
    {
        return m_size;
    }

    template <class T>
    inline bool xaxis_irange<T>::contains(size_type i) const noexcept
    {
        return i >= m_min && i < m_min + m_size;
    }

    template <class T>
    inline auto xaxis_irange<T>::operator()(size_type i) const noexcept -> size_type
    {
        return m_min + i;
    }

    template <class T>
    inline auto xaxis_irange<T>::step_size(size_type, size_type n) const noexcept -> size_type
    {
        return n;
    }

    template <class T>
    inline auto xaxis_irange<T>::revert_index(size_type i) const noexcept -> size_type
    {
        return i - m_min;
    }

    /***************************************
     * xaxis_istepped_range implementation *
     ***************************************/

    template <class T>
    inline xaxis_istepped_range<T>::xaxis_istepped_range(size_type min_val, size_type max_val, size_type step) noexcept
        : m_min(min_val), m_step(step), m_size(size_type(std::ceil(double(max_val - min_val) / double(step))))
    {
    }

    template <class T>
    inline auto xaxis_istepped_range<T>::size() const noexcept -> size_type
    {
        return m_size;
    }

    template <class T>
    inline bool xaxis_istepped_range<T>::contains(size_type i) const noexcept
    {
        return i >= m_min && i < m_min + m_size * m_step && ((i - m_min) % m_step == 0);
    }

    template <class T>
    inline auto xaxis_istepped_range<T>::operator()(size_type i) const noexcept -> size_type
    {
        return m_min + i * m_step;
    }
    
    template <class T>
    inline auto xaxis_istepped_range<T>::step_size(size_type, size_type n) const noexcept -> size_type
    {
        return m_step * n;
    }

    template <class T>
    inline auto xaxis_istepped_range<T>::revert_index(size_type i) const noexcept -> size_type
    {
        return (i - m_min) / m_step;
    }

    /*****************************
     * xaxis_iall implementation *
     *****************************/

    template <class T>
    inline xaxis_iall<T>::xaxis_iall(size_type size) noexcept
        : m_size(size)
    {
    }

    template <class T>
    inline auto xaxis_iall<T>::size() const noexcept -> size_type
    {
        return m_size;
    }

    template <class T>
    inline bool xaxis_iall<T>::contains(size_type i) const noexcept
    {
        return i < m_size;
    }

    template <class T>
    inline auto xaxis_iall<T>::operator()(size_type i) const noexcept -> size_type
    {
        return i;
    }

    template <class T>
    inline auto xaxis_iall<T>::step_size(size_type, size_type n) const noexcept -> size_type
    {
        return n;
    }

    template <class T>
    inline auto xaxis_iall<T>::revert_index(size_type i) const noexcept -> size_type
    {
        return i;
    }

    /*******************************
     * xaxis_islice implementation *
     *******************************/

    template <class T>
    template <class S>
    inline xaxis_islice<T>::xaxis_islice(S&& slice) noexcept
        : m_slice(std::forward<S>(slice))
    {
    }

    template <class T>
    inline auto xaxis_islice<T>::size() const noexcept -> size_type
    {
        return xtl::visit([](auto&& arg) { return arg.size(); }, m_slice);
    }

    template <class T>
    inline bool xaxis_islice<T>::contains(size_type i) const noexcept
    {
        return xtl::visit([i](auto&& arg) { return arg.contains(i); }, m_slice);
    }

    template <class T>
    inline auto xaxis_islice<T>::operator()(size_type i) const noexcept -> size_type
    {
        return xtl::visit([i](auto&& arg) { return arg(i); }, m_slice);
    }

    template <class T>
    inline auto xaxis_islice<T>::step_size(size_type i, size_type n) const noexcept -> size_type
    {
        return xtl::visit([i, n](auto&& arg) { return arg.step_size(i, n); }, m_slice);
    }

    template <class T>
    inline auto xaxis_islice<T>::revert_index(size_type i) const noexcept -> size_type
    {
        return xtl::visit([i](auto&& arg) { return arg.revert_index(i); }, m_slice);
    }

    /******************************
     * xaxis_range implementation *
     ******************************/

    template <class V>
    inline xaxis_range<V>::xaxis_range(const value_type& first, const value_type& last) noexcept
        : m_first(first), m_last(last)
    {
    }

    template <class V>
    inline xaxis_range<V>::xaxis_range(value_type&& first, value_type&& last) noexcept
        : m_first(std::move(first)), m_last(std::move(last))
    {
    }

    template <class V>
    template <class A>
    inline auto xaxis_range<V>::build_islice(const A& axis) const -> slice_type<A>
    {
        return slice_type<A>(axis[m_first], axis[m_last] + 1);
    }

    /**************************************
     * xaxis_stepped_range implementation *
     **************************************/

    template <class V>
    inline xaxis_stepped_range<V>::xaxis_stepped_range(const value_type& first, const value_type& last, size_type step) noexcept
        : m_first(first), m_last(last), m_step(step)
    {
    }

    template <class V>
    inline xaxis_stepped_range<V>::xaxis_stepped_range(value_type&& first, value_type&& last, size_type step) noexcept
        : m_first(std::move(first)), m_last(std::move(last)), m_step(step)
    {
    }

    template <class V>
    template <class A>
    inline auto xaxis_stepped_range<V>::build_islice(const A& axis) const -> slice_type<A>
    {
        return slice_type<A>(axis[m_first], axis[m_last] + 1, m_step);
    }

    /******************************
     * xaxis_slice implementation *
     ******************************/

    template <class L>
    template <class V>
    inline xaxis_slice<L>::xaxis_slice(const V& slice)
        : m_data(slice)
    {
    }

    template <class L>
    template <class V>
    inline xaxis_slice<L>::xaxis_slice(V&& slice)
        : m_data(std::move(slice))
    {
    }

    template <class L>
    template <class A>
    inline auto xaxis_slice<L>::build_islice(const A& axis) const -> slice_type<A>
    {
        return xtl::visit([&axis](auto&& arg) { return slice_type<A>(arg.build_islice(axis)); }, m_data);
    }

    /***********************************
     * helper functions implementation *
     ***********************************/

    template <class L>
    inline xaxis_slice<L> range(const slice_variant_t<L>& first, const slice_variant_t<L>& last)
    {
        return xaxis_slice<L>(xaxis_range<L>(first, last));
    }

    template <class L>
    inline xaxis_slice<L> range(slice_variant_t<L>&& first, slice_variant_t<L>&& last)
    {
        return xaxis_slice<L>(xaxis_range<L>(std::move(first), std::move(last)));
    }

    template <class S, class L>
    inline xaxis_slice<L> range(const slice_variant_t<L>& first, const slice_variant_t<L>& last, S step)
    {
        return xaxis_slice<L>(xaxis_stepped_range<L>(first, last, step));
    }

    template <class S, class L>
    inline xaxis_slice<L> range(slice_variant_t<L>&& first, slice_variant_t<L>&& last, S step)
    {
        return xaxis_slice<L>(xaxis_stepped_range<L>(std::move(first), std::move(last), step));
    }
}

#endif