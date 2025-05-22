# KDMAPI Emulator | Wrapping KDMAPI back to winmm! 
![image](https://github.com/user-attachments/assets/14c1a00c-cc04-408f-a215-60ed3cb8479b)

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
- [x] Zenith
      ![image](https://github.com/user-attachments/assets/e086b76b-e93c-4cce-9430-cb1da8bff920)
- [x] Kiva (Safe because Kiva used other methods to grab OmniMIDI's sound font list that does not require KDMAPI)
      ![image](https://github.com/user-attachments/assets/6a7a6492-c32b-4ba4-9551-7623865f1605)
- [x] Piano-FX Pro
      ![image](https://github.com/user-attachments/assets/7366a87d-97b0-4c78-bc46-c373a205a63b)
- [x] Piano From Above VIZ Khang
      ![image](https://github.com/user-attachments/assets/7cf5f0d7-d736-4530-b38e-2cb9adee95a1)
- [ ] Mmidi (Audio seems to be fine but has some weird visual glitches that I have no idea what caused it)
      ![image](https://github.com/user-attachments/assets/c0f4d20a-c865-4fc2-b32f-cc0ab1645730)
- [x] KDMAPI Passthrough VST Plugin For Fl Studio
      ![image](https://github.com/user-attachments/assets/24f69203-436e-485d-b52e-3c0b54d2dd3a)
- [x] Zig MIDI Player
      ![image](https://github.com/user-attachments/assets/cb27b097-4def-40b6-9a16-15457994d9cb)
- [x] Wasabi
      ![image](https://github.com/user-attachments/assets/25f9097f-35f0-4822-8a85-226fc7324c6f)


# For Windows 8 and above
Microsoft changed how EnterCriticalSection() and LeaveCriticalSection() works in Windows 8 which makes winmm super super slow. 

But you know what, winmm don't actually need EnterCriticalSection() and LeaveCriticalSection() at all! 

Not even Microsoft itself realizes that the winmm functions are actually THREAD SAFE. 

Locking the thread on every single note is just a pure waste of time. 

You can of course hack winmm.dll using IDA Pro and remove all EnterCriticalSection() and LeaveCriticalSection() function calls, and all your midi players will still work perfectly fine. 

I made a patch for 22000.1's winmmbase.dll in my Lindows 11.1 project, you can take a look at my patched IDA database: https://github.com/happymimimix/Lindows_11.1-Post_Reset/tree/master/winmm.dll

If you are also using this exact same build of windows, you can download the pre-patched winmmbase.dll in my repository and replace the one on your device.

However, if you are using any other version of windows, even if the build number differs by only 1, this will NOT work!

If you are using another build of windows, 22000.4 for example, you can use my IDA database as a reference and make your own patch.

You cannot patch any winmmbase.dll by loading my database in IDA Pro and click 'Apply patches to input file', it won't work! My database is created for that exact build only. You must create your own database and make your own patches. 

What you could do is use my database as a reference and ONLY AS A REFERENCE. Disassemble the winmm.dll on your build and open my database in a new window. Place the two windows side by side and check what's different between the two databases. 

# An easier way to patch
If you think disassembling and patching winmmbase.dll on your own is way too hard for you, I also have another solution: The pure winmm wrapper method. 

Be aware that this method does NOT apply globally and requires you to patch every single app that you're using.

You're all very familar with the traditional winmm wrapper that redirects function calls to KDMAPI right? 

This "pure" winmm wrapper functions in a similar way but provides support for ALL midi output devices.

You can download this patch here: 
[For_64bit.zip](https://github.com/user-attachments/files/20381898/For_64bit.zip)
[For_32bit.zip](https://github.com/user-attachments/files/20381897/For_32bit.zip)

# Installation
There are two different methods for installing this KDMAPI wrapper. 

The first method is to place my wrapper dll in `C:\Windows\System32`. 

This will apply this wrapper globally for all midi players but will conflict with the real KDMAPI. 

If you want to keep the real KDMAPI for most apps, only wraping a few specific apps. 
Then you can place my wrapper dll in the same directory as the executable file of your midi player. 

This way the KMDAPI wrapper is only available to these specific apps that you applied it to, all the other apps won't be affected. 

### Note: 
Some midi players like to access OmniMIDI.dll using an absolute path `C:\Windows\System32\OmniMIDI\OmniMIDI.dll` such as Zenith and Kiva. 

In this case, you can create a symbolic link to make sure the wrapper works correctly for these apps by running the following commands in CMD: 
```
mkdir C:\Windows\System32\OmniMIDI
mklink C:\Windows\System32\OmniMIDI\OmniMIDI.dll C:\Windows\System32\OmniMIDI.dll /H
```

# For 32bit apps
I think it's very unlikely for someone to create a 32bit black midi player.

It's 2025 and who the f\*\*k in this world still don't have a 64bit processor and need to suffer for the 32bit 4GB RAM limit?

However, I'll still provide support for them just in case. 

When running a 32bit app on 64bit Windows, the app will be accessing `C:\Windows\SysWOW64\OmniMIDI.dll` instead of `C:\Windows\System32\OmniMIDI.dll`. 

Why 32bit libraries are placed in SysWOW64 and 64bit libraries are placed in System32? I don't know, you should blame Microsoft for picking this confusing name! 

You should place the 32bit version of my wrapper dll in `C:\Windows\SysWOW64` to make 32bit apps work correctly. 

Always remember that System32 stores 64bit libraries! Do not place 32bit libraries in System32, they will NOT work. 
You should put 32bit libraries in SysWOW64! 

# Use the app specific winmm patch when applying KDMAPI wrapper globally
I strongly suggest you go for the more complex but more stable global winmm patch method if you want to use my KDMAPI wrapper globally.

Also, my modified Windows image Lindows 11.1 Pro will be released very soon in the near future featuring a pre-patched winmmbase.dll ready to use out of the box together with a complete driver signature enforcement kill. 

You could make a kernel mode midi driver on Lindows 11.1 if you want to. Driver signature enforcement has been completely doomed on Lindows 11.1 and you'll not suffer for DSE ever again. 

If you don't want to wait long for the release of Lindows 11.1 and also don't want to spend time patching winmmbase.dll on your own, there is very fortunately still a method that you can use: 

1. Download the app specific pure winmm patch here:
[For_64bit.zip](https://github.com/user-attachments/files/20381898/For_64bit.zip)
[For_32bit.zip](https://github.com/user-attachments/files/20381897/For_32bit.zip)

2. Rename the dll and make sure the new name is not longer than 5 characters. For example `wmm++.dll`.
  Be aware that any name more than 5 characters WILL FAIL! Do NOT use a name like `WinmmBut1000xFaster.dll`!

3. Download CFF Explorer from here: https://ntcore.com/explorer-suite/

4. Load my wrapper dll into CFF Explorer

5. Locate WINMM.dll in Import Directory:

![image](https://github.com/user-attachments/assets/f1a986f4-1086-4e40-9056-475d1f17d48a)

7. Double click on the dll name to rename it:

![image](https://github.com/user-attachments/assets/f2373643-38e3-48b0-a275-9aed619664ae)

9. Save the modified wrapper dll

![image](https://github.com/user-attachments/assets/3407a39c-3c87-4e6d-bade-9381f25be23a)

Finally, place the modified wrapper dll and the renamed pure winmm patch dll in System32 and enjoy!
