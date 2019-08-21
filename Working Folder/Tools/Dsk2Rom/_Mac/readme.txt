
                                DSK2ROM v0.80

                       Copyright 2007 Vincent van Dam
                          (vincentd@erg.verweg.com)


This is an adjusted diskrom that loads sectors from rom instead of a disk. It
can be used to convert .dsk images to .rom images. The converted .rom images
will act as a read only drive. If another diskinterface is present, it will be
omitted (thus not usuable).

Converting .dsk images can be done by either using the dsk2rom tool, or by
concatenating the custom a .dsk file to the custom diskrom. The dsk2rom tool
doesn't run on MSX, but adds a few interesting new features (like compression
and switching of mapper type).

To convert .dsk images to .rom images without the dsk2rom tool; use the 
following commands:

 * MSXDOS 2:         concat /b dsk2rom.rom+<dskfile> <romfile>  
 * Windows/MS-DOS:   copy /b dsk2rom.rom+<dskfile> <romfile>  
 * Unix:             cat dsk2rom.rom <dskfile> > <romfile>

The mapper that is used is for the converted rom without using the dsk2rom tool
itself is 'KonamiSCC'.

If the dsk2rom tool is used for conversion, a few more options will become
available. The dsk2rom tool is a command-line tool with a few options. The
available options will be displayed when the tool is started with no arguments
at all.

Exanples converting .dsk image to .rom images using the dsk2rom tool;

 * dsk2rom -f6c 2 alesteg.dsk alesteg.rom
 * dsk2rom -sfc 2 feedback.dsk feedback.rom

The most interesting option is compression (-c), it should be followed by a
number which defines which compression should be used. Compression level 0
means no compression at all, and has the highest performance in loading. The
resulting rom image will be the same as creating one without the tool. Level 1
compression will only index sectors, which means all redundant sectors will be
removed. Level 2 compression adds the 'pletter' compression scheme upon the
sectors and will result in smaller rom files, but also in a slightly lower
performance when loading the rom image.

By default the resulting .rom images will use the Konami SCC mapper, this can
be changed to the ASCII8 mapper. Some (bigger) flash cartridges use the ASCII8
mapper, the dsk2rom tool can path the kernel to use the ASCII8 mapper addresses
instead by adding the -a option when converting. Note that it is not wise to
let the flash software patch the .rom image for you, because most of the .rom
image is actually a data part, the flash software could wrongly patch this
data, corrupting the image.

Especially the SCC mapper can conflict with aggressive memory search methods.
To shield against this you can use the -s option (safe mode), which will
always result in a rom which will either be 512K or 1024K. This rom has will
be less vulnerable against memory searches.

Other options include; -5 and -6 to either switch to 50hz/pal or 60hz/ntsc
during boot of the rom. The ability to force the msx1 palette using -p, as well
as to allow booting of other diskroms to with -d.

The other options available are -v for verbose information while creating the
rom, and the -f option to create rom images in standard sizes (2^sizes).

This should be enough info to start converting .dsk images to .rom images.
Good luck!

                        Acknowledgements and thanks
                        ---------------------------

DSK2ROM by Vincent van Dam. The kernel was based on the disassembled diskroms
by Arjen Zeilemaker. The pletter compression used in the kernel and tooling is
made by Sjoerd Mastijn, which was based on Bitbuster by Arjan Bakker. A thank
you to Arturo Ragozini for suggesting to use pletter compression. A lot of
thanks go to Ramones for his testing and diskrom kernel hacking, and not to
forget his GETDPB implementation included in the kernel.


                              Version history
                              ---------------

[2007/06/18] 0.80
-----------------
* added safe mode to support dsk images with aggressive memory searching
* added setting 50/60hz during boot
* added setting msx1 palette during boot
* added non-exclusive boot mode

[2007/05/27] 0.70
-----------------
* added dsk2rom tooling for pc
* compression support
* improved mapper support (default scc, switchable via the dsk2rom tooling)
* removed last part of interrupt handler
* disabled h.hstke
* improved getdpb

[2007/05/22] 0.65
-----------------
* fixed loading into page 1
* removed custom interrupt handler
* another hack added to make sure it's always the only diskrom

[2007/05/22] 0.60
-----------------
* fixed loading into page 1
* support for ascii8 mapper

[2007/05/19] 0.50
-----------------
* first public release

