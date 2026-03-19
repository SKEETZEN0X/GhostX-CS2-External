#include <stdafx.hpp>

void menu::draw( )
{
	if ( ::GetAsyncKeyState( VK_INSERT ) & 1 )
		this->m_open = !this->m_open;
}
