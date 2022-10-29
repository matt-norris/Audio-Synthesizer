#include "pch.h"
#include "Flanger.h"


void CFlanger::SetNote(CNote* note)
{
    // Get a list of all attribute nodes and the
    // length of that list
    CComPtr<IXMLDOMNamedNodeMap> attributes;
    note->Node()->get_attributes(&attributes);
    long len;
    attributes->get_length(&len);

    // Loop over the list of attributes
    for (int i = 0; i < len; i++)
    {
        // Get attribute i
        CComPtr<IXMLDOMNode> attrib;
        attributes->get_item(i, &attrib);

        // Get the name of the attribute
        CComBSTR name;
        attrib->get_nodeName(&name);

        // Get the value of the attribute.  A CComVariant is a variable
        // that can have any type. It loads the attribute value as a
        // string (UNICODE), but we can then change it to an integer 
        // (VT_I4) or double (VT_R8) using the ChangeType function 
        // and then read its integer or double value from a member variable.
        CComVariant value;
        attrib->get_nodeValue(&value);

        if (name == "wet")
        {
            value.ChangeType(VT_R8);
            SetWet(value.dblVal);
        }

    }
   
}

void CFlanger::Process(double* frameIn, double* frameOut)
{

    // Loop over the channels
    for (int c = 0; c < 2; c++)
    {
        m_wrloc = (m_wrloc + 1) % QUEUESIZE;

        m_queue[m_wrloc] = frameIn[c];

        int delaylength = int(m_delay * GetSampleRate() + 0.5);
        m_rdloc = (m_wrloc + QUEUESIZE - delaylength) % QUEUESIZE;

        // Add output of the queue to the current input
        frameOut[c] = frameIn[c] + m_queue[m_rdloc];     

    }
    // Sweep delay for flanger effect
    m_delay -= m_x;
    if (m_delay <= .001) 
    {
        m_x = -.001;
    }
    if (m_delay >= .02)
    {
        m_x = .001;
    }
    

}


CFlanger::CFlanger(void)
{
    m_queue.resize(QUEUESIZE);
    m_wrloc = 0;
    m_rdloc = 0;
    m_x = .001;
    
}

CFlanger::~CFlanger(void)
{

}

