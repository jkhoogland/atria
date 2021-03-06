//
// Copyright (C) 2014, 2015 Ableton AG, Berlin. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

/*!
 * @file
 */

#pragma once

#include <atria/xform/detail/empty_transducer_error.hpp>
#include <atria/xform/transducer_impl.hpp>
#include <atria/xform/state_wrapper.hpp>
#include <atria/prelude/comp.hpp>

namespace atria {
namespace xform {

namespace detail {

struct iter_rf_gen
{
  struct tag {};

  template <typename ReducingFnT,
            typename InputRangeT>
  struct apply
  {
    ReducingFnT step;
    InputRangeT range;

    template <typename StateT, typename ...InputTs>
    auto operator() (StateT&& s, InputTs&& ...is)
      -> decltype(wrap_state<tag>(
                    step(state_unwrap(s), is..., *std::begin(range)),
                    std::make_tuple(std::begin(range), std::end(range))))
    {
      using std::get;
      using std::begin;
      using std::end;

      auto data = state_data(std::forward<StateT>(s), [&] {
          return std::make_tuple(begin(range), end(range));
        });

      return wrap_state<tag>(
        step(state_unwrap(std::forward<StateT>(s)),
             std::forward<InputTs>(is)...,
             *get<0>(data)++),
        std::move(data));
    }
  };

  template <typename T>
  friend bool state_wrapper_data_is_reduced(tag, T&& iters)
  {
    using std::get;
    return get<0>(iters) == get<1>(iters);
  }
};

} // namespace detail

template <typename T>
using iter_t = transducer_impl<detail::iter_rf_gen, T>;

/*!
 * Generator transducer produces the sequence passed as parameter, by
 * iterating over it.
 */
template <typename InputRangeT>
constexpr auto iter(InputRangeT&& r)
  -> iter_t<estd::decay_t<InputRangeT> >
{
  return iter_t<estd::decay_t<InputRangeT> > {
    detail::check_non_empty(std::forward<InputRangeT>(r)) };
}

template <typename InputRangeT, typename ...InputRangeTs>
constexpr auto iter(InputRangeT&& r, InputRangeTs&& ...rs)
  -> ABL_DECLTYPE_RETURN(
    comp(iter(std::forward<InputRangeT>(r)),
         iter(std::forward<InputRangeTs>(rs))...))

} // namespace xform
} // namespace atria
