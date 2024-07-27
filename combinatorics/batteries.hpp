#include <tuple>

// https://kevinushey.github.io/blog/2016/01/27/introduction-to-c++-variadic-templates/

template<typename T>
constexpr T max(T a) {
    return a;
}

template<typename T, typename... Rest>
constexpr T max(T a, Rest... rest) {
    T b = max(rest...);
    return a < b ? b : a;
}

// https://www.reedbeta.com/blog/python-like-enumerate-in-cpp17/

template <typename T,
          typename TIter = decltype(std::begin(std::declval<T>())),
          typename = decltype(std::end(std::declval<T>()))>
constexpr auto enumerate(T && iterable)
{
    struct iterator
    {
        size_t i;
        TIter iter;
        bool operator != (const iterator & other) const { return iter != other.iter; }
        void operator ++ () { ++i; ++iter; }
        auto operator * () const { return std::tie(i, *iter); }
    };
    struct iterable_wrapper
    {
        T iterable;
        auto begin() { return iterator{ 0, std::begin(iterable) }; }
        auto end() { return iterator{ 0, std::end(iterable) }; }
    };
    return iterable_wrapper{ std::forward<T>(iterable) };
}