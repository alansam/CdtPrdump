# CdtPrdump

## A Simple Implementation of the IBM z/OS AMDPRDMP PRINTDUMP Service Aid Program.

Produces a formatted hexadedimal and text representation of an input dataset as shown in the following example:

#### Sample Output from CdtPrdump

> ```
>
> 00000000  00010203 04050607 08090a0b 0c0d0e0f  ................
> 00000010  10111213 14151617 18191a1b 1c1d1e1f  ................
> 00000020  20212223 24252627 28292a2b 2c2d2e2f   !"#$%&'()*+,-./
> 00000030  30313233 34353637 38393a3b 3c3d3e3f  0123456789:;<=>?
> 00000040  40414243 44454647 48494a4b 4c4d4e4f  @ABCDEFGHIJKLMNO
> 00000050  50515253 54555657 58595a5b 5c5d5e5f  PQRSTUVWXYZ[\]^_
> 00000060  60616263 64656667 68696a6b 6c6d6e6f  `abcdefghijklmno
> 00000070  70717273 74757677 78797a7b 7c7d7e7f  pqrstuvwxyz{|}~.
> 00000080  80818283 84858687 88898a8b 8c8d8e8f  ................
> 00000090  90919293 94959697 98999a9b 9c9d9e9f  ................
> 000000A0  a0a1a2a3 a4a5a6a7 a8a9aaab acadaeaf  ................
> 000000B0  b0b1b2b3 b4b5b6b7 b8b9babb bcbdbebf  ................
> 000000C0  c0c1c2c3 c4c5c6c7 c8c9cacb cccdcecf  ................
> 000000D0  d0d1d2d3 d4d5d6d7 d8d9dadb dcdddedf  ................
> 000000E0  e0e1e2e3 e4e5e6e7 e8e9eaeb ecedeeef  ................
> 000000F0  f0f1f2f3 f4f5f6f7 f8f9fafb fcfdfeff  ................
>
> ```

### Program control.

**Usage:**  `prdump <switches> inFile outFile errorLog`

where:

> **inFile**
> : is the file to read.  (default stdin.)
>
> **outFile**
> : is where the results are written.  (default stdout.)
>
> **errorLog**
> : is where the error messages are written.  (default stderr.)
>

switches:
> **`-h`**
> :  Help.  This message.
>
> **`-?`**
> : Ditto.
>
> **`-n`**
> : Print the report in narrow format.
>
> **`-w`**
> : Print the report in wide format.  (default.)
>  
