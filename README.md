# KDMAPI Emulator | Wrapping KDMAPI back to winmm! 
Recently, coolsoft released Virtual MIDI Synth 2.13.9 which improved the speed of the driver massively. 

Virtual MIDI Synth 2.13.9 is almost as fast as OmniMIDI!

The only thing stopping me from uninstalling OmniMIDI and completely switch to Virtual MIDI Synth is KDMAPI and these KDMAPI exclusive midi players. 

Some midi players like Zenith, Mmidi, MPGL, Wasabi, and Ziggy MIDI Player are KDMAPI-exclusive! Which means these midi players only supports KDMAPI and does not have any support for winmm at all. 

Unfortunately KDMAPI is an OmniMIDI exclusive feature that no other midi synths supports. 

Does that mean it's absolutely impossible to use Virtual MIDI Synth or Loop MIDI on these KDMAPI exclusive midi players? 

NO, IT IS STILL POSSIBLE! 

Consider how people make winmm exclusive apps support KDMAPI. What do people do to apps like Fl Studio, Domino, Synthesia, and Piano From Above? 

That's right, we created a fake winmm.dll that wraps all midi related functions in winmm.dll to KDMAPI! 

This process can be done completely without touching the program's source code, you just need a fake winmm.dll placed in the same directory as the midi player exe and now this winmm exclusive midi player can use KDMAPI! 

Then I had this idea: Is doing the reverse possible? Wrapping KDMAPI back to winmm? 

The answer is YES! You can wrap KDMAPI back to winmm in the exact same way as how people wrap winmm to KDMAPI. 

Here, I present to you the "KDMAPI Emulator".

With this fake OmniMIDI.dll you can let these KDMAPI exclusive midi players use winmm and allow for the selection of a much wider range of midi output devices such as Virtual MIDI Synth, Loop MIDI, or Microsoft GS Wavetable Synth. 

# The limits of this wrapper
