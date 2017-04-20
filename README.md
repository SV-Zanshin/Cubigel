# Cubigel library
*Arduino* library for communicating with any Cubigel compressor family which uses the FDC1 communication protocol. The library allows reading the programmed compressor settings as well as the data sentences that are sent twice a second from the compressor.
Currently the library is limited to 2 devices, but it can easily be modified to support more; this limit was chosen to minimize memory usage and simplify implementation; particularly as most installations will have at most 2 compressors - one for the refrigerator and one for the freezer.
The library collects data in the background (piggybacking off the TIMER0_COMPA interrupt) and does not require manual polling. The data sentences containing RPM and amperage values are averaged automatically so that the correct value since the last reading is always returned

## Communication Protocol
The manufacturer has published several documents regarding communicating with the FDC1 controller on their website. The main FDC1 document is www.huayicompressor.es/phocadownload/user-manuals/user_manual_gd30fdc.pdf and the definition of the communication protocol can be found at https://www.sv-zanshin.com/r/manuals/cubigel_fdc1_communication_protocol.pdf

Details are available at the [GitHub DSFamily Wiki](https://github.com/SV-Zanshin/Cubigel/wiki)


![Zanshin Logo](https://www.sv-zanshin.com/images/gif/zanshinkanjitiny.gif) <img src="https://www.sv-zanshin.com/images/gif/zanshintext.gif" width="75"/>
