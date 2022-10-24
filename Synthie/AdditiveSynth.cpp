#include "pch.h"
#include "AdditiveSynth.h"
#include "Notes.h"
#include <vector>
#include <sstream>


CAdditiveSynth::CAdditiveSynth(void)
{
    m_duration = 0.1;
}

CAdditiveSynth::~CAdditiveSynth(void)
{
}

void CAdditiveSynth::Start()
{
    m_sinewave.SetSampleRate(GetSampleRate());
    m_sinewave.Start();
    m_time = 0;

    // Tell the AR object it gets its samples from 
    // the sine wave object.
    m_ar.SetSource(&m_sinewave);
    m_ar.SetSampleRate(GetSampleRate());
    m_ar.SetDuration(m_duration);
    m_ar.Start();
}


bool CAdditiveSynth::Generate()
{

    // Tell the component to generate an audio sample
    m_sinewave.SetAmplitude(.1);
    m_sinewave.SetFreq(1000);
    m_sinewave.Generate();
    bool valid = m_ar.Generate();

    // Read the component's sample and make it our resulting frame.
    m_frame[0] = m_ar.Frame(0);
    m_frame[1] = m_ar.Frame(1);

    // Update time
    m_time += GetSamplePeriod();

    // We return true until the time reaches the duration.
    if (valid == false)
    {
        // Clear vector of harmonics when we're done with this sample
        m_sound_def.clear();
    }
    return valid;
}

void CAdditiveSynth::SetNote(CNote* note)
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

        if (name == "duration")
        {
            value.ChangeType(VT_R8);
            SetDuration(value.dblVal);
        }
        else if (name == "note")
        {
            // This will be the fundamental fequency
            SetFreq(NoteToFrequency(value.bstrVal));
        }
        // Get the sound definition here
        else if (name == "harmonics") 
        {
            // Used code adapted from the tutorial here to help me split the harmonic string:
            // https://www.delftstack.com/howto/cpp/cpp-split-string-by-space/
            // Convert to regular wstring
            std::wstring convert_ws(value.bstrVal);
            // Convert wstring to string
            std::string convert_s(convert_ws.begin(),convert_ws.end());
            // Convert string to istringstream
            std::istringstream sound_def(convert_s);
            // String value to hold individual amplitudes
            std::string amp;
            // Use getline to get each amplitude from sound def
            while (getline(sound_def, amp, ' '))
            {
                // Pushback this harmonics amplitude, as a double, into the sound def vector
                m_sound_def.push_back(stod(amp));
            }
        }
    }
}