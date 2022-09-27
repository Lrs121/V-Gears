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

#include "decompiler/decompiler_disassembler.h"

Disassembler::Disassembler(InstVec &insts) : _insts(insts) {
	address_base_ = 0;
}

void Disassembler::Open(const char *filename) {
    stream_ = std::make_unique<BinaryReader>(BinaryReader::ReadAll(filename));
}

void Disassembler::DoDumpDisassembly(std::ostream &output) {
	InstIterator inst;
	for (inst = _insts.begin(); inst != _insts.end(); ++inst) {
		output << *inst << "\n";
	}
}

void Disassembler::Disassemble() {
	if (_insts.empty()) {
        stream_->Seek(0);
		DoDisassemble();
	}
}

void Disassembler::DumpDisassembly(std::ostream &output) {
	Disassemble();
	DoDumpDisassembly(output);
}
