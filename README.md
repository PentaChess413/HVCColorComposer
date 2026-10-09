# The HVC Color Composer
An innovative color picker for Photoshop from 2005, now released as open source. Not mine (created by Andrew Hussie).

# So what's all this?
It's a Photoshop (and only Photoshop pre-CS4, it can and will break other hosts) color picker with an additional color range, which is HVC/Munsell notation, and allows for a more "color-sciency" approach to color picking.

# And the innovative part?
Well, there are a bunch of HVC pickers now. However, besides the QOL features like integration with Adobe Color Books, it can measure the distance between colors as if they were two points in perceptual space and, most importantly of all, automatically create a palette of colors that are a given number of units apart from each other.  
TL;DR: Color picker that generates palettes quickly and supports color science notation (Munsell, HVC)

# How to build?
Clone the repo and open it in VS2003. Install the Adobe Photoshop CS1 SDK and copy the headers and such to the Adobe_PS_CS_SDK folder in the top-level directory. It is theoretically possible to build on Mac OS X, but it has not been tested there.

# Credits
* This software was distributed by Master Colors LLC.
* The Software Development Kit is Adobe Photoshop's CS1 SDK, which is unlisted in Adobe's website. You will have to go to this Adobe link instead: [Here](https://download.macromedia.com/pub/developer/photoshop/sdk/PhotoshopCSSDKWin.zip) for the Windows version. [Here](https://download.macromedia.com/pub/developer/photoshop/sdk/PhotoshopCSSDKMac.hqx) for the Mac OS X version (requires BinHex).
* The code release would not have been possible without Alex Lelièvre and Andrew Hussie's help!
* The GNU Mathematical Precision library is included, but not used.
* Some proprietary code was excised at the copyright owners' request and replaced with mine, namely:
    * Dynamic arrays (they use Sean Barrett's stb_ds implementation instead)
    * A Photoshop memory allocator (very basic one, uses Photoshop's bufferprocs. Doesn't seem to cause the plugin or host to explode.)
    * A byte swapper (now taking advantage of MSVC and Mac native functionality)
    * Debug macros (output to Debug on Windows. probably broken on the Mac)
* Everyone in the Homestuck Discord who gave feedback is appreciated!
# License
It's released under the GNU General Public License 3.0 or later. Basically, have fun with it, don't try to patent anything or put DRM on it, and share the source code of what you make with it.
