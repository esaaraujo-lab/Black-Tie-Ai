#pragma once
#include <JuceHeader.h>
class SampleIndex {public:struct Item{juce::String id,name,path,extension;juce::int64 size=0;};void scan(const juce::Array<juce::File>&,int);juce::Array<Item> search(const juce::String&,int)const;private:juce::Array<Item> items;};
