PUP Game Resource Unpacker and Packer
===============================================

Background
----------

In the early 2000s, I loved tinkering with games and extracting various resources from them—graphics, sounds, models. My main and only tools back then were the hex editors pview and hiew, and Pascal and C/C++ compilers. I did quite a bit of digging, though back then I was only interested in extracting resources from games, not in creating editing tools.

Some file formats had only been analyzed superficially—at a level sufficient for data extraction, but nothing more. Some were analyzed well enough that I was even able to implement not only data extraction but also reverse-compression.

After some time, I became mired in the depths of OOP, so much time was spent designing the program's object structure that I couldn't finish a single program. This was the beginning of a "creative depression," which led to me abandoning programming for a long time.

History
------

In September 2010, I decided to revisit my old ways and write a program for unpacking and packaging game resource files. To avoid repeating past mistakes, I decided to write it in pure C, without using OOP. I designed a plugin system and decided to implement support only for those formats that were well suited to it.

By February 2011, I had almost completed the program, planning to add support for another format (IWAD/PWAD). This format encountered difficulties, and work stalled. Although I periodically thought about the program and tried to complete support for it, I eventually found something more interesting to do and forgot about it.

I recently remembered it and thought, "Why waste it? Maybe someone will find it useful?" I tried compiling it, fixed a couple of errors that were preventing the build, removed the beginnings of the failed plugin, and decided to publish it all.

Since I was already using Debian by 2010, the program was also built on it. It's built using a shell script and requires the gcc, zlib1g, and zlib1g-dev packages. Building on other systems hasn't been tested.

Program
--------

The program is called PUP - Packer/UnPacker (the word "pup" can also be translated from English as "puppy"). The program has several operating modes:

1. If the program is run without options, help on available options is displayed.
2. If the --list option is specified, the program displays descriptions of all available plugins.
3. When the --unpack option is specified, the program will attempt to detect the format of the specified file and unpack its contents into the specified directory, saving the file's metadata to the specified file. If a metadata file name is not specified, the directory name with the ".txt" extension appended is used. If no directory name is specified, the file name without the extension is used. To prevent the program from attempting to determine the file format itself, you can specify the --plugin option with a specific format.
4. When the --pack option is specified, the program will pack data into the specified file from the specified directory using the metadata from the specified file. If a metadata file name is not specified, the directory name with the ".txt" extension appended is used. If no directory name is specified, the file name without the extension is used. When packing, be sure to specify the --plugin option specifying the format of the file being created.
5. When the --savemeta option is specified, the program attempts to detect the format of the specified file and save the metadata to the specified file. If the plugin does not contain metadata, the program will notify you and exit. If the metadata file name is not specified, the original file name is used with the extension replaced with ".txt." To prevent the program from attempting to determine the file format automatically, you can specify the --plugin option with a specific format.
6. When specifying the --print option, the program prints technical information from the specified file's resource directory to standard output (resource offset, its compressed and original size, resource name, etc.). To prevent the program from attempting to determine the file format automatically, you can specify the --plugin option with a specific format.

**Program Help:**
```sh
    $ ./pup
    Usage: pup --list
           pup --plugin <plugin> --pack <file> [<dir> [<meta>]]
           pup [--plugin <plugin>] --unpack <file> [<dir> [<meta>]]
           pup [--plugin <plugin>] --savemeta <file> [<meta>]
           pup [--plugin <plugin>] --print <file>
    Options:
           --plugin <plugin>   - specify certain plugin
    Modes:
           --list     - list of all supported plugins
           --pack <file> [<dir> [<meta>]]  - packing dir to specified file
           --unpack <file> [<dir> [<meta>]]  - unpacking specified file to dir
           --savemeta <file> [<meta>]  - only save metadata to specified metafile
           --print <file>    - print technical information to stdout
```
**List of supported formats:**

```sh
    $ ./pup --list
    Supported plugins:
    grp       - GRP-files of Duke Nukem 3D, Witchaven, Redneck Rampage, Shadow Warrior
    gob       - GOB-files of Star Wars: Dark Forces
    pak       - PAK-files of Dune II: The Building of a Dynasty
    viv       - VIV-files of the Need For the Speed 3: Hot Pursuit
    vpp       - VPP-files of Red Faction, The Punisher, Summoner
    pack      - PAK-files of Quake, Quake II, Half-Life, Heretic 2, MDK 2
    pack2     - PAK-files of Daikatana
    wad2      - WAD-file of Quake
    wad3      - WAD-files of Half-Life
    resource1 - RES-file of Armored Fist 2
    resource2 - RES-file of Comanche 3
    dpk4      - DPK-file of Starmageddon 2
    dat       - DAT-files of Fallout
    dat2      - DAT-files of Fallout 2
    rff20     - RFF-files of Blood, version 2.0
    rff30     - RFF-files of Blood, version 3.0
    rff31     - RFF-files of Blood, version 3.1
    labn      - LABN-files of Outlaws
```

The program does an excellent job of determining the type ...of the source file, so specifying the plugin to be used is entirely optional. You only need to specify the plugin during the packing process so that the program generates a file in the desired format.

Metadata is used only in the RFF format. Metadata consists of additional information fields that accompany each packed file. Since these fields cannot be reconstructed from the file's actual contents, they must be saved to a text file when unpacking an RFF file and loaded from a text file when packing one.

There are many different packers and unpackers for game files. What makes my program unique is its support for *Daikatana* PAK files, *Comanche 3* RES files, and *Blood* RFF files.

I analyzed the *Daikatana* PAK file format and its compression algorithm myself. I wrote the decompressor myself—which was quite easy—as well as the compressor, which proved to be significantly more difficult. I am not an expert on compression algorithms, so the compressor runs rather slowly; however, it achieves better compression than the developers' original compressor. The unpacking and packing functions were implemented on September 15, 2002, and a more efficient version of the packer followed on October 20, 2002.

I also analyzed the *Comanche 3* RES file format myself. The challenge in analyzing this format was that the resource names were encrypted, making it difficult to simply spot the filenames by eye.

At the time, I had a custom-built program that allowed me to locate files of specific formats within an arbitrary file based on their signatures. For instance, WAV and AVI files were identified by the RIFF signature (along with additional checks), PCX files by their characteristic signature, and so on. Using this program, I located the starting points of several files within the *Comanche 3* RES file. Next to these points, I could see their sizes and certain offsets. There were also fields containing data with unclear purposes. I wrote a program that listed all the resources and unpacked them into files named based on their resource index number.

I noticed that the final bytes of those obscure entries were often identical, leading me to hypothesize that this resulted from an XOR operation with a zero-value byte. Reasoning that a file extension usually appears at the end of a filename, I located resources with known formats. This allowed me to recover a four-byte encryption key that was applied three times to encrypt the 12-byte filename. My initial program, written on February 9, 2003, was capable only of unpacking resources.

Dealing with *Blood*'s.rff files proved to be both simpler and more complex. Although I found information about the file encryption algorithm online, it didn't apply to every .rff file I encountered. I located several versions of the BARF utility—designed for creating new *Blood* files—and used them to generate new archives. This revealed three distinct .rff file formats, for which I subsequently implemented unpacking and packing routines. However, the game refused to run using the original files after they had been unpacked and repacked. I eventually implemented a system to save and load .rff file metadata during the unpacking and packing processes, respectively. This approach worked, and *Blood* successfully ran using the rebuilt files.

An interesting story unfolded regarding the *Armored Fist 2* .res files. I received an email from Poland asking me to send the compiled program. The sender analyzed the code and requested support for *Armored Fist 2* game files. I found an archive of the game online; over the course of Friday evening, June 10, 2022, I deciphered the .res file format and implemented support for it—fortunately, it turned out to be very similar to the *Comanche 3* .res format.

The other formats were easier to handle. I analyzed some myself and read about others online.

The archive containing the program's source code can be downloaded here. It also includes a text file named `ideas.txt`, where I jotted down ideas and tracked the program's development stages.

(C) 2010–2013 Vladimir Stupin

The program is distributed under the GPL 3 license.
