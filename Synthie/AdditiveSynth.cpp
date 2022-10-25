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

    // Start the harmonics (defined up to 20 harmonics)
    for (size_t i = 1; i <= 20; i++)
    {
        // Create harmonic and ar component
        auto current_harmonic = new CSineWave();

        // Set parameters, freq and amp
        current_harmonic->SetSampleRate(GetSampleRate());

        // Case #1A: Harmonic is above nyquist
        if (m_freq * i >= GetSampleRate() / 2) 
        {
            current_harmonic->SetFreq(0);
        }

        // Case #2A: Harmonic is below nyquist
        else 
        {
            current_harmonic->SetFreq(m_freq * i);
        }
        
        // Case #1B: Harmonic Undefined, set amp to 0
        if (i > size(m_sound_def) && size(m_sound_def) != 0)
        {
            current_harmonic->SetAmplitude(0);
        }

        // Case #2B: Above nyquist, omit this harmonic
        else if (m_freq * i >= GetSampleRate() / 2) 
        {
            current_harmonic->SetAmplitude(0);
        }

        // Case #3B: No sound definition, set default amp and 0 for all harmonics
        else if (size(m_sound_def) == 0)
        {
            if (i == 1) 
            {
               // Default amp is .1
               current_harmonic->SetAmplitude(0.1);
            }
            else 
            {
                // No def, then the amp is 0
                current_harmonic->SetAmplitude(0);
            }
            
        }

        // Case #4B: Harmonic defined, set amp equal to corresponding amp
        else 
        {
            current_harmonic->SetAmplitude(m_sound_def[i - 1]);
        }

        // Start up the current harmonic, set time to 0
        current_harmonic->Start();
        m_time = 0;

        // Push back pointer to the current harmonic into the vector of all harmonics
        m_harmonics.push_back(*current_harmonic);
    }

}


bool CAdditiveSynth::Generate()
{
    // Values to total up the sinusoid frames
    double final_frame_1 = 0;
    double final_frame_2 = 0;

    // Generate the sample from all the harmonics, and add them together
    for (size_t i = 0; i <= 19; i++)
    {
        m_harmonics[i].Generate();
        final_frame_1 += m_harmonics[i].Frame(0);
        final_frame_2 += m_harmonics[i].Frame(1);
    }
 
    // Compute attack/release multiplier
    double gain;
    const double total = m_duration * (1.0 / (GetBeatsPerMinute() / 60.0));
    auto release = total - .05;
    if (m_time < .05)
    {
        gain = m_time / .05;
    }
    else if (m_time >= release)
    {
        gain = (total - m_time) / .05;
    }
    else
    {
        gain = 1;
    }

    // Add up final frame and AR gain, giving us the output frame.
    m_frame[0] = final_frame_1 * gain;
    m_frame[1] = final_frame_2 * gain;
    
    // Update time after we've added all the sinusoids together
    m_time += GetSamplePeriod();
    // Stop playing once we've reached duration (in terms of beats)
    bool valid = m_time < m_duration* (1.0 / (GetBeatsPerMinute() / 60.0));

    // We return true until the time reaches the duration.
    if (valid == false)
    {
        // Clear vector of harmonics and sound def when we're done with this sample
        m_sound_def.clear();
        m_harmonics.clear();
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
                // Case #3: Space, clear amp and pushback current amp to vector 
                else if (amps_ws[i] == ' ')
                {
                    // Pushback this harmonics amplitude, as a double, into the sound def vector
                    m_sound_def.push_back(stod(amp));
                    amp.clear();
                }
                
            }
            // Set the fundamental amplitude from the sound def if there is a sound def
            SetAmplitude(m_sound_def[0]);
        }
    }
}