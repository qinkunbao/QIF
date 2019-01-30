#pragma once
#include <list>
#include <iostream>
#include <set>
#include <algorithm>
#include "ins_types.h"
#include "Register.h"

namespace tana {
	class Tainter {
	private:
		std::list<t_type::T_ADDRESS> taintedAdress;
		bool taintedRegisters[GPR_NUM][REGISTER_SIZE];


	public:
		Tainter(t_type::T_ADDRESS, t_type::T_SIZE m_size);

		void taint(t_type::T_ADDRESS addr);

		void taint(t_type::T_ADDRESS addr, t_type::T_SIZE size);

		void taint(x86::x86_reg reg);

		void untaint(t_type::T_ADDRESS addr);

		void untaint(x86::x86_reg reg);

		void untaint(t_type::T_ADDRESS addr, t_type::T_SIZE size);

		bool isTainted(t_type::T_ADDRESS addr);

		bool isTainted(x86::x86_reg reg);

		std::list<t_type::T_ADDRESS> getTaintedAddress();

		// Imediate to Memory
		void spreadTaintImediate2Memory(t_type::T_ADDRESS addr, t_type::T_SIZE m_size);

		// Imediate to Register
		void spreadTaintImediate2Register(x86::x86_reg reg);

		// Register to Register
		void spreadTaintRegister2Register(x86::x86_reg dest, x86::x86_reg src);

		// Memory to memory
		void spreadTaintMemory2Memory(t_type::T_ADDRESS ip_addr, t_type::T_ADDRESS src, t_type::T_ADDRESS dest,
									  t_type::T_SIZE m_size);

		// Register to Memory
		void spreadTaintRegister2Memory(t_type::T_ADDRESS ip_addr, x86::x86_reg reg, t_type::T_ADDRESS addr);

		// Memory to Register
		void spreadTaintMemory2Register(t_type::T_ADDRESS ip_addr, t_type::T_ADDRESS addr, x86::x86_reg reg);

		void taintIns(Inst_Dyn& ins);

		void cleanALL();

	};
};