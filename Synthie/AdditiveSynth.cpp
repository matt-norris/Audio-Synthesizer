#include "pch.h"
#include "AdditiveSynth.h"
#include "Notes.h"
#include <vector>
#include <sstream>
#include <math.h>


CAdditiveSynth::CAdditiveSynth(void)
{
    m_duration = 0.1;
    m_sustain = false;
    m_vibrato_on = false;

    // Default vibrato has a depth of 10% and freq of 6 hz
    m_vibrato_depth = 10;
    m_vibrato_freq = 6;
    m_crossfading = false;
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

            current_harmonic->SetFundamentalFreq(m_freq * i);
        }
        
        // Case #1B: Harmonic Undefined, set amp to 0
        if (i > size(m_sound_def) && size(m_sound_def) != 0)
        {
            current_harmonic->SetAmplitude(0);
            current_harmonic->SetFundamentalAmplitude(0);
        }

        // Case #2B: Above nyquist, omit this harmonic
        else if (m_freq * i >= GetSampleRate() / 2) 
        {
            current_harmonic->SetAmplitude(0);
            current_harmonic->SetFundamentalAmplitude(0);
        }

        // Case #3B: No sound definition, set default amp and 0 for all harmonics
        else if (size(m_sound_def) == 0)
        {
            if (i == 1) 
            {
               // Default amp is .1
               current_harmonic->SetAmplitude(0.1);
               current_harmonic->SetFundamentalAmplitude(0.1);
            }
            else 
            {
                // No def, then the amp is 0
                current_harmonic->SetAmplitude(0);
                current_harmonic->SetFundamentalAmplitude(0);
            }
            
        }

        // Case #4B: Harmonic defined, set amp equal to corresponding amp
        else 
        {
            current_harmonic->SetAmplitude(m_sound_def[i - 1]);
            current_harmonic->SetFundamentalAmplitude(m_sound_def[i - 1]);
        }

        // Start up the current harmonic, set time to 0
        current_harmonic->Start();
        m_time = 0;

        // Push back pointer to the current harmonic into the vector of all harmonics
        m_harmonics.push_back(*current_harmonic);
    }

    // Set up crossfade harmonics if we need to
    if (m_crossfading == true)
    {
        StartCrossfade(m_crossfade_harmonics);
    }
    
}


bool CAdditiveSynth::Generate()
{
    // Values to total up the sinusoid frames
    double final_frame_0 = 0;
    double final_frame_1 = 0;
    const double total = m_duration * (1.0 / (GetBeatsPerMinute() / 60.0));

    // Generate the sample from all the harmonics, and add the waves together, giving the actual sound that will be played
    for (size_t i = 0; i <= 19; i++)
    {
        // Apply vibrato effect to sound if it needs it
        if (m_vibrato_on == true)
        {
            // Set up vibrato for this generation
            double vib = m_vibrato_depth * sin(m_vibrato_freq * 2 * PI * m_time);
            m_harmonics[i].SetFreq(m_harmonics[i].GetFundamentalFreq() + vib);

            // If note needs crossfading and vibrato, add it to those harmonics as well
            if (m_crossfading == true)
            {
                m_crossfade_harmonics[i].SetFreq(m_crossfade_harmonics[i].GetFundamentalFreq() + vib);
            }
        }

        // Apply crossfading if the sound requires it
        if (m_crossfading == true)
        {
            // Adjust amplitudes according to duration, weights should total to 1
            m_harmonics[i].SetAmplitude(m_harmonics[i].GetFundamentalAmplitude() * ((total - m_time) / total));
            m_crossfade_harmonics[i].SetAmplitude(m_crossfade_harmonics[i].GetFundamentalAmplitude() * (m_time / total));

            // Generate crossfade
            m_crossfade_harmonics[i].Generate();
        }

        // Generate sample from this harmonic
        m_harmonics[i].Generate();

        // Crossfade final frame
        if (m_crossfading == true)
        {
            // Add both samples to frame
            final_frame_0 += m_harmonics[i].Frame(0) + m_crossfade_harmonics[i].Frame(0);
            final_frame_1 += m_harmonics[i].Frame(1) + m_crossfade_harmonics[i].Frame(1);
        }

        // No crossfade
        else
        {

            // Add to final frame
            final_frame_0 += m_harmonics[i].Frame(0);
            final_frame_1 += m_harmonics[i].Frame(1);

        }
    }

    // Apply ADSR multiplier
    double adsr_gain = 1;
    const auto attack = 0.05;
    const auto release = 0.02;
    
    // Attack State
    if (m_time < attack)
    {
        adsr_gain = m_time / attack;
    }

    // Decay State
    else if (m_time > attack && m_sustain == false && m_time < total - release)
    {
        adsr_gain = attack / m_time;
        if (adsr_gain <= .5)
        {
            m_sustain = true;
        }
    }

    // Sustain State
    else if (m_sustain = true && m_time < total - release)
    {
        adsr_gain = 0.5;
    }

    // Release State
    else if (m_time >= total - release)
    {
        // Exit sustain state
        m_sustain = false;
        // Work down from 0.5 to 0
        adsr_gain = ((total - m_time) / .04);
    }


    // Add up final frame and ADSR gain, giving us the output frame.
    m_frame[0] = final_frame_0 * adsr_gain;
    m_frame[1] = final_frame_1 * adsr_gain;
    
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

        // Set vibrato back to false when sound is done
        m_vibrato_on = false;
        // Set crossfade back to false when sound is done
        m_crossfading = false;
    }

    return valid;
}

void CAdditiveSynth::StartCrossfade(std::vector<CSineWave> crossfade_harmonics)
{

    // Start the crossfade harmonics (defined up to 20 harmonics)
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

            current_harmonic->SetFundamentalFreq(m_freq * i);
        }

        // Case #1B: Harmonic Undefined, set amp to 0
        if (i > size(m_sound_def_cross) && size(m_sound_def_cross) != 0)
        {
            current_harmonic->SetAmplitude(0);
            current_harmonic->SetFundamentalAmplitude(0);
        }

        // Case #2B: Above nyquist, omit this harmonic
        else if (m_freq * i >= GetSampleRate() / 2)
        {
            current_harmonic->SetAmplitude(0);
            current_harmonic->SetFundamentalAmplitude(0);
        }

        // Case #3B: No sound definition, set default amp and 0 for all harmonics
        else if (size(m_sound_def_cross) == 0)
        {
            if (i == 1)
            {
                // Default amp is .1
                current_harmonic->SetAmplitude(0.1);
                current_harmonic->SetFundamentalAmplitude(0.1);
            }
            else
            {
                // No def, then the amp is 0
                current_harmonic->SetAmplitude(0);
                current_harmonic->SetFundamentalAmplitude(0);
            }

        }

        // Case #4B: Harmonic defined, set amp equal to corresponding amp
        else
        {
            current_harmonic->SetAmplitude(m_sound_def_cross[i - 1]);
            current_harmonic->SetFundamentalAmplitude(m_sound_def_cross[i - 1]);
        }

        // Start up the current harmonic, set time to 0
        current_harmonic->Start();
        m_time = 0;

        // Push back pointer to the current harmonic into the vector of all harmonics
        m_crossfade_harmonics.push_back(*current_harmonic);
    }

}

void CAdditiveSynth::SetNote(CNote* note)
{
    // Get a list of all attribute nodes and the
    // length of that list
    CComPtr<IXMLDOMNamedNodeMap> attributes;
    note->Node()->get_attributes(&attributes);
    long len;
    attributes->get_length(&len);
    m_vibrato_on = false;
    m_crossfading = false;
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

        // Set vibrato properties if the note needs it
        else if (name == "vibrato") 
        {
            value.ChangeType(VT_R8);
            // If vibrato is one, turn it on
            if (value.dblVal == 1.0) 
            {
                SetVibratoOn(true);
            }
            // Any other value set to false
            else
            {
                SetVibratoOn(false);
            }
        }

        // Set vibrato properties if the note needs it
        else if (name == "vib_depth")
        {
            value.ChangeType(VT_R8);
            m_vibrato_depth = value.dblVal;
        }

        // Set vibrato properties if the note needs it
        else if (name == "vib_freq")
        {
            value.ChangeType(VT_R8);
            m_vibrato_freq = value.dblVal;
        }

        // Set up crossfade sound if note needs it
        else if (name == "crossfade")
        {
            m_crossfading = true;
            // Set up vector of amplitudes
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
                    m_sound_def_cross.push_back(stod(amp));
                    amp.clear();
                }
                // Case #3: Space, clear amp and pushback current amp to vector 
                else if (amps_ws[i] == ' ')
                {
                    // Pushback this harmonics amplitude, as a double, into the sound def vector
                    m_sound_def_cross.push_back(stod(amp));
                    amp.clear();
                }

            }
        }
    }
}