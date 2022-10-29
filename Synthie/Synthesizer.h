#pragma once
#include "msxml2.h"
#include <list>
#include "Instrument.h"
#include <string>
#include <vector>
#include "Note.h"
#include "Flanger.h"
#include "Compression.h"
#include "NoiseGate.h"
using namespace std;
class CSynthesizer
{
public:
    //! Number of audio channels
    int GetNumChannels() { return m_channels; }

    //! Sample rate in samples per second
    double GetSampleRate() { return m_sampleRate; }

    //! Sample period in seconds (1/samplerate)
    double GetSamplePeriod() { return m_samplePeriod; }

    //! Set the number of channels
    void SetNumChannels(int n) { m_channels = n; }

    //! Set the sample rate
    void SetSampleRate(double s) { m_sampleRate = s;  m_samplePeriod = 1.0 / s; }

    //! Get the time since we started generating audio
    double GetTime() { return m_time; }

    //! Constructor
    CSynthesizer(void);

    virtual ~CSynthesizer(void);

    void Start(void);

    bool Generate(double* frame);

    void Clear(void);

    void OpenScore(CString& filename);

private:
    void XmlLoadScore(IXMLDOMNode* xml);

    void XmlLoadInstrument(IXMLDOMNode* xml);

    void XmlLoadNote(IXMLDOMNode* xml, std::wstring& instrument);

private:
    int		m_channels;
    double	m_sampleRate;
    double	m_samplePeriod;
    double m_time;
    std::list<CInstrument*>  m_instruments;
    double  m_bpm;                  //!< Beats per minute
    int     m_beatspermeasure;  //!< Beats per measure
    double  m_secperbeat;        //!< Seconds per beat
    std::vector<CNote> m_notes;
    int m_currentNote;          //!< The current note we are playing
    int m_measure;              //!< The current measure
    double m_beat;              //!< The current beat within the measure
    CFlanger m_flanger;
    CCompression m_compression;
    CNoiseGate m_noisegate;
};

#pragma comment(lib, "msxml2.lib")