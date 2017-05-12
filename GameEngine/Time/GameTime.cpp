#include "GameTime.h"

void CGameTime::Update(const float & delta_time)
{
	m_DeltaGameTime = delta_time / 1000;
	m_CurrentTime += m_DeltaGameTime;

	if (m_CurrentTime > 1)
		m_CurrentTime -= 1;
	if (m_CurrentTime < 0)
		m_CurrentTime += 1;

	m_DeltaTime = delta_time;	
}
