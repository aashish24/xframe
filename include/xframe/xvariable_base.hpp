/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_XVARIABLE_BASE_HPP
#define XFRAME_XVARIABLE_BASE_HPP

#include "xtensor/xnoalias.hpp"

#include "xcoordinate_system.hpp"
#include "xselecting.hpp"

namespace xf
{
    using xt::noalias;

    template <class C , class DM>
    struct enable_xvariable
        : std::enable_if<is_coordinate<C>::value && is_dimension<DM>::value,
                         xt::void_t<>
          >
    {
    };

    template <class C, class DM>
    using enable_xvariable_t = typename enable_xvariable<C, DM>::type;

    template <class D>
    struct xvariable_inner_types;

    template <class D>
    class xvariable_base : private xcoordinate_system<D>
    {
    public:

        using self_type = xvariable_base<D>;
        using derived_type = D;
        using coordinate_base = xcoordinate_system<D>;
        using inner_types = xvariable_inner_types<D>;

        using data_closure_type = typename inner_types::data_closure_type;
        using coordinate_closure_type = typename coordinate_base::coordinate_closure_type;
        static constexpr bool is_data_const = std::is_const<std::remove_reference_t<data_closure_type>>::value;

        using data_type = typename inner_types::data_type;
        using value_type = typename data_type::value_type;
        using reference = std::conditional_t<is_data_const,
                                             typename data_type::const_reference,
                                             typename data_type::reference>;
        using const_reference = typename data_type::const_reference;
        using pointer = std::conditional_t<is_data_const,
                                           typename data_type::const_pointer,
                                           typename data_type::pointer>;
        using const_pointer = typename data_type::const_pointer;
        using size_type = typename data_type::size_type;
        using difference_type = typename data_type::difference_type;

        using coordinate_type = typename coordinate_base::coordinate_type;
        using dimension_type = typename coordinate_base::dimension_type;
        using dimension_list = typename dimension_type::label_list;

        using coordinate_map = typename coordinate_type::map_type;
        using coordinate_initializer = std::initializer_list<typename coordinate_type::value_type>;
        using key_type = typename coordinate_map::key_type;

        template <std::size_t N = dynamic()>
        using selector_traits = xselector_traits<coordinate_type, dimension_type, N>;
        template <std::size_t N = dynamic()>
        using selector_type = typename selector_traits<N>::selector_type;
        template <std::size_t N = dynamic()>
        using selector_map_type = typename selector_traits<N>::selector_map_type;
        template <std::size_t N = dynamic()>
        using iselector_type = typename selector_traits<N>::iselector_type;
        template <std::size_t N = dynamic()>
        using iselector_map_type = typename selector_traits<N>::iselector_map_type;

        static const_reference missing();

        using coordinate_base::size;
        using coordinate_base::dimension;
        using coordinate_base::dimension_labels;
        using coordinate_base::coordinates;
        using coordinate_base::dimension_mapping;
        using coordinate_base::broadcast_coordinates;
        using coordinate_base::broadcast_dimensions;

        void resize(const coordinate_type& coords, const dimension_type& dims);
        void resize(coordinate_type&& coords, dimension_type&& dims);

        void reshape(const coordinate_type& coords, const dimension_type& dims);
        void reshape(coordinate_type&& coords, dimension_type&& dims);

        data_type& data() noexcept;
        const data_type& data() const noexcept;

        template <class... Args>
        reference operator()(Args... args);

        template <class... Args>
        const_reference operator()(Args... args) const;

        template <class It>
        reference element(It first, It last);
        template <class It>
        const_reference element(It first, It last) const;

        template <class... Args>
        reference locate(Args&&... args);

        template <class... Args>
        const_reference locate(Args&&... args) const;

        template <std::size_t N = dynamic()>
        reference select(const selector_map_type<N>& selector);

        template <class Join = DEFAULT_JOIN, std::size_t N = dynamic()>
        const_reference select(const selector_map_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        reference select(selector_map_type<N>&& selector);

        template <class Join = DEFAULT_JOIN, std::size_t N = dynamic()>
        const_reference select(selector_map_type<N>&& selector) const;

        template <std::size_t N = dynamic()>
        reference iselect(const iselector_map_type<N>& selector);

        template <std::size_t N = dynamic()>
        const_reference iselect(const iselector_map_type<N>& selector) const;

        template <std::size_t N = dynamic()>
        reference iselect(iselector_map_type<N>&& selector);

        template <std::size_t N = dynamic()>
        const_reference iselect(iselector_map_type<N>&& selector) const;

    protected:

        xvariable_base() = default;
        xvariable_base(coordinate_initializer coords);
        
        template <class C, class DM>
        xvariable_base(C&& coords, DM&& dims);

        template <class DM>
        xvariable_base(const coordinate_map& coords, DM&& dims);

        template <class DM>
        xvariable_base(coordinate_map&& coords, DM&& dims);

        ~xvariable_base() = default;

        xvariable_base(const xvariable_base&) = default;
        xvariable_base& operator=(const xvariable_base&) = default;

        xvariable_base(xvariable_base&&) = default;
        xvariable_base& operator=(xvariable_base&&) = default;

        typename data_type::shape_type compute_shape() const;

    private:

        static dimension_type make_dimension_mapping(coordinate_initializer coord);

        template <class C, class DM>
        void resize_impl(C&& coords, DM&& dims);

        template <class C, class DM>
        void reshape_impl(C&& coords, DM&& dims);

        template <std::size_t... I, class... Args>
        reference locate_impl(std::index_sequence<I...>, Args&&... args);

        template <std::size_t... I, class... Args>
        const_reference locate_impl(std::index_sequence<I...>, Args&&... args) const;

        template <class S>
        reference select_impl(const S& selector);

        template <class S>
        const_reference select_impl(const S& selector) const;

        template <class S>
        const_reference select_outer(const S& selector) const;

        template <class Join, class S>
        const_reference select_join(const S& selector) const;

        derived_type& derived_cast() noexcept;
        const derived_type& derived_cast() const noexcept;

        //coordinate_closure_type m_coordinate;
        //dimension_type m_dimension_mapping;
    };

    /*********************************
     * xvariable_base implementation *
     *********************************/

    template <class D>
    template <class C, class DM>
    inline xvariable_base<D>::xvariable_base(C&& coords, DM&& dims)
        : coordinate_base(std::forward<C>(coords), std::forward<DM>(dims))
    {
        // TODO: add assertion on data.shape, coordinate and dimension_mapping
        // consistency
    }

    template <class D>
    inline xvariable_base<D>::xvariable_base(coordinate_initializer coords)
        : xvariable_base(coordinate_type(coords), make_dimension_mapping(coords))
    {
    }

    template <class D>
    template <class DM>
    inline xvariable_base<D>::xvariable_base(const coordinate_map& coords, DM&& dims)
        : xvariable_base(coordinate_type(coords), std::forward<DM>(dims))
    {
    }

    template <class D>
    template <class DM>
    inline xvariable_base<D>::xvariable_base(coordinate_map&& coords, DM&& dims)
        : xvariable_base(coordinate_type(std::move(coords)), std::forward<DM>(dims))
    {
    }

    template <class D>
    inline auto xvariable_base<D>::missing() -> const_reference
    {
        return detail::static_missing<const_reference>();
    }
    
    template <class D>
    inline void xvariable_base<D>::resize(const coordinate_type& coords, const dimension_type& dims)
    {
        resize_impl(coords, dims);
    }

    template <class D>
    inline void xvariable_base<D>::resize(coordinate_type&& coords, dimension_type&& dims)
    {
        resize_impl(std::move(coords), std::move(dims));
    }
    
    template <class D>
    inline void xvariable_base<D>::reshape(const coordinate_type& coords, const dimension_type& dims)
    {
        reshape_impl(coords, dims);
    }

    template <class D>
    inline void xvariable_base<D>::reshape(coordinate_type&& coords, dimension_type&& dims)
    {
        reshape_impl(std::move(coords), std::move(dims));
    }

    template <class D>
    inline auto xvariable_base<D>::data() noexcept -> data_type&
    {
        return derived_cast().data_impl();
    }

    template <class D>
    inline auto xvariable_base<D>::data() const noexcept -> const data_type&
    {
        return derived_cast().data_impl();
    }

    template <class D>
    template <class... Args>
    inline auto xvariable_base<D>::operator()(Args... args) -> reference
    {
        return data()(args...);
    }

    template <class D>
    template <class... Args>
    inline auto xvariable_base<D>::operator()(Args... args) const -> const_reference
    {
        return data()(args...);
    }

    template <class D>
    template <class It>
    inline auto xvariable_base<D>::element(It first, It last) -> reference
    {
        return data().element(first, last);
    }

    template <class D>
    template <class It>
    inline auto xvariable_base<D>::element(It first, It last) const -> const_reference
    {
        return data().element(first, last);
    }

    template <class D>
    template <class... Args>
    inline auto xvariable_base<D>::locate(Args&&... args) -> reference
    {
        return locate_impl(std::make_index_sequence<sizeof...(Args)>(), std::forward<Args>(args)...);
    }

    template <class D>
    template <class... Args>
    inline auto xvariable_base<D>::locate(Args&&... args) const -> const_reference
    {
        return locate_impl(std::make_index_sequence<sizeof...(Args)>(), std::forward<Args>(args)...);
    }

    template <class D>
    inline auto xvariable_base<D>::compute_shape() const -> typename data_type::shape_type
    {
        using shape_type = typename data_type::shape_type;
        shape_type shape(dimension());
        for (auto& c : coordinates())
        {
            shape[dimension_mapping()[c.first]] = c.second.size();
        }
        return shape;
    }

    template <class D>
    template <class C, class DM>
    inline void xvariable_base<D>::resize_impl(C&& coords, DM&& dims)
    {
        coordinate_base::resize(std::forward<C>(coords), std::forward<DM>(dims));
        data().resize(compute_shape());
    }

    template <class D>
    template <class C, class DM>
    inline void xvariable_base<D>::reshape_impl(C&& coords, DM&& dims)
    {
        coordinate_base::resize(std::forward<C>(coords), std::forward<DM>(dims));
        data().reshape(compute_shape());
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::select(const selector_map_type<N>& selector) -> reference
    {
        return select_impl(selector_type<N>(selector));
    }

    template <class D>
    template <class Join, std::size_t N>
    inline auto xvariable_base<D>::select(const selector_map_type<N>& selector) const -> const_reference
    {
        return select_join<Join>(selector_type<N>(selector));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::select(selector_map_type<N>&& selector) -> reference
    {
        return select_impl(selector_type<N>(std::move(selector)));
    }

    template <class D>
    template <class Join, std::size_t N>
    inline auto xvariable_base<D>::select(selector_map_type<N>&& selector) const -> const_reference
    {
        return select_join<Join>(selector_type<N>(std::move(selector)));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::iselect(const iselector_map_type<N>& selector) -> reference
    {
        return select_impl(iselector_type<N>(selector));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::iselect(const iselector_map_type<N>& selector) const -> const_reference
    {
        return select_impl(iselector_type<N>(selector));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::iselect(iselector_map_type<N>&& selector) -> reference
    {
        return select_impl(iselector_type<N>(std::move(selector)));
    }

    template <class D>
    template <std::size_t N>
    inline auto xvariable_base<D>::iselect(iselector_map_type<N>&& selector) const -> const_reference
    {
        return select_impl(iselector_type<N>(std::move(selector)));
    }
    
    template <class D>
    inline auto xvariable_base<D>::make_dimension_mapping(coordinate_initializer coord) -> dimension_type
    {
        dimension_list tmp(coord.size());
        std::transform(coord.begin(), coord.end(), tmp.begin(), [](const auto& p) { return p.first; });
        return dimension_type(std::move(tmp));
    }

    template <class D>
    template <std::size_t... I, class... Args>
    inline auto xvariable_base<D>::locate_impl(std::index_sequence<I...>, Args&&... args) -> reference
    {
        return data()(coordinates()[dimension_mapping().labels()[I]][args]...);
    }

    template <class D>
    template <std::size_t... I, class... Args>
    inline auto xvariable_base<D>::locate_impl(std::index_sequence<I...>, Args&&... args) const -> const_reference
    {
        return data()(coordinates()[dimension_mapping().labels()[I]][args]...);
    }

    template <class D>
    template <class S>
    inline auto xvariable_base<D>::select_impl(const S& selector) -> reference
    {
        typename S::index_type idx = selector.get_index(coordinates(), dimension_mapping());
        return data().element(idx.cbegin(), idx.cend());
    }

    template <class D>
    template <class S>
    inline auto xvariable_base<D>::select_impl(const S& selector) const -> const_reference
    {
        typename S::index_type idx = selector.get_index(coordinates(), dimension_mapping());
        return data().element(idx.cbegin(), idx.cend());
    }
    
    template <class D>
    template <class S>
    inline auto xvariable_base<D>::select_outer(const S& selector) const -> const_reference
    {
        typename S::outer_index_type idx = selector.get_outer_index(coordinates(), dimension_mapping());
        return idx.second ? data().element(idx.first.cbegin(), idx.first.cend()) : missing();
    }
    
    template <class D>
    template <class Join, class S>
    inline auto xvariable_base<D>::select_join(const S& selector) const -> const_reference
    {
        return xtl::mpl::static_if<Join::id() == join::inner::id()>([&](auto self)
        {
            return self(*this).select_impl(selector);
        }, /*else*/ [&](auto self)
        {
            return self(*this).select_outer(selector);
        });
    }

    template <class D>
    inline auto xvariable_base<D>::derived_cast() noexcept -> derived_type&
    {
        return *static_cast<derived_type*>(this);
    }

    template <class D>
    inline auto xvariable_base<D>::derived_cast() const noexcept -> const derived_type&
    {
        return *static_cast<const derived_type*>(this);
    }
}

#endif

