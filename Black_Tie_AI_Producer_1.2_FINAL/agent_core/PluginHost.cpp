#include "PluginHost.h"

PluginHost::PluginHost()
{
    formats.addFormat(std::make_unique<juce::VST3PluginFormat>());
#if JUCE_PLUGINHOST_AU && JUCE_MAC
    formats.addFormat(std::make_unique<juce::AudioUnitPluginFormat>());
#endif
}
PluginHost::~PluginHost(){ release(); }
void PluginHost::prepare(double sr,int bs){ rate=sr>0?sr:44100.0; block=juce::jmax(1,bs); const juce::ScopedLock l(mutex); for(auto* p:chain) p->prepareToPlay(rate,block); }
void PluginHost::release(){ const juce::ScopedLock l(mutex); for(auto* p:chain) p->releaseResources(); chain.clear(); }
void PluginHost::process(juce::AudioBuffer<float>& b,juce::MidiBuffer& m){ juce::ScopedTryLock l(mutex); if(!l.isLocked()) return; for(auto* p:chain) p->processBlock(b,m); }
bool PluginHost::scan(const juce::FileSearchPath& paths,bool recursive,juce::String& error)
{
    try {
        const juce::ScopedLock l(mutex);
        known.clear();
        for(auto* fmt: formats.getFormats()) {
            juce::PluginDirectoryScanner scanner(known,*fmt,paths,recursive,juce::File());
            juce::String dead;
            while(scanner.scanNextFile(true,dead)) {}
            if(dead.isNotEmpty()) error << fmt->getName() << ": skipped/crashed plugin: " << dead << "\n";
        }
        return true;
    } catch(const std::exception& e){ error=e.what(); return false; }
}
juce::Array<PluginHost::PluginInfo> PluginHost::list() const
{
    const juce::ScopedLock l(mutex); juce::Array<PluginInfo> out;
    for(const auto& d:known.getTypes()) { PluginInfo i; i.name=d.name; i.vendor=d.manufacturerName; i.format=d.pluginFormatName; i.file=d.fileOrIdentifier; out.add(i); }
    return out;
}
juce::Array<PluginHost::PluginInfo> PluginHost::search(const juce::String&q,int maxResults) const
{
    auto all=list(); juce::Array<PluginInfo> out; const auto needle=q.toLowerCase().trim();
    for(const auto&i:all){ if(needle.isEmpty() || (i.name+" "+i.vendor+" "+i.format).toLowerCase().contains(needle)){out.add(i);if(out.size()>=maxResults)break;} } return out;
}
bool PluginHost::load(const juce::String& nameOrPath,juce::String& error)
{
    const juce::ScopedLock l(mutex); juce::PluginDescription chosen; bool found=false;
    for(const auto& d:known.getTypes()) if(d.name.equalsIgnoreCase(nameOrPath) || d.fileOrIdentifier==nameOrPath){chosen=d;found=true;break;}
    if(!found){ error="Plugin not found in local scan: "+nameOrPath; return false; }
    auto instance=formats.createPluginInstance(chosen,rate,block,error);
    if(!instance) return false;
    instance->addListener(this); instance->prepareToPlay(rate,block); chain.add(instance.release()); return true;
}
bool PluginHost::setParameter(const juce::String& parameterName,float normalized,juce::String& error)
{
    const juce::ScopedLock l(mutex); if(chain.isEmpty()){error="No hosted plugin";return false;}
    auto* p=chain.getLast();
    for(auto* param:p->getParameters()) if(param->getName(80).equalsIgnoreCase(parameterName) || param->getParameterID().equalsIgnoreCase(parameterName)){param->setValueNotifyingHost(juce::jlimit(0.0f,1.0f,normalized));return true;}
    error="Parameter not found: "+parameterName; return false;
}
juce::var PluginHost::state() const
{
    auto* root=new juce::DynamicObject(); juce::Array<juce::var> a; const juce::ScopedLock l(mutex);
    for(auto* p:chain){auto* o=new juce::DynamicObject();o->setProperty("name",p->getName());o->setProperty("parameters",p->getNumParameters());a.add(o);} root->setProperty("plugins",a);return root;
}
