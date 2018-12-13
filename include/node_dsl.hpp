#pragma once
#include <tuple>
#include <utility>

#include "Manager.h"


inline BaseNode& operator>>(BaseNode& lhs, BaseNode& rhs){
	lhs.children.insert(rhs.id);
	rhs.parents.insert(lhs.id);
	return rhs;
}

template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
  for_each_node(const std::tuple<Tp...> &, BaseNode& rhs) // Unused arguments are given no names.
  { }

template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I < sizeof...(Tp), void>::type
for_each_node(const std::tuple<Tp...>& t, BaseNode& rhs)
{
	std::get<I>(t) >> rhs;
	for_each_node<I + 1, Tp...>(t, rhs);
}

template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
  for_each_node_inv(BaseNode& rhs, const std::tuple<Tp...> &lhs ) // Unused arguments are given no names.
  { }

template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I < sizeof...(Tp), void>::type
for_each_node_inv(BaseNode& rhs, const std::tuple<Tp...>& lhs)
{
	rhs >> std::get<I>(lhs);
	for_each_node_inv<I + 1, Tp...>(rhs, lhs);
}

std::tuple<BaseNode&, BaseNode&> operator,(BaseNode& lhs, BaseNode& rhs){
	return std::forward_as_tuple(lhs, rhs);
}

template<typename...T1>
std::tuple<T1&..., BaseNode&> operator,(std::tuple<T1&...>& lhs, BaseNode& rhs){
	return std::tuple_cat(lhs, std::forward_as_tuple(rhs));
}

template<typename... T1>
BaseNode& operator>>(const std::tuple<T1&...>& lhs, BaseNode& rhs){
	for_each_node(lhs, rhs);
	return rhs;
}

template<typename... T1>
std::tuple<T1&...> operator>>(BaseNode& lhs, const std::tuple<T1&...>& rhs){
	for_each_node_inv(lhs, rhs);
	return rhs;
}
