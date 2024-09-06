#pragma once

#include <string>
#include <initializer_list>
#include <charconv>
#include <print>
#include <system_error>
#include <cmath>
#include "exceptions.hpp"

#define pure const = 0

typedef enum
{
	INT8,
	INT16,
	INT32,
	FLOAT,
	DOUBLE
} eOperandType;

class IOperand
{
public:
	virtual int getPrecision() pure;
	virtual eOperandType getType() pure;

	virtual IOperand const *operator+(IOperand const &rhs) pure;
	virtual IOperand const *operator-(IOperand const &rhs) pure;
	virtual IOperand const *operator*(IOperand const &rhs) pure;
	virtual IOperand const *operator/(IOperand const &rhs) pure;
	virtual IOperand const *operator%(IOperand const &rhs) pure;
	virtual bool operator!=(IOperand const &rhs) pure;

	virtual std::string const &toString() pure;
	virtual std::string toStringWithoutType() pure;

	virtual ~IOperand() {}
};

#define OP(t,n) \
	class IOperand_##n : public IOperand \
	{ \
		public: \
			t value; \
			std::string str; \
			IOperand_##n(t, std::string); \
			int getPrecision() const; \
			eOperandType getType() const; \
			IOperand const *operator+(IOperand const &) const; \
			IOperand const *operator-(IOperand const &) const; \
			IOperand const *operator*(IOperand const &) const; \
			IOperand const *operator/(IOperand const &) const; \
			IOperand const *operator%(IOperand const &) const; \
			bool operator!=(IOperand const &) const; \
			std::string const &toString() const; \
			std::string toStringWithoutType() const; \
	};
OP(char, int8);
OP(short, int16);
OP(int, int32);
OP(float, float);
OP(double, double);

IOperand const *createOperand(eOperandType type, std::string const &value);
eOperandType findTypeWithBiggestPrecision(IOperand const *n1, IOperand const *n2);
eOperandType findTypeFromString(std::string const &str);
