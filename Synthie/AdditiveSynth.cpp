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

    // Start the harmonics
    for (size_t i = 1; i <= 10; i++) 
    {
        // Create harmonic and ar component
        auto current_harmonic = new CSineWave();
        auto ar_comp = new CAR();

        // Set parameters, freq and amp
        current_harmonic->SetSampleRate(GetSampleRate());
        current_harmonic->SetFreq(m_freq * i);
        

        // Case #1: Harmonic Undefined, set amp to 0
        if (i > size(m_sound_def)) 
        {
            current_harmonic->SetAmplitude(0);
        }
        // Case #2: Harmonic defined, set amp equal to corresponding amp
        else 
        {
            current_harmonic->SetAmplitude(m_sound_def[i-1]);
        }

        // Start up
        current_harmonic->Start();
        m_time = 0;
        ar_comp->SetSource(current_harmonic);
        ar_comp->SetSampleRate(GetSampleRate());
        ar_comp->SetDuration(m_duration);
        ar_comp->Start();

        // Push back pointer to the current harmonic and it's AR component
        m_harmonics.push_back(*current_harmonic);
        m_har_AR.push_back(*ar_comp);
    }

}


bool CAdditiveSynth::Generate()
{

    // Tell the component to generate an audio sample
    m_harmonics[0].Generate();

    //bool valid = m_har_AR[0].Generate();

    // Read the component's sample and make it our resulting frame.
    m_frame[0] = m_harmonics[0].Frame(0);
    m_frame[1] = m_harmonics[0].Frame(1);

    // Update time after we've added all the sinusoids together
    m_time += GetSamplePeriod();
    bool valid = m_time >= m_duration * (1.0 / (GetBeatsPerMinute() / 60.0));
    // We return true until the time reaches the duration.
    if (valid)
    {
        // Clear vector of harmonics and sound def when we're done with this sample
        m_sound_def.clear();
        m_harmonics.clear();
    }

    return m_time < m_duration* (1.0 / (GetBeatsPerMinute() / 60.0));
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
            m_freq = NoteToFrequency(value.bstrVal);
        }

        // Get the sound definition here
        else if (name == "harmonics")
        {

            // Convert to regular wstring
            wstring amps_ws = value.bstrVal;
            // String value to hold individual amplitudes
            wstring amp;

            // Loop and get indiv values
            for (size_t i = 0; i < size(amps_ws); i++)
            {
                // Case #1: Valid Amplitude char
                if ((amps_ws[i] != ' ') && (i + 1 != size(amps_ws)))
                {
                    amp = amp + amps_ws[i];
                }
                // Case #2: Last valid char
                else if (i + 1 == size(amps_ws)) 
                {
                    amp = amp + amps_ws[i];
                    m_sound_def.push_back(stod(amp));
                    amp.clear();
                }
                // Case #3: Invalid char
                else if (amps_ws[i] == ' ')
                {
                    // Pushback this harmonics amplitude, as a double, into the sound def vector
                    m_sound_def.push_back(stod(amp));
                    amp.clear();
                }
                
            }
            // Set the fundamental amplitude from the sound def
            SetAmplitude(m_sound_def[0]);
        }
    }
}