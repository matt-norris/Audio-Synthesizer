#include "pch.h"
#include "AR.h"


CAR::CAR(void)
{
    // Set default values
    m_attack = 0.05;
    m_release = 0;
    m_duration = 0;
    m_time = 0;
}


CAR::~CAR(void)
{

}


void CAR::SetSource(CAudioNode* node)
{
    m_source = node;
}

void CAR::SetDuration(double dur)
{
    m_duration = dur;
}

void CAR::Start()
{
    // Use source start 
    m_source->Start();
}

bool CAR::Generate()
{
    // Compute attack/release multiplier
    double gain;
    const double total = m_duration * (1.0 / (GetBeatsPerMinute() / 60.0));
    m_release = total - m_attack;
    if (m_time < m_attack) 
    {
        gain = m_time / m_attack;
    } 
    else if (m_time >= m_release)
    {
        gain = (total - m_time) / m_attack;
    }
    else 
    {
        gain = 1;
    }

    // Take frame from source, then add gain
    m_frame[0] = m_source->Frame(0) * gain;
    m_frame[1] = m_source->Frame(1) * gain;

    // Add on to time
    m_time += m_source->GetSamplePeriod();
    // We return true until the time reaches the duration.
    return m_time < m_duration * (1.0 / (GetBeatsPerMinute() / 60.0));
}

