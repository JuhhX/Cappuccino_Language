#ifndef EVALUATIONS_H
#define EVALUATIONS_H

#include <vector>
#include <stack>
#include <string>

#include "General.h"
#include "Variables.h"
#include "StringUtilities.h"
using namespace std;

struct Operands {
	bool is_string = false;
	double double_value;
	string string_value;

	Operands() = default;
};

void multiply_expressions(stack<double>* numbers);
void divide_expressions(stack<double>* numbers);
void module_expressions(stack<double>* numbers);
void sum_expressions(stack<double>* numbers);
void subtraction_expressions(stack<double>* numbers);
bool isBooleanOperation(char params);
string resolveAssignment(string params);
string resolveAssignmentString(string params);
bool isOperation(string params);
void not_expression(stack<string>* ops, stack<string>* aux_ops, stack<Operands>* opr, stack<Operands>* aux_opr);
void more_or_less_expression(stack<string>* ops, stack<string>* aux_ops, stack<Operands>* opr, stack<Operands>* aux_opr);
void equals_or_different_expression(stack<string>* ops, stack<string>* aux_ops, stack<Operands>* opr, stack<Operands>* aux_opr);
void and_expression(stack<string>* ops, stack<string>* aux_ops, stack<Operands>* opr, stack<Operands>* aux_opr);
void or_expression(stack<string>* ops, stack<string>* aux_ops, stack<Operands>* opr, stack<Operands>* aux_opr);
string resolveEvaluation(string params);

#endif