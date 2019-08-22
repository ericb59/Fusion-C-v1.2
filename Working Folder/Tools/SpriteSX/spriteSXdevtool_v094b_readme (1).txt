spriteSX devtool v0.9.4b (August 2014)
Copyleft 303bcn 2014
This program is distributed under the terms of the GNU General Public License

WEB: https://code.google.com/p/spritesx-ed/
mail: aorante@gmail.com



--------------------------------------------------------------------------------
Index
                             
1. Description
2. License
3. Requirements
4. How to install
5. Features
6. History
7. Acknowledgements
8. Components
9. Code Examples



--------------------------------------------------------------------------------
1. Description

   Tool to create a collection of sprites, for TMS9918 & V9938 video processors 
   (MSX, colecovision, etc...), which provides the source code for Assembler, C 
   and Basic.

   The author's purpose is to provide to the community of developers, a utility 
   to simplify the work of creating and editing sprites, and the possibility to 
   enhance and add functionality, using the code provided on project website.
   
   This application is designed for agile and intuitive handling, but always can 
   be improved. We are working on it. We are waiting your suggestions. 

   This software was developed in Microsoft Visual Basic 2008 Express. 
   To obtain the source code, go to project website.
   


--------------------------------------------------------------------------------
2. License

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.


    
--------------------------------------------------------------------------------
3. Requirements 
 
   PC with Microsoft Windows OS and Framework 3.0



--------------------------------------------------------------------------------
4. How to install 
 
   Unpack and Copy spriteSXED folder somewhere on your hard disk. 
   
   For run, execute spriteSXED.exe 



--------------------------------------------------------------------------------
5. Features

  * Assistant to initialize a new project.
  * Projects with different types of sprites. (8x8, 16x16, mono and multicolor)
  * Load, Merge and Save projects, including the color palette.
  * New project format with back compatibility.
  * Delete, move, rotate and flip sprite.
  * Edit line color and the OR bit in multicolor mode (V9938).
  * Control the project sprites list (order, copy and delete).
  * Up to 255 sprites of 8x8 size and 64 sprites of 16x16 size, per project.
  * Edit the color palette (V9938). Allows reading or save the palette 
    independently.
  * Code Generation Window. Provides assembly code, formatted SDCC assembler, C 
    and Basic.
  * Load or Save Bitmap Window. Load/Save a bitmap PNG or a MSX Basic binary 
    SC2.
  * Project info window. Editing project information: name, version, author, 
    group and description.
  * Paint functions: Draw, Lines, Rectangles, Fill rectangles, Circles, 
    Fill circles and Fill. Positive or negative (right mouse button).
  * Undo and redo. 16 steps.
  * Shortcuts keys. 



--------------------------------------------------------------------------------
6. History

  v0.9.4b (August 2014)
  * Paint functions: Draw, Lines, Rectangles, Fill rectangles, Circles, 
    Fill circles and Fill. Positive or negative (right mouse button).
  * Undo and redo. 16 steps.
  * Shortcuts keys.
  * (GUIcontrols.ProjectPropertiesWin) Show project file name.   
  * Bug corrected. (MSXLibrary.palette512Dialog) Palette edit window. Tools to
    copy and exchange colors don't work.
  * Bug corrected. (MSXLibrary.ColorSelector). Don't show in taskbar.
  * Add Assembler examples.
  * Update SP16C_OR.BAS with improvement in bounce function.    

  
  
  v0.9.3b (july 2014)
  * Load or Save Bitmap window.
  * Progress window.
  * Fixed Bug in redrawing the lines 8x8 in the editor matrix. 
  * Removed License.dll Functionality included in the executable.
  * Project info window.
  * Improvements in the graphic design of some components.
  * Add Basic examples for SC2 binary files test.    



  v0.9b (march 2014)
  First version.
  
           

--------------------------------------------------------------------------------
7. Acknowledgements

  * JamQue/TPM for the idea
    http://www.thepetsmode.com/
      
  * JamQue, MsxKun, pentacour, kabish & zilogZ80a for testing.
   
  * Fubu by the algorithm to the fill tool.  
   
  * Dedicated to clubSPRITE
    http://www.clubsprite.net/



--------------------------------------------------------------------------------
8. Components

   spriteSXdevtool.exe       v0.9.4.7
   WizardSpriteProject.dll   v1
   GUIcontrols.dll           v1.1.2
   MSXLibrary.dll            v1.2.5

   Projects_sprite\          Projects.
   Palettes\                 Palettes.
   code_examples\            Example codes.
   SC2\                      MSX Basic VRAM binary files.
   Bitmaps\                  PNG files.



--------------------------------------------------------------------------------
9. Code Examples

   The package includes a few examples of sources in C (SDCC), Assembler (asMSX)
   and MSX BASIC, to test the data output and learning.
   

   File List: (code_examples\)
   
   *Assembler (ASM\)
    To assemble asMSX need. https://code.google.com/p/asmsx-license-gpl/
    sprite8_test\        <-- 8x8   monochrome MSX ROM 
    sprite16_test\       <-- 16x16 monochrome MSX ROM
    sprite8C_test\       <-- 8x8   multicolor MSX2 ROM 
    sprite16C_test\      <-- 16x16 multicolor MSX2 ROM
    
   *C (C\)
    Includes a Readme with notes for the compilation.      
    sprite8_test\        <-- 8x8   monochrome 16k MSX ROM 
    sprite16_test\       <-- 16x16 monochrome 16k MSX ROM
    sprite8C_test\       <-- 8x8   multicolor 16k MSX2 ROM
    sprite16C_test\      <-- 16x16 multicolor 16k MSX2 ROM
   
   *MSX BASIC (MSX_BASIC\)
    SP8.BAS              <-- 8x8   monochrome 
    SP16.BAS             <-- 16x16 monochrome
    SP8C.BAS             <-- 8x8   multicolor
    SP16C.BAS            <-- 16x16 multicolor
    SP16C_OR.BAS         <-- 16x16 multicolor OR (mixing of 2 sprites)
    
    LOADSPR.BAS          <-- test Sprites MSX Basic binary
    FRUTAS16.SC2         <-- for LOADSPR.BAS
    
    LOADSC2.BAS          <-- test SC2 output 
    LOADSC4.BAS          <-- test SC2 output with palette (V9938 or higher)
    FRUIT16C.SC2         <-- for LOADSC2.BAS and LOADSC4.BAS
    
    msx_spritesBASIC.dsk <-- Disk image with all examples in BASIC.




-------------------------------------------------------------------------------- 