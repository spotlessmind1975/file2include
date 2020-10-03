# file2include
Convert one or more binary file in an include/source file

## USAGE

<pre>file2include.exe [options]</pre>

By omitting the options, you get online help.

## OPTIONS (mandatory)

`-i <filename>` input filename of binary file to include

With this option you can specify the input file to be processed. If you want you can specify more than once this option to include more than one file.

`-h <filename>` output filename of incldue file

With this option you can indicate the name of the file where the C include file will be written to. The following symbols will be included in the include file:
 - `FILE_filename` with the index of the file given in input;
 - `_includedFileNNN` with the declaration of the reference to the symbol defined in the source file.

`-c <filename>` output filename of source file

With this option you can indicate the name of the file where the C source data file will be written to.

## OPTIONS

`-v`            make execution verbose

Activates the display of all essential information.