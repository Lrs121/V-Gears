/* ScummVM Tools
 *
 * ScummVM Tools is the legal property of its developers, whose
 * names are too numerous to list here. Please refer to the
 * COPYRIGHT file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "decompiler/instruction.h"
#include "decompiler/decompiler_codegen.h"
#include "decompiler/decompiler_engine.h"

bool outputStackEffect = true;

void setOutputStackEffect(bool value) {
	outputStackEffect = value;
}

void Instruction::WriteTodo(CodeGenerator *code_gen, std::string owner, std::string instruction){
    code_gen->AddOutputLine("-- Todo(\"" + instruction + "\")");
}

bool Instruction::isJump() const {
	return isCondJump() || IsUncondJump();
}

bool Instruction::isCondJump() const {
	return false;
}

bool Instruction::IsUncondJump() const {
	return false;
}

bool Instruction::isStackOp() const {
	return false;
}

bool Instruction::IsFuncCall() const {
	return false;
}

bool Instruction::isReturn() const {
	return false;
}

bool Instruction::isKernelCall() const {
	return false;
}

bool Instruction::isLoad() const {
	return false;
}

bool Instruction::isStore() const {
	return false;
}

uint32 Instruction::GetDestAddress() const {
	throw WrongTypeException();
}

std::ostream &Instruction::Print(std::ostream &output) const {
	output << boost::format("%08x: %s") % _address % _name;
	std::vector<ValuePtr>::const_iterator param;
	for (param = _params.begin(); param != _params.end(); ++param) {
		if (param != _params.begin())
			output << ",";
		output << " " << *param;
	}
	if (outputStackEffect)
		output << boost::format(" (%d)") % _stackChange;
	return output;
}

bool CondJumpInstruction::isCondJump() const {
	return true;
}

bool UncondJumpInstruction::IsUncondJump() const {
	return true;
}

void UncondJumpInstruction::ProcessInst(Function&, ValueStack&, Engine*, CodeGenerator*) {
}

bool StackInstruction::isStackOp() const {
	return true;
}

bool CallInstruction::IsFuncCall() const {
	return true;
}

bool LoadInstruction::isLoad() const {
	return true;
}

bool StoreInstruction::isStore() const {
	return true;
}

void DupStackInstruction::ProcessInst(Function&, ValueStack &stack, Engine*, CodeGenerator *codeGen) {
	std::stringstream s;
	ValuePtr p = stack.pop()->dup(s);
	if (s.str().length() > 0)
		codeGen->AddOutputLine(s.str());
	stack.push(p);
	stack.push(p);
}

void BoolNegateStackInstruction::ProcessInst(Function&, ValueStack &stack, Engine*, CodeGenerator*) {
	stack.push(stack.pop()->negate());
}

void BinaryOpStackInstruction::ProcessInst(Function&, ValueStack &stack, Engine*, CodeGenerator *codeGen) {
	ValuePtr op1 = stack.pop();
	ValuePtr op2 = stack.pop();
	if (codeGen->_binOrder == FIFO_ARGUMENT_ORDER)
		stack.push(new BinaryOpValue(op2, op1, _codeGenData));
	else if (codeGen->_binOrder == LIFO_ARGUMENT_ORDER)
		stack.push(new BinaryOpValue(op1, op2, _codeGenData));
}

bool ReturnInstruction::isReturn() const {
	return true;
}

void ReturnInstruction::ProcessInst(Function&, ValueStack&, Engine*, CodeGenerator *codeGen) {
	codeGen->AddOutputLine("return;");
}

void UnaryOpPrefixStackInstruction::ProcessInst(Function&, ValueStack &stack, Engine*, CodeGenerator*) {
	stack.push(new UnaryOpValue(stack.pop(), _codeGenData, false));
}

void UnaryOpPostfixStackInstruction::ProcessInst(Function& , ValueStack &stack, Engine*, CodeGenerator*) {
	stack.push(new UnaryOpValue(stack.pop(), _codeGenData, true));
}

void KernelCallStackInstruction::ProcessInst(Function&, ValueStack &stack, Engine*, CodeGenerator *codeGen) {
	codeGen->_argList.clear();
	bool returnsValue = (_codeGenData.find("r") == 0);
	std::string metadata = (!returnsValue ? _codeGenData : _codeGenData.substr(1));
	for (size_t i = 0; i < metadata.length(); i++)
		codeGen->processSpecialMetadata(this, metadata[i], i);
	stack.push(new CallValue(_name, codeGen->_argList));
	if (!returnsValue) {
		std::stringstream stream;
		stream << stack.pop() << ";";
		codeGen->AddOutputLine(stream.str());
	}
}

bool KernelCallInstruction::isKernelCall() const {
	return true;
}
