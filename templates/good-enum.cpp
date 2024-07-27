#include <iostream>

// ref: https://stackoverflow.com/questions/4969233/how-to-check-if-enum-value-is-valid

template<typename EnumType, EnumType... Values>
class EnumCheck;

template<typename EnumType>
class EnumCheck<EnumType>
{
public:
    template<typename IntType>
    static bool constexpr is_value(IntType) { return false; }
};

template<typename EnumType, EnumType V, EnumType... Next>
class EnumCheck<EnumType, V, Next...> : private EnumCheck<EnumType, Next...>
{
    using super = EnumCheck<EnumType, Next...>;

public:
    template<typename IntType>
    static bool constexpr is_value(IntType v)
    {
        return v == static_cast<IntType>(V) || super::is_value(v);
    }
};

typedef enum _CoffeMachineStates {
	EMPTY = 0x00,
	NO_ELECTRICITY = 0x01,
    BREWING = 0xFF,
} CoffeMachineStates;

using CoffeMachineStatesCheck = EnumCheck<CoffeMachineStates, 
    CoffeMachineStates::EMPTY,
    CoffeMachineStates::NO_ELECTRICITY,
    CoffeMachineStates::BREWING>;

int main(int argc, char const *argv[])
{
    unsigned char state = 0x2;
    if (!CoffeMachineStatesCheck::is_value(state)) {
        std::cout << "No coffee then" << std::endl;
    } else {
        std::cout << "State is " << static_cast<CoffeMachineStates>(state) << std::endl;
    }
    return 0;
}
