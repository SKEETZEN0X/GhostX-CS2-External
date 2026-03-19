#pragma once

class menu
{
public:
	void draw( );
	[[nodiscard]] bool is_open( ) const noexcept { return this->m_open; }

private:
	bool m_open{ false };
};
