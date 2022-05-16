# o\_C and Teensy 4.0

Just the drivers please, we're appless. Sans apps? Mit ohne apps?

## Allow myself to contradict... myself
So far my position on using a Teensy 4.0 to update [O_C](https://github.com/patrickdowling/O_C) has been _meh, it should work, but needs some effort and I have no interest_.
The argument I made was along these lines:

> Since it’s “just” a faster processor and more memory, that doesn’t solve the “single SPI shared by screen and DAC” bottleneck, so it doesn't really seem like that big a deal. The low-level drivers in `o_C` are fairly bare metal to enable driving the DAC/screen at a constant update rate, so those would need rewriting. Given the ad-hoc nature of the original “framework” (\*), jif one were to do that, replacing the framework would make sense. But that would mean porting all the apps, which is painful (\*\*) so no thanks.

There have been other attempts to shoehorn faster processors (T3.6, or my own [ocf4](https://github.com/patrickdowling/oCf4)) to replace the T3.2 but my theory is that without a plan to _use_ the ludicrous speed, they fizzled out. And once you actually upgrade to something with multiple SPI busses you might as well start fresh with a new design.

...eventually all that was just hot air because I ended up with a Teensy 4.0, downloaded the RT1060 datasheet, set a time budget of a weekend, and wrote new drivers.

(\*) It’s a horrible interface that evolved while trying to maintain backwards compatibility to the existing apps at the time, while writing new drivers. So while it's low overhead, mistakes where perhaps made :)

(\*\*) I know it’s painful because I refactored the framework twice already; the last ill-fated overhaul never made it to “production” but introduced an abstraction away from the ADC/DAC/TRx, as well as input/output labels and other fun things. It also fizzled out, because of the app rewrites necessary. Which is a shame because it would have made porting to T4.0 much easier.

## Where’s the apps?
- Not in this repo. This is mainly a rewrite of the `o_C` hardware drivers on a Teensy 4.0, so most of the interfaces have changed. It's not "production ready" (e.g. there's no tests) nor complete. 
- There’s new API for “processing” and “menus” (WIP).
- I may add a proof-of-concept shim layer to be able to port existing apps (shims may also have been a better repo name ;))
- Supporting and maintaining "all the apps" (which were written by multiple people) all in the same repo is a PITA and was never really scaleable. Similarly adding third-party or user apps becomes a mess of branches and merging.
- If anything, it's probably easiest to port Hemispheres to the new codebase because it already has an "opinionated" API. This may be a good or bad thing :)
- So there's really a bunch of work left to make this into anything resembling a "product".

If there is a plan here, it would be to keep the “core/framework” and the “apps” mostly separate. That might mean this repo would be added as a submodule somewhere, or that finally someone can make a configurable firmware builder (although with the amount of flash now, that might be moot). 

## Driver status
- [x] DAC8565 seems to work fine. Perhaps even better than before using hardware PCS and FIFO.
- [x] Screen uses the same DMA-based approach to transfer data while the apps are processing. There’s now hooks to set contrast and turn the thing off/on properly (for blanking).
- [x] SPI comes out at 31.25KHz. That’s marginally faster than before so might have side effects.
- [x] CVx: ADC now runs in-sync with the core ISR so the sample timing is deterministic. Quality/noisiness still needs evaluating and the parameters tuned.
- [x] TRx: Digital inputs use a `QTIMER` to count falling edges (rising on jack). This means the less-than-ideal per-input ISR overhead is gone, and simply checking if the counter has changed indicates the input was clocked. Is that better than polling? Maybe not, but the hardware's there so why not.
- [x] Switches work. No surpise there.
- [x] Encoders work. Ideally I'd have liked to use the hardware encoders but there didn’t seem to be an XBAR mapping.
- [x] Core ISR still running at 16.666Khz (see below).

Most of the driver details are implemented directly and not through existing libraries, except where they're not. What can I say, I'm more interested in knowing how things work under the hood.

## Missing
- Any kind of (unit) tests.
- Calibration.
- App status storage. Here’s the one thing where T4.0 actually has a huge disadvantage, if not a showstopper: There’s very little EEPROM storage (and it’s emulated in flash). Like 1080 bytes instead of 2K. That’s enough for calibration and a few bits & bobs but not app storage, which was already full.
- One option might be to use `LittleFS` but this gets deleted when uploading code... so not that helpful. There are some workarounds I have in mind but nothing ready.
- SD card would be the obvious choice but a) T4.1 won’t fit on all boards and b) it’s not standard on T4.0 without soldering.

## To-do
- The frequency counter hasn’t been implemented. This should work using either the `QTIMER` for capture (on all four TRx even) or possibly one of the other timers via `XBAR` (since the alternate functions don't map directly the used GPIOs).
- Break the screen transfers into configurable chunks. This would allow running the core ISR faster (e.g. 32KHz) although there would be more tearing (and that might sacrifice ADC sample quality). Also the platform is (IMO) simply not suited to audio.
- Evaluate ADC. I'm adding an extended debug menu for this.
- Also there are two ADCs we can use, and there’s probably a more streamlined scan approach (`QTIMER`->`ADC_HCn`).
- Filtering on the TRx inputs (`FILT_CNT` and `FILT_PER`).

## General/Hardware
- Given "When running at 600 MHz, Teensy 4.0 consumes approximately 100 mA current. Reducing CPU speed to 528 MHz or lower reduces power consumption." I’ve set the frequency to 480MHz. That's still 4x faster at least.
- There's also a temperature reading, which might be interesting to observe. Too bad it can't measure the VREG...
- While it seems like we *can* run the ADC to get 4 samples every 16.7KHz or more, that doesn't mean we should. The input conditioning will have an impact here (can be seen in practice with my [TU scope](https://github.com/patrickdowling/temps_utile-/wiki/APP:-silloscope)).
- Builds using [platformio](https://platformio.org/). This seems to be a better compromise than the Arduino IDE. Also way easier for automated build.
  * In theory it allows for freezing the framework version so there's no need to drag along a copy of libraries (except, see next item).
  * Unfortunately the brand-new platformio 6.0.0 seems to break the "build_src_flags only for project source" rule with explicit library versions :/
  * The Teensy core and other libraries are full of warnings. These are generally trivial to fix but it doesn't look like the owners are really responsive to PRs and maintaining a yet-another-fork annoying; I may still end up submitting them.
  * In the mean time, the includes are wrapped with `#pragma` etc. to enable better warnings/errors for the rest of the code and not specifying a specific version.
- Teensy 4.0 has a cache now. This means being careful around DMA and memory ordering.
- Using even more fancy DMA, it should actually be possible to chain the DAC/screen updates and get a more codec-like interface. I’ll leave this as an exercise for the reader.

## License
Yeah, after some back and forth I made it GPL. Ping me if you have a use case that requires something else.
