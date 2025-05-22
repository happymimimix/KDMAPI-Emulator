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

This process can be done completely without touching the program's source code, you just need a fake winmm.dll placed in the same directory as the midi player's executable file and now this winmm exclusive midi player can use KDMAPI! 

Then I had this idea: Is it possible to do the reverse? Wrapping KDMAPI back to winmm? 

The answer is YES! You can wrap KDMAPI back to winmm in the exact same way as how people wrap winmm to KDMAPI. 

Here, I present to you the "KDMAPI Emulator".

With this fake OmniMIDI.dll you can let these KDMAPI exclusive midi players use winmm and allow for the selection of a much wider range of midi output devices such as Virtual MIDI Synth, Loop MIDI, or Microsoft GS Wavetable Synth. 

# The limits of this wrapper
Majority of the KDMAPI exclusive midi players should work file with my wrapper dll but there are a few exceptions: 

1. It's not possible to do NoBuf in winmm.
  - OmniMIDI provides a SendDirectDataNoBuf() function in KDMAPI that disables buffering, but unfortunately that is not possible with winmm.
  - In this wrapper, SendDirectDataNoBuf() is exactly the same as SendDirectData().
  - You will encounter some unexpected behavior in midi players that need SendDirectDataNoBuf(), but this is already the best it could ever get in winmm.
2. No playback tracking.
  - OmniMIDI not only synthesize the midi notes sent from the midi player, it also provides some time tracking functionality using it's unique CookedPlayer component.
  - Unfortunately winmm does not have an equivalent component for CookedPlayer and all midi players that relies on OmniMIDI's CookedPlayer for playback tracking will NOT work.
  - This situation should be incredibly rare though as majority of the midi players implements their own playback tracking logic and does not rely on the driver. But if you do encounter a midi player that asks the driver to provide playback position information then I'm sorry but using real KDMAPI is your only choice.
3. No soundfont list & configuration info
  - The wrapper simply do not have access to information like this, winmm does not provide a unified interface for this.
  - If you need to edit the configuration & soundfont list, you should use the configurator provided by the midi synth that you're using instead of trying to manipulate the synth config through KDMAPI.

# App compatibility list
[*] Zenith
[*] Kiva
[*] Piano-FX Pro
[*] Piano From Above VIZ Khang
[ ] Mmidi (Audio seems to be fine but has some weird visual glitches that I have no idea what caused it)
[*] KDMAPI Passthrough VST Plugin For Fl Studio
[*] Wasabi
