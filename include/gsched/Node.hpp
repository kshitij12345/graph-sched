#pragma once
#include <tuple>
#include <utility>
#include <set>

namespace gsched{


template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
populate_tuple(const std::tuple<Tp...> &, std::vector<void*>&)
{ }

template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I < sizeof...(Tp), void>::type
populate_tuple(const std::tuple<Tp...>& t, std::vector<void*>& untyped_args)
{
	std::get<I>(t) = *reinterpret_cast<std::tuple_element<I,std::tuple<Tp...>>::type*>(untyped_args[I]);
	populate_tuple<I + 1, Tp...>(t, rhs);
}

struct BaseNode{
	// This is the base struct for Nodes.
	// It allows us to have derived nodes
	// for different function signatures
	// to exist in a single container.
	int id;
	std::set<int> parents;
	std::set<int> children;
	// the value computed by this node.
	void *retval;
	virtual ~BaseNode(){}
	virtual void operator()() = 0;
};

template<typename Functor> struct Node ;

template<typename R, typename...ArgsT>
struct Node<R(*)(ArgsT...)> : BaseNode // Specialization for function pointers.
{
	R(*f)(ArgsT...);
	Node(int id, R(*f1)(ArgsT...)) : f{f1}
	{
		this->id = id;
	}

	void operator()() {
		using arguments_t = std::tuple<ArgsT...>;
		arguments_t args;
		std::vector<void*> untyped_args(this->parents.size());
		for(int i : this->parents) {
			untyped_args[i] = this->nodes[i].retval;
		}
		populate_tuple(args, untyped_args)
		apply_tuple(f, args);
	}

};


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

template <std::size_t I = 0, typename ...Tp, typename ...Tn>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
for_each_node_dense(const std::tuple<Tp...>& lhs, const std::tuple<Tn...> rhs)
{ }

template <std::size_t I = 0, typename ...Tp, typename ...Tn>
inline typename std::enable_if<I < sizeof...(Tp), void>::type
for_each_node_dense(const std::tuple<Tp...>& lhs, const std::tuple<Tn...> rhs)
{
	for_each_node_inv(std::get<I>(lhs), rhs);
	for_each_node_dense<I + 1ul>(lhs,rhs);
}

inline std::tuple<BaseNode&, BaseNode&> operator,(BaseNode& lhs, BaseNode& rhs){
	return std::forward_as_tuple(lhs, rhs);
}

template<typename...T1>
inline std::tuple<T1&..., BaseNode&> operator,(const std::tuple<T1&...>& lhs, BaseNode& rhs){
	return std::tuple_cat(lhs, std::forward_as_tuple(rhs));
}

template<typename... T1>
inline BaseNode& operator>>(const std::tuple<T1&...>& lhs, BaseNode& rhs){
	for_each_node(lhs, rhs);
	return rhs;
}

template<typename... T1>
inline std::tuple<T1&...> operator>>(BaseNode& lhs, const std::tuple<T1&...>& rhs){
	for_each_node_inv(lhs, rhs);
	return rhs;
}

template <typename ...T1, typename ...T2>
inline const std::tuple<T2...>& operator>>(const std::tuple<T1...>& lhs, const std::tuple<T2...>& rhs){
	for_each_node_dense(lhs, rhs);
	return rhs;
}

} // namespace ends