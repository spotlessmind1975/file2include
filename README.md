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

## LICENSE
Copyright 2020-2021 Marco Spedaletti

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.