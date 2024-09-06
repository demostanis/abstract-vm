#include "ioperand.hpp"

template <typename T>
T stot(std::string const &value)
{
	T result{};
	auto [ptr, ec] = std::from_chars(value.data(),
			value.data() + value.size(),
			result);
	if (*ptr != 0)
		throw BadNumber();
	if (ec == std::errc())
		return result;
	throw BadNumber();
}

IOperand const *createInt8(std::string const &value)
{
	return new IOperand_int8(stot<char>(value), value);
}

IOperand const *createInt16(std::string const &value)
{
	return new IOperand_int16(stot<short>(value), value);
}

IOperand const *createInt32(std::string const &value)
{
	return new IOperand_int32(stot<int>(value), value);
}

IOperand const *createFloat(std::string const &value)
{
	return new IOperand_float(stot<float>(value), value);
}

IOperand const *createDouble(std::string const &value)
{
	return new IOperand_double(stot<double>(value), value);
}

eOperandType findTypeWithBiggestPrecision(IOperand const *n1, IOperand const *n2)
{
	if (n1->getPrecision() > n2->getPrecision())
		return n1->getType();
	return n2->getType();
}

// that's so useless really
IOperand const *createOperand(eOperandType type, std::string const &value)
{
	IOperand const *(*funcs[])(std::string const &) = {
		[INT8] = createInt8,
		[INT16] = createInt16,
		[INT32] = createInt32,
		[FLOAT] = createFloat,
		[DOUBLE] = createDouble
	};

	return funcs[type](value);
}

// c++ is the worst language
eOperandType findTypeFromString(std::string const &str)
{
	if (str == "int8")
		return INT8;
	else if (str == "int16")
		return INT16;
	else if (str == "int32")
		return INT32;
	else if (str == "float")
		return FLOAT;
	else if (str == "double")
		return DOUBLE;
	throw BadNumber();
}

std::string typeToStr(eOperandType t)
{
	if (t == INT8)
		return "int8";
	else if (t == INT16)
		return "int16";
	else if (t == INT32)
		return "int32";
	else if (t == FLOAT)
		return "float";
	return "double";
}

#define opn1n2(op,n1,n2) \
	std::stod((n2).toStringWithoutType()) op std::stod((n1).toStringWithoutType())

#define defineopop(op,n)\
	IOperand const *IOperand_##n::operator op(IOperand const &n2) const { \
		auto result = opn1n2(op, *this, n2); \
		return createOperand(findTypeWithBiggestPrecision( \
					static_cast<IOperand const *>(this), &n2), \
				std::format("{}", result)); \
	}

#define defineopop0(op,n)\
	IOperand const *IOperand_##n::operator op(IOperand const &n2) const { \
		auto isZero = std::stod(this->toStringWithoutType()) == 0; \
		if (isZero) throw DivisionByZero(); \
		auto result = opn1n2(op, *this, n2); \
		return createOperand(findTypeWithBiggestPrecision( \
					static_cast<IOperand const *>(this), &n2), \
				std::format("{}", result)); \
	}

#define IMPLOP(t,n) \
	IOperand_##n::IOperand_##n(t value, std::string str) : \
		value(value), str(std::format("{}({})", \
					typeToStr(getType()), str)) {} \
	defineopop(+,n) \
	defineopop(-,n) \
	defineopop(*,n) \
	defineopop0(/,n) \
	/* so gay, why isnt there operator%????? */ \
	IOperand const *IOperand_##n::operator%(IOperand const &n2) const { \
		auto isZero = std::stod(this->toStringWithoutType()) == 0; \
		if (isZero) throw DivisionByZero(); \
		auto result = fmod(\
				std::stod(this->toStringWithoutType()),\
				std::stod(n2.toStringWithoutType())); \
		return createOperand(findTypeWithBiggestPrecision( \
					static_cast<IOperand const *>(this), &n2), \
				std::format("{}", result)); \
	} \
	bool IOperand_##n::operator!=(IOperand const &other) const { \
		if (auto operand = dynamic_cast<const IOperand_##n*>(&other)) \
			return operand->value != this->value; \
		return true; \
	} \
	std::string const &IOperand_##n::toString() const { return str; }; \
	std::string IOperand_##n::toStringWithoutType() const { \
		auto paren = str.find('(')+1; \
		auto len = str.find(')')-paren; \
		return str.substr(paren, len); \
	}; \

// this is really the worst subject in the world
// WHY DON'T WE JUST USE THE TYPE FOR THE PRECISION? GOSHHHH
int IOperand_int8::getPrecision() const { return 0; }
eOperandType IOperand_int8::getType() const { return INT8; }

int IOperand_int16::getPrecision() const { return 1; }
eOperandType IOperand_int16::getType() const { return INT16; }

int IOperand_int32::getPrecision() const { return 2; }
eOperandType IOperand_int32::getType() const { return INT32; }

int IOperand_float::getPrecision() const { return 3; }
eOperandType IOperand_float::getType() const { return FLOAT; }

int IOperand_double::getPrecision() const { return 4; }
eOperandType IOperand_double::getType() const { return DOUBLE; }

IMPLOP(char, int8);
IMPLOP(short, int16);
IMPLOP(int, int32);
IMPLOP(float, float);
IMPLOP(double, double);
