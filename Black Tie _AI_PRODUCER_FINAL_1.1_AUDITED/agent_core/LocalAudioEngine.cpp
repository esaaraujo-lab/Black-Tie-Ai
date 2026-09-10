#include "LocalAudioEngine.h"
#include <limits>

bool LocalAudioEngine::transform(const juce::File& input, const juce::File& output, const Options& o, juce::String& error)
{
    if (!input.existsAsFile()) { error = "Input audio does not exist"; return false; }
    juce::AudioFormatManager fm; fm.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> reader(fm.createReaderFor(input));
    if (!reader) { error = "Unsupported audio format"; return false; }
    const int channels = juce::jmin(2, (int)reader->numChannels);
    const int64_t samples64 = (int64_t)reader->lengthInSamples;
    if (samples64 <= 0 || channels <= 0) { error = "Empty audio"; return false; }
    if (samples64 > std::numeric_limits<int>::max()) { error = "Input is too large for the in-memory transformer"; return false; }
    const int samples = (int) samples64;
    juce::AudioBuffer<float> src(channels, samples);
    if (!reader->read(&src, 0, samples, 0, true, true)) { error = "Could not decode the complete input"; return false; }

    double stretch = 1.0;
    if (o.sourceBpm > 0.0 && o.targetBpm > 0.0) stretch = o.sourceBpm / o.targetBpm;
    if (stretch < 0.125 || stretch > 8.0) { error = "BPM transform is outside the safe 8x range"; return false; }
    const auto out64 = (int64_t) std::llround((double) samples * stretch);
    if (out64 > std::numeric_limits<int>::max()) { error = "Output would be too large"; return false; }
    const int outLen = juce::jmax(1, (int) out64);
    juce::AudioBuffer<float> out(channels, outLen);
    for (int c=0;c<channels;++c) {
        auto* d=out.getWritePointer(c); const auto* s=src.getReadPointer(c);
        for(int i=0;i<outLen;++i){ double pos=(double)i/stretch; int a=juce::jlimit(0,(int)samples-1,(int)std::floor(pos)); int b=juce::jmin(a+1,(int)samples-1); float f=(float)(pos-a); d[i]=s[a]+(s[b]-s[a])*f; }
    }
    if (o.reverse) {
        for(int c=0;c<channels;++c) std::reverse(out.getWritePointer(c), out.getWritePointer(c)+outLen);
    }
    if (o.stutterRepeats > 1 && o.stutterSeconds > 0.0) {
        const int seg=juce::jlimit(1,outLen,(int)std::llround(o.stutterSeconds*reader->sampleRate));
        juce::AudioBuffer<float> copy(channels,seg); copy.clear();
        for(int c=0;c<channels;++c) copy.copyFrom(c,0,out,c,0,seg);
        const int repeats=juce::jmax(1,o.stutterRepeats); const int maxCopy=juce::jmin(outLen,seg*repeats);
        for(int c=0;c<channels;++c){ auto* d=out.getWritePointer(c); for(int i=0;i<maxCopy;++i)d[i]=copy.getSample(c,i%seg); }
    }
    out.applyGain(juce::Decibels::decibelsToGain(o.gainDb));
    const double sr=reader->sampleRate;
    juce::dsp::IIR::Filter<float> hp,lp;
    hp.coefficients=juce::dsp::IIR::Coefficients<float>::makeHighPass(sr,juce::jlimit(10.0f,(float)sr*0.45f,o.highPassHz));
    lp.coefficients=juce::dsp::IIR::Coefficients<float>::makeLowPass(sr,juce::jlimit(20.0f,(float)sr*0.49f,o.lowPassHz));
    juce::dsp::ProcessSpec spec{sr,(juce::uint32)outLen,1};
    hp.prepare(spec); lp.prepare(spec);
    for(int c=0;c<channels;++c){ juce::dsp::AudioBlock<float> block(out.getArrayOfWritePointers()+c,1,(size_t)outLen); juce::dsp::ProcessContextReplacing<float> ctx(block); hp.reset();lp.reset();hp.process(ctx);lp.process(ctx); }
    if(o.saturation>0.0f){ const float drive=1.0f+juce::jlimit(0.0f,20.0f,o.saturation); for(int c=0;c<channels;++c){auto*d=out.getWritePointer(c);for(int i=0;i<outLen;++i)d[i]=std::tanh(d[i]*drive)/std::tanh(drive);} }
    const int fi=juce::jlimit(0,outLen,(int)std::llround(o.fadeInSeconds*sr));
    const int fo=juce::jlimit(0,outLen,(int)std::llround(o.fadeOutSeconds*sr));
    for(int i=0;i<fi;++i){float g=(float)i/(float)juce::jmax(1,fi);for(int c=0;c<channels;++c)out.setSample(c,i,out.getSample(c,i)*g);}
    for(int i=0;i<fo;++i){float g=(float)(fo-i)/(float)juce::jmax(1,fo);int p=outLen-fo+i;for(int c=0;c<channels;++c)out.setSample(c,p,out.getSample(c,p)*g);}
    output.deleteFile(); juce::WavAudioFormat wav; std::unique_ptr<juce::OutputStream> stream(output.createOutputStream()); if(!stream){error="Cannot create output file";return false;}
    std::unique_ptr<juce::AudioFormatWriter> writer(wav.createWriterFor(stream.get(),sr,(unsigned int)channels,16,{},0)); if(!writer){error="Cannot create WAV writer";return false;} stream.release(); writer->writeFromAudioSampleBuffer(out,0,outLen); return true;
}
