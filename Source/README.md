# Faust API

This API allows to interact with a Faust object and its associated audio engine on iOS at a high level. The idea is that all the audio part of the app is implemented in Faust allowing developers to focus on the design of the app itself. 

For more details on how to create iOS apps from scratch using this tool, check the [`faust2api` documentation](https://ccrma.stanford.edu/~rmichon/faust2api) or the [*Adding Faust Real-Time Audio Support to iOS Apps Tutorial*](https://ccrma.stanford.edu/~rmichon/faustTutorials/#adding-faust-real-time-audio-support-to-ios-apps).

## Using This Package

This section is an accelerated version of the [*Adding Faust Real-Time Audio Support to iOS Apps Tutorial*](https://ccrma.stanford.edu/~rmichon/faustTutorials/#adding-faust-real-time-audio-support-to-ios-apps). We strongly recommend you to read it if this is the first time that you use this tool.

### App Set-Up

Very little work has to be done to integrate this package to your iOS app.

First, in your app configuration in XCode, make sure that the `AudioToolbox` framework is imported in `TARGETS/YouApp/BuildPhases/Link Binary With Libraries`. If you used the `-midi` option when generating the API, you'll also have to import the `CoreMIDI` framework.

Import `DspFaust.h` and `DspFaust.cpp` in your project (this can be done simply by dragging these files in your project tree). Then, import `DspFaust.h` (`#import "DspFaust.h"`) in the file where you want to create/control the Faust object (e.g. your main ViewController). Make sure that the file where you import `DspFaust.h` has the `.mm` extension (this is necessary to be able to use C++ code in your objective-c file).

### Using the API

The current Faust API is designed to seamlessly integrate to the life cycle of an iOS app. It is accessible through a single `DspFaust` object. The constructor of that object is used to set the sampling rate and the block size:

	DspFaust *dspFaust = new DspFaust(SR,blockSize);

The `start()` method is used to start the audio computing and would typically be placed in the `viewDidLoad` method of the app's main `ViewController`.

Similarly, `stop()` can be called to stop the audio computing and can be placed in `didReceiveMemoryWarning` along with the `DspFaust` destructor, etc.

It is possible to interact with the different parameters of the Faust object by using the `setParamValue` method. Two versions of this method exist: one where the parameter can be selected by its address and one where it can be selected using its ID. The [Parameters List](#parameters-list) section gives a list of the addresses and corresponding IDs of the current Faust object.

If your Faust object is polyphonic (e.g. if you used the `-nvoices` option when generating this API), then you can use the MIDI polyphony methods like `keyOn`, `keyOff`, etc.

It is possible to change the parameters of polyphonic voices independently using the `setVoiceParamValue` method. This method takes as one of its arguments the address to the voice returned by `keyOn` or `newVoice` when it is called. E.g:

	long voiceAddress = dspFaust->keyOn(70,100);
	dspFaust->setVoiceParamValue(1,voiceAddress,214);
	dspFaust->keyOff(70);
	
In the example above, a new note is created and its parameter ID 1 is modified. This note is then terminated. Note that parameters addresses (path) are different for independent voices than when using `setParamValue`. The list of these addresses is provided in a separate sub-section of the [Parameters List](#parameters-list) section.

Finally, note that new voices don't necessarily have to be created using `keyOn`. Indeed, you might choose to just use the `newVoice` method for that:

	long voiceAddress = dspFaust->newVoice;
	dspFaust->setVoiceParamValue(1,voiceAddress,214);
	dspFaust->deleteVoice(voiceAddress);

This is particularly useful when making apps where each finger of the user is an independent sound that doesn't necessarily has a pitch.

In case you would like to use the the built-in accelerometer or gyroscope of your device to control some of the parameters of your Faust object, all you have to do is to send the raw accelerometer data to it by using the `propagateAcc` or `propagateGyr` for the gyroscope. After that, mappings can be configured directly from the Faust code using [this technique](#using-built-in-sensors-to-control-parameters) or using the `setAccConverter` and `setGyrConverter` method.

## Parameters List

### Main Parameters

* **0**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_1/Attack`
* **1**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_1/Ratio`
* **2**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_1/Release`
* **3**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_1/Threshold`
* **4**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_2/Attack`
* **5**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_2/Ratio`
* **6**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_2/Release`
* **7**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_2/Threshold`
* **8**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_3/Attack`
* **9**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_3/Ratio`
* **10**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_3/Release`
* **11**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_3/Threshold`
* **12**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_4/Attack`
* **13**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_4/Ratio`
* **14**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_4/Release`
* **15**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_4/Threshold`
* **16**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_5/Attack`
* **17**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_5/Ratio`
* **18**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_5/Release`
* **19**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_5/Threshold`
* **20**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_6/Attack`
* **21**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_6/Ratio`
* **22**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_6/Release`
* **23**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_6/Threshold`
* **24**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_7/Attack`
* **25**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_7/Ratio`
* **26**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_7/Release`
* **27**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_7/Threshold`
* **28**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_8/Attack`
* **29**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_8/Ratio`
* **30**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_8/Release`
* **31**: `/GaitSoni/Mapping_Tabs/Comp_Section/Track_8/Threshold`
* **32**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_1/1_HPF/Freq`
* **33**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_1/1_HPF/Q`
* **34**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_1/2_Parametric_1/Freq`
* **35**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_1/2_Parametric_1/Gain`
* **36**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_1/2_Parametric_1/Q`
* **37**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_1/3_Parametric_2/Freq`
* **38**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_1/3_Parametric_2/Gain`
* **39**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_1/3_Parametric_2/Q`
* **40**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_1/4_LPF/Freq`
* **41**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_1/4_LPF/Q`
* **42**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_2/1_HPF/Freq`
* **43**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_2/1_HPF/Q`
* **44**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_2/2_Parametric_1/Freq`
* **45**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_2/2_Parametric_1/Gain`
* **46**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_2/2_Parametric_1/Q`
* **47**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_2/3_Parametric_2/Freq`
* **48**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_2/3_Parametric_2/Gain`
* **49**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_2/3_Parametric_2/Q`
* **50**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_2/4_LPF/Freq`
* **51**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_2/4_LPF/Q`
* **52**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_3/1_HPF/Freq`
* **53**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_3/1_HPF/Q`
* **54**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_3/2_Parametric_1/Freq`
* **55**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_3/2_Parametric_1/Gain`
* **56**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_3/2_Parametric_1/Q`
* **57**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_3/3_Parametric_2/Freq`
* **58**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_3/3_Parametric_2/Gain`
* **59**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_3/3_Parametric_2/Q`
* **60**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_3/4_LPF/Freq`
* **61**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_3/4_LPF/Q`
* **62**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_4/1_HPF/Freq`
* **63**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_4/1_HPF/Q`
* **64**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_4/2_Parametric_1/Freq`
* **65**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_4/2_Parametric_1/Gain`
* **66**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_4/2_Parametric_1/Q`
* **67**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_4/3_Parametric_2/Freq`
* **68**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_4/3_Parametric_2/Gain`
* **69**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_4/3_Parametric_2/Q`
* **70**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_4/4_LPF/Freq`
* **71**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_4/4_LPF/Q`
* **72**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_5/1_HPF/Freq`
* **73**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_5/1_HPF/Q`
* **74**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_5/2_Parametric_1/Freq`
* **75**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_5/2_Parametric_1/Gain`
* **76**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_5/2_Parametric_1/Q`
* **77**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_5/3_Parametric_2/Freq`
* **78**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_5/3_Parametric_2/Gain`
* **79**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_5/3_Parametric_2/Q`
* **80**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_5/4_LPF/Freq`
* **81**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_5/4_LPF/Q`
* **82**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_6/1_HPF/Freq`
* **83**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_6/1_HPF/Q`
* **84**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_6/2_Parametric_1/Freq`
* **85**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_6/2_Parametric_1/Gain`
* **86**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_6/2_Parametric_1/Q`
* **87**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_6/3_Parametric_2/Freq`
* **88**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_6/3_Parametric_2/Gain`
* **89**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_6/3_Parametric_2/Q`
* **90**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_6/4_LPF/Freq`
* **91**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_6/4_LPF/Q`
* **92**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_7/1_HPF/Freq`
* **93**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_7/1_HPF/Q`
* **94**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_7/2_Parametric_1/Freq`
* **95**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_7/2_Parametric_1/Gain`
* **96**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_7/2_Parametric_1/Q`
* **97**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_7/3_Parametric_2/Freq`
* **98**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_7/3_Parametric_2/Gain`
* **99**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_7/3_Parametric_2/Q`
* **100**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_7/4_LPF/Freq`
* **101**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_7/4_LPF/Q`
* **102**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_8/1_HPF/Freq`
* **103**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_8/1_HPF/Q`
* **104**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_8/2_Parametric_1/Freq`
* **105**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_8/2_Parametric_1/Gain`
* **106**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_8/2_Parametric_1/Q`
* **107**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_8/3_Parametric_2/Freq`
* **108**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_8/3_Parametric_2/Gain`
* **109**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_8/3_Parametric_2/Q`
* **110**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_8/4_LPF/Freq`
* **111**: `/GaitSoni/Mapping_Tabs/EQ_Section/Track_8/4_LPF/Q`
* **112**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Master_EQ/1_HPF/Freq`
* **113**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Master_EQ/1_HPF/Q`
* **114**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Master_EQ/2_Parametric_1/Freq`
* **115**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Master_EQ/2_Parametric_1/Gain`
* **116**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Master_EQ/2_Parametric_1/Q`
* **117**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Master_EQ/3_Parametric_2/Freq`
* **118**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Master_EQ/3_Parametric_2/Gain`
* **119**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Master_EQ/3_Parametric_2/Q`
* **120**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Master_EQ/4_LPF/Freq`
* **121**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Master_EQ/4_LPF/Q`
* **122**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Master_Gain/Master_Gain`
* **123**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Master_Gain/Soni_Buss_Gain`
* **124**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Gain/Track_1`
* **125**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Gain/Track_2`
* **126**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Gain/Track_3`
* **127**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Gain/Track_4`
* **128**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Gain/Track_5`
* **129**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Gain/Track_6`
* **130**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Gain/Track_7`
* **131**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Gain/Track_8`
* **132**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Mute/1`
* **133**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Mute/2`
* **134**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Mute/3`
* **135**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Mute/4`
* **136**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Mute/5`
* **137**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Mute/6`
* **138**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Mute/7`
* **139**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Mute/8`
* **140**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Variant/Variant_1`
* **141**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Variant/Variant_2`
* **142**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Variant/Variant_3`
* **143**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Variant/Variant_4`
* **144**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Variant/Variant_5`
* **145**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Variant/Variant_6`
* **146**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Variant/Variant_7`
* **147**: `/GaitSoni/Mapping_Tabs/Mixer_And_Master/Track_Variant/Variant_8`
* **148**: `/GaitSoni/Mapping_Tabs/Music_Info/Accent/T4_A_1`
* **149**: `/GaitSoni/Mapping_Tabs/Music_Info/Accent/T4_A_2`
* **150**: `/GaitSoni/Mapping_Tabs/Music_Info/Accent/T4_A_3`
* **151**: `/GaitSoni/Mapping_Tabs/Music_Info/Accent/T4_A_4`
* **152**: `/GaitSoni/Mapping_Tabs/Music_Info/Accent/T5_A_1`
* **153**: `/GaitSoni/Mapping_Tabs/Music_Info/Accent/T6_A_1`
* **154**: `/GaitSoni/Mapping_Tabs/Music_Info/Fluidity`
* **155**: `/GaitSoni/Mapping_Tabs/Music_Info/Pitch/T4_P_1`
* **156**: `/GaitSoni/Mapping_Tabs/Music_Info/Pitch/T4_P_2`
* **157**: `/GaitSoni/Mapping_Tabs/Music_Info/Pitch/T4_P_3`
* **158**: `/GaitSoni/Mapping_Tabs/Music_Info/Pitch/T4_P_4`
* **159**: `/GaitSoni/Mapping_Tabs/Music_Info/Pitch/T5_P_1`
* **160**: `/GaitSoni/Mapping_Tabs/Music_Info/Pitch/T6_P_1`
* **161**: `/GaitSoni/Mapping_Tabs/Music_Info/Pitch/T7_P_1`
* **162**: `/GaitSoni/Mapping_Tabs/Music_Info/Pitch/T7_P_2`
* **163**: `/GaitSoni/Mapping_Tabs/Music_Info/Pitch/T7_P_3`
* **164**: `/GaitSoni/Mapping_Tabs/Music_Info/Pitch/T7_P_4`
* **165**: `/GaitSoni/Mapping_Tabs/Music_Info/Tempo`
* **166**: `/GaitSoni/Mapping_Tabs/Music_Info/Velocity/T1_V_1`
* **167**: `/GaitSoni/Mapping_Tabs/Music_Info/Velocity/T2_V_1`
* **168**: `/GaitSoni/Mapping_Tabs/Music_Info/Velocity/T3_V_1`
* **169**: `/GaitSoni/Mapping_Tabs/Music_Info/Velocity/T4_V_1`
* **170**: `/GaitSoni/Mapping_Tabs/Music_Info/Velocity/T4_V_2`
* **171**: `/GaitSoni/Mapping_Tabs/Music_Info/Velocity/T4_V_3`
* **172**: `/GaitSoni/Mapping_Tabs/Music_Info/Velocity/T4_V_4`
* **173**: `/GaitSoni/Mapping_Tabs/Music_Info/Velocity/T5_V_1`
* **174**: `/GaitSoni/Mapping_Tabs/Music_Info/Velocity/T6_V_1`
* **175**: `/GaitSoni/Mapping_Tabs/Music_Info/Velocity/T7_V_1`
* **176**: `/GaitSoni/Mapping_Tabs/Music_Info/Velocity/T8_V_1`
* **177**: `/GaitSoni/Mapping_Tabs/Sonification_Types/AP_Values/Soni_1`
* **178**: `/GaitSoni/Mapping_Tabs/Sonification_Types/AP_Values/Soni_10`
* **179**: `/GaitSoni/Mapping_Tabs/Sonification_Types/AP_Values/Soni_11`
* **180**: `/GaitSoni/Mapping_Tabs/Sonification_Types/AP_Values/Soni_12`
* **181**: `/GaitSoni/Mapping_Tabs/Sonification_Types/AP_Values/Soni_13`
* **182**: `/GaitSoni/Mapping_Tabs/Sonification_Types/AP_Values/Soni_2`
* **183**: `/GaitSoni/Mapping_Tabs/Sonification_Types/AP_Values/Soni_3`
* **184**: `/GaitSoni/Mapping_Tabs/Sonification_Types/AP_Values/Soni_4`
* **185**: `/GaitSoni/Mapping_Tabs/Sonification_Types/AP_Values/Soni_5`
* **186**: `/GaitSoni/Mapping_Tabs/Sonification_Types/AP_Values/Soni_6`
* **187**: `/GaitSoni/Mapping_Tabs/Sonification_Types/AP_Values/Soni_7`
* **188**: `/GaitSoni/Mapping_Tabs/Sonification_Types/AP_Values/Soni_8`
* **189**: `/GaitSoni/Mapping_Tabs/Sonification_Types/AP_Values/Soni_9`


## API Reference

### `DspFaust()`
Default constructor, to be used wih audio drivers

 that impose their sample rate and buffer size

(like JACK and JUCE)



#### Arguments



* `auto_connect`: whether to automatically connect audio outpus to the hardware (usable with JACK)




---


### `DspFaust(int SR, int BS)`
Constructor.



#### Arguments



* `SR`: sampling rate

* `BS`: buffer size

* `auto_connect`: whether to automatically connect audio outputs to the hardware (usable with JACK)


---


### `DspFaust(cinst string& dsp_content, int SR, int BS)`
Constructor.



#### Arguments



* `dsp_content`: the DSP as a file or string

* `SR`: sampling rate

* `BS`: buffer size

* `auto_connect`: whether to automatically connect audio outputs to the hardware (usable with JACK)


---


### `bool start()`
Start the audio processing.



Returns `true` if successful and `false` if not.


---


### `void stop()`
Stop the audio processing.


---


### `bool isRunning()`
Returns `true` if audio is running.


---


### `long keyOn(int pitch, int velocity)`
Instantiate a new polyphonic voice. This method can

only be used if the `[style:poly]` metadata is used in

the Faust code or if the `-polyvoices` flag has been

provided before compilation.



`keyOn` will return 0 if the Faust object is not

polyphonic or the address to the allocated voice as

a `long` otherwise. This value can be used later with

[`setVoiceParamValue`](#setvoiceparamvalue) or

[`getVoiceParamValue`](#getvoiceparamvalue) to access

the parameters of a specific voice.



#### Arguments



* `pitch`: MIDI note number (0-127)

* `velocity`: MIDI velocity (0-127)


---


### `int keyOff(int pitch)`
De-instantiate a polyphonic voice. This method can

only be used if the `[style:poly]` metadata is used in

the Faust code or if the `-nvoices` flag has been

provided before compilation.



`keyOff` will return 0 if the object is not polyphonic

and 1 otherwise.



#### Arguments



* `pitch`: MIDI note number (0-127), should be the same

as the one used for `keyOn`


---


### `long newVoice()`
Instantiate a new polyphonic voice. This method can

only be used if the `[style:poly]` metadata is used in

the Faust code or if `-nvoices` flag has been

provided before compilation.



`newVoice` will return 0 if the Faust object is not

polyphonic or the address to the allocated voice as

a `long` otherwise. This value can be used later with

`setVoiceParamValue`, `getVoiceParamValue` or

`deleteVoice` to access the parameters of a specific

voice.


---


### `int deleteVoice(long voice)`
De-instantiate a polyphonic voice. This method can

only be used if the `[style:poly]` metadata is used in

the Faust code or if `-nvoices` flag has been

provided before compilation.



`deleteVoice` will return 0 if the object is not polyphonic

and 1 otherwise.



#### Arguments



* `voice`: the address of the voice given by `newVoice`


---


### `void allNotesOff()`
Terminates all the active voices, gently (with release when hard = false or immediately when hard = true).


---


### `void propagateMidi(int count, double time, int type, int channel, int data1, int data2)`
Take a raw MIDI message and propagate it to the Faust

DSP object. This method can be used concurrently with

[`keyOn`](#keyOn) and [`keyOff`](#keyOff).



`propagateMidi` can

only be used if the `[style:poly]` metadata is used in

the Faust code or if `-nvoices` flag has been

provided before compilation.



#### Arguments



* `count`: size of the message (1-3)

* `time`: time stamp

* `type`: message type (byte)

* `channel`: channel number

* `data1`: first data byte (should be `null` if `count<2`)

* `data2`: second data byte (should be `null` if `count<3`)


---


### `const char* getJSONUI()`
Returns the JSON description of the UI of the Faust object.


---


### `const char* getJSONMeta()`
Returns the JSON description of the metadata of the Faust object.


---


### `int getParamsCount()`
Returns the number of parameters of the Faust object.


---


### `void setParamValue(const char* address, float value)`
Set the value of one of the parameters of the Faust

object in function of its address (path).



#### Arguments



* `address`: address (path) of the parameter

* `value`: value of the parameter


---


### `void setParamValue(int id, float value)`
Set the value of one of the parameters of the Faust

object in function of its id.



#### Arguments



* `id`: id of the parameter

* `value`: value of the parameter


---


### `float getParamValue(const char* address)`
Returns the value of a parameter in function of its

address (path).



#### Arguments



* `address`: address (path) of the parameter


---


### `float getParamValue(int id)`
Returns the value of a parameter in function of its

id.



#### Arguments



* `id`: id of the parameter


---


### `void setVoiceParamValue(const char* address, uintptr_t voice, float value)`
Set the value of one of the parameters of the Faust

object in function of its address (path) for a

specific voice.



#### Arguments



* `address`: address (path) of the parameter

* `voice`: address of the polyphonic voice (retrieved

from `keyOn`

* `value`: value of the parameter


---


### `void setVoiceValue(int id, long voice, float value)`
Set the value of one of the parameters of the Faust

object in function of its id for a

specific voice.



#### Arguments



* `id`: id of the parameter

* `voice`: address of the polyphonic voice (retrieved

from `keyOn`

* `value`: value of the parameter


---


### `float getVoiceParamValue(const char* address, long voice)`
Returns the value of a parameter in function of its

address (path) for a specific voice.



#### Arguments



* `address`: address (path) of the parameter

* `voice`: address of the polyphonic voice (retrieved

from `keyOn`)


---


### `float getVoiceParamValue(int id, long voice)`
Returns the value of a parameter in function of its

id for a specific voice.



#### Arguments



* `id`: id of the parameter

* `voice`: address of the polyphonic voice (retrieved

from `keyOn`)


---


### `const char* getParamAddress(int id)`
Returns the address (path) of a parameter in function

of its ID.



#### Arguments



* `id`: id of the parameter


---


### `const char* getVoiceParamAddress(int id, long voice)`
Returns the address (path) of a parameter in function

of its ID.



#### Arguments



* `id`: id of the parameter

* `voice`: address of the polyphonic voice (retrieved

from `keyOn`)


---


### `float getParamMin(const char* address)`
Returns the minimum value of a parameter in function of

its address (path).



#### Arguments



* `address`: address (path) of the parameter


---


### `float getParamMin(int id)`
Returns the minimum value of a parameter in function

of its ID.



#### Arguments



* `id`: id of the parameter


---


### `float getParamMax(const char* address)`
Returns the maximum value of a parameter in function of

its address (path).



#### Arguments



* `address`: address (path) of the parameter


---


### `float getParamMax(int id)`
Returns the maximum value of a parameter in function

of its ID.



#### Arguments



* `id`: id of the parameter


---


### `float getParamInit(const char* address)`
Returns the default value of a parameter in function of

its address (path).



#### Arguments



* `address`: address (path) of the parameter


---


### `float getParamInit(int id)`
Returns the default value of a parameter in function

of its ID.



#### Arguments



* `id`: id of the parameter


---


### `const char* getMetadata(const char* address, const char* key)`
Returns the metadataof a parameter in function of

its address (path) and the metadata key.



#### Arguments



* `address`: address (path) of the parameter


---


### `const char* getMetadata(int id, const char* key)`
Returns the metadataof a parameter in function of

its iD and the metadata key.



#### Arguments



* `id`: id of the parameter


---


### `void propagateAcc(int acc, float v)`
Propagate the RAW value of a specific accelerometer

axis to the Faust object.



#### Arguments



* `acc`: the accelerometer axis (**0**: x, **1**: y, **2**: z)

* `v`: the RAW acceleromter value in m/s


---


### `void setAccConverter(int p, int acc, int curve, float amin, float amid, float amax)`
Set the conversion curve for the accelerometer.



#### Arguments



* `p`: the UI parameter id

* `acc`: the accelerometer axis (**0**: x, **1**: y, **2**: z)

* `curve`: the curve (**0**: up, **1**: down, **2**: up and down)

* `amin`: mapping min point

* `amid`: mapping middle point

* `amax`: mapping max point


---


### `void propagateGyr(int gyr, float v)`
Propagate the RAW value of a specific gyroscope

axis to the Faust object.



#### Arguments



* `gyr`: the gyroscope axis (**0**: x, **1**: y, **2**: z)

* `v`: the RAW acceleromter value in m/s


---


### `void setGyrConverter(int p, int gyr, int curve, float amin, float amid, float amax)`
Set the conversion curve for the gyroscope.



#### Arguments



* `p`: the UI parameter id

* `gyr`: the gyroscope axis (**0**: x, **1**: y, **2**: z)

* `curve`: the curve (**0**: up, **1**: down, **2**: up and down)

* `amin`: mapping min point

* `amid`: mapping middle point

* `amax`: mapping max point


---


### `float getCPULoad()`
Returns the CPU load (between 0 and 1.0).


---


### `void configureOSC(int xmit, int inport, int outport, int errport, const char* address)`
Change the OSC configuration



#### Arguments



* `xmit`: the xmit state (off-0 all-1 alias-2)

* `inport`: the input port number

* `outport`: the output port number

* `errport`: the error port number

* `address`: the destination IP address


---


### `bool isOSCOn()`
Return OSC Status


---

