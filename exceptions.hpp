#pragma once

class MoreSpecificThanStdExceptionBecauseTheScaleDemandsIt :
	public std::exception {};

#define defineerror(name, str) \
class name : public MoreSpecificThanStdExceptionBecauseTheScaleDemandsIt \
{ \
	virtual const char* what() const noexcept \
	{ \
		return str; \
	} \
};

defineerror(ParseError, "parse error");

defineerror(EmptyStack, "empty stack");
#define ensurestack() if (stack.size() == 0) throw EmptyStack();

defineerror(SmallStack,	"not enough values in stack");
#define ensureduo() if (stack.size() < 2) throw SmallStack();

defineerror(NotChar, "top of the stack is not an int8");

defineerror(BadNumber, "bad number, overflow?");

defineerror(UnknownInstruction, "unknown instruction");

defineerror(AssertionFailed, "assertion failed");

defineerror(DivisionByZero, "division by zero");

defineerror(InstructionAfterExit, "instructions after exit");

defineerror(MissingExit, "missing exit");

defineerror(MarkerNoStdin, "usage of ;; outside of standard input");

defineerror(NoUselessMarker, "you forgot to end your input with a useless ;;");
