#pragma once
#include <tuple>
#include <utility>
#include <set>
#include <functional>
#include <algorithm>

namespace gsched{

// forward reference to support
// the below using statement.
struct BaseNode;

using BaseNode_ref = std::reference_wrapper<BaseNode>;

using vec_BaseNode_ref = std::vector<BaseNode_ref>;

struct BaseNode{
	// This is the base struct for Nodes.
	// It allows us to have derived nodes
	// for different function signatures
	// to exist in a single container.
	int id;
	vec_BaseNode_ref parents_ref;
	vec_BaseNode_ref children_ref;
	
	virtual ~BaseNode(){}
	virtual void operator()() = 0;
};

template <typename F>
struct Node : BaseNode{
	// Derived from Base, it holds the actual
	// function. It can hold function of any
	// signature.
	F func;

	template<typename F1>
	Node(int id, F1&& f): func{std::move(f)} {
		this->id = id;
	}

	void operator()() {
		this->func();
	}
};

inline BaseNode& operator>>(BaseNode& lhs, BaseNode& rhs){
	lhs.children_ref.push_back(rhs);
	rhs.parents_ref.push_back(lhs);
	return rhs;
}

inline BaseNode& operator>>(const vec_BaseNode_ref& lhs, BaseNode& rhs){
	std::for_each(lhs.begin(), lhs.end(), [&](BaseNode& a){
		a >> rhs;
	});
	return rhs;
}

inline vec_BaseNode_ref operator>>(BaseNode& lhs, const vec_BaseNode_ref& rhs){
	std::for_each(rhs.begin(), rhs.end(), [&](BaseNode& a){
		lhs >> a;
	});
	return rhs;
}

inline vec_BaseNode_ref operator>>(vec_BaseNode_ref lhs, vec_BaseNode_ref rhs){
	for (size_t i = 0; i < lhs.size() ; ++i){
		lhs[i] >> rhs;
	}
	return rhs;
}

inline vec_BaseNode_ref operator,(BaseNode& lhs, BaseNode& rhs){
	std::vector<BaseNode_ref> arr {lhs, rhs};
	return arr;
}

inline vec_BaseNode_ref operator,(const vec_BaseNode_ref& lhs, BaseNode& rhs){
	vec_BaseNode_ref vec(lhs);
	
	vec.emplace_back(rhs);
	return vec;
}

// template<std::size_t I = 0, typename... Tp>
// inline typename std::enable_if<I == sizeof...(Tp), void>::type
// for_each_node(const std::tuple<Tp...> &, BaseNode& rhs) // Unused arguments are given no names.
// { }

// template<std::size_t I = 0, typename... Tp>
// inline typename std::enable_if<I < sizeof...(Tp), void>::type
// for_each_node(const std::tuple<Tp...>& t, BaseNode& rhs)
// {
// 	std::get<I>(t) >> rhs;
// 	for_each_node<I + 1, Tp...>(t, rhs);
// }

// template<std::size_t I = 0, typename... Tp>
// inline typename std::enable_if<I == sizeof...(Tp), void>::type
// for_each_node_inv(BaseNode& rhs, const std::tuple<Tp...> &lhs ) // Unused arguments are given no names.
// { }

// template<std::size_t I = 0, typename... Tp>
// inline typename std::enable_if<I < sizeof...(Tp), void>::type
// for_each_node_inv(BaseNode& rhs, const std::tuple<Tp...>& lhs)
// {
// 	rhs >> std::get<I>(lhs);
// 	for_each_node_inv<I + 1, Tp...>(rhs, lhs);
// }

// template <std::size_t I = 0, typename ...Tp, typename ...Tn>
// inline typename std::enable_if<I == sizeof...(Tp), void>::type
// for_each_node_dense(const std::tuple<Tp...>& lhs, const std::tuple<Tn...> rhs)
// { }

// template <std::size_t I = 0, typename ...Tp, typename ...Tn>
// inline typename std::enable_if<I < sizeof...(Tp), void>::type
// for_each_node_dense(const std::tuple<Tp...>& lhs, const std::tuple<Tn...> rhs)
// {
// 	for_each_node_inv(std::get<I>(lhs), rhs);
// 	for_each_node_dense<I + 1ul>(lhs,rhs);
// }

} // namespace ends