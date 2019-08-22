nMSXTiles v0.9.1
================

nMSXTiles v0.8.2
================
Manual and changes in nMSXTilesv0.8.2.html or nMSXTilesv0.8.2.pdf (sorry, only in spanish. In english coming soon).

nMSXtiles v0.8.1
================
Screens, tiles and sprites editor for the MSX screen 2.

Quickstart
-----------
Create a new project to edit tiles and assign them to screen positions.

Bank0 allows to edit 256 tiles from the upper zone of the screen.
Same for Bank1 to the middle zone, and Bank2 to the lower zone.

To select a tile click over a bank tile. Button <<FromBank>> edit the tile in pattern editor on the right screen.
Button <<ToBank> copies edited tile over bank position selected.

Click over a screen position to assign the selected tile. Keep in mind that yo cannot assign for example tiles from 
bank1 to the lower zone of screen (it has to be tiles from bank2).

Pattern editor have restrictions of MSX screen 2. Labels Foregr and Backgr show if clicks over editor
will put pixels of foreground or background respectively. Right column show selected foreground and
background colors for each pattern row. To change color click over foreground or background and
click over desired color on upper palette.

New features version 0.8.1:
	* Tiles interchange in the same bank without modifying screen design.
	* One project allow managing screens set up to 64x64 screens.
	* Many minor features that you can read at http://code.google.com/p/nmsxtiles/w/list (under construction).

New features version 0.4:
    * Undo: CTRL + Z: This version allows to undo "To Bank" and "assign a Tile to the screen".
    * Copy and Paste a block of tiles on the screen. For example, a tree, made up of 4 tiles, will be possible to select it 
	  and using CTRL + C and CTRL + V to copy it on other positions on the screen.
    * With the right mouse button is assigned the first tile of the bank to that position. Ie, it becomes a zero, "is deleted."
    * Turning over the screen, show in the right box the assigned tile number.
New features version 0.5:
	* Draw selection area.
	* Draw cursor in screen definition.
New features version 0.6:
	* Shift pixels in tile definition.
	* Flip horizontal and vertical in tile definition.
	* Export screen and tiles in binary data compressed with Pletter v0.5b ( Pletter is a PC based compressor with an MSX based decompressor, created by XL2S Entertainment [http://home.wanadoo.nl/smastijn/pletter.html] )
	* Import and Export Polka! v1.2 projects. ( Polka! is a GFX editor for SCREEN 2 and SCREEN 4, running in Windows, created by Wolf_ [ http://es.msx.org/downcat43p1.html ] )
	* Added Load from library option, that allows you to load a section of a nMSXtiles tiles file into current banks tiles.
Version 0.6.6
  * (Bug solved){Export ASM data hexadecimal menu}: Deactivate if there is no project.
  * When copying a tile, it's possible to fill a selected area of the screen with this tile.
  * Menu entry to fill Screen with tiles from 1 to 255.
  * When importing a PNG file with tiles it doesn't fill screen tiles from 1 to 255 automatically.
  * {Tiles blocks of} menu entry allow to copy to screen the X*Y tiles contiguous to the selected tile on the bank.
  * Export bank tiles as PNG file.
  * Import PNG file created with nMSXtiles to bank.
Version 0.6.7
  * Can export part or the screen.
  * Export Screen as PNG file.
  * (Bug solved) {Tiles Tools->Group Tiles} off if no active project.
  * (Bug solved) Query for save project before exit application.

Version 0.7.1
  * Solved a problem where importing any Polka files.
	
Sprites
-------
Sprites editor allows up to four sets each with four sprites planes. Combos with labels "Plane 0..3" select colors for
each sprite plane from selected set (lower combo). Left mouse button paints a dot on current position. Right mouse
button deletes the current dot from four sprites planes.

You can export data to a text file with easy notation.

Compilation
-----------
Source code is available and has been compiled and tested on Windows, Linux and Mac.
I have used Qt4 libraries from Trolltech. To compile on Windows I have used MinGW.

For any doubt mail me to rdelasheras @ gmail.com


-------------------
Ramón de las Heras Gómiz
rdelasheras @ gmail.com
www.pentacour.com
