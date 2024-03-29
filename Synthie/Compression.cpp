#include "pch.h"
#include "Compression.h"

void CCompression::SetNote(CNote* note)
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
            
        }
        if (name == "threshold")
        {
            value.ChangeType(VT_R8);
            m_threshold = value.dblVal;

        }
        if (name == "reduc_ratio")
        {
            value.ChangeType(VT_R8);
            m_ratio = value.dblVal;

        }

    }
}

void CCompression::Process(double* frameIn, double* frameOut)
{
    // Loop over the channels
    for (int c = 0; c < 2; c++)
    {
        // Works similar to Noise gate, if sound is above threshold lower it
        if (frameIn[c] > m_threshold)
        {
            double gain = frameIn[c] - m_threshold;
            // M_attenuate is the ratio level, this is a 4:1 ratio hard coded
            gain = gain * m_ratio;
            // Output at at level 25% above input threshold
            frameOut[c] = m_threshold + gain;
        }
        else
        {
            // Add output of the queue to the current input
            frameOut[c] = frameIn[c];
        }
    }
}

CCompression::CCompression(void)
{
    m_threshold = 1;
    m_ratio = .25;
}

CCompression::~CCompression(void)
{

}
