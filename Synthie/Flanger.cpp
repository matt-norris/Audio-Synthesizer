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

        if (name == "Delay")
        {
            value.ChangeType(VT_R8);
            m_sample_delay = value.dblVal;
        }

        if (name == "lfo_freq")
        {
            value.ChangeType(VT_R8);
            m_freq = value.dblVal;
        }

        if (name == "On") 
        {
            value.ChangeType(VT_R8);
            if (value.dblVal == 1.0) 
            {
                m_on = true;
            }
            else
            {
                m_on = false;
            }
            
        }


    }

}

void CFlanger::Process(double* frameIn, double* frameOut)
{


    // Loop over the channels
    for (int c = 0; c < 2; c++)
    {
        m_wrloc = (m_wrloc + 1) % QUEUESIZE;
        double x = frameIn[0] + frameIn[1];

        m_queue[m_wrloc] = x;

        // Calculate delay
        m_var_delay = (m_sample_delay * sin(m_x * 2 * PI) + m_sample_delay);
        m_x += m_freq / GetSampleRate();


        int delaylength = int(m_var_delay * GetSampleRate() + 0.5);
        m_rdloc = (m_wrloc + QUEUESIZE - delaylength) % QUEUESIZE;


        double y = x + m_queue[m_rdloc];
        if (m_on == true)
        {
            // Add output of the queue to the current input
            frameOut[c] =  .5*y;
        }
        else
        {
            frameOut[c] = frameIn[c];
        }

       
    }

}

void CFlanger::Clear()
{
    m_on = false;
    for (int i =0; i < m_queue.size(); i++) 
    {
       m_queue[i] = 0;
    }
}

CFlanger::CFlanger(void)
{
    m_queue.resize(QUEUESIZE);
    m_wrloc = 0;
    m_rdloc = 1;
    m_x = .005;
    m_var_delay = .01;
    m_on = false;
    m_freq = .09;
    m_sample_delay = .2;
    
}

CFlanger::~CFlanger(void)
{

}

