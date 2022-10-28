#include "pch.h"
#include "Flanger.h"

void CFlanger::Start()
{

}

bool CFlanger::Generate()
{
	return false;
}

void CFlanger::Process(double* frameIn, double* frameOut)
{
    // Loop over the channels
    //for (int c = 0; c < 2; c++)
    //{
    //    m_queue[m_wrloc + c] = frameIn[c];

        // Add output of the queue to the current input
    //    frameOut[c] = m_dry * frameIn[c] + m_wet * m_queue[m_rdloc + c];
    //}

    //m_wrloc = (m_wrloc + 2) % QSIZE;
    //m_rdloc = (m_rdloc + 2) % QSIZE;
}


CFlanger::CFlanger(void)
{

}

CFlanger::~CFlanger(void)
{

}
