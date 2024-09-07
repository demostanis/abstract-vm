#include <fstream>
#include <sstream>
#include <print>
#include <cstring>
#include <cerrno>
#include <deque>
#include <iostream>
#include "ioperand.hpp"
#include "exceptions.hpp"

const IOperand *parsePush(std::stringstream &ss)
{
	if (ss.get() != ' ')
		throw ParseError();

	std::string number;
	std::string type;

	std::getline(ss, type, '(');
	if (!ss.good())
		throw ParseError();
	std::getline(ss, number, ')');
	if (!ss.good())
		throw ParseError();
	auto otype = findTypeFromString(type);
	return createOperand(otype, number);
}

void dump(std::deque<const IOperand *> &stack)
{
	for (auto op : stack)
		std::println("{}", op->toString());
}

void checkTrailing(std::stringstream &ss)
{
	while (1)
	{
		auto c = ss.get();
		if (c == EOF)
			break;
		else if (c == ' ')
			continue;
		else if (c == ';')
			break;
		throw ParseError();
	}
}

#define opinstruction(s,op) \
	else if (instruction == s) \
	{ \
		checkTrailing(ss);\
		ensureduo();\
		auto n1 = stack.front();\
		stack.pop_front();\
		auto n2 = stack.front();\
		stack.pop_front();\
		stack.push_front(*n1 op *n2);\
		delete n1;\
		delete n2;\
	}\

std::string line;
size_t lineNumber = 0;
void processFile(std::ifstream &file, bool isStdin)
{
	std::deque<const IOperand *> stack;
	bool exited = false;
	bool hasUselessMarker = !isStdin;

	while (getline(file, line))
	{
		if (exited && line != ";;")
			throw InstructionAfterExit();

		lineNumber++;
		std::stringstream ss(line);
		std::string instruction;

		ss >> instruction;
		if (instruction == "push")
		{
			auto p = parsePush(ss);
			checkTrailing(ss);
			stack.push_front(p);
		}
		else if (instruction == "pop")
		{
			checkTrailing(ss);
			ensurestack();
			delete stack.front();
			stack.pop_front();
		}
		else if (instruction == "dump")
		{
			checkTrailing(ss);
			std::println("stack dump:");
			if (stack.size() == 0)
				std::println("empty stack");
			dump(stack);
		}
		else if (instruction == "print")
		{
			checkTrailing(ss);
			ensurestack();
			auto top = stack.front();
			if (auto c = dynamic_cast<const IOperand_int8 *>(top))
				std::println("{}", (char)c->value);
			else
				throw NotChar();
		}
		else if (instruction == "assert")
		{
			auto p = parsePush(ss);
			checkTrailing(ss);
			ensurestack();

			auto top = stack.front();
			if (*p != *top)
				throw AssertionFailed();
		}
		opinstruction("add", +)
		opinstruction("sub", -)
		opinstruction("mul", *)
		opinstruction("div", /)
		opinstruction("mod", %)
		else if (instruction == "exit")
		{
			checkTrailing(ss);
			exited = true;
		}
		else if (instruction == ";;")
		{
			if (isStdin)
			{
				if (!exited)
					throw MissingExit();
				hasUselessMarker = true;
			}
			else
				throw MarkerNoStdin();
		}
		else if (instruction != "" && instruction[0] != ';')
			throw UnknownInstruction();
	}
	if (!exited)
		throw MissingExit();
	if (!hasUselessMarker)
		throw NoUselessMarker();
}

int main(int argc, char **argv)
{
	try
	{
		if (argc == 2)
		{
			std::ifstream file(argv[1]);

			if (!file)
			{
				std::println(stderr, "failed to open {}: {}",
						argv[1], strerror(errno));
				return (EXIT_FAILURE);
			}
			processFile(file, false);
		}
		else if (argc == 1)
		{
			std::ifstream file("/dev/stdin");
			processFile(file, true);
		}
		else
		{
			std::println(stderr, "usage: ./avm program.avm");
			return (EXIT_FAILURE);
		}
	}
	catch (std::exception &e)
	{
		std::println("\033[32;1merror: {}\033[0m", e.what());
		std::println("  at line {}: \033[44;3m{}\033[0m", lineNumber, line);
		return (EXIT_FAILURE);
	}
}
