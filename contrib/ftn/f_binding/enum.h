/***********************************************************
Copyright 1989, 1990, 1991, by Fujitsu Limited.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Fujitsu not be used
in advertising or publicity pertaining to distribution of the software
without specific, written prior permission.  

FUJITSU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJITSU BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
******************************************************************/
/* This include file is based on ISO PHIGS FORTRAN Binding */
/* annotation style */
#define	  PUNCON	1
#define   PLDLN		2
/*  archive state */
#define   PARCL		0
#define	  PAROP		1
/* attribute identifier */
#define   PLN           0
#define   PLWSC         1
#define   PPLCI         2
#define   PMK           3
#define   PMKSC         4
#define   PPMCI         5
#define   PTXFN         6
#define   PTXPR         7
#define   PCHXP         8
#define   PCHSP         9
#define   PTXCI        10
#define   PIS          11
#define   PISI         12
#define   PICI         13
#define   PEDFG        14
#define   PEDT         15
#define   PEWSC        16
#define   PEDCI        17
#define   PPSHM        18
#define   PISHM        19
#define   PIRPR        20
#define   PIREQ        21
#define   PBIS         22
#define   PBISI        23
#define   PBIC         24
#define   PBISHM       25
#define   PBIRPR       26
#define   PBIREQ       27
#define   PCAPCR       28
#define   PSAPCR       29
/* aspect source */
#define   PBUNDL        0
#define   PINDIV        1
/* clipping indicator */
#define   PNCLIP        0
#define   PCLIP         1
/* colour available */
#define   PMONOC        0
#define   PCOLOR        1
/* colour model */
#define   PINDCT        0
#define   PRGB          1
#define   PCIE          2
#define   PHSV          3
#define   PHLS          4
/* composition type */
#define   PCPRE         0
#define   PCPOST        1
#define   PCREPL        2
/* conflict resolution */
#define   PCRMNT        0
#define   PCRABA        1
#define   PCRUPD        2
/* control flag */
#define   PCONDI        0
#define   PALWAY        1
/* deferral mode */
#define   PASAP         0
#define   PBNIG         1
#define   PBNIL         2
#define   PASTI         3
#define   PWAITD        4
/* device coordinate units */
#define   PMETRE        0
#define   POTHU         1
/* display surface empty */
#define   PNEMPT        0
#define   PEMPTY        1
/* dynamic modification */
#define   PIRG          0
#define   PIMM          1
#define   PCBS          2
/* echo switch */
#define   PNECHO        0
#define   PECHO         1
/* edit mode */
#define   PINSRT        0
#define   PREPLC        1
/* element type */
#define    PEALL     0
#define    PENIL     1
#define    PEPL3     2
#define    PEPL      3
#define    PEPM3     4
#define    PEPM      5
#define    PETX3     6
#define    PETX      7
#define    PEATR3    8
#define    PEATR     9
#define    PEFA3     10
#define    PEFA      11
#define    PEFAS3    12
#define    PEFAS     13
#define    PECA3     14
#define    PECA      15
#define    PEGDP3    16
#define    PEGDP     17
#define    PEPLI     18
#define    PEPMI     19
#define    PETXI     20
#define    PEII      21
#define    PEEDI     22
#define    PELN      23
#define    PELWSC    24
#define    PEPLCI    25
#define    PEMK      26
#define    PEMKSC    27
#define    PEPMCI    28
#define    PETXFN    29
#define    PETXPR    30
#define    PECHXP    31
#define    PECHSP    32
#define    PETXCI    33
#define    PECHH     34
#define    PECHUP    35
#define    PETXP     36
#define    PETXAL    37
#define    PEATCH    38
#define    PEATCU    39
#define    PEATP     40
#define    PEATAL    41
#define    PEANST    42
#define    PEIS      43
#define    PEISI     44
#define    PEICI     45
#define    PEEDFG    46
#define    PEEDT     47
#define    PEEWSC    48
#define    PEEDCI    49
#define    PEPA      50
#define    PEPRPV    51
#define    PEPARF    52
#define    PEADS     53
#define    PERES     54
#define    PEIASF    55
#define    PEHRID    56
#define    PELMT3    57
#define    PELMT     58
#define    PEGMT3    59
#define    PEGMT     60
#define    PEMCV3    61
#define    PEMCV     62
#define    PEMCLI    63
#define    PERMCV    64
#define    PEVWI     65
#define    PEEXST    66
#define    PELB      67
#define    PEAP      68
#define    PEGSE     69
#define    PEPKID    70
/* element type PHIGS+ */
#define    PEPLS3    71
#define    PEFSD3    72
#define    PETRS3    73
#define    PEQMD3    74
#define    PESFS3    75
#define    PENBSC    76
#define    PENBSS    77
#define    PECAP3    78
#define    PETXCL    79
#define    PEPMCL    80
#define    PEEDCL    81
#define    PEPLCL    82
#define    PECAPC    83
#define    PEPLSM    84
#define    PEICL     85
#define    PEBICL    86
#define    PEBISY    87
#define    PEBISI    88
#define    PERFP     89
#define    PEBRFP    90
#define    PEISM     91
#define    PEBISM    92
#define    PEIRFE    93
#define    PEBIRE    94
#define    PESAPC    95
#define    PEPSCH    96
#define    PEFDGM    97
#define    PEFCUM    98
#define    PELSST    99
#define    PEDPCI    100
#define    PECMI     101
#define    PERCLM    102
/* GDP attributes */
#define   PPLATT        0
#define   PPMATT        1
#define   PTXATT        2
#define   PINATT        3
#define   PEDATT        4
/* input class */
#define   PNCLAS        0
#define   PLOCAT        1
#define   PSTROK        2
#define   PVALUA        3
#define   PCHOIC        4
#define   PPICK         5
#define   PSTRIN        6
/* input device status */
#define   PNONE         0
#define   POK           1
#define   PNPICK        2
#define   PNCHOI        2
/* interior style */
#define   PHOLLO        0
#define   PSOLID        1
#define   PPATTR        2
#define   PHATCH        3
#define   PISEMP        4
/* linetype       */
#define   PLSOLI        1
#define   PLDASH        2
#define   PLDOT         3
#define   PLDASD        4
/* makertype      */
#define   PPOINT        1
#define   PPLUS         2
#define   PAST          3
#define   POMARK        4
#define   PXMARK        5
/* modellin clipping operater */
#define   PMCREP        1
#define   PMCINT        2
/* modification mode */
#define   PNIVE         0
#define   PUWOR         1
#define   PUQUM         2
/* more simultaneous events */
#define   PNMORE        0
#define   PMORE         1
/* off/on switch for edge flag and error handling mode */
#define   POFF          0
#define   PON           1
/* open-structure status */
#define   PNONST        0
#define   POPNST        1
/* operating mode */
#define   PREQU         0
#define   PSAMPL        1
#define   PEVENT        2
/* path order */
#define   PPOTOP        0
#define   PPOBOT        1
/* polyline/fill area control flag */
#define   PPLINE        0
#define   PFILLA        1
#define   PFILAS        2
/* presence of invalid values */
#define   PABSNT        0
#define   PPRSNT        1
/* reference handling flag */
#define   PDELE         0
#define   PKEEP         1
/* regeneration flag */
#define   PPOSTP        0
#define   PPERFO        1
/* relative input priority */
#define   PHIGHR        0
#define   PLOWER        1
/* search direction  */
#define   PBWD          0
#define   PFWD          1
/* search success indicator */
#define   PFAIL         0
#define   PSUCC         1
/* state of visual representation */
#define   PVROK         0
#define   PVRDFR        1
#define   PVRSIM        2
/* structure network source */
#define   PCSS          0
#define   PARCHV        1
/* structure state value */
#define   PSTCL         0
#define   PSTOP         1
/* structure status indicator */
#define   PSNOEX        0
#define   PSEMPT        1
#define   PSNEMP        2
/* system state value */
#define   PPHCL         0
#define   PPHOP         1
/* text alignment horizontal */
#define   PAHNOR        0
#define   PALEFT        1
#define   PACENT        2
#define   PARITE        3
/* text alignment vartical */
#define   PAVNOR        0
#define   PATOP         1
#define   PACAP         2
#define   PAHALF        3
#define   PABASE        4
#define   PABOTT        5
/* text path */
#define   PRIGHT        0
#define   PLEFT         1
#define   PUP           2
#define   PDOWN         3
/* text precision */
#define   PSTRP         0
#define   PCHARP        1
#define   PSTRKP        2
/* type of returned values */
#define   PSET          0
#define   PREALI        1
/* update state */
#define   PNPEND        0
#define   PPEND         1
/* vector/raster/other type */
#define   PVECTR        0
#define   PRASTR        1
#define   POTHWK        2
/* viewtype */
#define   PPARL         0
#define   PPERS         1
/* workstation category */
#define   POUTPT        0
#define   PINPUT        1
#define   POUTIN        2
#define   PMO           3
#define   PMI           4
/* workstation dependence indicator */
#define   PWKI          0
#define   PWKD          1
/* workstation state value */
#define   PWSCL         0
#define   PWSOP         1
/* current and requested values */
#define   PCURVL        0
#define   PRQSVL        1
/* error handling */
#define   EOPPH         0
#define   ECLPH         1
#define   EOPWK         2
#define   ECLWK         3
#define   ERST          4
#define   EUWK          5
#define   ESDUS         6
#define   EMSG          7
#define   EPL3          8
#define   EPL           9
#define   EPM3          10
#define   EPM           11
#define   ETX3          12
#define   ETX           13
#define   EATR3         14
#define   EATR          15
#define   EFA3          16
#define   EFA           17
#define   EFAS3         18
#define   EFAS          19
#define   ECA3          20
#define   ECA           21
#define   EGDP3         22
#define   EGDP          23
#define   ESPLI         24
#define   ESPMI         25
#define   ESTXI         26
#define   ESII          27
#define   ESEDI         28
#define   ESLN          29
#define   ESLWSC        30
#define   ESPLCI        31
#define   ESMK          32
#define   ESMKSC        33
#define   ESPMCI        34
#define   ESTXFN        35
#define   ESTXPR        36
#define   ESCHXP        37
#define   ESCHSP        38
#define   ESTXCI        39
#define   ESCHH         40
#define   ESCHUP        41
#define   ESTXP         42
#define   ESTXAL        43
#define   ESATCH        44
#define   ESATCU        45
#define   ESATP         46
#define   ESATAL        47
#define   ESANS         48
#define   ESIS          49
#define   ESISI         50
#define   ESICI         51
#define   ESEDFG        52
#define   ESEDT         53
#define   ESEWSC        54
#define   ESEDCI        55
#define   ESPA          56
#define   ESPRPV        57
#define   ESPARF        58
#define   EADS          59
#define   ERES          60
#define   ESIASF        61
#define   ESPLR         62
#define   ESPMR         63
#define   ESTXR         64
#define   ESIR          65
#define   ESEDR         66
#define   ESPAR         67
#define   ESCR          68
#define   ESHLFT        69
#define   ESIVFT        70
#define   ESCMD         71
#define   ESHRID        72
#define   ESHRM         73
#define   ESLMT3        74
#define   ESLMT         75
#define   ESGMT3        76
#define   ESGMT         77
#define   ESMCV3        78
#define   ESMCV         79
#define   ESMCLI        80
#define   ERMCV         81
#define   ESVWI         82
#define   ESVWR3        83
#define   ESVWR         84
#define   ESVTIP        85
#define   ESWKW3        86
#define   ESWKW         87
#define   ESWKV3        88
#define   ESWKV         89
#define   EOPST         90
#define   ECLST         91
#define   EEXST         92
#define   ELB           93
#define   EAP           94
#define   EGSE          95
#define   ESEDM         96
#define   ECELST        97
#define   ESEP          98
#define   EOSEP         99
#define   ESEPLB        100
#define   EDEL          101
#define   EDELRA        102
#define   EDELLB        103
#define   EEMST         104
#define   EDST          105
#define   EDSN          106
#define   EDSA          107
#define   ECSTID        108
#define   ECSTRF        109
#define   ECSTIR        110
#define   EPOST         111
#define   EUPOST        112
#define   EUPAST        113
#define   EOPARF        114
#define   ECLARF        115
#define   EARST         116
#define   EARSN         117
#define   EARAST        118
#define   ESCNRS        119
#define   ERSID         120
#define   EREST         121
#define   ERESN         122
#define   ERAST         123
#define   EREPAN        124
#define   EREPED        125
#define   EDSTAR        126
#define   EDSNAR        127
#define   EDASAR        128
#define   ESPKID        129
#define   ESPKFT        130
#define   EINLC3        131
#define   EINLC         132
#define   EINSK3        133
#define   EINSK         134
#define   EINVL3        135
#define   EINVL         136
#define   EINCH3        137
#define   EINCH         138
#define   EINPK3        139
#define   EINPK         140
#define   EINST3        141
#define   EINST         142
#define   ESLCM         143
#define   ESSKM         144
#define   ESVLM         145
#define   ESCHM         146
#define   ESPKM         147
#define   ESSTM         148
#define   ERQLC3        149
#define   ERQLC         150
#define   ERQSK3        151
#define   ERQSK         152
#define   ERQVL         153
#define   ERQCH         154
#define   ERQPK         155
#define   ERQST         156
#define   ESMLC3        157
#define   ESMLC         158
#define   ESMSK3        159
#define   ESMSK         160
#define   ESMVL         161
#define   ESMCH         162
#define   ESMPK         163
#define   ESMST         164
#define   EWAIT         165
#define   EFLUSH        166
#define   EGTLC3        167
#define   EGTLC         168
#define   EGTSK3        169
#define   EGTSK         170
#define   EGTVL         171
#define   EGTCH         172
#define   EGTPK         173
#define   EGTST         174
#define   EWITM         175
#define   EGTITM        176
#define   ERDITM        177
#define   EIITM         178
#define   ESERHM        179
#define   EESC          180 
#define   EPREC         181
#define   EUREC         182
/* error handling PHIGS+ */
#define   EPLSD3        301
#define   EFASD3        302
#define   ECAP3         303
#define   ESOFA3        304
#define   ETSD3         305
#define   EQMD3         306
#define   ENUBSC        307
#define   ENUBSS        308
#define   ESBII         309
#define   ESPLC         310
#define   ESPLSM        311
#define   ESPMC         312
#define   ESTXC         313
#define   ESFDM         314
#define   ESFCM         315
#define   ESIC          316
#define   ESISM         317
#define   ESRFP         318
#define   ESRFE         319
#define   ESBIS         320 
#define   ESBISI        321
#define   ESBIC         322
#define   ESBISM        323
#define   ESBRFP        324
#define   ESBRFE        325
#define   ESLSS         326
#define   ESEDC         327
#define   ESCAC         328
#define   ESSAC         329
#define   ESPCH         330
#define   ESRCM         331
#define   ESDCI         332
#define   ESCMI         333
#define   ESPLRP        334
#define   ESPMRP        335
#define   ESTXRP        336
#define   ESIRP         337
#define   ESEDRP        338
#define   ESPARP        339
#define   ESLSR         340
#define   ESDCR         341
#define   ESCMR         342
/* error handling PEX */
#define   EWTCRE        -1
#define   EWTSET        -2
#define   EWTGET        -3
#define   EWTDES        -4
#define   EOPPEX        -5
/* culling mode */
#define   PNCUL         0
#define   PBFAC         1
#define   PFFAC         2
/* disting mode */
#define   PDSNO         0
#define   PDSYES        1
/* depth cue mode */
#define   PSUPPR        0
#define   PALLOW        1
/* facet flag */
#define   PNOF          0
#define   PFCOLR        1
#define   PFNORM        2
#define   PFCONO        3
/* rationality */
#define   PRATIO        0
#define   PNONRA        1
/* vertex flag */
#define   PCOORD        0
#define   PCCOLR        1
#define   PCNORM        2
#define   PCCONO        3
/* edge flag */
#define   PNOE          0
#define   PEVIS         1
/* HLHSR identifier */
#define   PHIOFF        0
#define   PHION         1
/* HLHSR mode */
#define   PHMNON        0
#define   PHMZBF        1
/* ESCAPE error syncronization mode */
#define   PESOFF        0
#define   PESON         1
/* ESCAPE local input transformation type */
#define   PLCMOD        0
#define   PLCVIW        1
/* ESCAPE local input transformation matrix create type */
#define   PLCACC        0
#define   PLCGEN        1
/* ESCAPE local input conflation type */
#define   PLCABU        0
#define   PLCPRC        1
#define   PLCPRU        2
/* ESCAPE local input local input rotate axis */
#define   PLCFIR        0
#define   PLCSEC        1
#define   PLCTHI        2
/* ESCAPE view transformation effect mode */
#define   PNPC          0
#define   PVPC          1
/* ESCAPE input value reference mode */
#define   PINVAL        0
#define   PVAL          1
/* GDP arc close type */
#define   PACFAN        0
#define   PACCHD        1
/* GSE side point attribute */
#define   PSPCIR        0
#define   PSPSQU        1
#define   PSPFLA        2
/* PHIGS moniter ON/OFF flag */
#define   PMON          0
#define   PNOMON        1
/* clients side CSS flag */
#define   PSERVR        0
#define   PCLIET        1
/* buffer mode */
#define   PSINGL        0
#define   PDOUBL        1
