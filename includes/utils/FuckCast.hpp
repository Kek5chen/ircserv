#pragma once

namespace std {
	// This is very unsafe unless you know exactly what you're doing.
	// The types O and T have to be of the same size, and even then,
	// it might not make sense to cast between them.
	// Basically it's easy to fuck everything with this function and is probably against every coding guideline
	// on earth.
	// But it's cool and we need it
	template <typename T, typename F>
	inline T &fuck_cast(F &from) {
		return *reinterpret_cast<T*>(&from);
	}
}
