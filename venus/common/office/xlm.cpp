#include <stack>
#include <map>
#include <algorithm>
#include <assert.h>
#include "xlm.h"
#include "offcrypto_assist.h"
#include "../utils/codepage.h"
#include "./helper.h"
#include "./xlm_data.h"

const char* LblRecord_built_in_names[] = {
    "Consolidate_Area", // 0x00 
    "Auto_Open",    // 0x01 
    "Auto_Close",   // 0x02 
    "Extract",  // 0x03 
    "Database", // 0x04 
    "Criteria", // 0x05 
    "Print_Area",   // 0x06 
    "Print_Titles", // 0x07 
    "Recorder", // 0x08 
    "Data_Form",    // 0x09 
    "Auto_Activate",    // 0x0A 
    "Auto_Deactivate",  // 0x0B 
    "Sheet_Title",  // 0x0C 
    "_FilterDatabase",  // 0x0D 
};

// clang-format off
const char* OPCODE_NAMES[] = {
        NULL,                                                         //0
        NULL,                                                         //1
        NULL,                                                         //2
        NULL,                                                         //3
        NULL,                                                         //4
        NULL,                                                         //5
        "FORMULA : Cell Formula",                                     //6
        NULL,                                                         //7
        NULL,                                                         //8
        NULL,                                                         //9
        "EOF : End of File",                                          //10
        NULL,                                                         //11
        "CALCCOUNT : Iteration Count",                                //12
        "CALCMODE : Calculation Mode",                                //13
        "PRECISION : Precision",                                      //14
        "REFMODE : Reference Mode",                                   //15
        "DELTA : Iteration Increment",                                //16
        "ITERATION : Iteration Mode",                                 //17
        "PROTECT : Protection Flag",                                  //18
        "PASSWORD : Protection Password",                             //19
        "HEADER : Print Header on Each Page",                         //20
        "FOOTER : Print Footer on Each Page",                         //21
        "EXTERNCOUNT : Number of External References",                //22
        "EXTERNSHEET : External Reference",                           //23
        "LABEL : Cell Value, String Constant",                        //24
        "WINDOWPROTECT : Windows Are Protected",                      //25
        "VERTICALPAGEBREAKS : Explicit Column Page Breaks",           //26
        "HORIZONTALPAGEBREAKS : Explicit Row Page Breaks",            //27
        "NOTE : Comment Associated with a Cell",                      //28
        "SELECTION : Current Selection",                              //29
        NULL,                                                         //30
        NULL,                                                         //31
        NULL,                                                         //32
        NULL,                                                         //33
        "1904 : 1904 Date System",                                    //34
        NULL,                                                         //35
        NULL,                                                         //36
        NULL,                                                         //37
        "LEFTMARGIN : Left Margin Measurement",                       //38
        "RIGHTMARGIN : Right Margin Measurement",                     //39
        "TOPMARGIN : Top Margin Measurement",                         //40
        "BOTTOMMARGIN : Bottom Margin Measurement",                   //41
        "PRINTHEADERS : Print Row/Column Labels",                     //42
        "PRINTGRIDLINES : Print Gridlines Flag",                      //43
        NULL,                                                         //44
        NULL,                                                         //45
        NULL,                                                         //46
        "FILEPASS : File Is Password-Protected",                      //47
        NULL,                                                         //48
        NULL,                                                         //49
        NULL,                                                         //50
        NULL,                                                         //51
        NULL,                                                         //52
        NULL,                                                         //53
        NULL,                                                         //54
        NULL,                                                         //55
        NULL,                                                         //56
        NULL,                                                         //57
        NULL,                                                         //58
        NULL,                                                         //59
        "CONTINUE : Continues Long Records",                          //60
        "WINDOW1 : Window Information",                               //61
        NULL,                                                         //62
        NULL,                                                         //63
        "BACKUP : Save Backup Version of the File",                   //64
        "PANE : Number of Panes and Their Position",                  //65
        "CODEPAGE : Default Code Page",                               //66
        NULL,                                                         //67
        NULL,                                                         //68
        NULL,                                                         //69
        NULL,                                                         //70
        NULL,                                                         //71
        NULL,                                                         //72
        NULL,                                                         //73
        NULL,                                                         //74
        NULL,                                                         //75
        NULL,                                                         //76
        "PLS : Environment-Specific Print Record",                    //77
        NULL,                                                         //78
        NULL,                                                         //79
        "DCON : Data Consolidation Information",                      //80
        "DCONREF : Data Consolidation References",                    //81
        "DCONNAME : Data Consolidation Named References",             //82
        NULL,                                                         //83
        NULL,                                                         //84
        "DEFCOLWIDTH : Default Width for Columns",                    //85
        NULL,                                                         //86
        NULL,                                                         //87
        NULL,                                                         //88
        "XCT : CRN Record Count",                                     //89
        "CRN : Nonresident Operands",                                 //90
        "FILESHARING : File-Sharing Information",                     //91
        "WRITEACCESS : Write Access User Name",                       //92
        "OBJ : Describes a Graphic Object",                           //93
        "UNCALCED : Recalculation Status",                            //94
        "SAVERECALC : Recalculate Before Save",                       //95
        "TEMPLATE : Workbook Is a Template",                          //96
        NULL,                                                         //97
        NULL,                                                         //98
        "OBJPROTECT : Objects Are Protected",                         //99
        NULL,                                                         //100
        NULL,                                                         //101
        NULL,                                                         //102
        NULL,                                                         //103
        NULL,                                                         //104
        NULL,                                                         //105
        NULL,                                                         //106
        NULL,                                                         //107
        NULL,                                                         //108
        NULL,                                                         //109
        NULL,                                                         //110
        NULL,                                                         //111
        NULL,                                                         //112
        NULL,                                                         //113
        NULL,                                                         //114
        NULL,                                                         //115
        NULL,                                                         //116
        NULL,                                                         //117
        NULL,                                                         //118
        NULL,                                                         //119
        NULL,                                                         //120
        NULL,                                                         //121
        NULL,                                                         //122
        NULL,                                                         //123
        NULL,                                                         //124
        "COLINFO : Column Formatting Information",                    //125
        "RK : Cell Value, RK Number",                                 //126
        "IMDATA : Image Data",                                        //127
        "GUTS : Size of Row and Column Gutters",                      //128
        "WSBOOL : Additional Workspace Information",                  //129
        "GRIDSET : State Change of Gridlines Option",                 //130
        "HCENTER : Center Between Horizontal Margins",                //131
        "VCENTER : Center Between Vertical Margins",                  //132
        "BOUNDSHEET : Sheet Information",                             //133
        "WRITEPROT : Workbook Is Write-Protected",                    //134
        "ADDIN : Workbook Is an Add-in Macro",                        //135
        "EDG : Edition Globals",                                      //136
        "PUB : Publisher",                                            //137
        NULL,                                                         //138
        NULL,                                                         //139
        "COUNTRY : Default Country and WIN.INI Country",              //140
        "HIDEOBJ : Object Display Options",                           //141
        NULL,                                                         //142
        NULL,                                                         //143
        "SORT : Sorting Options",                                     //144
        "SUB : Subscriber",                                           //145
        "PALETTE : Color Palette Definition",                         //146
        NULL,                                                         //147
        "LHRECORD : .WK? File Conversion Information",                //148
        "LHNGRAPH : Named Graph Information",                         //149
        "SOUND : Sound Note",                                         //150
        NULL,                                                         //151
        "LPR : Sheet Was Printed Using LINE.PRINT(",                  //152
        "STANDARDWIDTH : Standard Column Width",                      //153
        "FNGROUPNAME : Function Group Name",                          //154
        "FILTERMODE : Sheet Contains Filtered List",                  //155
        "FNGROUPCOUNT : Built-in Function Group Count",               //156
        "AUTOFILTERINFO : Drop-Down Arrow Count",                     //157
        "AUTOFILTER : AutoFilter Data",                               //158
        NULL,                                                         //159
        "SCL : Window Zoom Magnification",                            //160
        "SETUP : Page Setup",                                         //161
        NULL,                                                         //162
        NULL,                                                         //163
        NULL,                                                         //164
        NULL,                                                         //165
        NULL,                                                         //166
        NULL,                                                         //167
        NULL,                                                         //168
        "COORDLIST : Polygon Object Vertex Coordinates",              //169
        NULL,                                                         //170
        "GCW : Global Column-Width Flags",                            //171
        NULL,                                                         //172
        NULL,                                                         //173
        "SCENMAN : Scenario Output Data",                             //174
        "SCENARIO : Scenario Data",                                   //175
        "SXVIEW : View Definition",                                   //176
        "SXVD : View Fields",                                         //177
        "SXVI : View Item",                                           //178
        NULL,                                                         //179
        "SXIVD : Row/Column Field IDs",                               //180
        "SXLI : Line Item Array",                                     //181
        "SXPI : Page Item",                                           //182
        NULL,                                                         //183
        "DOCROUTE : Routing Slip Information",                        //184
        "RECIPNAME : Recipient Name",                                 //185
        NULL,                                                         //186
        NULL,                                                         //187
        "SHRFMLA : Shared Formula",                                   //188
        "MULRK : Multiple  RK Cells",                                 //189
        "MULBLANK : Multiple Blank Cells",                            //190
        NULL,                                                         //191
        NULL,                                                         //192
        "MMS :  ADDMENU / DELMENU Record Group Count",                //193
        "ADDMENU : Menu Addition",                                    //194
        "DELMENU : Menu Deletion",                                    //195
        NULL,                                                         //196
        "SXDI : Data Item",                                           //197
        "SXDB : PivotTable Cache Data",                               //198
        NULL,                                                         //199
        NULL,                                                         //200
        NULL,                                                         //201
        NULL,                                                         //202
        NULL,                                                         //203
        NULL,                                                         //204
        "SXSTRING : String",                                          //205
        NULL,                                                         //206
        NULL,                                                         //207
        "SXTBL : Multiple Consolidation Source Info",                 //208
        "SXTBRGIITM : Page Item Name Count",                          //209
        "SXTBPG : Page Item Indexes",                                 //210
        "OBPROJ : Visual Basic Project",                              //211
        NULL,                                                         //212
        "SXIDSTM : Stream ID",                                        //213
        "RSTRING : Cell with Character Formatting",                   //214
        "DBCELL : Stream Offsets",                                    //215
        NULL,                                                         //216
        NULL,                                                         //217
        "BOOKBOOL : Workbook Option Flag",                            //218
        NULL,                                                         //219
        "SXEXT : External Source Information",                        //220
        "SCENPROTECT : Scenario Protection",                          //221
        "OLESIZE : Size of OLE Object",                               //222
        "UDDESC : Description String for Chart Autoformat",           //223
        "XF : Extended Format",                                       //224
        "INTERFACEHDR : Beginning of User Interface Records",         //225
        "INTERFACEEND : End of User Interface Records",               //226
        "SXVS : View Source",                                         //227
        NULL,                                                         //228
        "MERGECELLS : Merged Cells",                                  //229
        NULL,                                                         //230
        NULL,                                                         //231
        NULL,                                                         //232
        NULL,                                                         //233
        "TABIDCONF : Sheet Tab ID of Conflict History",               //234
        "MSODRAWINGGROUP : Microsoft Office Drawing Group",           //235
        "MSODRAWING : Microsoft Office Drawing",                      //236
        "MSODRAWINGSELECTION : Microsoft Office Drawing Selection",   //237
        NULL,                                                         //238
        NULL,                                                         //239
        "SXRULE : PivotTable Rule Data",                              //240
        "SXEX : PivotTable View Extended Information",                //241
        "SXFILT : PivotTable Rule Filter",                            //242
        NULL,                                                         //243
        "SXDXF : Pivot Table Formatting",                             //244
        "SXITM : Pivot Table Item Indexes",                           //245
        "SXNAME : PivotTable Name",                                   //246
        "SXSELECT : PivotTable Selection Information",                //247
        "SXPAIR : PivotTable Name Pair",                              //248
        "SXFMLA : Pivot Table Parsed Expression",                     //249
        NULL,                                                         //250
        "SXFORMAT : PivotTable Format Record",                        //251
        "SST : Shared String Table",                                  //252
        "LABELSST : Cell Value, String Constant/ SST",                //253
        NULL,                                                         //254
        "EXTSST : Extended Shared String Table",                      //255
        "SXVDEX : Extended PivotTable View Fields",                   //256
        NULL,                                                         //257
        NULL,                                                         //258
        "SXFORMULA : PivotTable Formula Record",                      //259
        NULL,                                                         //260
        NULL,                                                         //261
        NULL,                                                         //262
        NULL,                                                         //263
        NULL,                                                         //264
        NULL,                                                         //265
        NULL,                                                         //266
        NULL,                                                         //267
        NULL,                                                         //268
        NULL,                                                         //269
        NULL,                                                         //270
        NULL,                                                         //271
        NULL,                                                         //272
        NULL,                                                         //273
        NULL,                                                         //274
        NULL,                                                         //275
        NULL,                                                         //276
        NULL,                                                         //277
        NULL,                                                         //278
        NULL,                                                         //279
        NULL,                                                         //280
        NULL,                                                         //281
        NULL,                                                         //282
        NULL,                                                         //283
        NULL,                                                         //284
        NULL,                                                         //285
        NULL,                                                         //286
        NULL,                                                         //287
        NULL,                                                         //288
        NULL,                                                         //289
        "SXDBEX : PivotTable Cache Data",                             //290
        NULL,                                                         //291
        NULL,                                                         //292
        NULL,                                                         //293
        NULL,                                                         //294
        NULL,                                                         //295
        NULL,                                                         //296
        NULL,                                                         //297
        NULL,                                                         //298
        NULL,                                                         //299
        NULL,                                                         //300
        NULL,                                                         //301
        NULL,                                                         //302
        NULL,                                                         //303
        NULL,                                                         //304
        NULL,                                                         //305
        NULL,                                                         //306
        NULL,                                                         //307
        NULL,                                                         //308
        NULL,                                                         //309
        NULL,                                                         //310
        NULL,                                                         //311
        NULL,                                                         //312
        NULL,                                                         //313
        NULL,                                                         //314
        NULL,                                                         //315
        NULL,                                                         //316
        "TABID : Sheet Tab Index Array",                              //317
        NULL,                                                         //318
        NULL,                                                         //319
        NULL,                                                         //320
        NULL,                                                         //321
        NULL,                                                         //322
        NULL,                                                         //323
        NULL,                                                         //324
        NULL,                                                         //325
        NULL,                                                         //326
        NULL,                                                         //327
        NULL,                                                         //328
        NULL,                                                         //329
        NULL,                                                         //330
        NULL,                                                         //331
        NULL,                                                         //332
        NULL,                                                         //333
        NULL,                                                         //334
        NULL,                                                         //335
        NULL,                                                         //336
        NULL,                                                         //337
        NULL,                                                         //338
        NULL,                                                         //339
        NULL,                                                         //340
        NULL,                                                         //341
        NULL,                                                         //342
        NULL,                                                         //343
        NULL,                                                         //344
        NULL,                                                         //345
        NULL,                                                         //346
        NULL,                                                         //347
        NULL,                                                         //348
        NULL,                                                         //349
        NULL,                                                         //350
        NULL,                                                         //351
        "USESELFS : Natural Language Formulas Flag",                  //352
        "DSF : Double Stream File",                                   //353
        "XL5MODIFY : Flag for  DSF",                                  //354
        NULL,                                                         //355
        NULL,                                                         //356
        NULL,                                                         //357
        NULL,                                                         //358
        NULL,                                                         //359
        NULL,                                                         //360
        NULL,                                                         //361
        NULL,                                                         //362
        NULL,                                                         //363
        NULL,                                                         //364
        NULL,                                                         //365
        NULL,                                                         //366
        NULL,                                                         //367
        NULL,                                                         //368
        NULL,                                                         //369
        NULL,                                                         //370
        NULL,                                                         //371
        NULL,                                                         //372
        NULL,                                                         //373
        NULL,                                                         //374
        NULL,                                                         //375
        NULL,                                                         //376
        NULL,                                                         //377
        NULL,                                                         //378
        NULL,                                                         //379
        NULL,                                                         //380
        NULL,                                                         //381
        NULL,                                                         //382
        NULL,                                                         //383
        NULL,                                                         //384
        NULL,                                                         //385
        NULL,                                                         //386
        NULL,                                                         //387
        NULL,                                                         //388
        NULL,                                                         //389
        NULL,                                                         //390
        NULL,                                                         //391
        NULL,                                                         //392
        NULL,                                                         //393
        NULL,                                                         //394
        NULL,                                                         //395
        NULL,                                                         //396
        NULL,                                                         //397
        NULL,                                                         //398
        NULL,                                                         //399
        NULL,                                                         //400
        NULL,                                                         //401
        NULL,                                                         //402
        NULL,                                                         //403
        NULL,                                                         //404
        NULL,                                                         //405
        NULL,                                                         //406
        NULL,                                                         //407
        NULL,                                                         //408
        NULL,                                                         //409
        NULL,                                                         //410
        NULL,                                                         //411
        NULL,                                                         //412
        NULL,                                                         //413
        NULL,                                                         //414
        NULL,                                                         //415
        NULL,                                                         //416
        NULL,                                                         //417
        NULL,                                                         //418
        NULL,                                                         //419
        NULL,                                                         //420
        "FILESHARING2 : File-Sharing Information for Shared Lists",   //421
        NULL,                                                         //422
        NULL,                                                         //423
        NULL,                                                         //424
        "USERBVIEW : Workbook Custom View Settings",                  //425
        "USERSVIEWBEGIN : Custom View Settings",                      //426
        "USERSVIEWEND : End of Custom View Records",                  //427
        NULL,                                                         //428
        "QSI : External Data Range",                                  //429
        "SUPBOOK : Supporting Workbook",                              //430
        "PROT4REV : Shared Workbook Protection Flag",                 //431
        "CONDFMT : Conditional Formatting Range Information",         //432
        "CF : Conditional Formatting Conditions",                     //433
        "DVAL : Data Validation Information",                         //434
        NULL,                                                         //435
        NULL,                                                         //436
        "DCONBIN : Data Consolidation Information",                   //437
        "TXO : Text Object",                                          //438
        "REFRESHALL : Refresh Flag",                                  //439
        "HLINK : Hyperlink",                                          //440
        NULL,                                                         //441
        NULL,                                                         //442
        "SXFDBTYPE : SQL Datatype Identifier",                        //443
        "PROT4REVPASS : Shared Workbook Protection Password",         //444
        NULL,                                                         //445
        "DV : Data Validation Criteria",                              //446
        NULL,                                                         //447
        "EXCEL9FILE : Excel 9 File",                                  //448
        "RECALCID : Recalc Information",                              //449
        NULL,                                                         //450
        NULL,                                                         //451
        NULL,                                                         //452
        NULL,                                                         //453
        NULL,                                                         //454
        NULL,                                                         //455
        NULL,                                                         //456
        NULL,                                                         //457
        NULL,                                                         //458
        NULL,                                                         //459
        NULL,                                                         //460
        NULL,                                                         //461
        NULL,                                                         //462
        NULL,                                                         //463
        NULL,                                                         //464
        NULL,                                                         //465
        NULL,                                                         //466
        NULL,                                                         //467
        NULL,                                                         //468
        NULL,                                                         //469
        NULL,                                                         //470
        NULL,                                                         //471
        NULL,                                                         //472
        NULL,                                                         //473
        NULL,                                                         //474
        NULL,                                                         //475
        NULL,                                                         //476
        NULL,                                                         //477
        NULL,                                                         //478
        NULL,                                                         //479
        NULL,                                                         //480
        NULL,                                                         //481
        NULL,                                                         //482
        NULL,                                                         //483
        NULL,                                                         //484
        NULL,                                                         //485
        NULL,                                                         //486
        NULL,                                                         //487
        NULL,                                                         //488
        NULL,                                                         //489
        NULL,                                                         //490
        NULL,                                                         //491
        NULL,                                                         //492
        NULL,                                                         //493
        NULL,                                                         //494
        NULL,                                                         //495
        NULL,                                                         //496
        NULL,                                                         //497
        NULL,                                                         //498
        NULL,                                                         //499
        NULL,                                                         //500
        NULL,                                                         //501
        NULL,                                                         //502
        NULL,                                                         //503
        NULL,                                                         //504
        NULL,                                                         //505
        NULL,                                                         //506
        NULL,                                                         //507
        NULL,                                                         //508
        NULL,                                                         //509
        NULL,                                                         //510
        NULL,                                                         //511
        "DIMENSIONS : Cell Table Size",                               //512
        "BLANK : Cell Value, Blank Cell",                             //513
        NULL,                                                         //514
        "NUMBER : Cell Value, Floating-Point Number",                 //515
        "LABEL : Cell Value, String Constant",                        //516
        "BOOLERR : Cell Value, Boolean or Error",                     //517
        NULL,                                                         //518
        "STRING : String Value of a Formula",                         //519
        "ROW : Describes a Row",                                      //520
        NULL,                                                         //521
        NULL,                                                         //522
        "INDEX : Index Record",                                       //523
        NULL,                                                         //524
        NULL,                                                         //525
        NULL,                                                         //526
        NULL,                                                         //527
        NULL,                                                         //528
        NULL,                                                         //529
        NULL,                                                         //530
        NULL,                                                         //531
        NULL,                                                         //532
        NULL,                                                         //533
        NULL,                                                         //534
        NULL,                                                         //535
        "NAME : Defined Name",                                        //536
        NULL,                                                         //537
        NULL,                                                         //538
        NULL,                                                         //539
        NULL,                                                         //540
        NULL,                                                         //541
        NULL,                                                         //542
        NULL,                                                         //543
        NULL,                                                         //544
        "ARRAY : Array-Entered Formula",                              //545
        NULL,                                                         //546
        "EXTERNNAME : Externally Referenced Name",                    //547
        NULL,                                                         //548
        "DEFAULTROWHEIGHT : Default Row Height",                      //549
        NULL,                                                         //550
        NULL,                                                         //551
        NULL,                                                         //552
        NULL,                                                         //553
        NULL,                                                         //554
        NULL,                                                         //555
        NULL,                                                         //556
        NULL,                                                         //557
        NULL,                                                         //558
        NULL,                                                         //559
        NULL,                                                         //560
        "FONT : Font Description",                                    //561
        NULL,                                                         //562
        NULL,                                                         //563
        NULL,                                                         //564
        NULL,                                                         //565
        "TABLE : Data Table",                                         //566
        NULL,                                                         //567
        NULL,                                                         //568
        NULL,                                                         //569
        NULL,                                                         //570
        NULL,                                                         //571
        NULL,                                                         //572
        NULL,                                                         //573
        "WINDOW2 : Sheet Window Information",                         //574
        NULL,                                                         //575
        NULL,                                                         //576
        NULL,                                                         //577
        NULL,                                                         //578
        NULL,                                                         //579
        NULL,                                                         //580
        NULL,                                                         //581
        NULL,                                                         //582
        NULL,                                                         //583
        NULL,                                                         //584
        NULL,                                                         //585
        NULL,                                                         //586
        NULL,                                                         //587
        NULL,                                                         //588
        NULL,                                                         //589
        NULL,                                                         //590
        NULL,                                                         //591
        NULL,                                                         //592
        NULL,                                                         //593
        NULL,                                                         //594
        NULL,                                                         //595
        NULL,                                                         //596
        NULL,                                                         //597
        NULL,                                                         //598
        NULL,                                                         //599
        NULL,                                                         //600
        NULL,                                                         //601
        NULL,                                                         //602
        NULL,                                                         //603
        NULL,                                                         //604
        NULL,                                                         //605
        NULL,                                                         //606
        NULL,                                                         //607
        NULL,                                                         //608
        NULL,                                                         //609
        NULL,                                                         //610
        NULL,                                                         //611
        NULL,                                                         //612
        NULL,                                                         //613
        NULL,                                                         //614
        NULL,                                                         //615
        NULL,                                                         //616
        NULL,                                                         //617
        NULL,                                                         //618
        NULL,                                                         //619
        NULL,                                                         //620
        NULL,                                                         //621
        NULL,                                                         //622
        NULL,                                                         //623
        NULL,                                                         //624
        NULL,                                                         //625
        NULL,                                                         //626
        NULL,                                                         //627
        NULL,                                                         //628
        NULL,                                                         //629
        NULL,                                                         //630
        NULL,                                                         //631
        NULL,                                                         //632
        NULL,                                                         //633
        NULL,                                                         //634
        NULL,                                                         //635
        NULL,                                                         //636
        NULL,                                                         //637
        NULL,                                                         //638
        NULL,                                                         //639
        NULL,                                                         //640
        NULL,                                                         //641
        NULL,                                                         //642
        NULL,                                                         //643
        NULL,                                                         //644
        NULL,                                                         //645
        NULL,                                                         //646
        NULL,                                                         //647
        NULL,                                                         //648
        NULL,                                                         //649
        NULL,                                                         //650
        NULL,                                                         //651
        NULL,                                                         //652
        NULL,                                                         //653
        NULL,                                                         //654
        NULL,                                                         //655
        NULL,                                                         //656
        NULL,                                                         //657
        NULL,                                                         //658
        "STYLE : Style Information",                                  //659
        NULL,                                                         //660
        NULL,                                                         //661
        NULL,                                                         //662
        NULL,                                                         //663
        NULL,                                                         //664
        NULL,                                                         //665
        NULL,                                                         //666
        NULL,                                                         //667
        NULL,                                                         //668
        NULL,                                                         //669
        NULL,                                                         //670
        NULL,                                                         //671
        NULL,                                                         //672
        NULL,                                                         //673
        NULL,                                                         //674
        NULL,                                                         //675
        NULL,                                                         //676
        NULL,                                                         //677
        NULL,                                                         //678
        NULL,                                                         //679
        NULL,                                                         //680
        NULL,                                                         //681
        NULL,                                                         //682
        NULL,                                                         //683
        NULL,                                                         //684
        NULL,                                                         //685
        NULL,                                                         //686
        NULL,                                                         //687
        NULL,                                                         //688
        NULL,                                                         //689
        NULL,                                                         //690
        NULL,                                                         //691
        NULL,                                                         //692
        NULL,                                                         //693
        NULL,                                                         //694
        NULL,                                                         //695
        NULL,                                                         //696
        NULL,                                                         //697
        NULL,                                                         //698
        NULL,                                                         //699
        NULL,                                                         //700
        NULL,                                                         //701
        NULL,                                                         //702
        NULL,                                                         //703
        NULL,                                                         //704
        NULL,                                                         //705
        NULL,                                                         //706
        NULL,                                                         //707
        NULL,                                                         //708
        NULL,                                                         //709
        NULL,                                                         //710
        NULL,                                                         //711
        NULL,                                                         //712
        NULL,                                                         //713
        NULL,                                                         //714
        NULL,                                                         //715
        NULL,                                                         //716
        NULL,                                                         //717
        NULL,                                                         //718
        NULL,                                                         //719
        NULL,                                                         //720
        NULL,                                                         //721
        NULL,                                                         //722
        NULL,                                                         //723
        NULL,                                                         //724
        NULL,                                                         //725
        NULL,                                                         //726
        NULL,                                                         //727
        NULL,                                                         //728
        NULL,                                                         //729
        NULL,                                                         //730
        NULL,                                                         //731
        NULL,                                                         //732
        NULL,                                                         //733
        NULL,                                                         //734
        NULL,                                                         //735
        NULL,                                                         //736
        NULL,                                                         //737
        NULL,                                                         //738
        NULL,                                                         //739
        NULL,                                                         //740
        NULL,                                                         //741
        NULL,                                                         //742
        NULL,                                                         //743
        NULL,                                                         //744
        NULL,                                                         //745
        NULL,                                                         //746
        NULL,                                                         //747
        NULL,                                                         //748
        NULL,                                                         //749
        NULL,                                                         //750
        NULL,                                                         //751
        NULL,                                                         //752
        NULL,                                                         //753
        NULL,                                                         //754
        NULL,                                                         //755
        NULL,                                                         //756
        NULL,                                                         //757
        NULL,                                                         //758
        NULL,                                                         //759
        NULL,                                                         //760
        NULL,                                                         //761
        NULL,                                                         //762
        NULL,                                                         //763
        NULL,                                                         //764
        NULL,                                                         //765
        NULL,                                                         //766
        NULL,                                                         //767
        NULL,                                                         //768
        NULL,                                                         //769
        NULL,                                                         //770
        NULL,                                                         //771
        NULL,                                                         //772
        NULL,                                                         //773
        NULL,                                                         //774
        NULL,                                                         //775
        NULL,                                                         //776
        NULL,                                                         //777
        NULL,                                                         //778
        NULL,                                                         //779
        NULL,                                                         //780
        NULL,                                                         //781
        NULL,                                                         //782
        NULL,                                                         //783
        NULL,                                                         //784
        NULL,                                                         //785
        NULL,                                                         //786
        NULL,                                                         //787
        NULL,                                                         //788
        NULL,                                                         //789
        NULL,                                                         //790
        NULL,                                                         //791
        NULL,                                                         //792
        NULL,                                                         //793
        NULL,                                                         //794
        NULL,                                                         //795
        NULL,                                                         //796
        NULL,                                                         //797
        NULL,                                                         //798
        NULL,                                                         //799
        NULL,                                                         //800
        NULL,                                                         //801
        NULL,                                                         //802
        NULL,                                                         //803
        NULL,                                                         //804
        NULL,                                                         //805
        NULL,                                                         //806
        NULL,                                                         //807
        NULL,                                                         //808
        NULL,                                                         //809
        NULL,                                                         //810
        NULL,                                                         //811
        NULL,                                                         //812
        NULL,                                                         //813
        NULL,                                                         //814
        NULL,                                                         //815
        NULL,                                                         //816
        NULL,                                                         //817
        NULL,                                                         //818
        NULL,                                                         //819
        NULL,                                                         //820
        NULL,                                                         //821
        NULL,                                                         //822
        NULL,                                                         //823
        NULL,                                                         //824
        NULL,                                                         //825
        NULL,                                                         //826
        NULL,                                                         //827
        NULL,                                                         //828
        NULL,                                                         //829
        NULL,                                                         //830
        NULL,                                                         //831
        NULL,                                                         //832
        NULL,                                                         //833
        NULL,                                                         //834
        NULL,                                                         //835
        NULL,                                                         //836
        NULL,                                                         //837
        NULL,                                                         //838
        NULL,                                                         //839
        NULL,                                                         //840
        NULL,                                                         //841
        NULL,                                                         //842
        NULL,                                                         //843
        NULL,                                                         //844
        NULL,                                                         //845
        NULL,                                                         //846
        NULL,                                                         //847
        NULL,                                                         //848
        NULL,                                                         //849
        NULL,                                                         //850
        NULL,                                                         //851
        NULL,                                                         //852
        NULL,                                                         //853
        NULL,                                                         //854
        NULL,                                                         //855
        NULL,                                                         //856
        NULL,                                                         //857
        NULL,                                                         //858
        NULL,                                                         //859
        NULL,                                                         //860
        NULL,                                                         //861
        NULL,                                                         //862
        NULL,                                                         //863
        NULL,                                                         //864
        NULL,                                                         //865
        NULL,                                                         //866
        NULL,                                                         //867
        NULL,                                                         //868
        NULL,                                                         //869
        NULL,                                                         //870
        NULL,                                                         //871
        NULL,                                                         //872
        NULL,                                                         //873
        NULL,                                                         //874
        NULL,                                                         //875
        NULL,                                                         //876
        NULL,                                                         //877
        NULL,                                                         //878
        NULL,                                                         //879
        NULL,                                                         //880
        NULL,                                                         //881
        NULL,                                                         //882
        NULL,                                                         //883
        NULL,                                                         //884
        NULL,                                                         //885
        NULL,                                                         //886
        NULL,                                                         //887
        NULL,                                                         //888
        NULL,                                                         //889
        NULL,                                                         //890
        NULL,                                                         //891
        NULL,                                                         //892
        NULL,                                                         //893
        NULL,                                                         //894
        NULL,                                                         //895
        NULL,                                                         //896
        NULL,                                                         //897
        NULL,                                                         //898
        NULL,                                                         //899
        NULL,                                                         //900
        NULL,                                                         //901
        NULL,                                                         //902
        NULL,                                                         //903
        NULL,                                                         //904
        NULL,                                                         //905
        NULL,                                                         //906
        NULL,                                                         //907
        NULL,                                                         //908
        NULL,                                                         //909
        NULL,                                                         //910
        NULL,                                                         //911
        NULL,                                                         //912
        NULL,                                                         //913
        NULL,                                                         //914
        NULL,                                                         //915
        NULL,                                                         //916
        NULL,                                                         //917
        NULL,                                                         //918
        NULL,                                                         //919
        NULL,                                                         //920
        NULL,                                                         //921
        NULL,                                                         //922
        NULL,                                                         //923
        NULL,                                                         //924
        NULL,                                                         //925
        NULL,                                                         //926
        NULL,                                                         //927
        NULL,                                                         //928
        NULL,                                                         //929
        NULL,                                                         //930
        NULL,                                                         //931
        NULL,                                                         //932
        NULL,                                                         //933
        NULL,                                                         //934
        NULL,                                                         //935
        NULL,                                                         //936
        NULL,                                                         //937
        NULL,                                                         //938
        NULL,                                                         //939
        NULL,                                                         //940
        NULL,                                                         //941
        NULL,                                                         //942
        NULL,                                                         //943
        NULL,                                                         //944
        NULL,                                                         //945
        NULL,                                                         //946
        NULL,                                                         //947
        NULL,                                                         //948
        NULL,                                                         //949
        NULL,                                                         //950
        NULL,                                                         //951
        NULL,                                                         //952
        NULL,                                                         //953
        NULL,                                                         //954
        NULL,                                                         //955
        NULL,                                                         //956
        NULL,                                                         //957
        NULL,                                                         //958
        NULL,                                                         //959
        NULL,                                                         //960
        NULL,                                                         //961
        NULL,                                                         //962
        NULL,                                                         //963
        NULL,                                                         //964
        NULL,                                                         //965
        NULL,                                                         //966
        NULL,                                                         //967
        NULL,                                                         //968
        NULL,                                                         //969
        NULL,                                                         //970
        NULL,                                                         //971
        NULL,                                                         //972
        NULL,                                                         //973
        NULL,                                                         //974
        NULL,                                                         //975
        NULL,                                                         //976
        NULL,                                                         //977
        NULL,                                                         //978
        NULL,                                                         //979
        NULL,                                                         //980
        NULL,                                                         //981
        NULL,                                                         //982
        NULL,                                                         //983
        NULL,                                                         //984
        NULL,                                                         //985
        NULL,                                                         //986
        NULL,                                                         //987
        NULL,                                                         //988
        NULL,                                                         //989
        NULL,                                                         //990
        NULL,                                                         //991
        NULL,                                                         //992
        NULL,                                                         //993
        NULL,                                                         //994
        NULL,                                                         //995
        NULL,                                                         //996
        NULL,                                                         //997
        NULL,                                                         //998
        NULL,                                                         //999
        NULL,                                                         //1000
        NULL,                                                         //1001
        NULL,                                                         //1002
        NULL,                                                         //1003
        NULL,                                                         //1004
        NULL,                                                         //1005
        NULL,                                                         //1006
        NULL,                                                         //1007
        NULL,                                                         //1008
        NULL,                                                         //1009
        NULL,                                                         //1010
        NULL,                                                         //1011
        NULL,                                                         //1012
        NULL,                                                         //1013
        NULL,                                                         //1014
        NULL,                                                         //1015
        NULL,                                                         //1016
        NULL,                                                         //1017
        NULL,                                                         //1018
        NULL,                                                         //1019
        NULL,                                                         //1020
        NULL,                                                         //1021
        NULL,                                                         //1022
        NULL,                                                         //1023
        NULL,                                                         //1024
        NULL,                                                         //1025
        NULL,                                                         //1026
        NULL,                                                         //1027
        NULL,                                                         //1028
        NULL,                                                         //1029
        "FORMULA : Cell Formula",                                     //1030
        NULL,                                                         //1031
        NULL,                                                         //1032
        NULL,                                                         //1033
        NULL,                                                         //1034
        NULL,                                                         //1035
        NULL,                                                         //1036
        NULL,                                                         //1037
        NULL,                                                         //1038
        NULL,                                                         //1039
        NULL,                                                         //1040
        NULL,                                                         //1041
        NULL,                                                         //1042
        NULL,                                                         //1043
        NULL,                                                         //1044
        NULL,                                                         //1045
        NULL,                                                         //1046
        NULL,                                                         //1047
        NULL,                                                         //1048
        NULL,                                                         //1049
        NULL,                                                         //1050
        NULL,                                                         //1051
        NULL,                                                         //1052
        NULL,                                                         //1053
        "FORMAT : Number Format",                                     //1054
        NULL,                                                         //1055
        NULL,                                                         //1056
        NULL,                                                         //1057
        NULL,                                                         //1058
        NULL,                                                         //1059
        NULL,                                                         //1060
        NULL,                                                         //1061
        NULL,                                                         //1062
        NULL,                                                         //1063
        NULL,                                                         //1064
        NULL,                                                         //1065
        NULL,                                                         //1066
        NULL,                                                         //1067
        NULL,                                                         //1068
        NULL,                                                         //1069
        NULL,                                                         //1070
        NULL,                                                         //1071
        NULL,                                                         //1072
        NULL,                                                         //1073
        NULL,                                                         //1074
        NULL,                                                         //1075
        NULL,                                                         //1076
        NULL,                                                         //1077
        NULL,                                                         //1078
        NULL,                                                         //1079
        NULL,                                                         //1080
        NULL,                                                         //1081
        NULL,                                                         //1082
        NULL,                                                         //1083
        NULL,                                                         //1084
        NULL,                                                         //1085
        NULL,                                                         //1086
        NULL,                                                         //1087
        NULL,                                                         //1088
        NULL,                                                         //1089
        NULL,                                                         //1090
        NULL,                                                         //1091
        NULL,                                                         //1092
        NULL,                                                         //1093
        NULL,                                                         //1094
        NULL,                                                         //1095
        NULL,                                                         //1096
        NULL,                                                         //1097
        NULL,                                                         //1098
        NULL,                                                         //1099
        NULL,                                                         //1100
        NULL,                                                         //1101
        NULL,                                                         //1102
        NULL,                                                         //1103
        NULL,                                                         //1104
        NULL,                                                         //1105
        NULL,                                                         //1106
        NULL,                                                         //1107
        NULL,                                                         //1108
        NULL,                                                         //1109
        NULL,                                                         //1110
        NULL,                                                         //1111
        NULL,                                                         //1112
        NULL,                                                         //1113
        NULL,                                                         //1114
        NULL,                                                         //1115
        NULL,                                                         //1116
        NULL,                                                         //1117
        NULL,                                                         //1118
        NULL,                                                         //1119
        NULL,                                                         //1120
        NULL,                                                         //1121
        NULL,                                                         //1122
        NULL,                                                         //1123
        NULL,                                                         //1124
        NULL,                                                         //1125
        NULL,                                                         //1126
        NULL,                                                         //1127
        NULL,                                                         //1128
        NULL,                                                         //1129
        NULL,                                                         //1130
        NULL,                                                         //1131
        NULL,                                                         //1132
        NULL,                                                         //1133
        NULL,                                                         //1134
        NULL,                                                         //1135
        NULL,                                                         //1136
        NULL,                                                         //1137
        NULL,                                                         //1138
        NULL,                                                         //1139
        NULL,                                                         //1140
        NULL,                                                         //1141
        NULL,                                                         //1142
        NULL,                                                         //1143
        NULL,                                                         //1144
        NULL,                                                         //1145
        NULL,                                                         //1146
        NULL,                                                         //1147
        NULL,                                                         //1148
        NULL,                                                         //1149
        NULL,                                                         //1150
        NULL,                                                         //1151
        NULL,                                                         //1152
        NULL,                                                         //1153
        NULL,                                                         //1154
        NULL,                                                         //1155
        NULL,                                                         //1156
        NULL,                                                         //1157
        NULL,                                                         //1158
        NULL,                                                         //1159
        NULL,                                                         //1160
        NULL,                                                         //1161
        NULL,                                                         //1162
        NULL,                                                         //1163
        NULL,                                                         //1164
        NULL,                                                         //1165
        NULL,                                                         //1166
        NULL,                                                         //1167
        NULL,                                                         //1168
        NULL,                                                         //1169
        NULL,                                                         //1170
        NULL,                                                         //1171
        NULL,                                                         //1172
        NULL,                                                         //1173
        NULL,                                                         //1174
        NULL,                                                         //1175
        NULL,                                                         //1176
        NULL,                                                         //1177
        NULL,                                                         //1178
        NULL,                                                         //1179
        NULL,                                                         //1180
        NULL,                                                         //1181
        NULL,                                                         //1182
        NULL,                                                         //1183
        NULL,                                                         //1184
        NULL,                                                         //1185
        NULL,                                                         //1186
        NULL,                                                         //1187
        NULL,                                                         //1188
        NULL,                                                         //1189
        NULL,                                                         //1190
        NULL,                                                         //1191
        NULL,                                                         //1192
        NULL,                                                         //1193
        NULL,                                                         //1194
        NULL,                                                         //1195
        NULL,                                                         //1196
        NULL,                                                         //1197
        NULL,                                                         //1198
        NULL,                                                         //1199
        NULL,                                                         //1200
        NULL,                                                         //1201
        NULL,                                                         //1202
        NULL,                                                         //1203
        NULL,                                                         //1204
        NULL,                                                         //1205
        NULL,                                                         //1206
        NULL,                                                         //1207
        NULL,                                                         //1208
        NULL,                                                         //1209
        NULL,                                                         //1210
        NULL,                                                         //1211
        NULL,                                                         //1212
        NULL,                                                         //1213
        NULL,                                                         //1214
        NULL,                                                         //1215
        NULL,                                                         //1216
        NULL,                                                         //1217
        NULL,                                                         //1218
        NULL,                                                         //1219
        NULL,                                                         //1220
        NULL,                                                         //1221
        NULL,                                                         //1222
        NULL,                                                         //1223
        NULL,                                                         //1224
        NULL,                                                         //1225
        NULL,                                                         //1226
        NULL,                                                         //1227
        NULL,                                                         //1228
        NULL,                                                         //1229
        NULL,                                                         //1230
        NULL,                                                         //1231
        NULL,                                                         //1232
        NULL,                                                         //1233
        NULL,                                                         //1234
        NULL,                                                         //1235
        NULL,                                                         //1236
        NULL,                                                         //1237
        NULL,                                                         //1238
        NULL,                                                         //1239
        NULL,                                                         //1240
        NULL,                                                         //1241
        NULL,                                                         //1242
        NULL,                                                         //1243
        NULL,                                                         //1244
        NULL,                                                         //1245
        NULL,                                                         //1246
        NULL,                                                         //1247
        NULL,                                                         //1248
        NULL,                                                         //1249
        NULL,                                                         //1250
        NULL,                                                         //1251
        NULL,                                                         //1252
        NULL,                                                         //1253
        NULL,                                                         //1254
        NULL,                                                         //1255
        NULL,                                                         //1256
        NULL,                                                         //1257
        NULL,                                                         //1258
        NULL,                                                         //1259
        NULL,                                                         //1260
        NULL,                                                         //1261
        NULL,                                                         //1262
        NULL,                                                         //1263
        NULL,                                                         //1264
        NULL,                                                         //1265
        NULL,                                                         //1266
        NULL,                                                         //1267
        NULL,                                                         //1268
        NULL,                                                         //1269
        NULL,                                                         //1270
        NULL,                                                         //1271
        NULL,                                                         //1272
        NULL,                                                         //1273
        NULL,                                                         //1274
        NULL,                                                         //1275
        NULL,                                                         //1276
        NULL,                                                         //1277
        NULL,                                                         //1278
        NULL,                                                         //1279
        NULL,                                                         //1280
        NULL,                                                         //1281
        NULL,                                                         //1282
        NULL,                                                         //1283
        NULL,                                                         //1284
        NULL,                                                         //1285
        NULL,                                                         //1286
        NULL,                                                         //1287
        NULL,                                                         //1288
        NULL,                                                         //1289
        NULL,                                                         //1290
        NULL,                                                         //1291
        NULL,                                                         //1292
        NULL,                                                         //1293
        NULL,                                                         //1294
        NULL,                                                         //1295
        NULL,                                                         //1296
        NULL,                                                         //1297
        NULL,                                                         //1298
        NULL,                                                         //1299
        NULL,                                                         //1300
        NULL,                                                         //1301
        NULL,                                                         //1302
        NULL,                                                         //1303
        NULL,                                                         //1304
        NULL,                                                         //1305
        NULL,                                                         //1306
        NULL,                                                         //1307
        NULL,                                                         //1308
        NULL,                                                         //1309
        NULL,                                                         //1310
        NULL,                                                         //1311
        NULL,                                                         //1312
        NULL,                                                         //1313
        NULL,                                                         //1314
        NULL,                                                         //1315
        NULL,                                                         //1316
        NULL,                                                         //1317
        NULL,                                                         //1318
        NULL,                                                         //1319
        NULL,                                                         //1320
        NULL,                                                         //1321
        NULL,                                                         //1322
        NULL,                                                         //1323
        NULL,                                                         //1324
        NULL,                                                         //1325
        NULL,                                                         //1326
        NULL,                                                         //1327
        NULL,                                                         //1328
        NULL,                                                         //1329
        NULL,                                                         //1330
        NULL,                                                         //1331
        NULL,                                                         //1332
        NULL,                                                         //1333
        NULL,                                                         //1334
        NULL,                                                         //1335
        NULL,                                                         //1336
        NULL,                                                         //1337
        NULL,                                                         //1338
        NULL,                                                         //1339
        NULL,                                                         //1340
        NULL,                                                         //1341
        NULL,                                                         //1342
        NULL,                                                         //1343
        NULL,                                                         //1344
        NULL,                                                         //1345
        NULL,                                                         //1346
        NULL,                                                         //1347
        NULL,                                                         //1348
        NULL,                                                         //1349
        NULL,                                                         //1350
        NULL,                                                         //1351
        NULL,                                                         //1352
        NULL,                                                         //1353
        NULL,                                                         //1354
        NULL,                                                         //1355
        NULL,                                                         //1356
        NULL,                                                         //1357
        NULL,                                                         //1358
        NULL,                                                         //1359
        NULL,                                                         //1360
        NULL,                                                         //1361
        NULL,                                                         //1362
        NULL,                                                         //1363
        NULL,                                                         //1364
        NULL,                                                         //1365
        NULL,                                                         //1366
        NULL,                                                         //1367
        NULL,                                                         //1368
        NULL,                                                         //1369
        NULL,                                                         //1370
        NULL,                                                         //1371
        NULL,                                                         //1372
        NULL,                                                         //1373
        NULL,                                                         //1374
        NULL,                                                         //1375
        NULL,                                                         //1376
        NULL,                                                         //1377
        NULL,                                                         //1378
        NULL,                                                         //1379
        NULL,                                                         //1380
        NULL,                                                         //1381
        NULL,                                                         //1382
        NULL,                                                         //1383
        NULL,                                                         //1384
        NULL,                                                         //1385
        NULL,                                                         //1386
        NULL,                                                         //1387
        NULL,                                                         //1388
        NULL,                                                         //1389
        NULL,                                                         //1390
        NULL,                                                         //1391
        NULL,                                                         //1392
        NULL,                                                         //1393
        NULL,                                                         //1394
        NULL,                                                         //1395
        NULL,                                                         //1396
        NULL,                                                         //1397
        NULL,                                                         //1398
        NULL,                                                         //1399
        NULL,                                                         //1400
        NULL,                                                         //1401
        NULL,                                                         //1402
        NULL,                                                         //1403
        NULL,                                                         //1404
        NULL,                                                         //1405
        NULL,                                                         //1406
        NULL,                                                         //1407
        NULL,                                                         //1408
        NULL,                                                         //1409
        NULL,                                                         //1410
        NULL,                                                         //1411
        NULL,                                                         //1412
        NULL,                                                         //1413
        NULL,                                                         //1414
        NULL,                                                         //1415
        NULL,                                                         //1416
        NULL,                                                         //1417
        NULL,                                                         //1418
        NULL,                                                         //1419
        NULL,                                                         //1420
        NULL,                                                         //1421
        NULL,                                                         //1422
        NULL,                                                         //1423
        NULL,                                                         //1424
        NULL,                                                         //1425
        NULL,                                                         //1426
        NULL,                                                         //1427
        NULL,                                                         //1428
        NULL,                                                         //1429
        NULL,                                                         //1430
        NULL,                                                         //1431
        NULL,                                                         //1432
        NULL,                                                         //1433
        NULL,                                                         //1434
        NULL,                                                         //1435
        NULL,                                                         //1436
        NULL,                                                         //1437
        NULL,                                                         //1438
        NULL,                                                         //1439
        NULL,                                                         //1440
        NULL,                                                         //1441
        NULL,                                                         //1442
        NULL,                                                         //1443
        NULL,                                                         //1444
        NULL,                                                         //1445
        NULL,                                                         //1446
        NULL,                                                         //1447
        NULL,                                                         //1448
        NULL,                                                         //1449
        NULL,                                                         //1450
        NULL,                                                         //1451
        NULL,                                                         //1452
        NULL,                                                         //1453
        NULL,                                                         //1454
        NULL,                                                         //1455
        NULL,                                                         //1456
        NULL,                                                         //1457
        NULL,                                                         //1458
        NULL,                                                         //1459
        NULL,                                                         //1460
        NULL,                                                         //1461
        NULL,                                                         //1462
        NULL,                                                         //1463
        NULL,                                                         //1464
        NULL,                                                         //1465
        NULL,                                                         //1466
        NULL,                                                         //1467
        NULL,                                                         //1468
        NULL,                                                         //1469
        NULL,                                                         //1470
        NULL,                                                         //1471
        NULL,                                                         //1472
        NULL,                                                         //1473
        NULL,                                                         //1474
        NULL,                                                         //1475
        NULL,                                                         //1476
        NULL,                                                         //1477
        NULL,                                                         //1478
        NULL,                                                         //1479
        NULL,                                                         //1480
        NULL,                                                         //1481
        NULL,                                                         //1482
        NULL,                                                         //1483
        NULL,                                                         //1484
        NULL,                                                         //1485
        NULL,                                                         //1486
        NULL,                                                         //1487
        NULL,                                                         //1488
        NULL,                                                         //1489
        NULL,                                                         //1490
        NULL,                                                         //1491
        NULL,                                                         //1492
        NULL,                                                         //1493
        NULL,                                                         //1494
        NULL,                                                         //1495
        NULL,                                                         //1496
        NULL,                                                         //1497
        NULL,                                                         //1498
        NULL,                                                         //1499
        NULL,                                                         //1500
        NULL,                                                         //1501
        NULL,                                                         //1502
        NULL,                                                         //1503
        NULL,                                                         //1504
        NULL,                                                         //1505
        NULL,                                                         //1506
        NULL,                                                         //1507
        NULL,                                                         //1508
        NULL,                                                         //1509
        NULL,                                                         //1510
        NULL,                                                         //1511
        NULL,                                                         //1512
        NULL,                                                         //1513
        NULL,                                                         //1514
        NULL,                                                         //1515
        NULL,                                                         //1516
        NULL,                                                         //1517
        NULL,                                                         //1518
        NULL,                                                         //1519
        NULL,                                                         //1520
        NULL,                                                         //1521
        NULL,                                                         //1522
        NULL,                                                         //1523
        NULL,                                                         //1524
        NULL,                                                         //1525
        NULL,                                                         //1526
        NULL,                                                         //1527
        NULL,                                                         //1528
        NULL,                                                         //1529
        NULL,                                                         //1530
        NULL,                                                         //1531
        NULL,                                                         //1532
        NULL,                                                         //1533
        NULL,                                                         //1534
        NULL,                                                         //1535
        NULL,                                                         //1536
        NULL,                                                         //1537
        NULL,                                                         //1538
        NULL,                                                         //1539
        NULL,                                                         //1540
        NULL,                                                         //1541
        NULL,                                                         //1542
        NULL,                                                         //1543
        NULL,                                                         //1544
        NULL,                                                         //1545
        NULL,                                                         //1546
        NULL,                                                         //1547
        NULL,                                                         //1548
        NULL,                                                         //1549
        NULL,                                                         //1550
        NULL,                                                         //1551
        NULL,                                                         //1552
        NULL,                                                         //1553
        NULL,                                                         //1554
        NULL,                                                         //1555
        NULL,                                                         //1556
        NULL,                                                         //1557
        NULL,                                                         //1558
        NULL,                                                         //1559
        NULL,                                                         //1560
        NULL,                                                         //1561
        NULL,                                                         //1562
        NULL,                                                         //1563
        NULL,                                                         //1564
        NULL,                                                         //1565
        NULL,                                                         //1566
        NULL,                                                         //1567
        NULL,                                                         //1568
        NULL,                                                         //1569
        NULL,                                                         //1570
        NULL,                                                         //1571
        NULL,                                                         //1572
        NULL,                                                         //1573
        NULL,                                                         //1574
        NULL,                                                         //1575
        NULL,                                                         //1576
        NULL,                                                         //1577
        NULL,                                                         //1578
        NULL,                                                         //1579
        NULL,                                                         //1580
        NULL,                                                         //1581
        NULL,                                                         //1582
        NULL,                                                         //1583
        NULL,                                                         //1584
        NULL,                                                         //1585
        NULL,                                                         //1586
        NULL,                                                         //1587
        NULL,                                                         //1588
        NULL,                                                         //1589
        NULL,                                                         //1590
        NULL,                                                         //1591
        NULL,                                                         //1592
        NULL,                                                         //1593
        NULL,                                                         //1594
        NULL,                                                         //1595
        NULL,                                                         //1596
        NULL,                                                         //1597
        NULL,                                                         //1598
        NULL,                                                         //1599
        NULL,                                                         //1600
        NULL,                                                         //1601
        NULL,                                                         //1602
        NULL,                                                         //1603
        NULL,                                                         //1604
        NULL,                                                         //1605
        NULL,                                                         //1606
        NULL,                                                         //1607
        NULL,                                                         //1608
        NULL,                                                         //1609
        NULL,                                                         //1610
        NULL,                                                         //1611
        NULL,                                                         //1612
        NULL,                                                         //1613
        NULL,                                                         //1614
        NULL,                                                         //1615
        NULL,                                                         //1616
        NULL,                                                         //1617
        NULL,                                                         //1618
        NULL,                                                         //1619
        NULL,                                                         //1620
        NULL,                                                         //1621
        NULL,                                                         //1622
        NULL,                                                         //1623
        NULL,                                                         //1624
        NULL,                                                         //1625
        NULL,                                                         //1626
        NULL,                                                         //1627
        NULL,                                                         //1628
        NULL,                                                         //1629
        NULL,                                                         //1630
        NULL,                                                         //1631
        NULL,                                                         //1632
        NULL,                                                         //1633
        NULL,                                                         //1634
        NULL,                                                         //1635
        NULL,                                                         //1636
        NULL,                                                         //1637
        NULL,                                                         //1638
        NULL,                                                         //1639
        NULL,                                                         //1640
        NULL,                                                         //1641
        NULL,                                                         //1642
        NULL,                                                         //1643
        NULL,                                                         //1644
        NULL,                                                         //1645
        NULL,                                                         //1646
        NULL,                                                         //1647
        NULL,                                                         //1648
        NULL,                                                         //1649
        NULL,                                                         //1650
        NULL,                                                         //1651
        NULL,                                                         //1652
        NULL,                                                         //1653
        NULL,                                                         //1654
        NULL,                                                         //1655
        NULL,                                                         //1656
        NULL,                                                         //1657
        NULL,                                                         //1658
        NULL,                                                         //1659
        NULL,                                                         //1660
        NULL,                                                         //1661
        NULL,                                                         //1662
        NULL,                                                         //1663
        NULL,                                                         //1664
        NULL,                                                         //1665
        NULL,                                                         //1666
        NULL,                                                         //1667
        NULL,                                                         //1668
        NULL,                                                         //1669
        NULL,                                                         //1670
        NULL,                                                         //1671
        NULL,                                                         //1672
        NULL,                                                         //1673
        NULL,                                                         //1674
        NULL,                                                         //1675
        NULL,                                                         //1676
        NULL,                                                         //1677
        NULL,                                                         //1678
        NULL,                                                         //1679
        NULL,                                                         //1680
        NULL,                                                         //1681
        NULL,                                                         //1682
        NULL,                                                         //1683
        NULL,                                                         //1684
        NULL,                                                         //1685
        NULL,                                                         //1686
        NULL,                                                         //1687
        NULL,                                                         //1688
        NULL,                                                         //1689
        NULL,                                                         //1690
        NULL,                                                         //1691
        NULL,                                                         //1692
        NULL,                                                         //1693
        NULL,                                                         //1694
        NULL,                                                         //1695
        NULL,                                                         //1696
        NULL,                                                         //1697
        NULL,                                                         //1698
        NULL,                                                         //1699
        NULL,                                                         //1700
        NULL,                                                         //1701
        NULL,                                                         //1702
        NULL,                                                         //1703
        NULL,                                                         //1704
        NULL,                                                         //1705
        NULL,                                                         //1706
        NULL,                                                         //1707
        NULL,                                                         //1708
        NULL,                                                         //1709
        NULL,                                                         //1710
        NULL,                                                         //1711
        NULL,                                                         //1712
        NULL,                                                         //1713
        NULL,                                                         //1714
        NULL,                                                         //1715
        NULL,                                                         //1716
        NULL,                                                         //1717
        NULL,                                                         //1718
        NULL,                                                         //1719
        NULL,                                                         //1720
        NULL,                                                         //1721
        NULL,                                                         //1722
        NULL,                                                         //1723
        NULL,                                                         //1724
        NULL,                                                         //1725
        NULL,                                                         //1726
        NULL,                                                         //1727
        NULL,                                                         //1728
        NULL,                                                         //1729
        NULL,                                                         //1730
        NULL,                                                         //1731
        NULL,                                                         //1732
        NULL,                                                         //1733
        NULL,                                                         //1734
        NULL,                                                         //1735
        NULL,                                                         //1736
        NULL,                                                         //1737
        NULL,                                                         //1738
        NULL,                                                         //1739
        NULL,                                                         //1740
        NULL,                                                         //1741
        NULL,                                                         //1742
        NULL,                                                         //1743
        NULL,                                                         //1744
        NULL,                                                         //1745
        NULL,                                                         //1746
        NULL,                                                         //1747
        NULL,                                                         //1748
        NULL,                                                         //1749
        NULL,                                                         //1750
        NULL,                                                         //1751
        NULL,                                                         //1752
        NULL,                                                         //1753
        NULL,                                                         //1754
        NULL,                                                         //1755
        NULL,                                                         //1756
        NULL,                                                         //1757
        NULL,                                                         //1758
        NULL,                                                         //1759
        NULL,                                                         //1760
        NULL,                                                         //1761
        NULL,                                                         //1762
        NULL,                                                         //1763
        NULL,                                                         //1764
        NULL,                                                         //1765
        NULL,                                                         //1766
        NULL,                                                         //1767
        NULL,                                                         //1768
        NULL,                                                         //1769
        NULL,                                                         //1770
        NULL,                                                         //1771
        NULL,                                                         //1772
        NULL,                                                         //1773
        NULL,                                                         //1774
        NULL,                                                         //1775
        NULL,                                                         //1776
        NULL,                                                         //1777
        NULL,                                                         //1778
        NULL,                                                         //1779
        NULL,                                                         //1780
        NULL,                                                         //1781
        NULL,                                                         //1782
        NULL,                                                         //1783
        NULL,                                                         //1784
        NULL,                                                         //1785
        NULL,                                                         //1786
        NULL,                                                         //1787
        NULL,                                                         //1788
        NULL,                                                         //1789
        NULL,                                                         //1790
        NULL,                                                         //1791
        NULL,                                                         //1792
        NULL,                                                         //1793
        NULL,                                                         //1794
        NULL,                                                         //1795
        NULL,                                                         //1796
        NULL,                                                         //1797
        NULL,                                                         //1798
        NULL,                                                         //1799
        NULL,                                                         //1800
        NULL,                                                         //1801
        NULL,                                                         //1802
        NULL,                                                         //1803
        NULL,                                                         //1804
        NULL,                                                         //1805
        NULL,                                                         //1806
        NULL,                                                         //1807
        NULL,                                                         //1808
        NULL,                                                         //1809
        NULL,                                                         //1810
        NULL,                                                         //1811
        NULL,                                                         //1812
        NULL,                                                         //1813
        NULL,                                                         //1814
        NULL,                                                         //1815
        NULL,                                                         //1816
        NULL,                                                         //1817
        NULL,                                                         //1818
        NULL,                                                         //1819
        NULL,                                                         //1820
        NULL,                                                         //1821
        NULL,                                                         //1822
        NULL,                                                         //1823
        NULL,                                                         //1824
        NULL,                                                         //1825
        NULL,                                                         //1826
        NULL,                                                         //1827
        NULL,                                                         //1828
        NULL,                                                         //1829
        NULL,                                                         //1830
        NULL,                                                         //1831
        NULL,                                                         //1832
        NULL,                                                         //1833
        NULL,                                                         //1834
        NULL,                                                         //1835
        NULL,                                                         //1836
        NULL,                                                         //1837
        NULL,                                                         //1838
        NULL,                                                         //1839
        NULL,                                                         //1840
        NULL,                                                         //1841
        NULL,                                                         //1842
        NULL,                                                         //1843
        NULL,                                                         //1844
        NULL,                                                         //1845
        NULL,                                                         //1846
        NULL,                                                         //1847
        NULL,                                                         //1848
        NULL,                                                         //1849
        NULL,                                                         //1850
        NULL,                                                         //1851
        NULL,                                                         //1852
        NULL,                                                         //1853
        NULL,                                                         //1854
        NULL,                                                         //1855
        NULL,                                                         //1856
        NULL,                                                         //1857
        NULL,                                                         //1858
        NULL,                                                         //1859
        NULL,                                                         //1860
        NULL,                                                         //1861
        NULL,                                                         //1862
        NULL,                                                         //1863
        NULL,                                                         //1864
        NULL,                                                         //1865
        NULL,                                                         //1866
        NULL,                                                         //1867
        NULL,                                                         //1868
        NULL,                                                         //1869
        NULL,                                                         //1870
        NULL,                                                         //1871
        NULL,                                                         //1872
        NULL,                                                         //1873
        NULL,                                                         //1874
        NULL,                                                         //1875
        NULL,                                                         //1876
        NULL,                                                         //1877
        NULL,                                                         //1878
        NULL,                                                         //1879
        NULL,                                                         //1880
        NULL,                                                         //1881
        NULL,                                                         //1882
        NULL,                                                         //1883
        NULL,                                                         //1884
        NULL,                                                         //1885
        NULL,                                                         //1886
        NULL,                                                         //1887
        NULL,                                                         //1888
        NULL,                                                         //1889
        NULL,                                                         //1890
        NULL,                                                         //1891
        NULL,                                                         //1892
        NULL,                                                         //1893
        NULL,                                                         //1894
        NULL,                                                         //1895
        NULL,                                                         //1896
        NULL,                                                         //1897
        NULL,                                                         //1898
        NULL,                                                         //1899
        NULL,                                                         //1900
        NULL,                                                         //1901
        NULL,                                                         //1902
        NULL,                                                         //1903
        NULL,                                                         //1904
        NULL,                                                         //1905
        NULL,                                                         //1906
        NULL,                                                         //1907
        NULL,                                                         //1908
        NULL,                                                         //1909
        NULL,                                                         //1910
        NULL,                                                         //1911
        NULL,                                                         //1912
        NULL,                                                         //1913
        NULL,                                                         //1914
        NULL,                                                         //1915
        NULL,                                                         //1916
        NULL,                                                         //1917
        NULL,                                                         //1918
        NULL,                                                         //1919
        NULL,                                                         //1920
        NULL,                                                         //1921
        NULL,                                                         //1922
        NULL,                                                         //1923
        NULL,                                                         //1924
        NULL,                                                         //1925
        NULL,                                                         //1926
        NULL,                                                         //1927
        NULL,                                                         //1928
        NULL,                                                         //1929
        NULL,                                                         //1930
        NULL,                                                         //1931
        NULL,                                                         //1932
        NULL,                                                         //1933
        NULL,                                                         //1934
        NULL,                                                         //1935
        NULL,                                                         //1936
        NULL,                                                         //1937
        NULL,                                                         //1938
        NULL,                                                         //1939
        NULL,                                                         //1940
        NULL,                                                         //1941
        NULL,                                                         //1942
        NULL,                                                         //1943
        NULL,                                                         //1944
        NULL,                                                         //1945
        NULL,                                                         //1946
        NULL,                                                         //1947
        NULL,                                                         //1948
        NULL,                                                         //1949
        NULL,                                                         //1950
        NULL,                                                         //1951
        NULL,                                                         //1952
        NULL,                                                         //1953
        NULL,                                                         //1954
        NULL,                                                         //1955
        NULL,                                                         //1956
        NULL,                                                         //1957
        NULL,                                                         //1958
        NULL,                                                         //1959
        NULL,                                                         //1960
        NULL,                                                         //1961
        NULL,                                                         //1962
        NULL,                                                         //1963
        NULL,                                                         //1964
        NULL,                                                         //1965
        NULL,                                                         //1966
        NULL,                                                         //1967
        NULL,                                                         //1968
        NULL,                                                         //1969
        NULL,                                                         //1970
        NULL,                                                         //1971
        NULL,                                                         //1972
        NULL,                                                         //1973
        NULL,                                                         //1974
        NULL,                                                         //1975
        NULL,                                                         //1976
        NULL,                                                         //1977
        NULL,                                                         //1978
        NULL,                                                         //1979
        NULL,                                                         //1980
        NULL,                                                         //1981
        NULL,                                                         //1982
        NULL,                                                         //1983
        NULL,                                                         //1984
        NULL,                                                         //1985
        NULL,                                                         //1986
        NULL,                                                         //1987
        NULL,                                                         //1988
        NULL,                                                         //1989
        NULL,                                                         //1990
        NULL,                                                         //1991
        NULL,                                                         //1992
        NULL,                                                         //1993
        NULL,                                                         //1994
        NULL,                                                         //1995
        NULL,                                                         //1996
        NULL,                                                         //1997
        NULL,                                                         //1998
        NULL,                                                         //1999
        NULL,                                                         //2000
        NULL,                                                         //2001
        NULL,                                                         //2002
        NULL,                                                         //2003
        NULL,                                                         //2004
        NULL,                                                         //2005
        NULL,                                                         //2006
        NULL,                                                         //2007
        NULL,                                                         //2008
        NULL,                                                         //2009
        NULL,                                                         //2010
        NULL,                                                         //2011
        NULL,                                                         //2012
        NULL,                                                         //2013
        NULL,                                                         //2014
        NULL,                                                         //2015
        NULL,                                                         //2016
        NULL,                                                         //2017
        NULL,                                                         //2018
        NULL,                                                         //2019
        NULL,                                                         //2020
        NULL,                                                         //2021
        NULL,                                                         //2022
        NULL,                                                         //2023
        NULL,                                                         //2024
        NULL,                                                         //2025
        NULL,                                                         //2026
        NULL,                                                         //2027
        NULL,                                                         //2028
        NULL,                                                         //2029
        NULL,                                                         //2030
        NULL,                                                         //2031
        NULL,                                                         //2032
        NULL,                                                         //2033
        NULL,                                                         //2034
        NULL,                                                         //2035
        NULL,                                                         //2036
        NULL,                                                         //2037
        NULL,                                                         //2038
        NULL,                                                         //2039
        NULL,                                                         //2040
        NULL,                                                         //2041
        NULL,                                                         //2042
        NULL,                                                         //2043
        NULL,                                                         //2044
        NULL,                                                         //2045
        NULL,                                                         //2046
        NULL,                                                         //2047
        "HLINKTOOLTIP : Hyperlink Tooltip",                           //2048
        "WEBPUB : Web Publish Item",                                  //2049
        "QSISXTAG : PivotTable and Query Table Extensions",           //2050
        "DBQUERYEXT : Database Query Extensions",                     //2051
        "EXTSTRING :  FRT String",                                    //2052
        "TXTQUERY : Text Query Information",                          //2053
        "QSIR : Query Table Formatting",                              //2054
        "QSIF : Query Table Field Formatting",                        //2055
        NULL,                                                         //2056
        "BOF : Beginning of File",                                    //2057
        "OLEDBCONN : OLE Database Connection",                        //2058
        "WOPT : Web Options",                                         //2059
        "SXVIEWEX : Pivot Table OLAP Extensions",                     //2060
        "SXTH : PivotTable OLAP Hierarchy",                           //2061
        "SXPIEX : OLAP Page Item Extensions",                         //2062
        "SXVDTEX : View Dimension OLAP Extensions",                   //2063
        "SXVIEWEX9 : Pivot Table Extensions",                         //2064
        NULL,                                                         //2065
        "CONTINUEFRT : Continued  FRT",                               //2066
        "REALTIMEDATA : Real-Time Data (RTD)",                        //2067
        NULL,                                                         //2068
        NULL,                                                         //2069
        NULL,                                                         //2070
        NULL,                                                         //2071
        NULL,                                                         //2072
        NULL,                                                         //2073
        NULL,                                                         //2074
        NULL,                                                         //2075
        NULL,                                                         //2076
        NULL,                                                         //2077
        NULL,                                                         //2078
        NULL,                                                         //2079
        NULL,                                                         //2080
        NULL,                                                         //2081
        NULL,                                                         //2082
        NULL,                                                         //2083
        NULL,                                                         //2084
        NULL,                                                         //2085
        NULL,                                                         //2086
        NULL,                                                         //2087
        NULL,                                                         //2088
        NULL,                                                         //2089
        NULL,                                                         //2090
        NULL,                                                         //2091
        NULL,                                                         //2092
        NULL,                                                         //2093
        NULL,                                                         //2094
        NULL,                                                         //2095
        NULL,                                                         //2096
        NULL,                                                         //2097
        NULL,                                                         //2098
        NULL,                                                         //2099
        NULL,                                                         //2100
        NULL,                                                         //2101
        NULL,                                                         //2102
        NULL,                                                         //2103
        NULL,                                                         //2104
        NULL,                                                         //2105
        NULL,                                                         //2106
        NULL,                                                         //2107
        NULL,                                                         //2108
        NULL,                                                         //2109
        NULL,                                                         //2110
        NULL,                                                         //2111
        NULL,                                                         //2112
        NULL,                                                         //2113
        NULL,                                                         //2114
        NULL,                                                         //2115
        NULL,                                                         //2116
        NULL,                                                         //2117
        NULL,                                                         //2118
        NULL,                                                         //2119
        NULL,                                                         //2120
        NULL,                                                         //2121
        NULL,                                                         //2122
        NULL,                                                         //2123
        NULL,                                                         //2124
        NULL,                                                         //2125
        NULL,                                                         //2126
        NULL,                                                         //2127
        NULL,                                                         //2128
        NULL,                                                         //2129
        NULL,                                                         //2130
        NULL,                                                         //2131
        NULL,                                                         //2132
        NULL,                                                         //2133
        NULL,                                                         //2134
        NULL,                                                         //2135
        NULL,                                                         //2136
        NULL,                                                         //2137
        NULL,                                                         //2138
        NULL,                                                         //2139
        NULL,                                                         //2140
        NULL,                                                         //2141
        NULL,                                                         //2142
        NULL,                                                         //2143
        NULL,                                                         //2144
        NULL,                                                         //2145
        "SHEETEXT : Extra Sheet Info",                                //2146
        "BOOKEXT : Extra Book Info",                                  //2147
        "SXADDL : Pivot Table Additional Info",                       //2148
        "CRASHRECERR : Crash Recovery Error",                         //2149
        "HFPicture : Header / Footer Picture",                        //2150
        "FEATHEADR : Shared Feature Header",                          //2151
        "FEAT : Shared Feature Record",                               //2152
        NULL,                                                         //2153
        "DATALABEXT : Chart Data Label Extension",                    //2154
        "DATALABEXTCONTENTS : Chart Data Label Extension Contents",   //2155
        "CELLWATCH : Cell Watch",                                     //2156
        "FEATINFO : Shared Feature Info Record",                      //2157
        NULL,                                                         //2158
        NULL,                                                         //2159
        NULL,                                                         //2160
        "FEATHEADR11 : Shared Feature Header 11",                     //2161
        "FEAT11 : Shared Feature 11 Record",                          //2162
        "FEATINFO11 : Shared Feature Info 11 Record",                 //2163
        "DROPDOWNOBJIDS : Drop Down Object",                          //2164
        "CONTINUEFRT11 : Continue  FRT 11",                           //2165
        "DCONN : Data Connection",                                    //2166
        "LIST12 : Extra Table Data Introduced in Excel 2007",         //2167
        "FEAT12 : Shared Feature 12 Record",                          //2168
        "CONDFMT12 : Conditional Formatting Range Information 12",    //2169
        "CF12 : Conditional Formatting Condition 12",                 //2170
        "CFEX : Conditional Formatting Extension",                    //2171
        "XFCRC : XF Extensions Checksum",                             //2172
        "XFEXT : XF Extension",                                       //2173
        "EZFILTER12 : AutoFilter Data Introduced in Excel 2007",      //2174
        "CONTINUEFRT12 : Continue FRT 12",                            //2175
        NULL,                                                         //2176
        "SXADDL12 : Additional Workbook Connections Information",     //2177
        NULL,                                                         //2178
        NULL,                                                         //2179
        "MDTINFO : Information about a Metadata Type",                //2180
        "MDXSTR : MDX Metadata String",                               //2181
        "MDXTUPLE : Tuple MDX Metadata",                              //2182
        "MDXSET : Set MDX Metadata",                                  //2183
        "MDXPROP : Member Property MDX Metadata",                     //2184
        "MDXKPI : Key Performance Indicator MDX Metadata",            //2185
        "MDTB : Block of Metadata Records",                           //2186
        "PLV : Page Layout View Settings in Excel 2007",              //2187
        "COMPAT12 : Compatibility Checker 12",                        //2188
        "DXF : Differential XF",                                      //2189
        "TABLESTYLES : Table Styles",                                 //2190
        "TABLESTYLE : Table Style",                                   //2191
        "TABLESTYLEELEMENT : Table Style Element",                    //2192
        NULL,                                                         //2193
        "STYLEEXT : Named Cell Style Extension",                      //2194
        "NAMEPUBLISH : Publish To Excel Server Data for Name",        //2195
        "NAMECMT : Name Comment",                                     //2196
        "SORTDATA12 : Sort Data 12",                                  //2197
        "THEME : Theme",                                              //2198
        "GUIDTYPELIB : VB Project Typelib GUID",                      //2199
        "FNGRP12 : Function Group",                                   //2200
        "NAMEFNGRP12 : Extra Function Group",                         //2201
        "MTRSETTINGS : Multi-Threaded Calculation Settings",          //2202
        "COMPRESSPICTURES : Automatic Picture Compression Mode",      //2203
        "HEADERFOOTER : Header Footer",                               //2204
        NULL,                                                         //2205
        NULL,                                                         //2206
        NULL,                                                         //2207
        NULL,                                                         //2208
        NULL,                                                         //2209
        NULL,                                                         //2210
        "FORCEFULLCALCULATION : Force Full Calculation Settings",     //2211
        NULL,                                                         //2212
        NULL,                                                         //2213
        NULL,                                                         //2214
        NULL,                                                         //2215
        NULL,                                                         //2216
        NULL,                                                         //2217
        NULL,                                                         //2218
        NULL,                                                         //2219
        NULL,                                                         //2220
        NULL,                                                         //2221
        NULL,                                                         //2222
        NULL,                                                         //2223
        NULL,                                                         //2224
        NULL,                                                         //2225
        NULL,                                                         //2226
        NULL,                                                         //2227
        NULL,                                                         //2228
        NULL,                                                         //2229
        NULL,                                                         //2230
        NULL,                                                         //2231
        NULL,                                                         //2232
        NULL,                                                         //2233
        NULL,                                                         //2234
        NULL,                                                         //2235
        NULL,                                                         //2236
        NULL,                                                         //2237
        NULL,                                                         //2238
        NULL,                                                         //2239
        NULL,                                                         //2240
        "LISTOBJ : List Object",                                      //2241
        "LISTFIELD : List Field",                                     //2242
        "LISTDV : List Data Validation",                              //2243
        "LISTCONDFMT : List Conditional Formatting",                  //2244
        "LISTCF : List Cell Formatting",                              //2245
        "FMQRY : Filemaker queries",                                  //2246
        "FMSQRY : File maker queries",                                //2247
        "PLV : Page Layout View in Mac Excel 11",                     //2248
        "LNEXT : Extension information for borders in Mac Office 11", //2249
        "MKREXT : Extension information for markers in Mac Office 11",//2250
};

enum ptg_arg_type {
    ptg_arg_unknown,
    ptg_arg_v,     // regex: (var)*
    ptg_arg_r,     // regex: (ref)*
    ptg_arg_a,     // regex: (var|ref)*
    ptg_arg_va,  // regex: (val)(var|ref)*
    ptg_arg_vr,  // regex: (var)(ref)*
    ptg_arg_av,  // regex: (var|ref)(var)*
    ptg_arg_ra,
    ptg_arg_rv,  // regex: (ref)(var)*
    ptg_arg_rvr,
    ptg_arg_aav,
    ptg_arg_vrv,
    ptg_arg_var,
    ptg_arg_vav,
    ptg_arg_vva,
    ptg_arg_vaav,
    ptg_arg_vvav,
    ptg_arg_vvva,
    ptg_arg_vvvav,
    ptg_arg_vaaav,
    ptg_arg_aaava,
    ptg_arg_vaaava,
    ptg_arg_vaaaaav,
    ptg_arg_vaaaaaav,
    ptg_arg_vvaaaaaav,
    ptg_arg_vvvvvvvavvav,
};

typedef struct __st_ptg_func {
    int32_t     id;
    const char* name;
    int16_t     argc_min;
    int16_t     argc_max;
    int16_t     argt;
} ptg_func_t;

/*
* https://docs.microsoft.com/en-us/openspecs/office_file_formats/ms-xls/00b5dd7d-51ca-4938-b7b7-483fe0e5933b
*/
const ptg_func_t FUNCTIONS[] = {
    ptg_func_t{ 0, "COUNT", 1, 30, ptg_arg_a},
    ptg_func_t{ 1, "IF", 1, 3, ptg_arg_va},
    ptg_func_t{ 2, "ISNA", 1, 1, ptg_arg_v},
    ptg_func_t{ 3, "ISERROR", 1, 1, ptg_arg_v},
    ptg_func_t{ 4, "SUM", 1, 30, ptg_arg_a},
    ptg_func_t{ 5, "AVERAGE", 1, 30, ptg_arg_a},
    ptg_func_t{ 6, "MIN", 1, 30, ptg_arg_a},
    ptg_func_t{ 7, "MAX", 1, 30, ptg_arg_a},
    ptg_func_t{ 8, "ROW", 0, 1, ptg_arg_r},
    ptg_func_t{ 9, "COLUMN", 0, 1, ptg_arg_r},
    ptg_func_t{ 10, "NA", 0, 0, 0 },
    ptg_func_t{ 11, "NPV", 2, 30, ptg_arg_va},
    ptg_func_t{ 12, "STDEV", 1, 30, ptg_arg_a},
    ptg_func_t{ 13, "DOLLAR", 1, 2, ptg_arg_v},
    ptg_func_t{ 14, "FIXED", 1, 3, ptg_arg_v},
    ptg_func_t{ 15, "SIN", 1, 1, ptg_arg_v},
    ptg_func_t{ 16, "COS", 1, 1, ptg_arg_v},
    ptg_func_t{ 17, "TAN", 1, 1, ptg_arg_v},
    ptg_func_t{ 18, "ATAN", 1, 1, ptg_arg_v},
    ptg_func_t{ 19, "PI", 0, 0, 0},
    ptg_func_t{ 20, "SQRT", 1, 1, ptg_arg_v},
    ptg_func_t{ 21, "EXP", 1, 1, ptg_arg_v},
    ptg_func_t{ 22, "LN", 1, 1, ptg_arg_v},
    ptg_func_t{ 23, "LOG10", 1, 1, ptg_arg_v},
    ptg_func_t{ 24, "ABS", 1, 1, ptg_arg_v},
    ptg_func_t{ 25, "INT", 1, 1, ptg_arg_v},
    ptg_func_t{ 26, "SIGN", 1, 1, ptg_arg_v},
    ptg_func_t{ 27, "ROUND", 2, 2, ptg_arg_v},
    ptg_func_t{ 28, "LOOKUP", 2, 3, ptg_arg_va },
    ptg_func_t{ 29, "INDEX", 2, 4, ptg_arg_av},
    ptg_func_t{ 30, "REPT", 2, 2, ptg_arg_v},
    ptg_func_t{ 31, "MID", 3, 3, ptg_arg_v},
    ptg_func_t{ 32, "LEN", 1, 1, ptg_arg_v},
    ptg_func_t{ 33, "VALUE", 1, 1, ptg_arg_v},
    ptg_func_t{ 34, "TRUE", 0, 0, 0},
    ptg_func_t{ 35, "FALSE", 0, 0, 0},
    ptg_func_t{ 36, "AND", 1, 30, ptg_arg_a},
    ptg_func_t{ 37, "OR", 1, 30, ptg_arg_a},
    ptg_func_t{ 38, "NOT", 1, 1, ptg_arg_v},
    ptg_func_t{ 39, "MOD", 2, 2, ptg_arg_v},
    ptg_func_t{ 40, "DCOUNT", 3, 3, ptg_arg_ra},
    ptg_func_t{ 41, "DSUM", 3, 3, ptg_arg_ra},
    ptg_func_t{ 42, "DAVERAGE", 3, 3, ptg_arg_ra},
    ptg_func_t{ 43, "DMIN", 3, 3, ptg_arg_ra},
    ptg_func_t{ 44, "DMAX", 3, 3, ptg_arg_ra},
    ptg_func_t{ 45, "DSTDEV", 3, 3, ptg_arg_ra},
    ptg_func_t{ 46, "VAR", 1, 30, ptg_arg_a},
    ptg_func_t{ 47, "DVAR", 3, 3, ptg_arg_va},
    ptg_func_t{ 48, "TEXT", 2, 2, ptg_arg_v},
    ptg_func_t{ 49, "LINEST", 1, 4, ptg_arg_a}, // linest-params = (ref / val), [(ref / val), *2(ref / val)]
    ptg_func_t{ 50, "TREND", 1, 4, ptg_arg_a}, // trend-params = (ref / val), [(ref / val), [(ref / val), [ref / val]]]
    ptg_func_t{ 51, "LOGEST", 1, 4, ptg_arg_a},
    ptg_func_t{ 52, "GROWTH", 1, 4, ptg_arg_a},
    ptg_func_t{ 53, "GOTO", 1, 1, ptg_arg_r},
    ptg_func_t{ 54, "HALT", 0, 1, ptg_arg_v},
    ptg_func_t{ 55, "RETURN", 0, 1, ptg_arg_a},
    ptg_func_t{ 56, "PV", 3, 5, ptg_arg_v},
    ptg_func_t{ 57, "FV", 3, 5, ptg_arg_v},
    ptg_func_t{ 58, "NPER", 3, 5, ptg_arg_v},
    ptg_func_t{ 59, "PMT", 3, 5, ptg_arg_v},
    ptg_func_t{ 60, "RATE", 3, 6, ptg_arg_v},
    ptg_func_t{ 61, "MIRR", 3, 3, ptg_arg_av},
    ptg_func_t{ 62, "IRR", 1, 2, ptg_arg_av},
    ptg_func_t{ 63, "RAND", 0, 0, 0},
    ptg_func_t{ 64, "MATCH", 2, 3, ptg_arg_va},
    ptg_func_t{ 65, "DATE", 3, 3, ptg_arg_v},
    ptg_func_t{ 66, "TIME", 3, 3, ptg_arg_v},
    ptg_func_t{ 67, "DAY", 1, 1, ptg_arg_v},
    ptg_func_t{ 68, "MONTH", 1, 1, ptg_arg_v},
    ptg_func_t{ 69, "YEAR", 1, 1, ptg_arg_v},
    ptg_func_t{ 70, "WEEKDAY", 1, 2, ptg_arg_v},
    ptg_func_t{ 71, "HOUR", 1, 1, ptg_arg_v},
    ptg_func_t{ 72, "MINUTE", 1, 1, ptg_arg_v},
    ptg_func_t{ 73, "SECOND", 1, 1, ptg_arg_v},
    ptg_func_t{ 74, "NOW", 0, 0, 0},
    ptg_func_t{ 75, "AREAS", 1, 1, ptg_arg_r},
    ptg_func_t{ 76, "ROWS", 1, 1, ptg_arg_a},
    ptg_func_t{ 77, "COLUMNS", 1, 1, ptg_arg_a},
    ptg_func_t{ 78, "OFFSET", 3, 5, ptg_arg_rv},
    ptg_func_t{ 79, "ABSREF", 2, 2, ptg_arg_vr},
    ptg_func_t{ 80, "RELREF", 2, 2, ptg_arg_r},
    ptg_func_t{ 81, "ARGUMENT", 0, 3, ptg_arg_var},
    ptg_func_t{ 82, "SEARCH", 2, 3, ptg_arg_v},
    ptg_func_t{ 83, "TRANSPOSE", 1, 1, ptg_arg_v},
    ptg_func_t{ 84, "ERROR", 0, 2, ptg_arg_va},
    ptg_func_t{ 85, "STEP", 0, 0, 0},
    ptg_func_t{ 86, "TYPE", 1, 1, ptg_arg_v},
    ptg_func_t{ 87, "ECHO", 1, 1, ptg_arg_v},
    ptg_func_t{ 88, "SET.NAME", 1, 2, ptg_arg_va},
    ptg_func_t{ 89, "CALLER", 0, 0, 0},
    ptg_func_t{ 90, "DEREF", 1, 1, ptg_arg_r},
    ptg_func_t{ 91, "WINDOWS", 0, 2, ptg_arg_v},
    ptg_func_t{ 92, "SERIES", 4, 5, ptg_arg_aaava},
    ptg_func_t{ 93, "DOCUMENTS", 0, 2, ptg_arg_v},
    ptg_func_t{ 94, "ACTIVE.CELL", 0, 0, 0},
    ptg_func_t{ 95, "SELECTION", 0, 0, 0},
    ptg_func_t{ 96, "RESULT", 0, 1, ptg_arg_v},
    ptg_func_t{ 97, "ATAN2", 2, 2, ptg_arg_v},
    ptg_func_t{ 98, "ASIN", 1, 1, ptg_arg_v},
    ptg_func_t{ 99, "ACOS", 1, 1, ptg_arg_v},
    ptg_func_t{ 100, "CHOOSE", 2, 30, ptg_arg_va },
    ptg_func_t{ 101, "HLOOKUP", 3, 4, ptg_arg_vaav },
    ptg_func_t{ 102, "VLOOKUP", 3, 4, ptg_arg_vaav },
    ptg_func_t{ 103, "LINKS", 0, 2, ptg_arg_v },
    ptg_func_t{ 104, "INPUT", 1, 7, ptg_arg_v }, // input-params = val, [val, [val, [val, [val, [val, [val]]]]]]
    ptg_func_t{ 105, "ISREF", 1, 1, ptg_arg_a},
    ptg_func_t{ 106, "GET.FORMULA", 1, 1, ptg_arg_a},
    ptg_func_t{ 107, "GET.NAME", 1, 2, ptg_arg_v},
    ptg_func_t{ 108, "SET.VALUE", 2, 2, ptg_arg_rv},
    ptg_func_t{ 109, "LOG", 1, 2, ptg_arg_v },
    ptg_func_t{ 110, "EXEC", 1, 4, ptg_arg_v },
    ptg_func_t{ 111, "CHAR", 1, 1, ptg_arg_v },
    ptg_func_t{ 112, "LOWER", 1, 1, ptg_arg_v },
    ptg_func_t{ 113, "UPPER", 1, 1, ptg_arg_v },
    ptg_func_t{ 114, "PROPER", 1, 1, ptg_arg_v },
    ptg_func_t{ 115, "LEFT", 1, 2, ptg_arg_v },
    ptg_func_t{ 116, "RIGHT", 1, 2, ptg_arg_v },
    ptg_func_t{ 117, "EXACT", 2, 2, ptg_arg_v},
    ptg_func_t{ 118, "TRIM", 1, 1, ptg_arg_v },
    ptg_func_t{ 119, "REPLACE", 4, 4, ptg_arg_v },
    ptg_func_t{ 120, "SUBSTITUTE", 3, 4, ptg_arg_v },
    ptg_func_t{ 121, "CODE", 1, 1, ptg_arg_v },
    ptg_func_t{ 122, "NAMES", 0, 3, ptg_arg_v },
    ptg_func_t{ 123, "DIRECTORY", 0, 1, ptg_arg_v },
    ptg_func_t{ 124, "FIND", 2, 3, ptg_arg_v },
    ptg_func_t{ 125, "CELL", 1, 2, ptg_arg_vr },
    ptg_func_t{ 126, "ISERR", 1, 1, ptg_arg_v },
    ptg_func_t{ 127, "ISTEXT", 1, 1, ptg_arg_v },
    ptg_func_t{ 128, "ISNUMBER", 1, 1, ptg_arg_v },
    ptg_func_t{ 129, "ISBLANK", 1, 1, ptg_arg_v },
    ptg_func_t{ 130, "T", 1, 1, ptg_arg_a },
    ptg_func_t{ 131, "N", 1, 1, ptg_arg_a },
    ptg_func_t{ 132, "FOPEN", 1, 2, ptg_arg_v },
    ptg_func_t{ 133, "FCLOSE", 1, ptg_arg_v },
    ptg_func_t{ 134, "FSIZE", 1, 1, ptg_arg_v },
    ptg_func_t{ 135, "FREADLN", 1, 1, ptg_arg_v },
    ptg_func_t{ 136, "FREAD", 2, 2, ptg_arg_v },
    ptg_func_t{ 137, "FWRITELN", 2, 2, ptg_arg_v },
    ptg_func_t{ 138, "FWRITE", 2, 2, ptg_arg_v },
    ptg_func_t{ 139, "FPOS", 1, 2, ptg_arg_v },
    ptg_func_t{ 140, "DATEVALUE", 1, 1, ptg_arg_v },
    ptg_func_t{ 141, "TIMEVALUE", 1, 1, ptg_arg_v },
    ptg_func_t{ 142, "SLN", 3, 3, ptg_arg_v },
    ptg_func_t{ 143, "SYD", 4, 4, ptg_arg_v },
    ptg_func_t{ 144, "DDB", 4, 5, ptg_arg_v },
    ptg_func_t{ 145, "GET.DEF", 1, 3, ptg_arg_v },
    ptg_func_t{ 146, "REFTEXT", 1, 2, ptg_arg_rv },
    ptg_func_t{ 147, "TEXTREF", 1, 2, ptg_arg_v },
    ptg_func_t{ 148, "INDIRECT", 1, 2, ptg_arg_v },
    ptg_func_t{ 149, "REGISTER", 1, 30, ptg_arg_v }, // register-params = val, [val, [val, [val, [val, [val, [val, [val, [val, [val, *20(val)]]]]]]]]]
    ptg_func_t{ 150, "CALL", 1, 30, ptg_arg_va },
    ptg_func_t{ 151, "ADD.BAR", 0, 1, ptg_arg_v },
    ptg_func_t{ 152, "ADD.MENU", 2, 4, ptg_arg_vaav },
    ptg_func_t{ 153, "ADD.COMMAND", 3, 5, ptg_arg_vaaav },
    ptg_func_t{ 154, "ENABLE.COMMAND", 4, 5, ptg_arg_v },
    ptg_func_t{ 155, "CHECK.COMMAND", 4, 5, ptg_arg_v },
    ptg_func_t{ 156, "RENAME.COMMAND", 4, 5, ptg_arg_v },
    ptg_func_t{ 157, "SHOW.BAR", 0, 1, ptg_arg_v },
    ptg_func_t{ 158, "DELETE.MENU", 2, 3, ptg_arg_v },
    ptg_func_t{ 159, "DELETE.COMMAND", 3, 4, ptg_arg_v },
    ptg_func_t{ 160, "GET.CHART.ITEM", 1, 3, ptg_arg_v },
    ptg_func_t{ 161, "DIALOG.BOX", 1, 1, ptg_arg_a },
    ptg_func_t{ 162, "CLEAN", 1, 1, ptg_arg_v },
    ptg_func_t{ 163, "MDETERM", 1, 1, ptg_arg_v },
    ptg_func_t{ 164, "MINVERSE", 1, 1, ptg_arg_v },
    ptg_func_t{ 165, "MMULT", 2, 2, ptg_arg_v },
    ptg_func_t{ 166, "FILES", 0, 2, ptg_arg_v }, // files-params = *2(val)
    ptg_func_t{ 167, "IPMT", 4, 6, ptg_arg_v },
    ptg_func_t{ 168, "PPMT", 4, 6, ptg_arg_v },
    ptg_func_t{ 169, "COUNTA", 1, 30, ptg_arg_a },
    ptg_func_t{ 170, "CANCEL.KEY", 0, 2, ptg_arg_vr },
    ptg_func_t{ 171, "FOR", 3, 4, ptg_arg_v },
    ptg_func_t{ 172, "WHILE", 1, 1, ptg_arg_v },
    ptg_func_t{ 173, "BREAK", 0, 0, 0 },
    ptg_func_t{ 174, "NEXT", 0, 0, 0 },
    ptg_func_t{ 175, "INITIATE", 2, 2, ptg_arg_v },
    ptg_func_t{ 176, "REQUEST", 2, 2, ptg_arg_v },
    ptg_func_t{ 177, "POKE", 3, 3, ptg_arg_va },
    ptg_func_t{ 178, "EXECUTE", 2, 2, ptg_arg_v },
    ptg_func_t{ 179, "TERMINATE", 1, 1, ptg_arg_v },
    ptg_func_t{ 180, "RESTART", 0, 1, ptg_arg_v },
    ptg_func_t{ 181, "HELP", 0, 1, ptg_arg_v },
    ptg_func_t{ 182, "GET.BAR", 0, 4, ptg_arg_v },
    ptg_func_t{ 183, "PRODUCT", 1, 30, ptg_arg_a },
    ptg_func_t{ 184, "FACT", 1, 1, ptg_arg_v },
    ptg_func_t{ 185, "GET.CELL", 1, 2, ptg_arg_vr },
    ptg_func_t{ 186, "GET.WORKSPACE", 1, 1, ptg_arg_v },
    ptg_func_t{ 187, "GET.WINDOW", 1, 2, ptg_arg_v },
    ptg_func_t{ 188, "GET.DOCUMENT", 1, 2, ptg_arg_v },
    ptg_func_t{ 189, "DPRODUCT", 3, 3, ptg_arg_ra },
    ptg_func_t{ 190, "ISNONTEXT", 1, 1, ptg_arg_v },
    ptg_func_t{ 191, "GET.NOTE", 0, 3, ptg_arg_av },
    ptg_func_t{ 192, "NOTE", 0, 4, ptg_arg_va },
    ptg_func_t{ 193, "STDEVP", 1, 30, ptg_arg_a },
    ptg_func_t{ 194, "VARP", 1, 30, ptg_arg_a },
    ptg_func_t{ 195, "DSTDEVP", 3, 3, ptg_arg_ra },
    ptg_func_t{ 196, "DVARP", 3, 3, ptg_arg_ra },
    ptg_func_t{ 197, "TRUNC", 1, 2, ptg_arg_v },
    ptg_func_t{ 198, "ISLOGICAL", 1, 1, ptg_arg_v },
    ptg_func_t{ 199, "DCOUNTA", 3, 3, ptg_arg_ra },
    ptg_func_t{ 200, "DELETE.BAR", 1, 1, ptg_arg_v },
    ptg_func_t{ 201, "UNREGISTER", 1, 1, ptg_arg_v },
    ptg_func_t{ 202, nullptr, 0, 0, 0 },
    ptg_func_t{ 203, nullptr, 0, 0, 0 },
    ptg_func_t{ 204, "USDOLLAR", 1, 2, ptg_arg_v },
    ptg_func_t{ 205, "FINDB", 2, 3, ptg_arg_v },
    ptg_func_t{ 206, "SEARCHB", 2, 3, ptg_arg_v },
    ptg_func_t{ 207, "REPLACEB", 4, 4, ptg_arg_v },
    ptg_func_t{ 208, "LEFTB", 1, 2, ptg_arg_v },
    ptg_func_t{ 209, "RIGHTB", 1, 2, ptg_arg_v },
    ptg_func_t{ 210, "MIDB", 3, 3, ptg_arg_v },
    ptg_func_t{ 211, "LENB", 1, 1, ptg_arg_v },
    ptg_func_t{ 212, "ROUNDUP", 2, 2, ptg_arg_v },
    ptg_func_t{ 213, "ROUNDDOWN", 2, 2, ptg_arg_v },
    ptg_func_t{ 214, "ASC", 1, 1, ptg_arg_v },
    ptg_func_t{ 215, "DBCS", 1, 1, ptg_arg_v },
    ptg_func_t{ 216, "RANK", 2, 3, ptg_arg_vrv},
    ptg_func_t{ 217, nullptr, 0, 0, 0 },
    ptg_func_t{ 218, nullptr, 0, 0, 0 },
    ptg_func_t{ 219, "ADDRESS", 2, 5, ptg_arg_v },
    ptg_func_t{ 220, "DAYS360", 2, 3, ptg_arg_v },
    ptg_func_t{ 221, "TODAY", 0, 0, 0 },
    ptg_func_t{ 222, "VDB", 5, 7, ptg_arg_v },
    ptg_func_t{ 223, "ELSE", 0, 0, 0 },
    ptg_func_t{ 224, "ELSE.IF", 1, 1, ptg_arg_v },
    ptg_func_t{ 225, "END.IF", 0, 0, 0 },
    ptg_func_t{ 226, "FOR.CELL", 1, 3, ptg_arg_va },
    ptg_func_t{ 227, "MEDIAN", 1, 30, ptg_arg_a },
    ptg_func_t{ 228, "SUMPRODUCT", 1, 30, ptg_arg_v },
    ptg_func_t{ 229, "SINH", 1, 1, ptg_arg_v },
    ptg_func_t{ 230, "COSH", 1, 1, ptg_arg_v },
    ptg_func_t{ 231, "TANH", 1, 1, ptg_arg_v },
    ptg_func_t{ 232, "ASINH", 1, 1, ptg_arg_v },
    ptg_func_t{ 233, "ACOSH", 1, 1, ptg_arg_v },
    ptg_func_t{ 234, "ATANH", 1, 1, ptg_arg_v },
    ptg_func_t{ 235, "DGET", 3, 3, ptg_arg_ra },
    ptg_func_t{ 236, "CREATE.OBJECT", 2, 11, ptg_arg_va },
    ptg_func_t{ 237, "VOLATILE", 0, 1, ptg_arg_v },
    ptg_func_t{ 238, "LAST.ERROR", 0, 0, 0 },
    ptg_func_t{ 239, "CUSTOM.UNDO", 0, 2, ptg_arg_v },
    ptg_func_t{ 240, "CUSTOM.REPEAT", 0, 3, ptg_arg_v },
    ptg_func_t{ 241, "FORMULA.CONVERT", 2, 5, ptg_arg_va },
    ptg_func_t{ 242, "GET.LINK.INFO", 2, 4, ptg_arg_v },
    ptg_func_t{ 243, "TEXT.BOX", 1, 4, ptg_arg_v },
    ptg_func_t{ 244, "INFO", 1, 1, ptg_arg_v },
    ptg_func_t{ 245, "GROUP", 0, 0, 0 },
    ptg_func_t{ 246, "GET.OBJECT", 1, 5, ptg_arg_v },
    ptg_func_t{ 247, "DB", 4, 5, ptg_arg_v },
    ptg_func_t{ 248, "PAUSE", 0, 1, ptg_arg_v },
    ptg_func_t{ 249, nullptr, 0, 0, 0 },
    ptg_func_t{ 250, nullptr, 0, 0, 0 },
    ptg_func_t{ 251, "RESUME", 0, 1, ptg_arg_v },
    ptg_func_t{ 252, "FREQUENCY", 2, 2, ptg_arg_a },
    ptg_func_t{ 253, "ADD.TOOLBAR", 0, 2, ptg_arg_v },
    ptg_func_t{ 254, "DELETE.TOOLBAR", 1, 1, ptg_arg_v },
    ptg_func_t{ 255, "User Defined Function", 1, 30, ptg_arg_a },
    ptg_func_t{ 256, "RESET.TOOLBAR", 1, 1, ptg_arg_v },
    ptg_func_t{ 257, "EVALUATE", 1, 1, ptg_arg_v },
    ptg_func_t{ 258, "GET.TOOLBAR", 1, 2, ptg_arg_v },
    ptg_func_t{ 259, "GET.TOOL", 1, 3, ptg_arg_v },
    ptg_func_t{ 260, "SPELLING.CHECK", 1, 3, ptg_arg_v },
    ptg_func_t{ 261, "ERROR.TYPE", 1, 1, ptg_arg_v },
    ptg_func_t{ 262, "APP.TITLE", 0, 1, ptg_arg_v },
    ptg_func_t{ 263, "WINDOW.TITLE", 0, 1, ptg_arg_v },
    ptg_func_t{ 264, "SAVE.TOOLBAR", 0, 2, ptg_arg_v },
    ptg_func_t{ 265, "ENABLE.TOOL", 3, 3, ptg_arg_v },
    ptg_func_t{ 266, "PRESS.TOOL", 3, 3, ptg_arg_v },
    ptg_func_t{ 267, "REGISTER.ID", 2, 3, ptg_arg_v },
    ptg_func_t{ 268, "GET.WORKBOOK", 1, 2, ptg_arg_v },
    ptg_func_t{ 269, "AVEDEV", 1, 30, ptg_arg_a },
    ptg_func_t{ 270, "BETADIST", 3, 5, ptg_arg_v },
    ptg_func_t{ 271, "GAMMALN", 1, 1, ptg_arg_v },
    ptg_func_t{ 272, "BETAINV", 3, 5, ptg_arg_v },
    ptg_func_t{ 273, "BINOMDIST", 4, 4, ptg_arg_v },
    ptg_func_t{ 274, "CHIDIST", 2, 2, ptg_arg_v },
    ptg_func_t{ 275, "CHIINV", 2, 2, ptg_arg_v },
    ptg_func_t{ 276, "COMBIN", 2, 2, ptg_arg_v },
    ptg_func_t{ 277, "CONFIDENCE", 3, 3, ptg_arg_v },
    ptg_func_t{ 278, "CRITBINOM", 3, 3, ptg_arg_v },
    ptg_func_t{ 279, "EVEN", 1, 1, ptg_arg_v },
    ptg_func_t{ 280, "EXPONDIST", 3, 3, ptg_arg_v },
    ptg_func_t{ 281, "FDIST", 3, 3, ptg_arg_v },
    ptg_func_t{ 282, "FINV", 3, 3, ptg_arg_v },
    ptg_func_t{ 283, "FISHER", 1, 1, ptg_arg_v },
    ptg_func_t{ 284, "FISHERINV", 1, 1, ptg_arg_v },
    ptg_func_t{ 285, "FLOOR", 2, 2, ptg_arg_v },
    ptg_func_t{ 286, "GAMMADIST", 4, 4, ptg_arg_v },
    ptg_func_t{ 287, "GAMMAINV", 3, 3, ptg_arg_v },
    ptg_func_t{ 288, "CEILING", 2, 2, ptg_arg_v },
    ptg_func_t{ 289, "HYPGEOMDIST", 4, 4, ptg_arg_v },
    ptg_func_t{ 290, "LOGNORMDIST", 3, 3, ptg_arg_v },
    ptg_func_t{ 291, "LOGINV", 3, 3, ptg_arg_v },
    ptg_func_t{ 292, "NEGBINOMDIST", 3, 3, ptg_arg_v },
    ptg_func_t{ 293, "NORMDIST", 4, 4, ptg_arg_v },
    ptg_func_t{ 294, "NORMSDIST", 1, 1, ptg_arg_v },
    ptg_func_t{ 295, "NORMINV", 3, 3, ptg_arg_v },
    ptg_func_t{ 296, "NORMSINV", 1, 1, ptg_arg_v },
    ptg_func_t{ 297, "STANDARDIZE", 3, 3, ptg_arg_v },
    ptg_func_t{ 298, "ODD", 1, 1, ptg_arg_v },
    ptg_func_t{ 299, "PERMUT", 2, 2, ptg_arg_v },
    ptg_func_t{ 300, "POISSON", 3, 3, ptg_arg_v },
    ptg_func_t{ 301, "TDIST", 3, 3, ptg_arg_v },
    ptg_func_t{ 302, "WEIBULL", 4, 4, ptg_arg_v },
    ptg_func_t{ 303, "SUMXMY2", 2, 2, ptg_arg_v },
    ptg_func_t{ 304, "SUMX2MY2", 2, 2, ptg_arg_v },
    ptg_func_t{ 305, "SUMX2PY2", 2, 2, ptg_arg_v },
    ptg_func_t{ 306, "CHITEST", 2, 2, ptg_arg_v },
    ptg_func_t{ 307, "CORREL", 2, 2, ptg_arg_v },
    ptg_func_t{ 308, "COVAR", 2, 2, ptg_arg_v },
    ptg_func_t{ 309, "FORECAST", 3, 3, ptg_arg_v },
    ptg_func_t{ 310, "FTEST", 2, 2, ptg_arg_v },
    ptg_func_t{ 311, "INTERCEPT", 2, 2, ptg_arg_v },
    ptg_func_t{ 312, "PEARSON", 2, 2, ptg_arg_v },
    ptg_func_t{ 313, "RSQ", 2, 2, ptg_arg_v },
    ptg_func_t{ 314, "STEYX", 2, 2, ptg_arg_v },
    ptg_func_t{ 315, "SLOPE", 2, 2, ptg_arg_v },
    ptg_func_t{ 316, "TTEST", 4, 4, ptg_arg_v },
    ptg_func_t{ 317, "PROB", 3, 4, ptg_arg_v },
    ptg_func_t{ 318, "DEVSQ", 1, 30, ptg_arg_a },
    ptg_func_t{ 319, "GEOMEAN", 1, 30, ptg_arg_a },
    ptg_func_t{ 320, "HARMEAN", 1, 30, ptg_arg_a },
    ptg_func_t{ 321, "SUMSQ", 1, 30, ptg_arg_a },
    ptg_func_t{ 322, "KURT", 1, 30, ptg_arg_a },
    ptg_func_t{ 323, "SKEW", 1, 30, ptg_arg_a },
    ptg_func_t{ 324, "ZTEST", 2, 3, ptg_arg_av },
    ptg_func_t{ 325, "LARGE", 2, 2, ptg_arg_av },
    ptg_func_t{ 326, "SMALL", 2, 2, ptg_arg_av },
    ptg_func_t{ 327, "QUARTILE", 2, 2, ptg_arg_av },
    ptg_func_t{ 328, "PERCENTILE", 2, 2, ptg_arg_av },
    ptg_func_t{ 329, "PERCENTRANK", 2, 3, ptg_arg_av },
    ptg_func_t{ 330, "MODE", 1, 30, ptg_arg_v },
    ptg_func_t{ 331, "TRIMMEAN", 2, 2, ptg_arg_av },
    ptg_func_t{ 332, "TINV", 2, 2, ptg_arg_v },
    ptg_func_t{ 333, nullptr, 0, 0, 0 },
    ptg_func_t{ 334, "MOVIE.COMMAND", 3, 4, ptg_arg_v },
    ptg_func_t{ 335, "GET.MOVIE", 2, 3, ptg_arg_v },
    ptg_func_t{ 336, "CONCATENATE", 1, 30, ptg_arg_v },
    ptg_func_t{ 337, "POWER", 2, 2, ptg_arg_v },
    ptg_func_t{ 338, "PIVOT.ADD.DATA", 2, 9, ptg_arg_v },
    ptg_func_t{ 339, "GET.PIVOT.TABLE", 1, 2, ptg_arg_v },
    ptg_func_t{ 340, "GET.PIVOT.FIELD", 1, 3, ptg_arg_v },
    ptg_func_t{ 341, "GET.PIVOT.ITEM", 1, 4, ptg_arg_v },
    ptg_func_t{ 342, "RADIANS", 1, 1, ptg_arg_v },
    ptg_func_t{ 343, "DEGREES", 1, 1, ptg_arg_v },
    ptg_func_t{ 344, "SUBTOTAL", 2, 30, ptg_arg_vr },
    ptg_func_t{ 345, "SUMIF", 2, 3, ptg_arg_rvr },
    ptg_func_t{ 346, "COUNTIF", 2, 2, ptg_arg_rv},
    ptg_func_t{ 347, "COUNTBLANK", 1, 1, ptg_arg_r },
    ptg_func_t{ 348, "SCENARIO.GET", 1, 2, ptg_arg_rv },
    ptg_func_t{ 349, "OPTIONS.LISTS.GET", 1, 1, ptg_arg_v },
    ptg_func_t{ 350, "ISPMT", 4, 4, ptg_arg_v },
    ptg_func_t{ 351, "DATEDIF", 3, 3, ptg_arg_v },
    ptg_func_t{ 352, "DATESTRING", 1, 1, ptg_arg_v },
    ptg_func_t{ 353, "NUMBERSTRING", 2, 2, ptg_arg_v },
    ptg_func_t{ 354, "ROMAN", 1, 2, ptg_arg_v },
    ptg_func_t{ 355, "OPEN.DIALOG", 0, 4, ptg_arg_v },
    ptg_func_t{ 356, "SAVE.DIALOG", 0, 5, ptg_arg_v },
    ptg_func_t{ 357, "VIEW.GET", 1, 2, ptg_arg_v },
    ptg_func_t{ 358, "GETPIVOTDATA", 2, 17, ptg_arg_aav },
    ptg_func_t{ 359, "HYPERLINK", 1, 2, ptg_arg_v },
    ptg_func_t{ 360, "PHONETIC", 1, 1, ptg_arg_r },
    ptg_func_t{ 361, "AVERAGEA", 1, 30, ptg_arg_a },
    ptg_func_t{ 362, "MAXA", 1, 30, ptg_arg_a },
    ptg_func_t{ 363, "MINA", 1, 30, ptg_arg_a },
    ptg_func_t{ 364, "STDEVPA", 1, 30, ptg_arg_a },
    ptg_func_t{ 365, "VARPA", 1, 30, ptg_arg_a },
    ptg_func_t{ 366, "STDEVA", 1, 30, ptg_arg_a },
    ptg_func_t{ 367, "VARA", 1, 30, ptg_arg_a },
    ptg_func_t{ 368, "BAHTTEXT", 1, 1, ptg_arg_v },
    ptg_func_t{ 369, "THAIDAYOFWEEK", 1, 1, ptg_arg_v },
    ptg_func_t{ 370, "THAIDIGIT", 1, 1, ptg_arg_v },
    ptg_func_t{ 371, "THAIMONTHOFYEAR", 1, 1, ptg_arg_v },
    ptg_func_t{ 372, "THAINUMSOUND", 1, 1, ptg_arg_v },
    ptg_func_t{ 373, "THAINUMSTRING", 1, 1, ptg_arg_v },
    ptg_func_t{ 374, "THAISTRINGLENGTH", 1, 1, ptg_arg_v },
    ptg_func_t{ 375, "ISTHAIDIGIT", 1, 1, ptg_arg_v },
    ptg_func_t{ 376, "ROUNDBAHTDOWN", 1, 1, ptg_arg_v },
    ptg_func_t{ 377, "ROUNDBAHTUP", 1, 1, ptg_arg_v },
    ptg_func_t{ 378, "THAIYEAR", 1, 1, ptg_arg_v },
    ptg_func_t{ 379, "RTD", 3, 30, ptg_arg_v },
};

/*
* https://docs.microsoft.com/en-us/openspecs/office_file_formats/ms-xls/0b8acba5-86d2-4854-836e-0afaee743d44
*/
const ptg_func_t EXTENDED_FUNCTIONS[] = {
    ptg_func_t{ 0  , "BEEP", 0, 1, ptg_arg_v }, // 32768
    ptg_func_t{ 1  , "OPEN", 0, 17, ptg_arg_v },
    ptg_func_t{ 2  , "OPEN.LINKS", 0, 15, ptg_arg_v },
    ptg_func_t{ 3  , "CLOSE.ALL", 0, 0, 0 },
    ptg_func_t{ 4  , "SAVE", 0, 0, 0 },
    ptg_func_t{ 5  , "SAVE.AS", 0, 7, ptg_arg_v },
    ptg_func_t{ 6  , "FILE.DELETE", 0, 1, ptg_arg_v },
    ptg_func_t{ 7  , "PAGE.SETUP", 0, 30, ptg_arg_v },
    ptg_func_t{ 8  , "PRINT", 0, 17, ptg_arg_v },
    ptg_func_t{ 9  , "PRINTER.SETUP", 0, 1, ptg_arg_v },
    ptg_func_t{ 10 , "QUIT", 0, 0, 0 },
    ptg_func_t{ 11 , "NEW.WINDOW", 0, 0, 0 },
    ptg_func_t{ 12 , "ARRANGE.ALL", 0, 4, ptg_arg_v },
    ptg_func_t{ 13 , "WINDOW.SIZE", 0, 3, ptg_arg_v },
    ptg_func_t{ 14 , "WINDOW.MOVE", 0, 3, ptg_arg_v },
    ptg_func_t{ 15 , "FULL", 0, 1, ptg_arg_v },
    ptg_func_t{ 16 , "CLOSE", 0, 2, ptg_arg_v },
    ptg_func_t{ 17 , "RUN", 0, 2, ptg_arg_av },
    ptg_func_t{ 18 , nullptr, 0, 0, 0 },
    ptg_func_t{ 19 , nullptr, 0, 0, 0 },
    ptg_func_t{ 20 , nullptr, 0, 0, 0 },
    ptg_func_t{ 21 , nullptr, 0, 0, 0 },
    ptg_func_t{ 22 , "SET.PRINT.AREA", 0, 1, ptg_arg_a },
    ptg_func_t{ 23 , "SET.PRINT.TITLES", 0, 2, ptg_arg_a },
    ptg_func_t{ 24 , "SET.PAGE.BREAK", 0, 0, 0 },
    ptg_func_t{ 25 , "REMOVE.PAGE.BREAK", 0, 2, ptg_arg_v },
    ptg_func_t{ 26 , "FONT", 0, 2, ptg_arg_v },
    ptg_func_t{ 27 , "DISPLAY", 0, 9, ptg_arg_v },
    ptg_func_t{ 28 , "PROTECT.DOCUMENT", 0, 7, ptg_arg_v },
    ptg_func_t{ 29 , "PRECISION", 0, 1, ptg_arg_v },
    ptg_func_t{ 30 , "A1.R1C1", 0, 1, ptg_arg_v },
    ptg_func_t{ 31 , "CALCULATE.NOW", 0, 0, 0 },
    ptg_func_t{ 32 , "CALCULATION", 0, 11, ptg_arg_v },
    ptg_func_t{ 33 , nullptr, 0, 0, 0 },
    ptg_func_t{ 34 , "DATA.FIND", 0, 1, ptg_arg_v },
    ptg_func_t{ 35 , "EXTRACT", 0, 1, ptg_arg_v },
    ptg_func_t{ 36 , "DATA.DELETE", 0, 0, 0 },
    ptg_func_t{ 37 , "SET.DATABASE", 0, 0, 0 },
    ptg_func_t{ 38 , "SET.CRITERIA", 0, 0, 0 },
    ptg_func_t{ 39 , "SORT", 0, 17, ptg_arg_vaaaaaav },
    ptg_func_t{ 40 , "DATA.SERIES", 0, 6, ptg_arg_v },
    ptg_func_t{ 41 , "TABLE", 0, 2, ptg_arg_a },
    ptg_func_t{ 42 , "FORMAT.NUMBER", 0, 1, ptg_arg_v },
    ptg_func_t{ 43 , "ALIGNMENT", 0, 10, ptg_arg_v },
    ptg_func_t{ 44 , "STYLE", 0, 2, ptg_arg_v },
    ptg_func_t{ 45 , "BORDER", 0, 27, ptg_arg_v },
    ptg_func_t{ 46 , "CELL.PROTECTION", 0, 2, ptg_arg_v },
    ptg_func_t{ 47 , "COLUMN.WIDTH", 0, 5, ptg_arg_va },
    ptg_func_t{ 48 , "UNDO", 0, 0, 0 },
    ptg_func_t{ 49 , "CUT", 0, 2, ptg_arg_a },
    ptg_func_t{ 50 , "COPY", 0, 2, ptg_arg_a },
    ptg_func_t{ 51 , "PASTE", 0, 1, ptg_arg_a },
    ptg_func_t{ 52 , "CLEAR", 0, 1, ptg_arg_v },
    ptg_func_t{ 53 , "PASTE.SPECIAL", 0, 7, ptg_arg_v },
    ptg_func_t{ 54 , "EDIT.DELETE", 0, 1, ptg_arg_v },
    ptg_func_t{ 55 , "INSERT", 0, 2, ptg_arg_v },
    ptg_func_t{ 56 , "FILL.RIGHT", 0, 0, 0 },
    ptg_func_t{ 57 , "FILL.DOWN", 0, 0, 0 },
    ptg_func_t{ 58 , nullptr, 0, 0, 0 },
    ptg_func_t{ 59 , nullptr, 0, 0, 0 },
    ptg_func_t{ 60 , nullptr, 0, 0, 0 },
    ptg_func_t{ 61 , "DEFINE.NAME", 0, 7, ptg_arg_vaaaaav },
    ptg_func_t{ 62 , "CREATE.NAMES", 0, 4, ptg_arg_v },
    ptg_func_t{ 63 , "FORMULA.GOTO", 0, 2, ptg_arg_av },
    ptg_func_t{ 64 , "FORMULA.FIND", 0, 12, ptg_arg_v },
    ptg_func_t{ 65 , "SELECT.LAST.CELL", 0, 0, 0 },
    ptg_func_t{ 66 , "SHOW.ACTIVE.CELL", 0, 0, 0 },
    ptg_func_t{ 67 , "GALLERY.AREA", 0, 2, ptg_arg_v },
    ptg_func_t{ 68 , "GALLERY.BAR", 0, 2, ptg_arg_v },
    ptg_func_t{ 69 , "GALLERY.COLUMN", 0, 2, ptg_arg_v },
    ptg_func_t{ 70 , "GALLERY.LINE", 0, 2, ptg_arg_v },
    ptg_func_t{ 71 , "GALLERY.PIE", 0, 2, ptg_arg_v },
    ptg_func_t{ 72 , "GALLERY.SCATTER", 0, 2, ptg_arg_v },
    ptg_func_t{ 73 , "COMBINATION", 0, 1, ptg_arg_v },
    ptg_func_t{ 74 , "PREFERRED", 0, 0, 0 },
    ptg_func_t{ 75 , "ADD.OVERLAY", 0, 0, 0 },
    ptg_func_t{ 76 , "GRIDLINES", 0, 7, ptg_arg_v },
    ptg_func_t{ 77 , "SET.PREFERRED", 0, 1, ptg_arg_v },
    ptg_func_t{ 78 , "AXES", 0, 6, ptg_arg_v },
    ptg_func_t{ 79 , "LEGEND", 0, 1, ptg_arg_v },
    ptg_func_t{ 80 , "ATTACH.TEXT", 0, 3, ptg_arg_v },
    ptg_func_t{ 81 , "ADD.ARROW", 0, 0, 0 },
    ptg_func_t{ 82 , "SELECT.CHART", 0, 0, 0 },
    ptg_func_t{ 83 , "SELECT.PLOT.AREA", 0, 0, 0 },
    ptg_func_t{ 84 , "PATTERNS", 0, 13, ptg_arg_v },
    ptg_func_t{ 85 , "MAIN.CHART", 0, 10, ptg_arg_v },
    ptg_func_t{ 86 , "OVERLAY", 0, 12, ptg_arg_v },
    ptg_func_t{ 87 , "SCALE", 0, 10, ptg_arg_v },
    ptg_func_t{ 88 , "FORMAT.LEGEND", 0, 1, ptg_arg_v },
    ptg_func_t{ 89 , "FORMAT.TEXT", 0, 11, ptg_arg_v },
    ptg_func_t{ 90 , "EDIT.REPEAT", 0, 0, 0 },
    ptg_func_t{ 91 , "PARSE", 0, 2, ptg_arg_va },
    ptg_func_t{ 92 , "JUSTIFY", 0, 0, 0 },
    ptg_func_t{ 93 , "HIDE", 0, 0, 0 },
    ptg_func_t{ 94 , "UNHIDE", 0, 1, ptg_arg_v },
    ptg_func_t{ 95 , "WORKSPACE", 0, 16, ptg_arg_v },
    ptg_func_t{ 96 , "FORMULA", 0, 2, ptg_arg_va },
    ptg_func_t{ 97 , "FORMULA.FILL", 0, 2, ptg_arg_va },
    ptg_func_t{ 98 , "FORMULA.ARRAY", 0, 2, ptg_arg_va },
    ptg_func_t{ 99 , "DATA.FIND.NEXT", 0, 0, 0 },
    ptg_func_t{ 100, "DATA.FIND.PREV", 0, 0, 0 },
    ptg_func_t{ 101, "FORMULA.FIND.NEXT", 0, 0, 0 },
    ptg_func_t{ 102, "FORMULA.FIND.PREV", 0, 0, 0 },
    ptg_func_t{ 103, "ACTIVATE", 0, 2, ptg_arg_v },
    ptg_func_t{ 104, "ACTIVATE.NEXT", 0, 1, ptg_arg_v },
    ptg_func_t{ 105, "ACTIVATE.PREV", 0, 1, ptg_arg_v },
    ptg_func_t{ 106, "UNLOCKED.NEXT", 0, 0, 0 },
    ptg_func_t{ 107, "UNLOCKED.PREV", 0, 0, 0 },
    ptg_func_t{ 108, "COPY.PICTURE", 0, 3, ptg_arg_v },
    ptg_func_t{ 109, "SELECT", 0, 2, ptg_arg_a },
    ptg_func_t{ 110, "DELETE.NAME", 0, 1, ptg_arg_v },
    ptg_func_t{ 111, "DELETE.FORMAT", 0, 1, ptg_arg_v },
    ptg_func_t{ 112, "VLINE", 0, 1, ptg_arg_v },
    ptg_func_t{ 113, "HLINE", 0, 1, ptg_arg_v },
    ptg_func_t{ 114, "VPAGE", 0, 1, ptg_arg_v },
    ptg_func_t{ 115, "HPAGE", 0, 1, ptg_arg_v },
    ptg_func_t{ 116, "VSCROLL", 0, 2, ptg_arg_v },
    ptg_func_t{ 117, "HSCROLL", 0, 2, ptg_arg_v },
    ptg_func_t{ 118, "ALERT", 0, 3, ptg_arg_v },
    ptg_func_t{ 119, "NEW", 0, 3, ptg_arg_v },
    ptg_func_t{ 120, "CANCEL.COPY", 0, 1, ptg_arg_v },
    ptg_func_t{ 121, "SHOW.CLIPBOARD", 0, 0, 0 },
    ptg_func_t{ 122, "MESSAGE", 0, 2, ptg_arg_v },
    ptg_func_t{ 123, nullptr, 0, 0, 0 },
    ptg_func_t{ 124, "PASTE.LINK", 0, 0, 0 },
    ptg_func_t{ 125, "APP.ACTIVATE", 0, 2, ptg_arg_v },
    ptg_func_t{ 126, "DELETE.ARROW", 0, 0, 0 },
    ptg_func_t{ 127, "ROW.HEIGHT", 0, 4, ptg_arg_va },
    ptg_func_t{ 128, "FORMAT.MOVE", 0, 3, ptg_arg_va },
    ptg_func_t{ 129, "FORMAT.SIZE", 0, 3, ptg_arg_va },
    ptg_func_t{ 130, "FORMULA.REPLACE", 0, 11, ptg_arg_v },
    ptg_func_t{ 131, "SEND.KEYS", 0, 2, ptg_arg_v },
    ptg_func_t{ 132, "SELECT.SPECIAL", 0, 3, ptg_arg_v },
    ptg_func_t{ 133, "APPLY.NAMES", 0, 7, ptg_arg_v },
    ptg_func_t{ 134, "REPLACE.FONT", 0, 10, ptg_arg_v },
    ptg_func_t{ 135, "FREEZE.PANES", 0, 3, ptg_arg_v },
    ptg_func_t{ 136, "SHOW.INFO", 0, 1, ptg_arg_v },
    ptg_func_t{ 137, "SPLIT", 0, 2, ptg_arg_v },
    ptg_func_t{ 138, "ON.WINDOW", 0, 2, ptg_arg_v },
    ptg_func_t{ 139, "ON.DATA", 0, 2, ptg_arg_v },
    ptg_func_t{ 140, "DISABLE.INPUT", 0, 1, ptg_arg_v },
    ptg_func_t{ 141, nullptr, 0, 0, 0 },
    ptg_func_t{ 142, "OUTLINE", 0, 4, ptg_arg_v },
    ptg_func_t{ 143, "LIST.NAMES", 0, 0, 0 },
    ptg_func_t{ 144, "FILE.CLOSE", 0, 2, ptg_arg_v },
    ptg_func_t{ 145, "SAVE.WORKBOOK", 0, 6, ptg_arg_v },
    ptg_func_t{ 146, "DATA.FORM", 0, 0, 0 },
    ptg_func_t{ 147, "COPY.CHART", 0, 1, ptg_arg_v },
    ptg_func_t{ 148, "ON.TIME", 0, 4, ptg_arg_v },
    ptg_func_t{ 149, "WAIT", 0, 1, ptg_arg_v },
    ptg_func_t{ 150, "FORMAT.FONT", 0, 15, ptg_arg_v },
    ptg_func_t{ 151, "FILL.UP", 0, 0, 0 },
    ptg_func_t{ 152, "FILL.LEFT", 0, 0, 0 },
    ptg_func_t{ 153, "DELETE.OVERLAY", 0, 0, 0 },
    ptg_func_t{ 154, nullptr, 0, 0, 0 },
    ptg_func_t{ 155, "SHORT.MENUS", 0, 1, ptg_arg_v },
    ptg_func_t{ 156, nullptr, 0, 0, 0 },
    ptg_func_t{ 157, nullptr, 0, 0, 0 },
    ptg_func_t{ 158, nullptr, 0, 0, 0 },
    ptg_func_t{ 159, "SET.UPDATE.STATUS", 0, 3, ptg_arg_v },
    ptg_func_t{ 160, nullptr, 0, 0, 0 },
    ptg_func_t{ 161, "COLOR.PALETTE", 0, 1, ptg_arg_v },
    ptg_func_t{ 162, "DELETE.STYLE", 0, 1, ptg_arg_v },
    ptg_func_t{ 163, "WINDOW.RESTORE", 0, 1, ptg_arg_v },
    ptg_func_t{ 164, "WINDOW.MAXIMIZE", 0, 1, ptg_arg_v },
    ptg_func_t{ 165, nullptr, 0, 0, 0 },
    ptg_func_t{ 166, "CHANGE.LINK", 0, 3, ptg_arg_v },
    ptg_func_t{ 167, "CALCULATE.DOCUMENT", 0, 0, 0 },
    ptg_func_t{ 168, "ON.KEY", 0, 2, ptg_arg_v },
    ptg_func_t{ 169, "APP.RESTORE", 0, 0, 0 },
    ptg_func_t{ 170, "APP.MOVE", 0, 2, ptg_arg_v },
    ptg_func_t{ 171, "APP.SIZE", 0, 2, ptg_arg_v },
    ptg_func_t{ 172, "APP.MINIMIZE", 0, 0, 0 },
    ptg_func_t{ 173, "APP.MAXIMIZE", 0, 0, 0 },
    ptg_func_t{ 174, "BRING.TO.FRONT", 0, 0, 0 },
    ptg_func_t{ 175, "SEND.TO.BACK", 0, 0, 0 },
    ptg_func_t{ 176, nullptr, 0, 0, 0 },
    ptg_func_t{ 177, nullptr, 0, 0, 0 },
    ptg_func_t{ 178, nullptr, 0, 0, 0 },
    ptg_func_t{ 179, nullptr, 0, 0, 0 },
    ptg_func_t{ 180, nullptr, 0, 0, 0 },
    ptg_func_t{ 181, nullptr, 0, 0, 0 },
    ptg_func_t{ 182, nullptr, 0, 0, 0 },
    ptg_func_t{ 183, nullptr, 0, 0, 0 },
    ptg_func_t{ 184, nullptr, 0, 0, 0 },
    ptg_func_t{ 185, "MAIN.CHART.TYPE", 0, 1, ptg_arg_v },
    ptg_func_t{ 186, "OVERLAY.CHART.TYPE", 0, 1, ptg_arg_v },
    ptg_func_t{ 187, "SELECT.END", 0, 1, ptg_arg_v },
    ptg_func_t{ 188, "OPEN.MAIL", 0, 2, ptg_arg_v },
    ptg_func_t{ 189, "SEND.MAIL", 0, 3, ptg_arg_av },
    ptg_func_t{ 190, "STANDARD.FONT", 0, 9, ptg_arg_v },
    ptg_func_t{ 191, "CONSOLIDATE", 0, 5, ptg_arg_v },
    ptg_func_t{ 192, "SORT.SPECIAL", 0, 14, ptg_arg_vvaaaaaav },
    ptg_func_t{ 193, "GALLERY.3D.AREA", 0, 1, ptg_arg_v },
    ptg_func_t{ 194, "GALLERY.3D.COLUMN", 0, 1, ptg_arg_v },
    ptg_func_t{ 195, "GALLERY.3D.LINE", 0, 1, ptg_arg_v },
    ptg_func_t{ 196, "GALLERY.3D.PIE", 0, 1, ptg_arg_v },
    ptg_func_t{ 197, "VIEW.3D", 0, 6, ptg_arg_v },
    ptg_func_t{ 198, "GOAL.SEEK", 0, 3, ptg_arg_a },
    ptg_func_t{ 199, "WORKGROUP", 0, 1, ptg_arg_v },
    ptg_func_t{ 200, "FILL.GROUP", 0, 1, ptg_arg_v },
    ptg_func_t{ 201, "UPDATE.LINK", 0, 2, ptg_arg_v },
    ptg_func_t{ 202, "PROMOTE", 0, 1, ptg_arg_v },
    ptg_func_t{ 203, "DEMOTE", 0, 1, ptg_arg_v },
    ptg_func_t{ 204, "SHOW.DETAIL", 0, 4, ptg_arg_v },
    ptg_func_t{ 205, nullptr, 0, 0, 0 },
    ptg_func_t{ 206, "UNGROUP", 0, 0, 0 },
    ptg_func_t{ 207, "OBJECT.PROPERTIES", 0, 2, ptg_arg_v },
    ptg_func_t{ 208, "SAVE.NEW.OBJECT", 0, 1, ptg_arg_v },
    ptg_func_t{ 209, "SHARE", 0, 0, 0 },
    ptg_func_t{ 210, "SHARE.NAME", 0, 1, ptg_arg_v },
    ptg_func_t{ 211, "DUPLICATE", 0, 0, 0 },
    ptg_func_t{ 212, "APPLY.STYLE", 0, 1, ptg_arg_v },
    ptg_func_t{ 213, "ASSIGN.TO.OBJECT", 0, 1, ptg_arg_a },
    ptg_func_t{ 214, "OBJECT.PROTECTION", 0, 2, ptg_arg_v },
    ptg_func_t{ 215, "HIDE.OBJECT", 0, 2, ptg_arg_v },
    ptg_func_t{ 216, "SET.EXTRACT", 0, 0, 0 },
    ptg_func_t{ 217, "CREATE.PUBLISHER", 0, 4, ptg_arg_v },
    ptg_func_t{ 218, "SUBSCRIBE.TO", 0, 2, ptg_arg_v },
    ptg_func_t{ 219, "ATTRIBUTES", 0, 2, ptg_arg_v },
    ptg_func_t{ 220, "SHOW.TOOLBAR", 0, 10, ptg_arg_v },
    ptg_func_t{ 221, nullptr, 0, 0, 0 },
    ptg_func_t{ 222, "PRINT.PREVIEW", 0, 1, ptg_arg_v },
    ptg_func_t{ 223, "EDIT.COLOR", 0, 4, ptg_arg_v },
    ptg_func_t{ 224, "SHOW.LEVELS", 0, 2, ptg_arg_v },
    ptg_func_t{ 225, "FORMAT.MAIN", 0, 14, ptg_arg_v },
    ptg_func_t{ 226, "FORMAT.OVERLAY", 0, 14, ptg_arg_v },
    ptg_func_t{ 227, "ON.RECALC", 0, 2, ptg_arg_v },
    ptg_func_t{ 228, "EDIT.SERIES", 0, 7, ptg_arg_va },
    ptg_func_t{ 229, "DEFINE.STYLE", 0, 14, ptg_arg_v },
    ptg_func_t{ 230, nullptr, 0, 0, 0 },
    ptg_func_t{ 231, nullptr, 0, 0, 0 },
    ptg_func_t{ 232, nullptr, 0, 0, 0 },
    ptg_func_t{ 233, nullptr, 0, 0, 0 },
    ptg_func_t{ 234, nullptr, 0, 0, 0 },
    ptg_func_t{ 235, nullptr, 0, 0, 0 },
    ptg_func_t{ 236, nullptr, 0, 0, 0 },
    ptg_func_t{ 237, nullptr, 0, 0, 0 },
    ptg_func_t{ 238, nullptr, 0, 0, 0 },
    ptg_func_t{ 239, nullptr, 0, 0, 0 },
    ptg_func_t{ 240, "LINE.PRINT", 0, 11, ptg_arg_v },
    ptg_func_t{ 241, nullptr, 0, 0, 0 },
    ptg_func_t{ 242, nullptr, 0, 0, 0 },
    ptg_func_t{ 243, "ENTER.DATA", 0, 1, ptg_arg_a },
    ptg_func_t{ 244, nullptr, 0, 0, 0 },
    ptg_func_t{ 245, nullptr, 0, 0, 0 },
    ptg_func_t{ 246, nullptr, 0, 0, 0 },
    ptg_func_t{ 247, nullptr, 0, 0, 0 },
    ptg_func_t{ 248, nullptr, 0, 0, 0 },
    ptg_func_t{ 249, "GALLERY.RADAR", 0, 2, ptg_arg_v },
    ptg_func_t{ 250, "MERGE.STYLES", 0, 1, ptg_arg_v },
    ptg_func_t{ 251, "EDITION.OPTIONS", 0, 7, ptg_arg_va },
    ptg_func_t{ 252, "PASTE.PICTURE", 0, 0, 0 },
    ptg_func_t{ 253, "PASTE.PICTURE.LINK", 0, 0, 0 },
    ptg_func_t{ 254, "SPELLING", 0, 6, ptg_arg_v },
    ptg_func_t{ 255, nullptr, 0, 0, 0 },
    ptg_func_t{ 256, "ZOOM", 0, 1, ptg_arg_v },
    ptg_func_t{ 257, nullptr, 0, 0, 0 },
    ptg_func_t{ 258, nullptr, 0, 0, 0 },
    ptg_func_t{ 259, "INSERT.OBJECT", 0, 13, ptg_arg_vvvvvvvavvav },
    ptg_func_t{ 260, "WINDOW.MINIMIZE", 0, 1, ptg_arg_v },
    ptg_func_t{ 261, nullptr, 0, 0, 0 },
    ptg_func_t{ 262, nullptr, 0, 0, 0 },
    ptg_func_t{ 263, nullptr, 0, 0, 0 },
    ptg_func_t{ 264, nullptr, 0, 0, 0 },
    ptg_func_t{ 265, "SOUND.NOTE", 0, 3, ptg_arg_av },
    ptg_func_t{ 266, "SOUND.PLAY", 0, 3, ptg_arg_av },
    ptg_func_t{ 267, "FORMAT.SHAPE", 0, 5, ptg_arg_vvav },
    ptg_func_t{ 268, "EXTEND.POLYGON", 0, 1, ptg_arg_v },
    ptg_func_t{ 269, "FORMAT.AUTO", 0, 7, ptg_arg_v },
    ptg_func_t{ 270, nullptr, 0, 0, 0 },
    ptg_func_t{ 271, nullptr, 0, 0, 0 },
    ptg_func_t{ 272, "GALLERY.3D.BAR", 0, 1, ptg_arg_v },
    ptg_func_t{ 273, "GALLERY.3D.SURFACE", 0, 1, ptg_arg_v },
    ptg_func_t{ 274, "FILL.AUTO", 0, 2, ptg_arg_av },
    ptg_func_t{ 275, nullptr, 0, 0, 0 },
    ptg_func_t{ 276, "CUSTOMIZE.TOOLBAR", 0, 1, ptg_arg_v },
    ptg_func_t{ 277, "ADD.TOOL", 0, 3, ptg_arg_v },
    ptg_func_t{ 278, "EDIT.OBJECT", 0, 1, ptg_arg_v },
    ptg_func_t{ 279, "ON.DOUBLECLICK", 0, 2, ptg_arg_v },
    ptg_func_t{ 280, "ON.ENTRY", 0, 2, ptg_arg_v },
    ptg_func_t{ 281, "WORKBOOK.ADD", 0, 3, ptg_arg_v },
    ptg_func_t{ 282, "WORKBOOK.MOVE", 0, 3, ptg_arg_v },
    ptg_func_t{ 283, "WORKBOOK.COPY", 0, 3, ptg_arg_v },
    ptg_func_t{ 284, "WORKBOOK.OPTIONS", 0, 3, ptg_arg_v },
    ptg_func_t{ 285, "SAVE.WORKSPACE", 0, 1, ptg_arg_v },
    ptg_func_t{ 286, nullptr, 0, 0, 0 },
    ptg_func_t{ 287, nullptr, 0, 0, 0 },
    ptg_func_t{ 288, "CHART.WIZARD", 0, 14, ptg_arg_vav },
    ptg_func_t{ 289, "DELETE.TOOL", 0, 2, ptg_arg_v },
    ptg_func_t{ 290, "MOVE.TOOL", 0, 6, ptg_arg_v },
    ptg_func_t{ 291, "WORKBOOK.SELECT", 0, 3, ptg_arg_v },
    ptg_func_t{ 292, "WORKBOOK.ACTIVATE", 0, 2, ptg_arg_v },
    ptg_func_t{ 293, "ASSIGN.TO.TOOL", 0, 3, ptg_arg_vva },
    ptg_func_t{ 294, nullptr, 0, 0, 0 },
    ptg_func_t{ 295, "COPY.TOOL", 0, 2, ptg_arg_v },
    ptg_func_t{ 296, "RESET.TOOL", 0, 2, ptg_arg_v },
    ptg_func_t{ 297, "CONSTRAIN.NUMERIC", 0, 1, ptg_arg_v },
    ptg_func_t{ 298, "PASTE.TOOL", 0, 2, ptg_arg_v },
    ptg_func_t{ 299, nullptr, 0, 0, 0 },
    ptg_func_t{ 300, nullptr, 0, 0, 0 },
    ptg_func_t{ 301, nullptr, 0, 0, 0 },
    ptg_func_t{ 302, "WORKBOOK.NEW", 0, 3, ptg_arg_v },
    ptg_func_t{ 303, nullptr, 0, 0, 0 },
    ptg_func_t{ 304, nullptr, 0, 0, 0 },
    ptg_func_t{ 305, "SCENARIO.CELLS", 0, 1, ptg_arg_a },
    ptg_func_t{ 306, "SCENARIO.DELETE", 0, 1, ptg_arg_v },
    ptg_func_t{ 307, "SCENARIO.ADD", 0, 6, ptg_arg_vvav },
    ptg_func_t{ 308, "SCENARIO.EDIT", 0, 7, ptg_arg_vvvav },
    ptg_func_t{ 309, "SCENARIO.SHOW", 0, 1, ptg_arg_v },
    ptg_func_t{ 310, "SCENARIO.SHOW.NEXT", 0, 0, 0 },
    ptg_func_t{ 311, "SCENARIO.SUMMARY", 0, 2, ptg_arg_av },
    ptg_func_t{ 312, "PIVOT.TABLE.WIZARD", 0, 16, ptg_arg_vaav },
    ptg_func_t{ 313, "PIVOT.FIELD.PROPERTIES", 0, 7, ptg_arg_v },
    ptg_func_t{ 314, "PIVOT.FIELD", 0, 4, ptg_arg_v },
    ptg_func_t{ 315, "PIVOT.ITEM", 0, 4, ptg_arg_v },
    ptg_func_t{ 316, "PIVOT.ADD.FIELDS", 0, 5, ptg_arg_v },
    ptg_func_t{ 317, nullptr, 0, 0, 0 },
    ptg_func_t{ 318, "OPTIONS.CALCULATION", 0, 10, ptg_arg_v },
    ptg_func_t{ 319, "OPTIONS.EDIT", 0, 11, ptg_arg_v },
    ptg_func_t{ 320, "OPTIONS.VIEW", 0, 18, ptg_arg_v },
    ptg_func_t{ 321, "ADDIN.MANAGER", 0, 3, ptg_arg_v },
    ptg_func_t{ 322, "MENU.EDITOR", 0, 0, 0 },
    ptg_func_t{ 323, "ATTACH.TOOLBARS", 0, 0, 0 },
    ptg_func_t{ 324, "VBAActivate", 0, 2, ptg_arg_v },
    ptg_func_t{ 325, "OPTIONS.CHART", 0, 3, ptg_arg_v },
    ptg_func_t{ 326, nullptr, 0, 0, 0 },
    ptg_func_t{ 327, nullptr, 0, 0, 0 },
    ptg_func_t{ 328, "VBA.INSERT.FILE", 0, 1, ptg_arg_v },
    ptg_func_t{ 329, nullptr, 0, 0, 0 },
    ptg_func_t{ 330, "VBA.PROCEDURE.DEFINITION", 0, 0, 0 },
    ptg_func_t{ 331, nullptr, 0, 0, 0 },
    ptg_func_t{ 332, nullptr, 0, 0, 0 },
    ptg_func_t{ 333, nullptr, 0, 0, 0 },
    ptg_func_t{ 334, nullptr, 0, 0, 0 },
    ptg_func_t{ 335, nullptr, 0, 0, 0 },
    ptg_func_t{ 336, "ROUTING.SLIP", 0, 6, ptg_arg_av },
    ptg_func_t{ 337, nullptr, 0, 0, 0 },
    ptg_func_t{ 338, "ROUTE.DOCUMENT", 0, 0, 0 },
    ptg_func_t{ 339, "MAIL.LOGON", 0, 3, ptg_arg_aav },
    ptg_func_t{ 340, nullptr, 0, 0, 0 },
    ptg_func_t{ 341, nullptr, 0, 0, 0 },
    ptg_func_t{ 342, "INSERT.PICTURE", 0, 2, ptg_arg_v },
    ptg_func_t{ 343, "EDIT.TOOL", 0, 0, 0 },
    ptg_func_t{ 344, "GALLERY.DOUGHNUT", 0, 2, ptg_arg_v },
    ptg_func_t{ 345, nullptr, 0, 0, 0 },
    ptg_func_t{ 346, nullptr, 0, 0, 0 },
    ptg_func_t{ 347, nullptr, 0, 0, 0 },
    ptg_func_t{ 348, nullptr, 0, 0, 0 },
    ptg_func_t{ 349, nullptr, 0, 0, 0 },
    ptg_func_t{ 350, "CHART.TREND", 0, 8, ptg_arg_v },
    ptg_func_t{ 351, nullptr, 0, 0, 0 },
    ptg_func_t{ 352, "PIVOT.ITEM.PROPERTIES", 0, 7, ptg_arg_v },
    ptg_func_t{ 353, nullptr, 0, 0, 0 },
    ptg_func_t{ 354, "WORKBOOK.INSERT", 0, 1, ptg_arg_v },
    ptg_func_t{ 355, "OPTIONS.TRANSITION", 0, 5, ptg_arg_v },
    ptg_func_t{ 356, "OPTIONS.GENERAL", 0, 14, ptg_arg_v },
    ptg_func_t{ 357, nullptr, 0, 0, 0 },
    ptg_func_t{ 358, nullptr, 0, 0, 0 },
    ptg_func_t{ 359, nullptr, 0, 0, 0 },
    ptg_func_t{ 360, nullptr, 0, 0, 0 },
    ptg_func_t{ 361, nullptr, 0, 0, 0 },
    ptg_func_t{ 362, nullptr, 0, 0, 0 },
    ptg_func_t{ 363, nullptr, 0, 0, 0 },
    ptg_func_t{ 364, nullptr, 0, 0, 0 },
    ptg_func_t{ 365, nullptr, 0, 0, 0 },
    ptg_func_t{ 366, nullptr, 0, 0, 0 },
    ptg_func_t{ 367, nullptr, 0, 0, 0 },
    ptg_func_t{ 368, nullptr, 0, 0, 0 },
    ptg_func_t{ 369, nullptr, 0, 0, 0 },
    ptg_func_t{ 370, "FILTER.ADVANCED", 0, 5, ptg_arg_vaaav },
    ptg_func_t{ 371, nullptr, 0, 0, 0 },
    ptg_func_t{ 372, nullptr, 0, 0, 0 },
    ptg_func_t{ 373, "MAIL.ADD.MAILER", 0, 0, 0 },
    ptg_func_t{ 374, "MAIL.DELETE.MAILER", 0, 0, 0 },
    ptg_func_t{ 375, "MAIL.REPLY", 0, 0, 0 },
    ptg_func_t{ 376, "MAIL.REPLY.ALL", 0, 0, 0 },
    ptg_func_t{ 377, "MAIL.FORWARD", 0, 0, 0 },
    ptg_func_t{ 378, "MAIL.NEXT.LETTER", 0, 0, 0 },
    ptg_func_t{ 379, "DATA.LABEL", 0, 10, ptg_arg_v },
    ptg_func_t{ 380, "INSERT.TITLE", 0, 5, ptg_arg_v },
    ptg_func_t{ 381, "FONT.PROPERTIES", 0, 14, ptg_arg_v },
    ptg_func_t{ 382, "MACRO.OPTIONS", 0, 10, ptg_arg_v },
    ptg_func_t{ 383, "WORKBOOK.HIDE", 0, 2, ptg_arg_v },
    ptg_func_t{ 384, "WORKBOOK.UNHIDE", 0, 1, ptg_arg_v },
    ptg_func_t{ 385, "WORKBOOK.DELETE", 0, 1, ptg_arg_v },
    ptg_func_t{ 386, "WORKBOOK.NAME", 0, 2, ptg_arg_v },
    ptg_func_t{ 387, nullptr, 0, 0, 0 },
    ptg_func_t{ 388, "GALLERY.CUSTOM", 0, 1, ptg_arg_v },
    ptg_func_t{ 389, nullptr, 0, 0, 0 },
    ptg_func_t{ 390, "ADD.CHART.AUTOFORMAT", 0, 2, ptg_arg_v },
    ptg_func_t{ 391, "DELETE.CHART.AUTOFORMAT", 0, 1, ptg_arg_v },
    ptg_func_t{ 392, "CHART.ADD.DATA", 0, 6, ptg_arg_vav },
    ptg_func_t{ 393, "AUTO.OUTLINE", 0, 0, 0 },
    ptg_func_t{ 394, "TAB.ORDER", 0, 0, 0 },
    ptg_func_t{ 395, "SHOW.DIALOG", 0, 1, ptg_arg_v },
    ptg_func_t{ 396, "SELECT.ALL", 0, 0, 0 },
    ptg_func_t{ 397, "UNGROUP.SHEETS", 0, 0, 0 },
    ptg_func_t{ 398, "SUBTOTAL.CREATE", 0, 6, ptg_arg_v },
    ptg_func_t{ 399, "SUBTOTAL.REMOVE", 0, 0, 0 },
    ptg_func_t{ 400, "RENAME.OBJECT", 0, 1, ptg_arg_v },
    ptg_func_t{ 401, nullptr, 0, 0, 0 },
    ptg_func_t{ 402, nullptr, 0, 0, 0 },
    ptg_func_t{ 403, nullptr, 0, 0, 0 },
    ptg_func_t{ 404, nullptr, 0, 0, 0 },
    ptg_func_t{ 405, nullptr, 0, 0, 0 },
    ptg_func_t{ 406, nullptr, 0, 0, 0 },
    ptg_func_t{ 407, nullptr, 0, 0, 0 },
    ptg_func_t{ 408, nullptr, 0, 0, 0 },
    ptg_func_t{ 409, nullptr, 0, 0, 0 },
    ptg_func_t{ 410, nullptr, 0, 0, 0 },
    ptg_func_t{ 411, nullptr, 0, 0, 0 },
    ptg_func_t{ 412, "WORKBOOK.SCROLL", 0, 2, ptg_arg_v },
    ptg_func_t{ 413, "WORKBOOK.NEXT", 0, 0, 0 },
    ptg_func_t{ 414, "WORKBOOK.PREV", 0, 0, 0 },
    ptg_func_t{ 415, "WORKBOOK.TAB.SPLIT", 0, 1, ptg_arg_v },
    ptg_func_t{ 416, "FULL.SCREEN", 0, 1, ptg_arg_v },
    ptg_func_t{ 417, "WORKBOOK.PROTECT", 0, 3, ptg_arg_v },
    ptg_func_t{ 418, nullptr, 0, 0, 0 },
    ptg_func_t{ 419, nullptr, 0, 0, 0 },
    ptg_func_t{ 420, "SCROLLBAR.PROPERTIES", 0, 7, ptg_arg_v },
    ptg_func_t{ 421, "PIVOT.SHOW.PAGES", 0, 2, ptg_arg_v },
    ptg_func_t{ 422, "TEXT.TO.COLUMNS", 0, 14, ptg_arg_vav },
    ptg_func_t{ 423, "FORMAT.CHARTTYPE", 0, 4, ptg_arg_v },
    ptg_func_t{ 424, "LINK.FORMAT", 0, 0, 0 },
    ptg_func_t{ 425, "TRACER.DISPLAY", 0, 2, ptg_arg_v },
    ptg_func_t{ 426, nullptr, 0, 0, 0 },
    ptg_func_t{ 427, nullptr, 0, 0, 0 },
    ptg_func_t{ 428, nullptr, 0, 0, 0 },
    ptg_func_t{ 429, nullptr, 0, 0, 0 },
    ptg_func_t{ 430, "TRACER.NAVIGATE", 0, 3, ptg_arg_v },
    ptg_func_t{ 431, "TRACER.CLEAR", 0, 0, 0 },
    ptg_func_t{ 432, "TRACER.ERROR", 0, 0, 0 },
    ptg_func_t{ 433, "PIVOT.FIELD.GROUP", 0, 4, ptg_arg_v },
    ptg_func_t{ 434, "PIVOT.FIELD.UNGROUP", 0, 0, 0 },
    ptg_func_t{ 435, "CHECKBOX.PROPERTIES", 0, 5, ptg_arg_v },
    ptg_func_t{ 436, "LABEL.PROPERTIES", 0, 3, ptg_arg_v },
    ptg_func_t{ 437, "LISTBOX.PROPERTIES", 0, 5, ptg_arg_v },
    ptg_func_t{ 438, "EDITBOX.PROPERTIES", 0, 4, ptg_arg_v },
    ptg_func_t{ 439, "PIVOT.REFRESH", 0, 1, ptg_arg_v },
    ptg_func_t{ 440, "LINK.COMBO", 0, 1, ptg_arg_v },
    ptg_func_t{ 441, "OPEN.TEXT", 0, 17, ptg_arg_v },
    ptg_func_t{ 442, "HIDE.DIALOG", 0, 1, ptg_arg_v },
    ptg_func_t{ 443, "SET.DIALOG.FOCUS", 0, 1, ptg_arg_v },
    ptg_func_t{ 444, "ENABLE.OBJECT", 0, 2, ptg_arg_v },
    ptg_func_t{ 445, "PUSHBUTTON.PROPERTIES", 0, 6, ptg_arg_v },
    ptg_func_t{ 446, "SET.DIALOG.DEFAULT", 0, 1, ptg_arg_v },
    ptg_func_t{ 447, "FILTER", 0, 6, ptg_arg_v },
    ptg_func_t{ 448, "FILTER.SHOW.ALL", 0, 0, 0 },
    ptg_func_t{ 449, "CLEAR.OUTLINE", 0, 0, 0 },
    ptg_func_t{ 450, "FUNCTION.WIZARD", 0, 1, ptg_arg_v },
    ptg_func_t{ 451, "ADD.LIST.ITEM", 0, 2, ptg_arg_v },
    ptg_func_t{ 452, "SET.LIST.ITEM", 0, 2, ptg_arg_v },
    ptg_func_t{ 453, "REMOVE.LIST.ITEM", 0, 2, ptg_arg_v },
    ptg_func_t{ 454, "SELECT.LIST.ITEM", 0, 2, ptg_arg_v },
    ptg_func_t{ 455, "SET.CONTROL.VALUE", 0, 1, ptg_arg_v },
    ptg_func_t{ 456, "SAVE.COPY.AS", 0, 1, ptg_arg_v },
    ptg_func_t{ 457, nullptr, 0, 0, 0 },
    ptg_func_t{ 458, "OPTIONS.LISTS.ADD", 0, 2, ptg_arg_va },
    ptg_func_t{ 459, "OPTIONS.LISTS.DELETE", 0, 1, ptg_arg_v },
    ptg_func_t{ 460, "SERIES.AXES", 0, 1, ptg_arg_v },
    ptg_func_t{ 461, "SERIES.X", 0, 1, ptg_arg_a },
    ptg_func_t{ 462, "SERIES.Y", 0, 2, ptg_arg_a },
    ptg_func_t{ 463, "ERRORBAR.X", 0, 4, ptg_arg_vvva },
    ptg_func_t{ 464, "ERRORBAR.Y", 0, 4, ptg_arg_vvva },
    ptg_func_t{ 465, "FORMAT.CHART", 0, 18, ptg_arg_av },
    ptg_func_t{ 466, "SERIES.ORDER", 0, 3, ptg_arg_v },
    ptg_func_t{ 467, "MAIL.LOGOFF", 0, 0, 0 },
    ptg_func_t{ 468, "CLEAR.ROUTING.SLIP", 0, 1, ptg_arg_v },
    ptg_func_t{ 469, "APP.ACTIVATE.MICROSOFT", 0, 1, ptg_arg_v },
    ptg_func_t{ 470, "MAIL.EDIT.MAILER", 0, 6, ptg_arg_vaaava },
    ptg_func_t{ 471, "ON.SHEET", 0, 3, ptg_arg_v },
    ptg_func_t{ 472, "STANDARD.WIDTH", 0, 1, ptg_arg_v },
    ptg_func_t{ 473, "SCENARIO.MERGE", 0, 1, ptg_arg_v },
    ptg_func_t{ 474, "SUMMARY.INFO", 0, 5, ptg_arg_v },
    ptg_func_t{ 475, "FIND.FILE", 0, 0, 0 },
    ptg_func_t{ 476, "ACTIVE.CELL.FONT", 0, 14, ptg_arg_v },
    ptg_func_t{ 477, "ENABLE.TIPWIZARD", 0, 1, ptg_arg_v },
    ptg_func_t{ 478, "VBA.MAKE.ADDIN", 0, 1, ptg_arg_v },
    ptg_func_t{ 479, nullptr, 0, 0, 0 },
    ptg_func_t{ 480, "INSERTDATATABLE", 0, 1, ptg_arg_v },
    ptg_func_t{ 481, "WORKGROUP.OPTIONS", 0, 0, 0 },
    ptg_func_t{ 482, "MAIL.SEND.MAILER", 0, 2, ptg_arg_v },
    ptg_func_t{ 483, nullptr, 0, 0, 0 },
    ptg_func_t{ 484, nullptr, 0, 0, 0 },
    ptg_func_t{ 485, "AUTOCORRECT", 0, 2, ptg_arg_v },
    ptg_func_t{ 486, nullptr, 0, 0, 0 },
    ptg_func_t{ 487, nullptr, 0, 0, 0 },
    ptg_func_t{ 488, nullptr, 0, 0, 0 },
    ptg_func_t{ 489, "POST.DOCUMENT", 0, 1, ptg_arg_v },
    ptg_func_t{ 490, nullptr, 0, 0, 0 },
    ptg_func_t{ 491, "PICKLIST", 0, 0, 0 },
    ptg_func_t{ 492, nullptr, 0, 0, 0 },
    ptg_func_t{ 493, "VIEW.SHOW", 0, 1, ptg_arg_v },
    ptg_func_t{ 494, "VIEW.DEFINE", 0, 3, ptg_arg_v },
    ptg_func_t{ 495, "VIEW.DELETE", 0, 1, ptg_arg_v },
    ptg_func_t{ 496, nullptr, 0, 0, 0 },
    ptg_func_t{ 497, nullptr, 0, 0, 0 },
    ptg_func_t{ 498, nullptr, 0, 0, 0 },
    ptg_func_t{ 499, nullptr, 0, 0, 0 },
    ptg_func_t{ 500, nullptr, 0, 0, 0 },
    ptg_func_t{ 501, nullptr, 0, 0, 0 },
    ptg_func_t{ 502, nullptr, 0, 0, 0 },
    ptg_func_t{ 503, nullptr, 0, 0, 0 },
    ptg_func_t{ 504, nullptr, 0, 0, 0 },
    ptg_func_t{ 505, nullptr, 0, 0, 0 },
    ptg_func_t{ 506, nullptr, 0, 0, 0 },
    ptg_func_t{ 507, nullptr, 0, 0, 0 },
    ptg_func_t{ 508, nullptr, 0, 0, 0 },
    ptg_func_t{ 509, "SHEET.BACKGROUND", 0, 2, ptg_arg_v },
    ptg_func_t{ 510, "INSERT.MAP.OBJECT", 0, 0, 0 },
    ptg_func_t{ 511, "OPTIONS.MENONO", 0, 5, ptg_arg_v },
    ptg_func_t{ 512, nullptr, 0, 0, 0 },
    ptg_func_t{ 513, nullptr, 0, 0, 0 },
    ptg_func_t{ 514, nullptr, 0, 0, 0 },
    ptg_func_t{ 515, nullptr, 0, 0, 0 },
    ptg_func_t{ 516, nullptr, 0, 0, 0 },
    ptg_func_t{ 517, "MSOCHECKS", 0, 0, 0 },
    ptg_func_t{ 518, "NORMAL", 0, 0, 0 },
    ptg_func_t{ 519, "LAYOUT", 0, 0, 0 },
    ptg_func_t{ 520, "RM.PRINT.AREA", 0, 1, ptg_arg_a },
    ptg_func_t{ 521, "CLEAR.PRINT.AREA", 0, 0, 0 },
    ptg_func_t{ 522, "ADD.PRINT.AREA", 0, 0, 0 },
    ptg_func_t{ 523, "MOVE.BRK", 0, 4, ptg_arg_v },
    ptg_func_t{ 524, nullptr, 0, 0, 0 },
    ptg_func_t{ 525, nullptr, 0, 0, 0 },
    ptg_func_t{ 526, nullptr, 0, 0, 0 },
    ptg_func_t{ 527, nullptr, 0, 0, 0 },
    ptg_func_t{ 528, nullptr, 0, 0, 0 },
    ptg_func_t{ 529, nullptr, 0, 0, 0 },
    ptg_func_t{ 530, nullptr, 0, 0, 0 },
    ptg_func_t{ 531, nullptr, 0, 0, 0 },
    ptg_func_t{ 532, nullptr, 0, 0, 0 },
    ptg_func_t{ 533, nullptr, 0, 0, 0 },
    ptg_func_t{ 534, nullptr, 0, 0, 0 },
    ptg_func_t{ 535, nullptr, 0, 0, 0 },
    ptg_func_t{ 536, nullptr, 0, 0, 0 },
    ptg_func_t{ 537, nullptr, 0, 0, 0 },
    ptg_func_t{ 538, nullptr, 0, 0, 0 },
    ptg_func_t{ 539, nullptr, 0, 0, 0 },
    ptg_func_t{ 540, nullptr, 0, 0, 0 },
    ptg_func_t{ 541, nullptr, 0, 0, 0 },
    ptg_func_t{ 542, nullptr, 0, 0, 0 },
    ptg_func_t{ 543, nullptr, 0, 0, 0 },
    ptg_func_t{ 544, nullptr, 0, 0, 0 },
    ptg_func_t{ 545, "HIDECURR.NOTE", 0, 2, ptg_arg_av },
    ptg_func_t{ 546, "HIDEALL.NOTES", 0, 1, ptg_arg_v },
    ptg_func_t{ 547, "DELETE.NOTE", 0, 1, ptg_arg_a },
    ptg_func_t{ 548, "TRAVERSE.NOTES", 0, 2, ptg_arg_av },
    ptg_func_t{ 549, "ACTIVATE.NOTES", 0, 2, ptg_arg_av },
    ptg_func_t{ 550, nullptr, 0, 0, 0 },
    ptg_func_t{ 551, nullptr, 0, 0, 0 },
    ptg_func_t{ 552, nullptr, 0, 0, 0 },
    ptg_func_t{ 553, nullptr, 0, 0, 0 },
    ptg_func_t{ 554, nullptr, 0, 0, 0 },
    ptg_func_t{ 555, nullptr, 0, 0, 0 },
    ptg_func_t{ 556, nullptr, 0, 0, 0 },
    ptg_func_t{ 557, nullptr, 0, 0, 0 },
    ptg_func_t{ 558, nullptr, 0, 0, 0 },
    ptg_func_t{ 559, nullptr, 0, 0, 0 },
    ptg_func_t{ 560, nullptr, 0, 0, 0 },
    ptg_func_t{ 561, nullptr, 0, 0, 0 },
    ptg_func_t{ 562, nullptr, 0, 0, 0 },
    ptg_func_t{ 563, nullptr, 0, 0, 0 },
    ptg_func_t{ 564, nullptr, 0, 0, 0 },
    ptg_func_t{ 565, nullptr, 0, 0, 0 },
    ptg_func_t{ 566, nullptr, 0, 0, 0 },
    ptg_func_t{ 567, nullptr, 0, 0, 0 },
    ptg_func_t{ 568, nullptr, 0, 0, 0 },
    ptg_func_t{ 569, nullptr, 0, 0, 0 },
    ptg_func_t{ 570, nullptr, 0, 0, 0 },
    ptg_func_t{ 571, nullptr, 0, 0, 0 },
    ptg_func_t{ 572, nullptr, 0, 0, 0 },
    ptg_func_t{ 573, nullptr, 0, 0, 0 },
    ptg_func_t{ 574, nullptr, 0, 0, 0 },
    ptg_func_t{ 575, nullptr, 0, 0, 0 },
    ptg_func_t{ 576, nullptr, 0, 0, 0 },
    ptg_func_t{ 577, nullptr, 0, 0, 0 },
    ptg_func_t{ 578, nullptr, 0, 0, 0 },
    ptg_func_t{ 579, nullptr, 0, 0, 0 },
    ptg_func_t{ 580, nullptr, 0, 0, 0 },
    ptg_func_t{ 581, nullptr, 0, 0, 0 },
    ptg_func_t{ 582, nullptr, 0, 0, 0 },
    ptg_func_t{ 583, nullptr, 0, 0, 0 },
    ptg_func_t{ 584, nullptr, 0, 0, 0 },
    ptg_func_t{ 585, nullptr, 0, 0, 0 },
    ptg_func_t{ 586, nullptr, 0, 0, 0 },
    ptg_func_t{ 587, nullptr, 0, 0, 0 },
    ptg_func_t{ 588, nullptr, 0, 0, 0 },
    ptg_func_t{ 589, nullptr, 0, 0, 0 },
    ptg_func_t{ 590, nullptr, 0, 0, 0 },
    ptg_func_t{ 591, nullptr, 0, 0, 0 },
    ptg_func_t{ 592, nullptr, 0, 0, 0 },
    ptg_func_t{ 593, nullptr, 0, 0, 0 },
    ptg_func_t{ 594, nullptr, 0, 0, 0 },
    ptg_func_t{ 595, nullptr, 0, 0, 0 },
    ptg_func_t{ 596, nullptr, 0, 0, 0 },
    ptg_func_t{ 597, nullptr, 0, 0, 0 },
    ptg_func_t{ 598, nullptr, 0, 0, 0 },
    ptg_func_t{ 599, nullptr, 0, 0, 0 },
    ptg_func_t{ 600, nullptr, 0, 0, 0 },
    ptg_func_t{ 601, nullptr, 0, 0, 0 },
    ptg_func_t{ 602, nullptr, 0, 0, 0 },
    ptg_func_t{ 603, nullptr, 0, 0, 0 },
    ptg_func_t{ 604, nullptr, 0, 0, 0 },
    ptg_func_t{ 605, nullptr, 0, 0, 0 },
    ptg_func_t{ 606, nullptr, 0, 0, 0 },
    ptg_func_t{ 607, nullptr, 0, 0, 0 },
    ptg_func_t{ 608, nullptr, 0, 0, 0 },
    ptg_func_t{ 609, nullptr, 0, 0, 0 },
    ptg_func_t{ 610, nullptr, 0, 0, 0 },
    ptg_func_t{ 611, nullptr, 0, 0, 0 },
    ptg_func_t{ 612, nullptr, 0, 0, 0 },
    ptg_func_t{ 613, nullptr, 0, 0, 0 },
    ptg_func_t{ 614, nullptr, 0, 0, 0 },
    ptg_func_t{ 615, nullptr, 0, 0, 0 },
    ptg_func_t{ 616, nullptr, 0, 0, 0 },
    ptg_func_t{ 617, nullptr, 0, 0, 0 },
    ptg_func_t{ 618, nullptr, 0, 0, 0 },
    ptg_func_t{ 619, nullptr, 0, 0, 0 },
    ptg_func_t{ 620, "PROTECT.REVISIONS", 0, 0, 0 },
    ptg_func_t{ 621, "UNPROTECT.REVISIONS", 0, 0, 0 },
    ptg_func_t{ 622, nullptr, 0, 0, 0 },
    ptg_func_t{ 623, nullptr, 0, 0, 0 },
    ptg_func_t{ 624, nullptr, 0, 0, 0 },
    ptg_func_t{ 625, nullptr, 0, 0, 0 },
    ptg_func_t{ 626, nullptr, 0, 0, 0 },
    ptg_func_t{ 627, nullptr, 0, 0, 0 },
    ptg_func_t{ 628, nullptr, 0, 0, 0 },
    ptg_func_t{ 629, nullptr, 0, 0, 0 },
    ptg_func_t{ 630, nullptr, 0, 0, 0 },
    ptg_func_t{ 631, nullptr, 0, 0, 0 },
    ptg_func_t{ 632, nullptr, 0, 0, 0 },
    ptg_func_t{ 633, nullptr, 0, 0, 0 },
    ptg_func_t{ 634, nullptr, 0, 0, 0 },
    ptg_func_t{ 635, nullptr, 0, 0, 0 },
    ptg_func_t{ 636, nullptr, 0, 0, 0 },
    ptg_func_t{ 637, nullptr, 0, 0, 0 },
    ptg_func_t{ 638, nullptr, 0, 0, 0 },
    ptg_func_t{ 639, nullptr, 0, 0, 0 },
    ptg_func_t{ 640, nullptr, 0, 0, 0 },
    ptg_func_t{ 641, nullptr, 0, 0, 0 },
    ptg_func_t{ 642, nullptr, 0, 0, 0 },
    ptg_func_t{ 643, nullptr, 0, 0, 0 },
    ptg_func_t{ 644, nullptr, 0, 0, 0 },
    ptg_func_t{ 645, nullptr, 0, 0, 0 },
    ptg_func_t{ 646, nullptr, 0, 0, 0 },
    ptg_func_t{ 647, "OPTIONS.ME", 0, 9, ptg_arg_av },
    ptg_func_t{ 648, nullptr, 0, 0, 0 },
    ptg_func_t{ 649, nullptr, 0, 0, 0 },
    ptg_func_t{ 650, nullptr, 0, 0, 0 },
    ptg_func_t{ 651, nullptr, 0, 0, 0 },
    ptg_func_t{ 652, nullptr, 0, 0, 0 },
    ptg_func_t{ 653, "WEB.PUBLISH", 0, 9, ptg_arg_v },
    ptg_func_t{ 654, nullptr, 0, 0, 0 },
    ptg_func_t{ 655, nullptr, 0, 0, 0 },
    ptg_func_t{ 656, nullptr, 0, 0, 0 },
    ptg_func_t{ 657, nullptr, 0, 0, 0 },
    ptg_func_t{ 658, nullptr, 0, 0, 0 },
    ptg_func_t{ 659, nullptr, 0, 0, 0 },
    ptg_func_t{ 660, nullptr, 0, 0, 0 },
    ptg_func_t{ 661, nullptr, 0, 0, 0 },
    ptg_func_t{ 662, nullptr, 0, 0, 0 },
    ptg_func_t{ 663, nullptr, 0, 0, 0 },
    ptg_func_t{ 664, nullptr, 0, 0, 0 },
    ptg_func_t{ 665, nullptr, 0, 0, 0 },
    ptg_func_t{ 666, nullptr, 0, 0, 0 },
    ptg_func_t{ 667, "NEWWEBQUERY", 0, 1, ptg_arg_v },
    ptg_func_t{ 668, nullptr, 0, 0, 0 },
    ptg_func_t{ 669, nullptr, 0, 0, 0 },
    ptg_func_t{ 670, nullptr, 0, 0, 0 },
    ptg_func_t{ 671, nullptr, 0, 0, 0 },
    ptg_func_t{ 672, nullptr, 0, 0, 0 },
    ptg_func_t{ 673, "PIVOT.TABLE.CHART", 0, 16, ptg_arg_vaav },
    ptg_func_t{ 674, nullptr, 0, 0, 0 },
    ptg_func_t{ 675, nullptr, 0, 0, 0 },
    ptg_func_t{ 676, nullptr, 0, 0, 0 },
    ptg_func_t{ 677, nullptr, 0, 0, 0 },
    ptg_func_t{ 678, nullptr, 0, 0, 0 },
    ptg_func_t{ 679, nullptr, 0, 0, 0 },
    ptg_func_t{ 680, nullptr, 0, 0, 0 },
    ptg_func_t{ 681, nullptr, 0, 0, 0 },
    ptg_func_t{ 682, nullptr, 0, 0, 0 },
    ptg_func_t{ 683, nullptr, 0, 0, 0 },
    ptg_func_t{ 684, nullptr, 0, 0, 0 },
    ptg_func_t{ 685, nullptr, 0, 0, 0 },
    ptg_func_t{ 686, nullptr, 0, 0, 0 },
    ptg_func_t{ 687, nullptr, 0, 0, 0 },
    ptg_func_t{ 688, nullptr, 0, 0, 0 },
    ptg_func_t{ 689, nullptr, 0, 0, 0 },
    ptg_func_t{ 690, nullptr, 0, 0, 0 },
    ptg_func_t{ 691, nullptr, 0, 0, 0 },
    ptg_func_t{ 692, nullptr, 0, 0, 0 },
    ptg_func_t{ 693, nullptr, 0, 0, 0 },
    ptg_func_t{ 694, nullptr, 0, 0, 0 },
    ptg_func_t{ 695, nullptr, 0, 0, 0 },
    ptg_func_t{ 696, nullptr, 0, 0, 0 },
    ptg_func_t{ 697, nullptr, 0, 0, 0 },
    ptg_func_t{ 698, nullptr, 0, 0, 0 },
    ptg_func_t{ 699, nullptr, 0, 0, 0 },
    ptg_func_t{ 700, nullptr, 0, 0, 0 },
    ptg_func_t{ 701, nullptr, 0, 0, 0 },
    ptg_func_t{ 702, nullptr, 0, 0, 0 },
    ptg_func_t{ 703, nullptr, 0, 0, 0 },
    ptg_func_t{ 704, nullptr, 0, 0, 0 },
    ptg_func_t{ 705, nullptr, 0, 0, 0 },
    ptg_func_t{ 706, nullptr, 0, 0, 0 },
    ptg_func_t{ 707, nullptr, 0, 0, 0 },
    ptg_func_t{ 708, nullptr, 0, 0, 0 },
    ptg_func_t{ 709, nullptr, 0, 0, 0 },
    ptg_func_t{ 710, nullptr, 0, 0, 0 },
    ptg_func_t{ 711, nullptr, 0, 0, 0 },
    ptg_func_t{ 712, nullptr, 0, 0, 0 },
    ptg_func_t{ 713, nullptr, 0, 0, 0 },
    ptg_func_t{ 714, nullptr, 0, 0, 0 },
    ptg_func_t{ 715, nullptr, 0, 0, 0 },
    ptg_func_t{ 716, nullptr, 0, 0, 0 },
    ptg_func_t{ 717, nullptr, 0, 0, 0 },
    ptg_func_t{ 718, nullptr, 0, 0, 0 },
    ptg_func_t{ 719, nullptr, 0, 0, 0 },
    ptg_func_t{ 720, nullptr, 0, 0, 0 },
    ptg_func_t{ 721, nullptr, 0, 0, 0 },
    ptg_func_t{ 722, nullptr, 0, 0, 0 },
    ptg_func_t{ 723, nullptr, 0, 0, 0 },
    ptg_func_t{ 724, nullptr, 0, 0, 0 },
    ptg_func_t{ 725, nullptr, 0, 0, 0 },
    ptg_func_t{ 726, nullptr, 0, 0, 0 },
    ptg_func_t{ 727, nullptr, 0, 0, 0 },
    ptg_func_t{ 728, nullptr, 0, 0, 0 },
    ptg_func_t{ 729, nullptr, 0, 0, 0 },
    ptg_func_t{ 730, nullptr, 0, 0, 0 },
    ptg_func_t{ 731, nullptr, 0, 0, 0 },
    ptg_func_t{ 732, nullptr, 0, 0, 0 },
    ptg_func_t{ 733, nullptr, 0, 0, 0 },
    ptg_func_t{ 734, nullptr, 0, 0, 0 },
    ptg_func_t{ 735, nullptr, 0, 0, 0 },
    ptg_func_t{ 736, nullptr, 0, 0, 0 },
    ptg_func_t{ 737, nullptr, 0, 0, 0 },
    ptg_func_t{ 738, nullptr, 0, 0, 0 },
    ptg_func_t{ 739, nullptr, 0, 0, 0 },
    ptg_func_t{ 740, nullptr, 0, 0, 0 },
    ptg_func_t{ 741, nullptr, 0, 0, 0 },
    ptg_func_t{ 742, nullptr, 0, 0, 0 },
    ptg_func_t{ 743, nullptr, 0, 0, 0 },
    ptg_func_t{ 744, nullptr, 0, 0, 0 },
    ptg_func_t{ 745, nullptr, 0, 0, 0 },
    ptg_func_t{ 746, nullptr, 0, 0, 0 },
    ptg_func_t{ 747, nullptr, 0, 0, 0 },
    ptg_func_t{ 748, nullptr, 0, 0, 0 },
    ptg_func_t{ 749, nullptr, 0, 0, 0 },
    ptg_func_t{ 750, nullptr, 0, 0, 0 },
    ptg_func_t{ 751, nullptr, 0, 0, 0 },
    ptg_func_t{ 752, nullptr, 0, 0, 0 },
    ptg_func_t{ 753, "OPTIONS.SAVE", 0, 4, ptg_arg_v },
    ptg_func_t{ 754, nullptr, 0, 0, 0 },
    ptg_func_t{ 755, "OPTIONS.SPELL", 0, 12, ptg_arg_v },
    ptg_func_t{ 756, nullptr, 0, 0, 0 },
    ptg_func_t{ 757, nullptr, 0, 0, 0 },
    ptg_func_t{ 758, nullptr, 0, 0, 0 },
    ptg_func_t{ 759, nullptr, 0, 0, 0 },
    ptg_func_t{ 760, nullptr, 0, 0, 0 },
    ptg_func_t{ 761, nullptr, 0, 0, 0 },
    ptg_func_t{ 762, nullptr, 0, 0, 0 },
    ptg_func_t{ 763, nullptr, 0, 0, 0 },
    ptg_func_t{ 764, nullptr, 0, 0, 0 },
    ptg_func_t{ 765, nullptr, 0, 0, 0 },
    ptg_func_t{ 766, nullptr, 0, 0, 0 },
    ptg_func_t{ 767, nullptr, 0, 0, 0 },
    ptg_func_t{ 768, nullptr, 0, 0, 0 },
    ptg_func_t{ 769, nullptr, 0, 0, 0 },
    ptg_func_t{ 770, nullptr, 0, 0, 0 },
    ptg_func_t{ 771, nullptr, 0, 0, 0 },
    ptg_func_t{ 772, nullptr, 0, 0, 0 },
    ptg_func_t{ 773, nullptr, 0, 0, 0 },
    ptg_func_t{ 774, nullptr, 0, 0, 0 },
    ptg_func_t{ 775, nullptr, 0, 0, 0 },
    ptg_func_t{ 776, nullptr, 0, 0, 0 },
    ptg_func_t{ 777, nullptr, 0, 0, 0 },
    ptg_func_t{ 778, nullptr, 0, 0, 0 },
    ptg_func_t{ 779, nullptr, 0, 0, 0 },
    ptg_func_t{ 780, nullptr, 0, 0, 0 },
    ptg_func_t{ 781, nullptr, 0, 0, 0 },
    ptg_func_t{ 782, nullptr, 0, 0, 0 },
    ptg_func_t{ 783, nullptr, 0, 0, 0 },
    ptg_func_t{ 784, nullptr, 0, 0, 0 },
    ptg_func_t{ 785, nullptr, 0, 0, 0 },
    ptg_func_t{ 786, nullptr, 0, 0, 0 },
    ptg_func_t{ 787, nullptr, 0, 0, 0 },
    ptg_func_t{ 788, nullptr, 0, 0, 0 },
    ptg_func_t{ 789, nullptr, 0, 0, 0 },
    ptg_func_t{ 790, nullptr, 0, 0, 0 },
    ptg_func_t{ 791, nullptr, 0, 0, 0 },
    ptg_func_t{ 792, nullptr, 0, 0, 0 },
    ptg_func_t{ 793, nullptr, 0, 0, 0 },
    ptg_func_t{ 794, nullptr, 0, 0, 0 },
    ptg_func_t{ 795, nullptr, 0, 0, 0 },
    ptg_func_t{ 796, nullptr, 0, 0, 0 },
    ptg_func_t{ 797, nullptr, 0, 0, 0 },
    ptg_func_t{ 798, nullptr, 0, 0, 0 },
    ptg_func_t{ 799, nullptr, 0, 0, 0 },
    ptg_func_t{ 800, nullptr, 0, 0, 0 },
    ptg_func_t{ 801, nullptr, 0, 0, 0 },
    ptg_func_t{ 802, nullptr, 0, 0, 0 },
    ptg_func_t{ 803, nullptr, 0, 0, 0 },
    ptg_func_t{ 804, nullptr, 0, 0, 0 },
    ptg_func_t{ 805, nullptr, 0, 0, 0 },
    ptg_func_t{ 806, nullptr, 0, 0, 0 },
    ptg_func_t{ 807, nullptr, 0, 0, 0 },
    ptg_func_t{ 808, "HIDEALL.INKANNOTS", 0, 1, ptg_arg_v },
};

struct ptg_item_t {
    uint32_t    id;
    const char* token;
    uint8_t     type;
    const ptg_item_t* subs;
};

/*
* @Reference: [MS-XLS] - v20190618, page_no: 81, 824
*   operand token
*   operator token
*   control token   : subsequent sections
*   display token   : subsequent sections
*   mem token       : subsequent sections
*/
enum ptg_token_type {
    token_unknown,
    token_operand,
    token_operator_unary,
    token_operator_2ref,
    token_operator_2val,
    token_control,    /* do not perform operations or push values onto the stack. */
                    /* can be ignored when converting parsed expressions into textual formulas. */
    token_display,     /* do not perform operations or push values onto the stack */
    token_mem,
    token_func_call,
};

const ptg_item_t ptg_elf_subs[] = {
    ptg_item_t{ 0x01,  "PtgElfLel", token_operand, 0 },
    ptg_item_t{ 0x02,  "PtgElfRw", token_operand, 0 },
    ptg_item_t{ 0x03,  "PtgElfCol", token_operand, 0 },
    ptg_item_t{ 0x04,  nullptr, 0, 0 },
    ptg_item_t{ 0x05,  nullptr, 0, 0 },
    ptg_item_t{ 0x06,  "PtgElfRwV", token_operand, 0 },
    ptg_item_t{ 0x07,  "PtgElfColV", token_operand, 0 },
    ptg_item_t{ 0x08,  nullptr, 0, 0 },
    ptg_item_t{ 0x09,  nullptr, 0, 0 },
    ptg_item_t{ 0x0A,  "PtgElfRadical", token_operand, 0 },
    ptg_item_t{ 0x0B,  "PtgElfRadicalS", token_operand, 0 },
    ptg_item_t{ 0x0C,  nullptr, 0, 0 },
    ptg_item_t{ 0x0D,  "PtgElfColS", token_operand, 0 },
    ptg_item_t{ 0x0E,  nullptr, 0, 0 },
    ptg_item_t{ 0x0F,  "PtgElfColSV", token_operand, 0 },
    ptg_item_t{ 0x10,  "PtgElfRadicalLel", token_operand, 0 },
    ptg_item_t{ 0x11,  nullptr, 0, 0 },
    ptg_item_t{ 0x12,  nullptr, 0, 0 },
    ptg_item_t{ 0x13,  nullptr, 0, 0 },
    ptg_item_t{ 0x14,  nullptr, 0, 0 },
    ptg_item_t{ 0x15,  nullptr, 0, 0 },
    ptg_item_t{ 0x16,  nullptr, 0, 0 },
    ptg_item_t{ 0x17,  nullptr, 0, 0 },
    ptg_item_t{ 0x18,  nullptr, 0, 0 },
    ptg_item_t{ 0x19,  nullptr, 0, 0 },
    ptg_item_t{ 0x1A,  nullptr, 0, 0 },
    ptg_item_t{ 0x1B,  nullptr, 0, 0 },
    ptg_item_t{ 0x1C,  nullptr, 0, 0 },
    ptg_item_t{ 0x1D,  "PtgSxName", token_operand, 0 },
};

const ptg_item_t ptg_attr_subs[] = {
    ptg_item_t{ 0x00,  nullptr, 0, 0 },
    ptg_item_t{ 0x01,  "PtgAttrSemi", 0, 0 },
    ptg_item_t{ 0x02,  "PtgAttrIf", token_control, 0 },
    ptg_item_t{ 0x03,  nullptr, 0, 0 },
    ptg_item_t{ 0x04,  "PtgAttrChoose", token_control, 0 },
    ptg_item_t{ 0x05,  nullptr, 0, 0 },
    ptg_item_t{ 0x06,  nullptr, 0, 0 },
    ptg_item_t{ 0x07,  nullptr, 0, 0 },
    ptg_item_t{ 0x08,  "PtgAttrGoto", token_control, 0 },
    ptg_item_t{ 0x09,  nullptr, 0, 0 },
    ptg_item_t{ 0x0A,  nullptr, 0, 0 },
    ptg_item_t{ 0x0B,  nullptr, 0, 0 },
    ptg_item_t{ 0x0C,  nullptr, 0, 0 },
    ptg_item_t{ 0x0D,  nullptr, 0, 0 },
    ptg_item_t{ 0x0E,  nullptr, 0, 0 },
    ptg_item_t{ 0x0F,  nullptr, 0, 0 },
    ptg_item_t{ 0x10,  "PtgAttrSum", token_func_call, 0 },
    ptg_item_t{ 0x11,  nullptr, 0, 0 },
    ptg_item_t{ 0x12,  nullptr, 0, 0 },
    ptg_item_t{ 0x13,  nullptr, 0, 0 },
    ptg_item_t{ 0x14,  nullptr, 0, 0 },
    ptg_item_t{ 0x15,  nullptr, 0, 0 },
    ptg_item_t{ 0x16,  nullptr, 0, 0 },
    ptg_item_t{ 0x17,  nullptr, 0, 0 },
    ptg_item_t{ 0x18,  nullptr, 0, 0 },
    ptg_item_t{ 0x19,  nullptr, 0, 0 },
    ptg_item_t{ 0x1A,  nullptr, 0, 0 },
    ptg_item_t{ 0x1B,  nullptr, 0, 0 },
    ptg_item_t{ 0x1C,  nullptr, 0, 0 },
    ptg_item_t{ 0x1D,  nullptr, 0, 0 },
    ptg_item_t{ 0x1E,  nullptr, 0, 0 },
    ptg_item_t{ 0x1F,  nullptr, 0, 0 },
    ptg_item_t{ 0x20,  "AttrBaxcel_1", 0, 0 },
    ptg_item_t{ 0x21,  "AttrBaxcel_2", 0, 0 },
    ptg_item_t{ 0x22,  nullptr, 0, 0 },
    ptg_item_t{ 0x23,  nullptr, 0, 0 },
    ptg_item_t{ 0x24,  nullptr, 0, 0 },
    ptg_item_t{ 0x25,  nullptr, 0, 0 },
    ptg_item_t{ 0x26,  nullptr, 0, 0 },
    ptg_item_t{ 0x27,  nullptr, 0, 0 },
    ptg_item_t{ 0x28,  nullptr, 0, 0 },
    ptg_item_t{ 0x29,  nullptr, 0, 0 },
    ptg_item_t{ 0x2A,  nullptr, 0, 0 },
    ptg_item_t{ 0x2B,  nullptr, 0, 0 },
    ptg_item_t{ 0x2C,  nullptr, 0, 0 },
    ptg_item_t{ 0x2D,  nullptr, 0, 0 },
    ptg_item_t{ 0x2E,  nullptr, 0, 0 },
    ptg_item_t{ 0x2F,  nullptr, 0, 0 },
    ptg_item_t{ 0x30,  nullptr, 0, 0 },
    ptg_item_t{ 0x31,  nullptr, 0, 0 },
    ptg_item_t{ 0x32,  nullptr, 0, 0 },
    ptg_item_t{ 0x33,  nullptr, 0, 0 },
    ptg_item_t{ 0x34,  nullptr, 0, 0 },
    ptg_item_t{ 0x35,  nullptr, 0, 0 },
    ptg_item_t{ 0x36,  nullptr, 0, 0 },
    ptg_item_t{ 0x37,  nullptr, 0, 0 },
    ptg_item_t{ 0x38,  nullptr, 0, 0 },
    ptg_item_t{ 0x39,  nullptr, 0, 0 },
    ptg_item_t{ 0x3A,  nullptr, 0, 0 },
    ptg_item_t{ 0x3B,  nullptr, 0, 0 },
    ptg_item_t{ 0x3C,  nullptr, 0, 0 },
    ptg_item_t{ 0x3D,  nullptr, 0, 0 },
    ptg_item_t{ 0x3E,  nullptr, 0, 0 },
    ptg_item_t{ 0x3F,  nullptr, 0, 0 },
    ptg_item_t{ 0x40,  "PtgAttrSpace", token_display, 0 },
    ptg_item_t{ 0x41,  "PtgAttrSpaceSemi", token_display, 0 },
};

const ptg_item_t ptg_items[] = {
    ptg_item_t{ 0,  nullptr, 0, 0 },
    ptg_item_t{ 1, "ptgExp", 0, 0 },
    ptg_item_t{ 2, "ptgTbl", 0, 0 },
    ptg_item_t{ 3, "ptgAdd", token_operator_2val, 0 },
    ptg_item_t{ 4, "ptgSub", token_operator_2val, 0 },
    ptg_item_t{ 5, "ptgMul", token_operator_2val, 0 },
    ptg_item_t{ 6, "ptgDiv", token_operator_2val, 0 },
    ptg_item_t{ 7, "ptgPower", token_operator_2val, 0 },
    ptg_item_t{ 8, "ptgConcat", token_operator_2val, 0 },
    ptg_item_t{ 9, "ptgLT", token_operator_2val, 0 },
    ptg_item_t{ 10, "ptgLE", token_operator_2val, 0 },
    ptg_item_t{ 11, "ptgEQ", token_operator_2val, 0 },
    ptg_item_t{ 12, "ptgGE", token_operator_2val, 0 },
    ptg_item_t{ 13, "ptgGT", token_operator_2val, 0 },
    ptg_item_t{ 14, "ptgNE", token_operator_2val, 0 },
    ptg_item_t{ 15, "ptgIsect", token_operator_2ref, 0 },
    ptg_item_t{ 16, "ptgUnion", token_operator_2ref, 0 },
    ptg_item_t{ 17, "ptgRange", token_operator_2ref, 0 },
    ptg_item_t{ 18, "ptgUplus", token_operator_unary, 0 },
    ptg_item_t{ 19, "ptgUminus", token_operator_unary, 0 },
    ptg_item_t{ 20, "ptgPercent", token_operator_unary, 0 },
    ptg_item_t{ 21, "ptgParen", token_display, 0 },
    ptg_item_t{ 22, "ptgMissArg", token_operand, 0 },
    ptg_item_t{ 23, "ptgStr", token_operand, 0 },
    ptg_item_t{ 24, "ptgElf", token_operand, ptg_elf_subs },
    ptg_item_t{ 25, "ptgAttr", token_display, ptg_attr_subs },
    ptg_item_t{ 26, "ptgSheet", 0, 0 },
    ptg_item_t{ 27, "ptgEndSheet", 0, 0 },
    ptg_item_t{ 28, "ptgErr", token_operand, 0 },
    ptg_item_t{ 29, "ptgBool", token_operand, 0 },
    ptg_item_t{ 30, "ptgInt", token_operand, 0 },
    ptg_item_t{ 31, "ptgNum", token_operand, 0 },
    ptg_item_t{ 32, "ptgArray", token_operand, 0 },
    ptg_item_t{ 33, "ptgFunc", token_func_call, 0 },
    ptg_item_t{ 34, "ptgFuncVar", token_func_call, 0 },
    ptg_item_t{ 35, "ptgName", token_operand, 0 },
    ptg_item_t{ 36, "ptgRef", token_operand, 0 },
    ptg_item_t{ 37, "ptgArea", token_operand, 0 },
    ptg_item_t{ 38, "ptgMemArea", token_mem, 0 },
    ptg_item_t{ 39, "ptgMemErr", token_mem, 0 },
    ptg_item_t{ 40, "ptgMemNoMem", token_mem, 0 },
    ptg_item_t{ 41, "ptgMemFunc", token_mem, 0 },
    ptg_item_t{ 42, "ptgRefErr", token_operand, 0 },
    ptg_item_t{ 43, "ptgAreaErr", token_operand, 0 },
    ptg_item_t{ 44, "ptgRefN", token_operand, 0 },
    ptg_item_t{ 0x2D, "ptgAreaN", token_operand, 0 },
    ptg_item_t{ 46,  nullptr, 0, 0 },
    ptg_item_t{ 47,  nullptr, 0, 0 },
    ptg_item_t{ 48,  nullptr, 0, 0 },
    ptg_item_t{ 49,  nullptr, 0, 0 },
    ptg_item_t{ 50,  nullptr, 0, 0 },
    ptg_item_t{ 51,  nullptr, 0, 0 },
    ptg_item_t{ 52,  nullptr, 0, 0 },
    ptg_item_t{ 53,  nullptr, 0, 0 },
    ptg_item_t{ 54,  nullptr, 0, 0 },
    ptg_item_t{ 55,  nullptr, 0, 0 },
    ptg_item_t{ 56,  nullptr, 0, 0 },
    ptg_item_t{ 0x39, "ptgNameX", token_operand, 0 },
    ptg_item_t{ 0x3A, "ptgRef3d", token_operand, 0 },
    ptg_item_t{ 0x3B, "ptgArea3d", token_operand, 0 },
    ptg_item_t{ 0x3C, "ptgRefErr3d", token_operand, 0 },
    ptg_item_t{ 0x3D, "ptgAreaErr3d", token_operand, 0 },
    ptg_item_t{ 62,  nullptr, 0, 0 },
    ptg_item_t{ 63,  nullptr, 0, 0 },
    ptg_item_t{ 0x40, "ptgArray", token_operand, 0 },
    ptg_item_t{ 0x41, "ptgFunc", token_func_call, 0 },
    ptg_item_t{ 0x42, "ptgFuncVar", token_func_call, 0 },
    ptg_item_t{ 0x43, "ptgName", token_operand, 0 },
    ptg_item_t{ 0x44, "ptgRef", token_operand, 0 },
    ptg_item_t{ 0x45, "ptgArea", token_operand, 0 },
    ptg_item_t{ 0x46, "ptgMemArea", token_mem, 0 },
    ptg_item_t{ 0x47, "ptgMemErr", token_mem, 0 },
    ptg_item_t{ 0x48, "ptgMemNoMem", token_mem, 0 },
    ptg_item_t{ 0x49, "ptgMemFunc", token_mem, 0 },
    ptg_item_t{ 0x4A, "ptgRefErr", token_operand, 0 },
    ptg_item_t{ 0x4B, "ptgAreaErr", token_operand, 0 },
    ptg_item_t{ 0x4C, "ptgRefN", token_operand, 0 },
    ptg_item_t{ 0x4D, "ptgAreaN", token_operand, 0 },
    ptg_item_t{ 78,  nullptr, 0, 0 },
    ptg_item_t{ 79,  nullptr, 0, 0 },
    ptg_item_t{ 80,  nullptr, 0, 0 },
    ptg_item_t{ 81,  nullptr, 0, 0 },
    ptg_item_t{ 82,  nullptr, 0, 0 },
    ptg_item_t{ 83,  nullptr, 0, 0 },
    ptg_item_t{ 84,  nullptr, 0, 0 },
    ptg_item_t{ 85,  nullptr, 0, 0 },
    ptg_item_t{ 86,  nullptr, 0, 0 },
    ptg_item_t{ 87,  nullptr, 0, 0 },
    ptg_item_t{ 88,  nullptr, 0, 0 },
    ptg_item_t{ 0x59, "ptgNameX", token_operand, 0 },
    ptg_item_t{ 0x5A, "ptgRef3d", token_operand, 0 },
    ptg_item_t{ 0x5B, "ptgArea3d", token_operand, 0 },
    ptg_item_t{ 0x5C, "ptgRefErr3d", token_operand, 0 },
    ptg_item_t{ 0x5D, "ptgAreaErr3d", token_operand, 0 },
    ptg_item_t{ 94,  nullptr, 0, 0 },
    ptg_item_t{ 95,  nullptr, 0, 0 },
    ptg_item_t{ 0x60, "ptgArray", token_operand, 0 },
    ptg_item_t{ 0x61, "ptgFunc", token_func_call, 0 },
    ptg_item_t{ 0x62, "ptgFuncVar", token_operand, 0 },
    ptg_item_t{ 0x63, "ptgName", token_operand, 0 },
    ptg_item_t{ 0x64, "ptgRef", token_operand, 0 },
    ptg_item_t{ 0x65, "ptgArea", token_operand, 0 },
    ptg_item_t{ 0x66, "ptgMemArea", token_mem, 0 },
    ptg_item_t{ 0x67, "ptgMemErr", token_mem, 0 },
    ptg_item_t{ 0x68, "ptgMemNoMem", token_mem, 0 },
    ptg_item_t{ 0x69, "ptgMemFunc", token_mem, 0 },
    ptg_item_t{ 0x6A, "ptgRefErr", token_operand, 0 },
    ptg_item_t{ 0x6B, "ptgAreaErr", token_operand, 0 },
    ptg_item_t{ 0x6C, "ptgRefN", token_operand, 0 },
    ptg_item_t{ 0x6D, "ptgAreaN", token_operand, 0 },
    ptg_item_t{ 110,  nullptr, 0, 0 },
    ptg_item_t{ 111,  nullptr, 0, 0 },
    ptg_item_t{ 112,  nullptr, 0, 0 },
    ptg_item_t{ 113,  nullptr, 0, 0 },
    ptg_item_t{ 114,  nullptr, 0, 0 },
    ptg_item_t{ 115,  nullptr, 0, 0 },
    ptg_item_t{ 116,  nullptr, 0, 0 },
    ptg_item_t{ 117,  nullptr, 0, 0 },
    ptg_item_t{ 118,  nullptr, 0, 0 },
    ptg_item_t{ 119,  nullptr, 0, 0 },
    ptg_item_t{ 120,  nullptr, 0, 0 },
    ptg_item_t{ 0x79, "ptgNameX", token_operand, 0 },
    ptg_item_t{ 0x7A, "ptgRef3d", token_operand, 0 },
    ptg_item_t{ 0x7B, "ptgArea3d", token_operand, 0 },
    ptg_item_t{ 0x7C, "ptgRefErr3d", token_operand, 0 },
    ptg_item_t{ 0x7D, "ptgAreaErr3d", token_operand, 0 },
};

/*
* @Reference: [MS-XLS] - v20190618, page_no: 789
*/
typedef enum ptg_expr {
    ptgExp = 0x01,
    ptgTbl = 0x02,
    ptgAdd = 0x03,
    ptgSub = 0x04,
    ptgMul = 0x05,
    ptgDiv = 0x06,
    ptgPower = 0x07,
    ptgConcat = 0x08,
    ptgLt = 0x09,
    ptgLe = 0x0A,
    ptgEq = 0x0B,
    ptgGe = 0x0C,
    ptgGt = 0x0D,
    ptgNe = 0x0E,
    ptgIsect = 0x0F,
    ptgUnion = 0x10,
    ptgRange = 0x11,
    ptgUplus = 0x12,
    ptgUminus = 0x13,
    ptgPercent = 0x14,
    ptgParen = 0x15,
    ptgMissArg = 0x16,
    ptgStr = 0x17,

    ptgElf = 0x18,       //ptgEscape1    = 0x18,
    ptgElfLef = 0x0118,
    ptgElfRw = 0x0218,
    ptgElfCol = 0x0318,
    ptgElfRwV = 0x0618,
    ptgElfColV = 0x0718,
    ptgElfRadical = 0x0A18,
    ptgElfRadicalS = 0x0B18,
    ptgElfColS = 0x0D18,
    ptgElfColSV = 0x0F18,
    PtgElfRadicalLel = 0x1018,
    PtgSxName = 0x1D18,

    ptgAttr = 0x19,
    ptgAttrSemi = 0x0119,
    ptgAttrIf = 0x0219,
    ptgAttrChoose = 0x0419,
    PtgAttrGoto = 0x0819,
    ptgAttrSum = 0x1019,
    ptgAttrBaxcel_1 = 0x2019,
    ptgAttrBaxcel_2 = 0x2119,
    ptgAttrSpace = 0x4019,
    ptgAttrSpaceSemi = 0x4119,

    ptgErr = 0x1C,
    ptgBool = 0x1D,
    ptgInt = 0x1E,
    ptgNum = 0x1F,
    ptgArray = 0x20,
    ptgFunc = 0x21,
    ptgFuncVar = 0x22,
    ptgName = 0x23,
    ptgRef = 0x24,
    ptgArea = 0x25,
    ptgMemArea = 0x26,
    ptgMemErr = 0x27,
    ptgMemNoMem = 0x28,
    ptgMemFunc = 0x29,
    ptgRefErr = 0x2A,
    ptgAreaErr = 0x2B,
    ptgRefN = 0x2C,
    ptgAreaN = 0x2D,
    ptgNameX = 0x39,
    ptgRef3d = 0x3A,
    ptgArea3d = 0x3B,
    ptgRefErr3d = 0x3C,
    ptgAreaErr3d = 0x3D,
    ptgArrayV = 0x40,
    ptgFuncV = 0x41,
    ptgFuncVarV = 0x42,
    ptgNameV = 0x43,
    ptgRefV = 0x44,
    ptgAreaV = 0x45,
    ptgMemAreaV = 0x46,
    ptgMemErrV = 0x47,
    ptgMemNoMemV = 0x48,
    ptgMemFuncV = 0x49,
    ptgRefErrV = 0x4A,
    ptgAreaErrV = 0x4B,
    ptgRefNV = 0x4C,
    ptgAreaNV = 0x4D,
    ptgNameXV = 0x59,
    ptgRef3dV = 0x5A,
    ptgArea3dV = 0x5B,
    ptgRefErr3dV = 0x5C,
    ptgAreaErr3dV = 0x5D,
    ptgArrayA = 0x60,
    ptgFuncA = 0x61,
    ptgFuncVarA = 0x62,
    ptgNameA = 0x63,
    ptgRefA = 0x64,
    ptgAreaA = 0x65,
    ptgMemAreaA = 0x66,
    ptgMemErrA = 0x67,
    ptgMemNoMemA = 0x68,
    ptgMemFuncA = 0x69,
    ptgRefErrA = 0x6A,
    ptgAreaErrA = 0x6B,
    ptgRefNA = 0x6C,
    ptgAreaNA = 0x6D,
    ptgNameXA = 0x79,
    ptgRef3dA = 0x7A,
    ptgArea3dA = 0x7B,
    ptgRefErr3dA = 0x7C,
    ptgAreaErr3dA = 0x7D,

    ptgUnknown = 0x7F,
} ptg_expr;

typedef enum {
    msoblip_ERROR = 0x00, // Error reading the file.
    msoblip_UNKNOWN = 0x01, // Unknown BLIPtype.
    msoblip_EMF = 0x02, // EMF.
    msoblip_WMF = 0x03, // WMF.
    msoblip_PICT = 0x04, // Macintosh PICT.
    msoblip_JPEG = 0x05, // JPEG.
    msoblip_PNG = 0x06, // PNG.
    msoblip_DIB = 0x07, // DIB
    msoblip_TIFF = 0x11, // TIFF
    msoblip_CMYKJPEG = 0x12, // JPEG in the YCCK or CMYK color space.
} mso_blip_type;

static const ptg_func_t* get_function_item(unsigned index) {
    if (index < sizeof(FUNCTIONS) / sizeof(FUNCTIONS[0])) {
        return &FUNCTIONS[index];
    }
    else if (index >= 0x8000 && (index - 0x8000 < sizeof(EXTENDED_FUNCTIONS) / sizeof(EXTENDED_FUNCTIONS[0]))) {
        /*
        * the highest bit is fCeFunc flag.
        * If fCeFunc is 1, then this field specifies a Cetab value.
        * If fCeFunc is 0, then this field specifies a Ftab value.
        */
        return &EXTENDED_FUNCTIONS[index - 0x8000];
    }
    return nullptr;
}

int decrypt_xlm_record_inplace(const bin_encryption_t* encrypt, const char16_t* passwd, uint32_t offset, uint8_t* data, uint32_t size, uint32_t* processed, bool little_endian) {
    if (nullptr == encrypt || data == nullptr || size < sizeof(biff::record_header_t)) {
        return BQ::INVALID_ARG;
    }

    biff::record_header_t rcd;
    rcd.opcode = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data), little_endian);
    rcd.length = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + sizeof(rcd.opcode)), little_endian);
    if (rcd.length > size) {
        return BQ::ERR_FORMAT_XLM;
    }
    uint32_t rcd_size = rcd.length + sizeof(biff::record_header_t);

    int nret = BQ::OK;
    uint8_t cache[sizeof(biff::record_header_t) + sizeof(uint32_t)] = { 0 };
    uint32_t decrypt_start = sizeof(biff::record_header_t);
    switch (rcd.opcode) {
        case biff::OPC_FilePass: {
        case biff::OPC_BOF:
        case biff::OPC_UsrExcl:
        case biff::OPC_FileLock:
        case biff::OPC_RRDInfo:
        case biff::OPC_RRDHead:
        case biff::OPC_InterfaceHdr: {
            decrypt_start += rcd.length;
            break;
        }
        case biff::OPC_BoundSheet: {
            decrypt_start += sizeof(uint32_t);
            break;
        }
        default:
            break;
        }
    }
    
    crypto_binary::block_info_t block;
    for (uint32_t pos = 0; decrypt_start < rcd_size && pos < rcd_size; pos += rcd_size) {
        memcpy(cache, data, decrypt_start);
        block.offset_relative = (offset + pos);
        block.xor_array_index = offset + pos + rcd.length;
        nret = crypto_binary().decrypt_inplace(encrypt, passwd, data, rcd_size, &block);
        if (nret != BQ::OK) {
            break;
        }
        memcpy(data, cache, decrypt_start);
    }

    if (BQ::OK == nret && rcd_size) {
        *processed = rcd_size;
    }
    return nret;
}

int xlm::decrypt_xlm_inplace(const bin_encryption_t* encrypt, const char16_t* passwd, uint8_t* buffer, uint32_t size, bool little_endian) {
    if (nullptr == encrypt || buffer == nullptr || size < sizeof(biff::record_header_t)) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    biff::record_header_t rcd;

    if (encrypt->type == encryinfo_bin_xor_m1) {
        for (uint32_t i = 0, rcd_size = 0; i + sizeof(biff::record_header_t) <= size; i += rcd_size) {
            nret = decrypt_xlm_record_inplace(encrypt, passwd, i, buffer + i, size - i, &rcd_size, little_endian);
            if (nret != BQ::OK) {
                break;
            }
        }
        return nret;
    }

    /*
    * 
    * optimization for RC4 and RC4 CAPI
    * 
    */
    uint8_t* decrypted = (uint8_t*)malloc(size);
    if (nullptr == decrypted) {
        return BQ::ERR_MALLOC;
    }
    ON_SCOPE_EXIT([&]() { if (decrypted) free(decrypted); });
    memcpy(decrypted, buffer, size);

    for (uint32_t pos = 0; pos + sizeof(biff::record_header_t) <= size; ) {
        rcd.opcode = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(decrypted + pos), little_endian);
        rcd.length = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(decrypted + pos + sizeof(rcd.opcode)), little_endian);

        memset(decrypted + pos, 0, sizeof(biff::record_header_t));
        pos += sizeof(biff::record_header_t);

        if (pos + rcd.length > size) {
            nret = BQ::ERR_FORMAT;
            break;
        }

        switch (rcd.opcode) {
            case biff::OPC_FilePass: {
            case biff::OPC_BOF:
            case biff::OPC_UsrExcl:
            case biff::OPC_FileLock:
            case biff::OPC_RRDInfo:
            case biff::OPC_RRDHead:
            case biff::OPC_InterfaceHdr: {
                memset(decrypted + pos, 0, rcd.length);
                break;
            }
            case biff::OPC_BoundSheet: {
                memset(decrypted + pos, 0, sizeof(uint32_t));
                break;
            }
            default:
                break;
            }
        }
        pos += rcd.length;
    }

    if (nret != BQ::OK) {
        return nret;
    }

    nret = crypto_binary().decrypt_inplace(encrypt, passwd, decrypted, size);
    if (nret != BQ::OK) {
        return nret;
    }

    for (uint32_t pos = 0; pos + sizeof(biff::record_header_t) <= size; ) {
        rcd.opcode = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(buffer + pos), little_endian);
        rcd.length = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(buffer + pos + sizeof(rcd.opcode)), little_endian);
        pos += sizeof(biff::record_header_t);

        if (pos + rcd.length > size) {
            nret = BQ::ERR_FORMAT;
            break;
        }

        switch (rcd.opcode) {
            case biff::OPC_FilePass:
            case biff::OPC_BOF:
            case biff::OPC_UsrExcl:
            case biff::OPC_FileLock:
            case biff::OPC_RRDInfo:
            case biff::OPC_RRDHead:
            case biff::OPC_InterfaceHdr: {
                break;
            }
            case biff::OPC_BoundSheet: {
                uint32_t offset = pos + sizeof(uint32_t);
                memcpy(buffer + offset, decrypted + offset, rcd.length - sizeof(uint32_t));
                break;
            }
            default: {
                memcpy(buffer + pos, decrypted + pos, rcd.length);
                break;
            }
        }
        pos += rcd.length;
    }

    return nret;
}

int xlm_parse_function(uint32_t func_id, uint8_t argc, std::stack<xl_bin_cell_t::val_t>& evaluation_stack, bool deobfuscation) {
    auto func_item = get_function_item(func_id);
    if (func_item == nullptr) {
        return BQ::INVALID_ARG;
    }

    std::string fun_info;
    if (deobfuscation) {
        bool processed = false;
        switch (func_id)
        {
        case 111: { // char
            if (evaluation_stack.size() > 0) {
                auto tmp = evaluation_stack.top().evaluation;
                char* _eptr = nullptr;
                long arg = strtol(tmp.c_str(), &_eptr, 10); // std::stoi()
                if (_eptr == tmp.c_str()) {
                    break;
                }
                if (arg >= 0 && arg <= 255) {
                    processed = true;
                    evaluation_stack.pop();
                    fun_info = char(arg);
                }
            }
            break;
        }
        case 88: {
            if (evaluation_stack.size() > 1) {
                auto r = evaluation_stack.top().evaluation; evaluation_stack.pop();
                auto l = evaluation_stack.top().evaluation; evaluation_stack.pop();
                fun_info = l + "=" + r;
                processed = true;
            }
        }
        default:
            break;
        }
        if (processed) {
            evaluation_stack.push(fun_info);
            return BQ::OK;
        }
    }

    uint8_t argc_min = argc == 0xFF ? func_item->argc_min : argc;
    uint8_t argc_max = argc == 0xFF ? func_item->argc_max : argc;

    std::vector<std::string> args;
    for (int16_t i = 0; i < argc_max && !evaluation_stack.empty(); i++, evaluation_stack.pop()) {
        assert(evaluation_stack.top().is_str());
        args.push_back(evaluation_stack.top().evaluation);
    }
    assert((argc == 0xFF && argc_min <= argc_max) || (argc <= func_item->argc_max && argc >= func_item->argc_min));

    std::string func_name = (func_item->name == nullptr ? "<unknown function>" : func_item->name);
    if (func_id == 255) {
        func_name = args[args.size() - 1];
    }

    for (int32_t i = args.size() - (func_id == 255 ? 2 : 1); i >= 0; i--) {
        fun_info += (fun_info.empty() ? "" : ",") + args[i];
    }
    fun_info = func_name + "(" + fun_info + ")";
    if (args.size() < (size_t)func_item->argc_min) {
        fun_info += "' invalid argc";
    }
    evaluation_stack.push(fun_info);

    return BQ::OK;
}

int xlm_parse_formula(const cfb_ctx_t* ctx, xl_bin_datas_t* xl_bin, xl_bin_cell_t* cell, bool force_val_str);

xl_bin_cell_t* find_sheet_cell(xl_bin_sheet_t* sheet, uint16_t row, uint16_t col) {
    if (sheet == nullptr) {
        return nullptr;
    }

    xl_bin_cell_t* target_cell = nullptr;
    for (uint32_t i = 0; i < sheet->cells.size(); i++) {
        auto cur_cell = &sheet->cells[i];
        if (cur_cell->col == col && cur_cell->row == row) {
            target_cell = cur_cell;
            break;
        }
    }

    return target_cell;
}

int parse_xl_cell(const cfb_ctx_t* ctx, xl_bin_datas_t* xl, uint32_t sheet_id, uint32_t pos, uint32_t* rcd_len, xl_bin_cell_t** out=nullptr) {
    if (xl == nullptr || xl->data == nullptr || xl->size == 0 || sheet_id >= xl->sheets.size()) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    bool little_endian = xl->little_endian;
    bool deobfuscation = ctx ? ctx->xlm_deobfuscation : false;
    auto cur_sheet = &xl->sheets[sheet_id];

    biff::record_header_t rcd;
    xlm_records(little_endian).read_header(xl->data + pos, sizeof(rcd), rcd);
    if (rcd.length > BIFF8_MAX_RECORD_DATA_LENGTH || pos + rcd.length > xl->size) {
        return BQ::ERR_BIFF_LEN;
    }
    
    auto rcd_data = xl->data + pos + sizeof(rcd);
    auto rcd_size = (uint32_t)rcd.length;

    xl_bin_cell_t* xl_cell = nullptr;
    auto pre_install_cell = [](xl_bin_sheet_t* sheet, uint16_t r, uint16_t c) -> xl_bin_cell_t* {
        if (sheet == nullptr) {
            return nullptr;
        }
        xl_bin_cell_t* exist = nullptr;
        if (r != -1 && c != -1) {
            exist = find_sheet_cell(sheet, r, c);
        }
        if (nullptr == exist) {
            xl_bin_cell_t xl_cell;
            xl_cell.row = r;
            xl_cell.col = c;
            sheet->cells.push_back(xl_cell);
            exist = &sheet->cells[sheet->cells.size() - 1];
        }
        return exist;
    };
    
    switch (rcd.opcode) {
    case biff::OPC_Blank: {
        break;
    }
    case biff::OPC_Number: {
        break;
    }
    case biff::OPC_BoolErr: {
        break;
    }
    case biff::OPC_Label: {
        break;
    }
    case biff::OPC_LabelSst: {
        constexpr uint32_t size_min = sizeof(label_sst_t);
        if (rcd_size < size_min) {
            nret = BQ::ERR_XL_CELL_VAL;
            break;
        }
        label_sst_t label_sst;
        memcpy(&label_sst, rcd_data, sizeof(label_sst));
        label_sst.cell.row = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)&label_sst.cell.row, little_endian);
        label_sst.cell.col = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)&label_sst.cell.col, little_endian);
        label_sst.cell.ixfe = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)&label_sst.cell.ixfe, little_endian);
        label_sst.isst = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)&label_sst.isst, little_endian);

        if (label_sst.isst >= xl->sst.size()) {
            break;
        }

        xl_cell = pre_install_cell(cur_sheet, label_sst.cell.row, label_sst.cell.col);
        xl_cell->col = label_sst.cell.col;
        xl_cell->row = label_sst.cell.row;
        xl_cell->ixfe = label_sst.cell.ixfe;
        xl_cell->val = xl->sst[label_sst.isst];
        break;
    }
    case biff::OPC_Array: {
        record_array_t array_rcd;
        int processed = xlm_records().read_Array(rcd_data, rcd_size, array_rcd);
        if (processed <= 0) {
            break;
        }

        xl_cell = pre_install_cell(cur_sheet, -1, -1);
        xl_cell->record_type = biff::OPC_Array;
        xl_cell->row = 0;
        xl_cell->col = 0;
        xl_cell->ixfe = 0;
        xl_cell->sheet_id = cur_sheet->sheet_id;
        xl_cell->val.raw = array_rcd.formula.rgce_rgcb;
        nret = xlm_parse_formula(ctx, xl, xl_cell, false);
        if (nret != BQ::OK) {
            auto sheet_name = cur_sheet->sheet_id < xl->sheets.size() ? xl->sheets[cur_sheet->sheet_id].bs.name : "";
            MESSAGE_WARN(ctx, "[parse_xlm] OPC_Array xlm_parse_formula(%s R%uC%u) error\n", sheet_name, array_rcd.ref.rwFirst, array_rcd.ref.colFirst);
        }
        break;
    }
    case biff::OPC_ShrFmla: {
        shrfmla_item_t shrfmla;
        shrfmla.sheet_id = cur_sheet->sheet_id;
        xlm_records().read_ShrFmla(rcd_data, rcd_size, shrfmla.data);
        xl->shareds.push_back(shrfmla);
        break;
    }
    case biff::OPC_Formula: {
        record_formula_t formula;
        xlm_records().read_Formula(rcd_data, rcd_size, formula);

        uint32_t shrfmla_index = 0, array_index = 0;
        do {
            if (formula.formula.rgce_rgcb.size < 5) {
                break;
            }
            auto rgce_data = formula.formula.rgce_rgcb.data;
            uint8_t ptg = uint8_t((*rgce_data) & 0x7f);
            if (ptg != ptgExp) {
                break;
            }
            uint16_t ptg_row = rgce_data[1] | (rgce_data[2] << 8);
            uint16_t ptg_col = rgce_data[3] | (rgce_data[4] << 8);
            if (ptg_col != formula.col || ptg_row != formula.row || 0 == (ptg_row + ptg_col)) {
                break;
            }

            /*
            * if current Formula record is PtgExp information
            * , it MUST be followed by either a ShrFmla record or an Array record
            */
            biff::record_header_t header;
            if (xl->size <= rcd_size + pos + sizeof(header)) {
                break;
            }
            xlm_records().read_header(rcd_data + rcd_size, sizeof(header), header);
            if (xl->size <= pos + rcd_size + sizeof(header) + header.length) {
                break;
            }

            if (header.opcode == biff::OPC_ShrFmla) {
                assert(formula.fShrFmla() == true);
                shrfmla_item_t shrfmla_item;
                xlm_records().read_ShrFmla(rcd_data, rcd_size, shrfmla_item.data);
                shrfmla_item.sheet_id = cur_sheet->sheet_id;
                shrfmla_item.val.raw = shrfmla_item.data.formula.rgce_rgcb;
                xl->shareds.push_back(shrfmla_item);
                shrfmla_index = xl->shareds.size() - 1;
            }
            else if (header.opcode == biff::OPC_Array) {
                record_array_t array_rcd;
                xlm_records().read_Array(rcd_data, rcd_size, array_rcd);
                // TODO: 
            }
            else {
                nret = BQ::ERR_FORMAT;
                rcd_size = xl->size;
                break;
            }
            rcd_size += sizeof(header) + header.length;
        } while (false);

        std::string str_value;
        if (formula.val.is_blank()) {
            str_value = " ";
        }
        else if (formula.val.is_str()) {
            biff::record_header_t header;
            if (xl->size <= rcd_size + pos + sizeof(header)) {
                break;
            }
            xlm_records().read_header(rcd_data + rcd_size, sizeof(header), header);
            if (xl->size <= pos + rcd_size + sizeof(header) + header.length) {
                break;
            }
            if (header.opcode != biff::OPC_String) {
                nret = BQ::ERR_FORMAT;
                rcd_size = xl->size;
                break;
            }

            int processed = xlm_records().read_String(ctx, rcd_data + rcd_size + sizeof(header), header.length, str_value, true);
            if (processed > 0) {
                // 
            }
            rcd_size += sizeof(header) + header.length;
        }

        xl_cell = pre_install_cell(cur_sheet, formula.row, formula.col);
        xl_cell->row = formula.row;
        xl_cell->col = formula.col;
        xl_cell->ixfe = formula.ixfe;
        xl_cell->sheet_id = cur_sheet->sheet_id;
        xl_cell->record_type = biff::OPC_Formula;
        xl_cell->val.raw = formula.formula.rgce_rgcb;
        xl_cell->is_shrfmla = formula.fShrFmla();

        if (!str_value.empty()) {
            xl_cell->val.evaluation = str_value;
            if (formula.fShrFmla() && shrfmla_index > 0) {
                xl->shareds[shrfmla_index].val = str_value;
            }
        }
        else {
            nret = xlm_parse_formula(ctx, xl, xl_cell, false);
            if (nret != BQ::OK) {
                auto sheet_name = cur_sheet->sheet_id < xl->sheets.size() ? xl->sheets[cur_sheet->sheet_id].bs.name : "";
                MESSAGE_WARN(ctx, "[parse_xlm] xlm_parse_formula(%s R%uC%u) error \n", sheet_name, formula.row, formula.col);
            }
        }
        break;
    }
    case biff::OPC_String: {
        /* specifies the string value of a formula */
        break;
    }
    default: {
        nret = BQ::INVALID_ARG;
    }
    }

    if (xl_cell && out) {
        *out = xl_cell;
    }
    
    if (rcd_len) {
        *rcd_len = rcd_size;
    }

    return nret;
}

int xlm_search_cell(const cfb_ctx_t* ctx, xl_bin_datas_t* xl, uint32_t sheet_id, uint16_t row, uint16_t col, xl_bin_cell_t::val_t* val) {
    if (xl == nullptr || xl->data == nullptr || xl->size == 0 || sheet_id >= xl->sheets.size() || val == nullptr) {
        return BQ::INVALID_ARG;
    }
    
    int nret = BQ::OK;
    auto sheet = &xl->sheets[sheet_id];

    xl_bin_cell_t* target_cell = find_sheet_cell(sheet, row, col);
    if (target_cell) {
        *val = target_cell->val;
        return nret;
    }

    biff::record_header_t rcd;
    uint32_t cell_pos = 0, cell_bytes = 0;

    for (uint32_t pos = sheet->bs.lbPlyPos, rcd_size = 0, size = xl->size, beof_cnt = 0; pos < size; pos += rcd_size) {
        xlm_records().read_header(xl->data + pos, sizeof(rcd), rcd);
        if (rcd.length > BIFF8_MAX_RECORD_DATA_LENGTH || pos + rcd.length > size) {
            nret = BQ::ERR_BIFF_LEN;
            break;
        }

        auto rcd_data = xl->data + (pos += sizeof(biff::record_header_t));
        rcd_size = rcd.length;

        switch (rcd.opcode) {
            case biff::OPC_BOF:
            case biff::OPC_EOF: {
                if (++beof_cnt > 1) {
                    rcd_size = size;
                }
                break;
            }
            case biff::OPC_Array:
            case biff::OPC_ShrFmla: {
                xlm_refu_t refu;
                xlm_records(xl->little_endian).read_RefU(rcd_data, rcd_size, refu);
                if (row >= refu.rwFirst && row <= refu.rwLast && col >= refu.colFirst && col <= refu.colLast) {
                    cell_pos = pos;
                }
                break;
            }
            case biff::OPC_Blank:
            case biff::OPC_Number:
            case biff::OPC_BoolErr:
            case biff::OPC_Label:
            case biff::OPC_LabelSst:
            case biff::OPC_Formula: {
                xlm_cell_t cell;
                xlm_records(xl->little_endian).read_Cell(rcd_data, rcd_size, cell);
                if (cell.col == col && cell.row == row) {
                    cell_pos = pos;
                }
                break;
            }
            default: {
                break;
            }
        }

        if (cell_pos) {
            cell_pos -= sizeof(biff::record_header_t);
            cell_bytes = size - pos;
            break;
        }
    }

    if (nret != BQ::OK) {
        return nret;
    }

    if (0 == cell_pos) {
        /*
        * there is no the target cell. 
        * i think the value of the cell should be empty string.
        */
        *val = "";
        return BQ::OK;
    } 

    nret = parse_xl_cell(ctx, xl, sheet_id, cell_pos, nullptr, &target_cell);
    if (nret != BQ::OK) {
        return nret;
    }

    if (target_cell) {
        *val = target_cell->val;
    }
    else {
        return BQ::ERR_XL_CELL_VAL;
    }
    return nret;
}

int xlm_parse_refs(const cfb_ctx_t* ctx, xl_bin_datas_t* xl, const xl_bin_cell_t* cell, const xti_index_t* ixti, uint16_t row, uint16_t col, xl_bin_cell_t::val_t* val) {
    if (xl == nullptr || ixti == nullptr || !ixti->validable()) {
        return BQ::INVALID_ARG;
    }
    /*
    * ixti ---> XTI ---> SupBook
    *   SupBook 确定 supporting link 的类型等信息，如 是否是 self-reference 等
    *   XTI 确定 supporting link 的作用域，如，当是 self-reference 时，引用的 BoundSheet8 的 zero-based 的索引
    */
    if (ixti->index >= xl->xtis.size()) {
        return BQ::INVALID_ARG;
    }
    auto cur_xti = &xl->xtis[ixti->index];
    if (cur_xti->isupbook >= xl->supbooks.size()) {
        return BQ::INVALID_ARG;
    }
    auto cur_supbook = &xl->supbooks[cur_xti->isupbook];
    if (cur_supbook->cch == 0x0401) {
        //  self-referencing
    }
    else if (cur_supbook->cch == 0x3A01) {
        // add-in referencing 
    }
    else if (cur_supbook->cch == 0x0001 || cur_supbook->cch == 0x00ff) {
        /*
        * The type of supporting link specified by this record is specified by virtPath.
        * This value is the count of characters in virtPath.
        */
    }
    else {
        return BQ::INVALID_ARG;
    }

    if (cur_supbook->cch != 0x0401) {
        return BQ::NOT_IMPLEMENT;
    }

    if (cur_xti->itab_first < 0 || (uint32_t)cur_xti->itab_first >= xl->sheets.size()) {
        /*
        * If the supporting link type is self-referencing, then this value 
        * specifies the zero-based index of a BoundSheet8 record in the Globals Substream 
        */
        return BQ::INVALID_ARG;
    }

    auto col_num2str = [](uint32_t col_zero_base) -> std::string {
        std::string rst;
        for (uint32_t col = col_zero_base+1, base = 26; col; col /= base) {
            char cur = 'A' + (col % (base+1));
            rst = std::string(1, cur) + rst;
        }
        return rst;
    };

    uint32_t sheet_id = cur_xti->itab_first;
    int nret = xlm_search_cell(ctx, xl, sheet_id, row, col, val);
    if (nret != BQ::OK && val && sheet_id < xl->sheets.size()) {
        std::string cell_pos = xl->sheets[sheet_id].bs.name;
        cell_pos += "!" + col_num2str(col) + std::to_string(row);
        *val = cell_pos;
        nret = BQ::OK;
    }

    return nret;
};

int xlm_parse_formula(const cfb_ctx_t* ctx, xl_bin_datas_t* xl_bin, xl_bin_cell_t* cell, bool force_val_str = false) {
    if (xl_bin == nullptr || nullptr == cell || nullptr == cell->val.raw.data || 0 == cell->val.raw.size) {
        return BQ::INVALID_ARG;
    }
    bool little_endian = xl_bin->little_endian;
    bool deobfuscation = ctx ? ctx->xlm_deobfuscation : false;

    int nret = BQ::ERR;
    auto data = cell->val.raw.data;
    auto data_size = cell->val.raw.size;
    auto& sheet_cells = xl_bin->sheets[cell->sheet_id].cells;
    
    const size_t buffer_size = 514;
    char buffer[buffer_size] = { 0 };
    std::stack<xl_bin_cell_t::val_t> evaluation_stack;

    auto concat_operator_2operands = [](const char* token_symbol, std::stack<xl_bin_cell_t::val_t>& stack) {
        if (stack.size() < 2) {
            return;
        }
        auto a = stack.top(); stack.pop();
        auto b = stack.top(); stack.pop();
        if (a.is_str() && b.is_str()) {
            stack.push(b.evaluation + (token_symbol ? token_symbol : "[operator_unknown]") + a.evaluation);
        }
        else {
            stack.push(a); stack.push(token_symbol); stack.push(b);
        }
    };

    unsigned data_pos = 0;
    const ptg_item_t* curr_ptgitem = nullptr;
    const ptg_item_t* last_ptgitem = nullptr;
    while (data_pos < data_size) {
        /* ptg_expr */
        uint8_t ptg = uint8_t(data[data_pos] & 0x7f);

        last_ptgitem = curr_ptgitem;
        if (((uint8_t)data[data_pos]) < sizeof(ptg_items) / sizeof(ptg_items[0])) {
            curr_ptgitem = &ptg_items[ptg];
        }
        else {
            curr_ptgitem = nullptr;
        }

        switch (ptg) {
        case ptgAdd: {
            concat_operator_2operands("+", evaluation_stack);
            data_pos += 1;
            break;
        }
        case ptgSub: {
            concat_operator_2operands("-", evaluation_stack);
            data_pos += 1;
            break;
        }
        case ptgMul: {
            concat_operator_2operands("*", evaluation_stack);
            data_pos += 1;
            break;
        }
        case ptgDiv: {
            concat_operator_2operands("/", evaluation_stack);
            data_pos += 1;
            break;
        }
        case ptgPower: {
            concat_operator_2operands("^", evaluation_stack);
            data_pos += 1;
            break;
        }
        case ptgConcat: {
            if (!deobfuscation || (last_ptgitem && last_ptgitem->type == token_func_call)) {
                concat_operator_2operands("&", evaluation_stack);
            }
            else {
                concat_operator_2operands("", evaluation_stack);
            }
            data_pos += 1;
            break;
        }
        case ptgLt: {
            concat_operator_2operands("<", evaluation_stack);
            data_pos += 1;
            break;
        }
        case ptgLe: {
            concat_operator_2operands("<=", evaluation_stack);
            data_pos += 1;
            break;
        }
        case ptgEq: {
            concat_operator_2operands("==", evaluation_stack);
            data_pos += 1;
            break;
        }
        case ptgGe: {
            concat_operator_2operands(">=", evaluation_stack);
            data_pos += 1;
            break;
        }
        case ptgGt: {
            concat_operator_2operands(">", evaluation_stack);
            data_pos += 1;
            break;
        }
        case ptgNe: {
            concat_operator_2operands("<>", evaluation_stack);
            data_pos += 1;
            break;
        }
        case ptgIsect: {
            /* token_symbol may be " ", i am not sure. */
            concat_operator_2operands("∩", evaluation_stack);
            data_pos += 1;
            break;
        }
        case ptgUnion: {
            /* token_symbol may be "," */
            concat_operator_2operands("∪", evaluation_stack);
            data_pos += 1;
            break;
        }
        case ptgRange: {
            concat_operator_2operands(":", evaluation_stack);
            data_pos += 1;
            break;
        }
        case ptgUplus: {
            if (evaluation_stack.size() > 0) {
                auto& a = evaluation_stack.top(); evaluation_stack.pop();
                assert(a.is_str());
                evaluation_stack.push("+" + a.evaluation);
            }
            data_pos += 1;
            break;
        }
        case ptgUminus: {
            if (evaluation_stack.size() > 0) {
                auto a = evaluation_stack.top(); evaluation_stack.pop();
                assert(a.is_str());
                evaluation_stack.push("-" + a.evaluation);
            }
            data_pos += 1;
            break;
        }
        case ptgPercent: {
            if (evaluation_stack.size() > 0) {
                auto a = evaluation_stack.top(); evaluation_stack.pop();
                if (a.is_str()) {
                    evaluation_stack.push(a.evaluation + "%");
                }
                else {
                    /* it should not happen */
                    evaluation_stack.push(a);
                    evaluation_stack.push("%");
                }
            }
            data_pos += 1;
            break;
        }
        case ptgMissArg: {
            evaluation_stack.push("[missarg]");
            data_pos += 1;
            break;
        }
        /* display */
        case ptgParen: {
            auto a = evaluation_stack.top(); evaluation_stack.pop();
            if (a.is_str()) {
                evaluation_stack.push("(" + a.evaluation + ")");
            }
            else {
                /* it should not happen */
                evaluation_stack.push("(");
                evaluation_stack.push(a);
                evaluation_stack.push(")");
            }
            data_pos += 1;
            break;
        }
        case ptgAttr: {
            if (data_pos + 1 >= data_size) {
                goto done;
            }

            uint8_t second = data[data_pos + 1];
            if (second >= sizeof(ptg_attr_subs) / sizeof(ptg_attr_subs[0])) {
                data_pos += 4;
                break;
            }

            auto attr_item = ptg_attr_subs[second];
            if (attr_item.token == nullptr) {
                data_pos += 4;
                break;
            }

            /* control tokens are ignored when converting parsed expressions into textual formulas */
            switch ((second << 8) | ptgAttr) {
            case ptgAttrSpace:
            case ptgAttrSpaceSemi:
                /* just ignore */
                break;
            case ptgAttrSemi:
                /* ignore */
                break;
            case ptgAttrSum: {
                auto a = evaluation_stack.top(); evaluation_stack.pop();
                assert(a.is_str());
                evaluation_stack.push("SUM(" + a.evaluation + ")");
                break;
            }
            case ptgAttrBaxcel_1:
            case ptgAttrBaxcel_2:
                break;
            case ptgAttrChoose: {
                if (data_pos + 3 >= data_size) {
                    break;
                }
                uint16_t coffset = data[data_pos + 2] | (data[data_pos + 3] << 8);
                data_pos += 2 * (coffset + 1);
            }
            default:
                break;
            }
            data_pos += 4;
            break;
        }
        /* operand */
        case ptgRefErr: {
            evaluation_stack.push("#REF!");
            data_pos += 5;
            break;
        }
        case ptgStr: {
            if (data_pos + 3 > data_size) {
                data_pos += 3;
                break;
            }

            /* the count of characters */
            uint8_t cch = data[data_pos + 1];
            uint8_t fHighByte = data[data_pos + 2];
            uint32_t max_size = data_size - data_pos - 3;

            if (cch == 0) {
                data_pos += 3;
            }
            else if (fHighByte == 0x01) {
                uint8_t* utf8 = NULL;
                uint32_t utf8_size = 0;
                ON_SCOPE_EXIT([&] { if (utf8) free(utf8); });

                if (max_size > (uint32_t)cch * 2) {
                    max_size = cch * 2;
                }

                std::u16string str_val;
                for (uint32_t i = 0; i < max_size; i += 2) {
                    str_val += std::u16string(1, *(const char16_t*)(data + data_pos + 3 + i));
                }
                std::string str_utf = XsUtils::u16s2utf(str_val);
                if (!str_val.empty() && str_utf.empty()) {
                    evaluation_stack.push("<Failed to decode UTF16LE string>");
                }
                else {
                    evaluation_stack.push(str_utf);
                }

                data_pos += 3 + max_size;
            }
            else if (fHighByte == 0x00) {
                memset(buffer, 0, buffer_size);
                if (max_size > cch) {
                    max_size = cch;
                }
                memcpy(buffer, data + data_pos + 3, max_size);
                evaluation_stack.push(buffer);

                data_pos += 3 + max_size;
            }
            else if (data_pos + 1 + cch <= data_size) {
                /*
                * try to get single-character string
                *
                * it's so sad, microsoft may do not use the fHighByte field.
                * when this happened, the string should be treated as singe-byte characters.
                */
                memset(buffer, 0, buffer_size);
                memcpy(buffer, data + data_pos + 2, cch);
                evaluation_stack.push(buffer);

                data_pos += 2 + cch;
            }
            else {
                goto done;
            }
            break;
        }
        case ptgBool: {
            if (data_pos + 1 >= data_size) {
                goto done;
            }

            evaluation_stack.push(data[data_pos + 1] ? "TRUE" : "FALSE");
            data_pos += 2;
            break;
        }
        case ptgInt: {
            if (data_pos + 2 >= data_size) {
                goto done;
            }

            evaluation_stack.push(std::to_string(data[data_pos + 1] | (data[data_pos + 2] << 8)));
            data_pos += 3;
            break;
        }
        case ptgName:
        case ptgNameV: {
            if (data_pos + 4 >= data_size) {
                goto done;
            }

            /* specifies a one-based index of a Lbl record in the collection of Lbl records in the Globals Substream */
            uint32_t nameindex = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + data_pos + 1), little_endian);

            if (nameindex > 0 && nameindex <= xl_bin->lbls.size()) {
                evaluation_stack.push(xl_bin->lbls[nameindex - 1].name);
            }
            else {
                memset(buffer, 0, buffer_size);
                sprintf(buffer, "NAME_IDX  0x%08x", nameindex);
                evaluation_stack.push(buffer);
            }

            data_pos += 5;
            break;
        }
        case ptgNum: {
            if (data_pos + 8 >= data_size) {
                goto done;
            }

            double val = *(double*)(data + data_pos + 1);
            evaluation_stack.push(std::to_string(val));

            data_pos += 9;
            break;
        }
        case ptgArrayA:
        case ptgArrayV:
        case ptgArray: {
            data_pos += 15;
            break;
        }
        /* function */
        case ptgFuncVar:
        case ptgFuncVarV:
        case ptgFuncVarA: {
            if (data_pos + 3 >= data_size) {
                goto done;
            }
            uint8_t cparams = data[data_pos + 1];
            uint16_t func_id = data[data_pos + 2] | (data[data_pos + 3] << 8);
            xlm_parse_function(func_id, cparams, evaluation_stack, deobfuscation);

            data_pos += 4;
            if (func_id == 0x806d) {
                data_pos += 9;
            }
            break;
        }
        case ptgFunc:
        case ptgFuncV:
        case ptgFuncA: {
            if (data_pos + 2 >= data_size) {
                goto done;
            }

            uint16_t func_id = data[data_pos + 1] | (data[data_pos + 2] << 8);
            xlm_parse_function(func_id, -1, evaluation_stack, deobfuscation);

            data_pos += 3;
            break;
        }
        /* mem token */
        case ptgMemArea: {
            if (data_pos + 6 >= data_size) {
                goto done;
            }
            evaluation_stack.push(" REFERENCE-EXPRESSION");
            data_pos += 7;
            break;
        }
        /*
        * reference
        */
        case ptgRef:
        case ptgRefV: {
            if (data_pos + 4 >= data_size) {
                goto done;
            }

            uint8_t ptg_data_type = (data[data_pos] >> 5) & 0x3;
            uint16_t row = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + data_pos + 1), little_endian);
            uint16_t col = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + data_pos + 3), little_endian);
            bool col_relative_reference = (col >> 14) & 0x1;
            bool row_relative_reference = (col >> 15) & 0x1;
            row &= 0xFFFF, col &= 0x00FF;

            const xl_bin_cell_t::val_t* find_ref_cell = nullptr;
            if (ptg_data_type == 0x02) /* 0x02 value; 0x01 reference; 0x03 an array of values; */ {
                for (size_t i = 0; i < sheet_cells.size(); i++) {
                    if (sheet_cells[i].col == col && sheet_cells[i].row == row) {
                        find_ref_cell = &sheet_cells[i].val;
                        break;
                    }
                }
                if (find_ref_cell && find_ref_cell->is_str()) {
                    if (find_ref_cell->evaluation.empty()) {
                        memset(buffer, 0, buffer_size);
                        sprintf(buffer, "R%uC%u", row, col);
                        evaluation_stack.push(buffer);
                    }
                    else {
                        evaluation_stack.push(*find_ref_cell);
                    }
                }
                else if (force_val_str) {
                    if (deobfuscation) {
                        evaluation_stack.push("");
                    }
                    else {
                        memset(buffer, 0, buffer_size);
                        sprintf(buffer, "R%uC%u", row, col);
                        evaluation_stack.push(buffer);
                    }
                }
                else {
                    std::stack<xl_bin_cell_t::val_t>().swap(evaluation_stack);
                    evaluation_stack.push(xl_bin_cell_t::val_t(data, data_size));
                    /* break the while loop */
                    data_pos = data_size;
                }
            }
            else {
                /* TODO: */
                assert((ptg_data_type == 0x01) || (ptg_data_type == 0x03));
                memset(buffer, 0, buffer_size);
                sprintf(buffer, "R%uC%u", row, col);
                evaluation_stack.push(buffer);
            }

            data_pos += 5;
            break;
        }
        /*
        * external reference consumers : PtgRef3d、PtgRefErr3d、PtgArea3d、PtgAreaErr3d、PtgNameX
        * 
        * Only RRDDefName and Lbl have the NameParsedFormula structure.
        */
        case ptgRef3d:
        case ptgRef3dV: {
            if (data_pos + 6 >= data_size) {
                goto done;
            }

            rgce_loc_t loc;
            xti_index_t ixti;
            ixti.index = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + data_pos + 1), little_endian);
            loc.row.rw = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + data_pos + 3), little_endian);
            loc.column.col = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(data + data_pos + 5), little_endian);
            bool col_relative_reference = (loc.column.col >> 14) & 0x1;
            bool row_relative_reference = (loc.column.col >> 15) & 0x1;
            auto row = loc.row.rw & 0xFFFF;
            auto col = loc.column.col & 0x00FF;

            if (cell->record_type == biff::OPC_RRDInsDel || cell->record_type == biff::OPC_RRDMove
                || cell->record_type == biff::OPC_RRDRenSheet || cell->record_type == biff::OPC_RRInsertSh
                || cell->record_type == biff::OPC_RRDDefName || cell->record_type == biff::OPC_RRDConflict
                || cell->record_type == biff::OPC_RRDTQSIF || cell->record_type == biff::OPC_RRFormat
                || cell->record_type == biff::OPC_RRAutoFmt || cell->record_type == biff::OPC_Note
                || cell->record_type == biff::OPC_RRDChgCell || cell->record_type == biff::OPC_RRDUserView) {
                /* is part of a revision */
                memset(buffer, 0, buffer_size);
                sprintf(buffer, "revision R%uC%u", row, col);
                evaluation_stack.push(buffer);
            }
            else {
                xl_bin_cell_t::val_t val;
                nret = xlm_parse_refs(ctx, xl_bin, cell, &ixti, row, col, &val);
                if (nret == BQ::OK) {
                    evaluation_stack.push(val);
                }
                else {
                    memset(buffer, 0, buffer_size);
                    sprintf(buffer, "R%uC%u(ixti=%u)", row, col, ixti.index);
                    evaluation_stack.push(buffer);
                }
            }

            data_pos += 7;
            break;
        }
        case ptgNameX: {
            if (data_pos + 6 >= data_size) {
                goto done;
            }

            uint16_t ixti = data[data_pos + 1];
            uint32_t nameindex = XsUtils::byteorder2host<uint32_t>(*(uint32_t*)(data + data_pos + 3), little_endian);

            /* todo */
            memset(buffer, 0, buffer_size);
            sprintf(buffer, "NAMEX_IDX %u 0x%08x", ixti, nameindex);
            evaluation_stack.push(buffer);

            data_pos += 7;
            break;
        }
        case ptgArea:
        case ptgAreaV: {
            if (data_pos + 8 >= data_size) {
                goto done;
            }

            uint16_t row1 = data[data_pos + 1] | (data[data_pos + 2] << 8);
            uint16_t column1 = data[data_pos + 3] | (data[data_pos + 4] << 8);
            uint16_t row2 = data[data_pos + 5] | (data[data_pos + 6] << 8);
            uint16_t column2 = data[data_pos + 7] | (data[data_pos + 8] << 8);

            memset(buffer, 0, buffer_size);
            sprintf(buffer, " R%s%uC%s%u:R%s%uC%s%u",
                (row1 & (1 << 14)) ? "~" : "",
                (unsigned)((row1 & 0x3fff) + ((row1 & (1 << 14)) ? 0 : 1)),
                (row1 & (1 << 15)) ? "~" : "",
                (unsigned)(column1 + ((row1 & (1 << 15)) ? 0 : 1)),
                (row2 & (1 << 14)) ? "~" : "",
                (unsigned)((row2 & 0x3fff) + ((row2 & (1 << 14)) ? 0 : 1)),
                (row2 & (1 << 15)) ? "~" : "",
                (unsigned)(column2 + ((row2 & (1 << 15)) ? 0 : 1)));
            evaluation_stack.push(buffer);

            data_pos += 9;
            break;
        }
        /* mem token */
        case ptgExp: {
            if (data_pos + 4 >= data_size) {
                goto done;
            }
            auto ptg_exp_r = data[data_pos + 1] | (data[data_pos + 2] << 8);
            auto ptg_exp_c = data[data_pos + 3] | (data[data_pos + 4] << 8);

            if (cell->is_shrfmla) {
                for (auto shrfmla : xl_bin->shareds) {
                    if (shrfmla.data.ref.colFirst > cell->col || shrfmla.data.ref.colLast < cell->col ||
                        shrfmla.data.ref.rwFirst > cell->row || shrfmla.data.ref.rwLast < cell->row) {
                        continue;
                    }
                    if (shrfmla.val.is_str()) {
                        cell->val.evaluation = shrfmla.val.evaluation;
                    }
                    break;
                }
            }
            else {
                // TODO: array
            }

            data_pos += 5;
            break;
        }
        default:
            if (ptg < sizeof(ptg_items) / sizeof(ptg_items[0])) {
                // ("Encountered unexpected ptg token: %s\n", TOKENS[ptg]);
            }
            else {
                // ("Encountered unknown ptg token: 0x%02x\n", ptg);
            }
            goto done;
        }
    }

    nret = 0;
done:
    cell->val.reset();
    for (uint32_t i = 0; !evaluation_stack.empty(); i++, evaluation_stack.pop()) {
        auto& cur = evaluation_stack.top();
        if (cur.is_str() == false) {
            cell->val = xl_bin_cell_t::val_t(data, data_size);
            break;
        }
        if (i == 0) {
            cell->val.evaluation = cur.evaluation;
            continue;
        }
        cell->val.evaluation = cur.evaluation + "\n" + cell->val.evaluation;
    }
    if (nret != 0 && cell->val.is_str()) {
        cell->val.evaluation += "' some errors";
    }
    return nret;
}

int xlm_handle_fmlasheet(const cfb_ctx_t* ctx, xl_bin_datas_t* xl_bin, uint32_t sheet_id, const file_obj_t* container) {
    if (ctx == nullptr || ctx->file_creator == nullptr || xl_bin == nullptr) {
        return BQ::INVALID_ARG;
    }

    xl_bin_sheet_t* sheet = nullptr;
    for (uint32_t i = 0; i < xl_bin->sheets.size(); i++) {
        if (xl_bin->sheets[i].sheet_id == sheet_id) {
            sheet = &xl_bin->sheets[i];
            break;
        }
    }
    if (sheet == nullptr || sheet_id >= xl_bin->sheets.size()) {
        return BQ::INVALID_ARG;
    }
    auto& formulas = sheet->cells;
    auto macro_sheet = &xl_bin->sheets[sheet_id].bs;
    if (formulas.size() == 0) {
        return BQ::OK;
    }

    if (macro_sheet->file == nullptr) {
        macro_sheet->file = (ctx->file_creator)(container, ctx->temp_dirpath, 0, macro_sheet->name, 0);
        if (macro_sheet->file == nullptr) {
            return BQ::ERR_FILE_CREATOR;
        }
    }

    std::sort(formulas.begin(), formulas.end(), [](const xl_bin_cell_t& a, const xl_bin_cell_t& b) { if (a.col == b.col) { return a.row < b.row; } return a.col < b.col; });

    int nret = BQ::OK;
    bool deobfuscation = ctx->xlm_deobfuscation;

    std::vector<uint32_t> need_parse;
    std::map<std::string, uint32_t> has_parse;
    for (uint32_t i = 0; i < formulas.size(); i++) {
        if (formulas[i].val.is_str()) {
            has_parse.insert(std::pair<std::string, uint32_t>("R" + std::to_string(formulas[i].row) + "C" + std::to_string(formulas[i].col), i));
        }
        else {
            need_parse.push_back(i);
        }
    }
    for (int i = 0, loops = 0, last_loops_left = 0, left = need_parse.size(); left > 0; i++) {
        if (loops != i / need_parse.size()) {
            loops = i / need_parse.size();
            if (last_loops_left && left == last_loops_left) {
                break;
            }
            last_loops_left = left;
        }
        auto cur = &formulas[need_parse[i % need_parse.size()]];
        if (cur->val.is_str()) continue;
        xlm_parse_formula(ctx, xl_bin, cur, true);
        if (cur->val.is_str()) left--;
    }

    for (size_t i = 0; i < need_parse.size(); i++) {
        auto cur = &formulas[need_parse[i]];
        if (cur->val.is_str()) continue;
        xlm_parse_formula(ctx, xl_bin, cur, true);
    }

    auto fill_macros = [](const cfb_ctx_t* ctx, const char* name, std::vector<xl_bin_cell_t>& formulas, ifile* out, uint32_t& size) {
        uint32_t cb_processed = 0;

        std::string header, sheet_name, col_header;
        if (ctx && ctx->write_subfile_header && ctx->header_vba_xlm) {
            header = ctx->header_vba_xlm;
            header += "XLM\n";
        }
        sheet_name = name == nullptr ? "<unknown>" : name;
        col_header = (ctx && ctx->header_xlm_col) ? ctx->header_xlm_col : "\n";

        std::string line = header;
        if (out) {
            if (line.size() > 0) out->write(cb_processed, (const uint8_t*)line.c_str(), line.size());
        }
        cb_processed += line.size();

        for (uint32_t i = 0; i < formulas.size(); i++) {
            if (i == 0 || formulas[i].col != formulas[i - 1].col) {
                line = col_header + sheet_name;
                line += ".C" + std::to_string(formulas[i].col) + "R" + std::to_string(formulas[i].row) + "\n";
            }
            else {
                line = "";
            }
            line += "R" + std::to_string(formulas[i].row) + "C" + std::to_string(formulas[i].col) + " ";
            line += formulas[i].val.evaluation + "\n";
            if (out) {
                out->write(cb_processed, (const uint8_t*)line.c_str(), line.size());
            }
            cb_processed += line.size();
        }

        size = cb_processed;
    };

    uint32_t total_size = 0;
    fill_macros(ctx, macro_sheet->name, formulas, 0, total_size);
    fill_macros(ctx, macro_sheet->name, formulas, macro_sheet->file, total_size);

    return nret;
}

int parse_xl_sst(const cfb_ctx_t* ctx, xl_bin_datas_t* xl, uint32_t pos, uint32_t* rcd_len) {
    if (xl == nullptr || xl->data == nullptr || xl->size == 0) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    bool little_endian = xl->little_endian;
    bool deobfuscation = ctx ? ctx->xlm_deobfuscation : false;

    biff::record_header_t rcd;
    xlm_records(little_endian).read_header(xl->data + pos, sizeof(rcd), rcd);
    if (rcd.length > BIFF8_MAX_RECORD_DATA_LENGTH || pos + rcd.length > xl->size) {
        return BQ::ERR_BIFF_LEN;
    }

    if (rcd.opcode != biff::OPC_SST) {
        return BQ::ERR_FORMAT;
    }

    auto rcd_data = xl->data + pos + sizeof(rcd);
    auto rcd_size = (uint32_t)rcd.length;
    if (rcd_size < 8) {
        return 0;
    }
    int32_t cstTotal = XsUtils::byteorder2host<int32_t>(*(int32_t*)(rcd_data + 0), little_endian);
    int32_t cstUnique = XsUtils::byteorder2host<int32_t>(*(int32_t*)(rcd_data + 4), little_endian);

    /* for XLUnicodeRichExtendedString */
    for (uint32_t offset = 8; offset < rcd_size; ) {
        xl_uni_rich_extended_str_t uni_rich_str;
        uni_rich_str.cch = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(rcd_data + offset), little_endian);
        offset += sizeof(uni_rich_str.cch);
        uni_rich_str.flags = *(rcd_data + offset);
        offset += sizeof(uni_rich_str.flags);
        if (uni_rich_str.fRichSt()) {
            uni_rich_str.cRun = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(rcd_data + offset), little_endian);
            offset += sizeof(uni_rich_str.cRun);
        }
        if (uni_rich_str.fExtSt()) {
            uni_rich_str.cbExtRst = XsUtils::byteorder2host<int32_t>(*(int32_t*)(rcd_data + offset), little_endian);
            offset += sizeof(uni_rich_str.cbExtRst);
        }

        std::u16string u16str;
        xlm_records(little_endian).read_uni_str_rgb(rcd_data + offset, rcd_size - offset, uni_rich_str.cch, uni_rich_str.fHighByte(), u16str);
        xl->sst.push_back(XsUtils::u16s2utf(u16str));
        offset += uni_rich_str.fHighByte() ? uni_rich_str.cch * 2 : uni_rich_str.cch;

        if (uni_rich_str.fRichSt()) {
            offset += sizeof(format_run_t) * uni_rich_str.cRun;
        }
        if (uni_rich_str.fExtSt()) {
            uni_rich_str.extRst.reserved = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(rcd_data + offset), little_endian);
            uni_rich_str.extRst.cb = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(rcd_data + offset + 2), little_endian);
            offset += uni_rich_str.extRst.cb + 4;
        }
    }

    if (rcd_len) {
        *rcd_len = rcd_size;
    }

    return nret;
}

int xlm::parse_xlm(const cfb_ctx_t* ctx, uint8_t* buffer, uint32_t size, const file_obj_t* container, ms_workbook_t* xlm, ifile* drawings) {
    if (buffer == nullptr || size == 0 || xlm == nullptr) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    xlm->codepage = cp_utf16_le;
    xlm->biff_version = 0;

    size_t drawinggroup_len = 0;
    size_t previous_biff8_opcode = (size_t)biff::OPC_Invalid; // Initialize to 0x0, which isn't even in our enum.
                                                        // This variable will allow the OPC_CONTINUE record
                                                        // to know which record it is continuing.
    xl_bin_datas_t xl_bin;
    xl_bin.data = buffer;
    xl_bin.size = size;
    xl_bin.little_endian = is_little_endian;
    xl_bin.shareds.push_back(shrfmla_item_t());
    xl_bin_sheet_t* cur_sheet = nullptr;

    bool deobfuscation = ctx ? ctx->xlm_deobfuscation : false;
    biff::record_header_t rcd;
    
    for (uint32_t pos = 0, bof_count = 0; pos + sizeof(biff::record_header_t) <= size; ) {
        xlm_records().read_header(buffer + pos, sizeof(rcd), rcd);
#if _DEBUG
        if (_DEBUG) {
            constexpr uint32_t opcode_names_cnt = sizeof(OPCODE_NAMES) / sizeof(OPCODE_NAMES[0]);
            const char* name = (rcd.opcode < opcode_names_cnt) ? OPCODE_NAMES[rcd.opcode] : nullptr;
            char cache[128] = { 0 };
            sprintf(cache, "%04x %6d   %s", rcd.opcode, rcd.length, name ? name : "unknown");
        }
#endif

        if (rcd.length > BIFF8_MAX_RECORD_DATA_LENGTH || pos + rcd.length > size) {
            nret = BQ::ERR_BIFF_LEN;
            break;
        }

        uint8_t* data = buffer + (pos += sizeof(biff::record_header_t));
        uint32_t data_size = rcd.length;

        switch (rcd.opcode) {
        case biff::OPC_BOF: {
            if (data_size < 8) {
                break;
            }

            uint16_t cur_bof_type = biff::BT_INVALID;
            xlm->biff_version = biff::version(data - sizeof(rcd), data_size + sizeof(rcd), &cur_bof_type, is_little_endian);
            if (xlm->biff_version == biff::unknown) {
                return BQ::ERR_FORMAT_XLM;
            }

            cur_sheet = nullptr;
            for (size_t i = 0; i < xl_bin.sheets.size(); i++) {
                if (xl_bin.sheets[i].bs.lbPlyPos + sizeof(biff::record_header_t) == pos) {
                    cur_sheet = &xl_bin.sheets[i];
                    cur_sheet->sheet_id = i;
                    cur_sheet->sheet_type = cur_bof_type;
                    break;
                }
            }

            bof_count++;
            break;
        }
        case biff::OPC_EOF: {
            break;
        }
        case biff::OPC_Codepage: {
            if (data_size < 2) {
                break;
            }
            xlm->codepage = *(uint16_t*)data;
            break;
        }
        case biff::OPC_SST: {
            parse_xl_sst(ctx, &xl_bin, pos - sizeof(biff::record_header_t), &data_size);
            break;
        }
        case biff::OPC_Lbl: {
            lbl_record_t lbl;
            uint32_t lbl_size_min = (uint8_t*)&lbl.name[1] - (uint8_t*)&lbl;
            auto record = &lbl;
            if (data_size <= lbl_size_min) {
                break;
            }
            assert(bof_count == 1);
            memcpy(&lbl, data, lbl_size_min);
            lbl.itab = XsUtils::byteorder2host<uint16_t>(lbl.itab, is_little_endian);
            lbl.flags = XsUtils::byteorder2host<uint16_t>(lbl.flags, is_little_endian);

            int cce_offset = 0;
            if (record->flags & 0x20) {
                bool high_byte = (record->name[0] & 0x01) == 0x01;
                /* high_byte should be false, but that high_byte is true happened */
                uint8_t code = high_byte ? record->name[0] : *(data + lbl_size_min);

                if (code >= (sizeof(LblRecord_built_in_names) / sizeof(LblRecord_built_in_names[0]))) {
                    record->name[0] = '?';
                    record->name[1] = 0;
                }
                else {
                    strcpy((char*)record->name, LblRecord_built_in_names[code]);
                }
                cce_offset = lbl_size_min + (high_byte ? 0 : 1);
            }
            else {
                int name_offset = 0;
                bool single_char = false;

                if (0 == (record->name[0] & 0xFE)) {
                    /* XLUnicodeStringNoCch */
                    single_char = (0 == (record->name[0] & 0x01));
                    name_offset = lbl_size_min;
                }
                else if (*(data + lbl_size_min) == 0 && data_size > 1 + lbl_size_min) {
                    record->name[2] = *(data + lbl_size_min + 1);
                    if (0 == (record->name[2] & 0xFE)) {
                        /* XLNameUnicodeString */
                        assert(record->name[0] == record->cch);
                        single_char = (0 == (record->name[2] & 0x01));
                        name_offset = lbl_size_min + 2;
                    }
                }
                else {
                    const char* temp = read_ansi_str(data + lbl_size_min - 1, data_size + 1 - lbl_size_min);
                    if (temp == nullptr && lbl_size_min - 1 + record->cch != data_size) {
                        break;
                    }
                    single_char = true;
                    name_offset = lbl_size_min - 1;
                }

                if (single_char) {
                    memset(record->name, 0, record->cch + 1);
                    memcpy(record->name, data + name_offset, record->cch);
                }
                else {
                    char buffer[512 + 2] = { 0 };
                    memcpy(buffer, data + name_offset, record->cch * 2);
                    memset(record->name, 0, record->cch + 1);
                    memcpy(record->name, XsUtils::u16s2utf((char16_t*)buffer).c_str(), record->cch);
                }
                cce_offset = name_offset + (single_char ? record->cch : record->cch * 2);
            }

            if (record->cce > 0 && cce_offset > 0) {
                /*
                * TODO: 
                *    xlm_parse_formula(data + cce_offset, record->cce, ....)
                * 
                *    when ixti == 0xFFFF, row and col is not documented. 
                *    we can reproduce it: make a macro sheet in ooxml and export it to xls.
                */
                lbl.rgce.ptg = uint8_t(data[cce_offset] & 0x7f);
                if (lbl.rgce.ptg == ptgRef3d || lbl.rgce.ptg == ptgRef3dA || lbl.rgce.ptg == ptgRef3dV) {
                    lbl.rgce.ixti = *(uint16_t*)(data + cce_offset + 1);
                    if (lbl.rgce.ixti > 0xFF00 && record->cce >= 15 + 3) {
                        assert(record->itab == 0 || record->itab == 1);
                        uint16_t iscope_1base = (record->itab == 1) ? record->reserved1 : record->itab;
                        uint16_t isheet_0base_a = *(uint16_t*)(data + cce_offset + 11);
                        uint16_t isheet_0base_b = *(uint16_t*)(data + cce_offset + 13);
                        assert(isheet_0base_a == isheet_0base_b);
                        lbl.rgce.row = *(uint16_t*)(data + cce_offset + 15);
                        lbl.rgce.col = *(uint16_t*)(data + cce_offset + 17);
                    }

                    if (lbl.rgce.ptg == ptgRef3d && lbl.rgce.ixti < 0xFFFF && record->cce == 7) {
                        lbl.rgce.row = *(uint16_t*)(data + cce_offset + 3);
                        lbl.rgce.col = *(uint16_t*)(data + cce_offset + 5);
                    }
                }
            }

            xl_bin.lbls.push_back(lbl);
            break;
        }
        case biff::OPC_SupBook: {
            if (data_size < sizeof(supbook_record_t)) {
                break;
            }

            supbook_record_t supbook_item;
            supbook_item = *(const supbook_record_t*)data;
            supbook_item.ctab = XsUtils::byteorder2host<uint16_t>(supbook_item.ctab, is_little_endian);
            supbook_item.cch = XsUtils::byteorder2host<uint16_t>(supbook_item.cch, is_little_endian);
            xl_bin.supbooks.push_back(supbook_item);

            break;
        }
        case biff::OPC_ExternSheet: {
            if (data_size < 2) {
                break;
            }

            uint16_t xti_cnt = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)data, is_little_endian);
            
            xti_record_t xti_item;
            for (uint16_t i = 0; i < xti_cnt && data_size >= 2 + (i+1) * sizeof(xti_record_t); i++) {
                xti_item = *(xti_record_t*)(data + 2 + i * sizeof(xti_record_t));
                xti_item.isupbook = XsUtils::byteorder2host<uint16_t>(xti_item.isupbook, is_little_endian);
                xti_item.itab_first = XsUtils::byteorder2host<int16_t>(xti_item.itab_first, is_little_endian);
                xti_item.itab_last = XsUtils::byteorder2host<int16_t>(xti_item.itab_last, is_little_endian);
                xl_bin.xtis.push_back(xti_item);
            }

            break;
        }
        case biff::OPC_MsoDrawingGroup: {
            /*
             * Extract the entire drawing group before we parse it.
             */
            if (0 == drawinggroup_len) {
                /* Found beginning of a drawing group */
                drawinggroup_len = data_size;
            }
            else {
                /* already found the beginning of a drawing group, extract the remaining chunks */
                drawinggroup_len += data_size;
            }
            if (drawings && (0 > drawings->write(-1, data, data_size))) {
                nret = BQ::ERR_WRITE;
                break;
            }
            break;
        }
        case biff::OPC_Continue: {
            if ((biff::OPC_MsoDrawingGroup == previous_biff8_opcode) && (0 != drawinggroup_len)) {
                /* already found the beginning of an image, extract the remaining chunks */
                drawinggroup_len += data_size;
            }
            if (drawings && (0 > drawings->write(-1, data, data_size))) {
                nret = BQ::ERR_WRITE;
                break;
            }
            break;
        }
        case biff::OPC_BoundSheet: {
            /*
            * using cur_boundsheet.lbPlyPos, we can accelerate the travel for macro sheet.
            *    1. travel the globals substream, then we can get the position of macro sheet and lbl records.
            *   2. seek to the macro sheet substream and travels, until EOF
            */
            record_boundsheet8_t cur_boundsheet;
            memset(&cur_boundsheet, 0, sizeof(cur_boundsheet));

            uint32_t boundsheet_size_min = (uint8_t*)cur_boundsheet.name - (uint8_t*)&cur_boundsheet.lbPlyPos;
            if (data_size < boundsheet_size_min) {
                MESSAGE_WARN(ctx, "[extract_macros_and_images] Skipping broken BOUNDSHEET record (length %u) \n", data_size);
                break;
            }

            memcpy(&cur_boundsheet, data, boundsheet_size_min);
            if (cur_boundsheet.cch >= 32) {
                MESSAGE_WARN(ctx, "[extract_macros_and_images] invalid boundsheet.cch (may be encrypted) \n");
                pos = size - data_size;
                break;
            }

            uint32_t name_bytes = cur_boundsheet.cch;
            if (cur_boundsheet.cch_flag == 0x01) {
                name_bytes *= 2;
                if (name_bytes > data_size - boundsheet_size_min) {
                    name_bytes = data_size - boundsheet_size_min;
                }
                if (name_bytes > sizeof(cur_boundsheet.name) - 2) {
                    name_bytes = sizeof(cur_boundsheet.name) - 2;
                }
                memcpy(cur_boundsheet.name, data + boundsheet_size_min, name_bytes);
            }
            else if (cur_boundsheet.cch_flag == 0x00) {
                if (name_bytes > data_size - boundsheet_size_min) {
                    name_bytes = data_size - boundsheet_size_min;
                }
                if (name_bytes > sizeof(cur_boundsheet.name) - 1) {
                    name_bytes = sizeof(cur_boundsheet.name) - 1;
                }
                memcpy(cur_boundsheet.name, data + boundsheet_size_min, name_bytes);
            }
            else {
                if (name_bytes > data_size - boundsheet_size_min + 1) {
                    name_bytes = data_size - boundsheet_size_min + 1;
                }
                if (name_bytes > sizeof(cur_boundsheet.name) - 1) {
                    name_bytes = sizeof(cur_boundsheet.name) - 1;
                }
                memcpy(cur_boundsheet.name, data + boundsheet_size_min - 1, name_bytes);
            }

#if _DEBUG
            std::string sheet_info = "BOUNDSHEET - ";
            if (cur_boundsheet.cch_flag & 0x01) {
                sheet_info += XsUtils::u16s2utf((char16_t*)cur_boundsheet.name);
            }
            else {
                sheet_info += std::string((char*)cur_boundsheet.name);
            }
            sheet_info += " ";

            switch (data[4]) {
            case 0:
                sheet_info += "visible";
                break;
            case 1:
                sheet_info += "hidden";
                break;
            case 2:
                sheet_info += "very hidden";
                break;
            default:
                sheet_info += "unknown_visibility";
                break;
            }
            sheet_info += " , ";
            switch (data[5]) {
            case 0:
                sheet_info += "worksheet or dialog sheet";
                break;
            case 1:
                sheet_info += "Excel 4.0 macro sheet";
                break;
            case 2:
                sheet_info += "chart";
                break;
            case 6:
                sheet_info += "Visual Basic module";
                break;
            default:
                sheet_info += "unknown type";
                break;
            }
#endif

            std::string fmla_name;
            if (cur_boundsheet.cch_flag & 0x01) {
                fmla_name += XsUtils::u16s2utf((char16_t*)cur_boundsheet.name);
            }
            else {
                fmla_name += std::string((char*)cur_boundsheet.name);
            }

            xl_bin_sheet_t sheet;
            sheet.sheet_id = xl_bin.sheets.size();
            sheet.bs.flags = cur_boundsheet.flags;
            sheet.bs.lbPlyPos = cur_boundsheet.lbPlyPos;
            memcpy(sheet.bs.name, fmla_name.c_str(), std::min<size_t>(fmla_name.size(), sizeof(sheet.bs.name) - 1));
            xl_bin.sheets.push_back(sheet);
            break;
        }
        case biff::OPC_String:
        case biff::OPC_Blank:
        case biff::OPC_Number:
        case biff::OPC_BoolErr:
        case biff::OPC_Label:
        case biff::OPC_LabelSst:
        case biff::OPC_Array:
        case biff::OPC_ShrFmla:
        case biff::OPC_Formula: {
            if (nullptr == cur_sheet || cur_sheet->sheet_type != biff::BT_MACRO_SHEET) {
                assert(cur_sheet != nullptr);
                break;
            }
            parse_xl_cell(ctx, &xl_bin, cur_sheet->sheet_id, pos - sizeof(biff::record_header_t), &data_size);
            break;
        }
        case biff::OPC_FilePass:
        case biff::OPC_Password:
        case biff::OPC_Prot4RevPass:
        default:
            break;
        }

        /* Keep track of which biff record we're continuing if we encounter OPC_CONTINUE */
        if (biff::OPC_Continue != rcd.opcode) {
            previous_biff8_opcode = rcd.opcode;
        }
        pos += data_size;
    }

    for (uint32_t i = 0; i < xl_bin.sheets.size(); i++) {
        auto& cur_sheet = xl_bin.sheets[i];
        if (cur_sheet.sheet_type != biff::BT_MACRO_SHEET) {
            continue;
        }
        xlm_handle_fmlasheet(ctx, &xl_bin, cur_sheet.sheet_id, container);
    }
    xl_bin_utils::copy_boundsheets(xl_bin.sheets, &xlm->boundsheets, &xlm->boundsheet_cnt);

    return nret;
}

int xlm::reset_boundsheet(const cfb_ctx_t* ctx, uint8_t* buffer, uint32_t& size, xl_bin_boundsheet_t* bs) {
    if (buffer == nullptr || size == 0 || bs == nullptr || size <= bs->lbPlyPos || bs->lbPlyPos < sizeof(biff::record_header_t)) {
        return BQ::INVALID_ARG;
    }

    int nret = BQ::OK;
    biff::record_header_t rcd;

    for (uint32_t pos = bs->lbPlyPos; pos + sizeof(biff::record_header_t) <= size; ) {
        rcd.opcode = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(buffer + pos), is_little_endian);
        rcd.length = XsUtils::byteorder2host<uint16_t>(*(uint16_t*)(buffer + pos + sizeof(rcd.opcode)), is_little_endian);
        pos += sizeof(biff::record_header_t);

        if (pos + rcd.length > size) {
            nret = BQ::ERR_FORMAT;
            break;
        }

        if (biff::OPC_EOF == rcd.opcode) {
            break;
        }
        else if (biff::OPC_BOF == rcd.opcode) {
            if (pos != bs->lbPlyPos + sizeof(biff::record_header_t)) {
                break;
            }
        }
        else {
            /*
            * method 1
            *    keep all biff::recod_header_t, then just reset data bytes to 0
            *
            * method 2: TODO:
            *    keep the BOF and EOF, then write a Blank or BookBool or Note which is the same size as current boundsheet.
            */
            memset(buffer + pos, 0, rcd.length);
        }
        
        pos += rcd.length;
    }

    return nret;
}

/////////////////////////////////////////////////////////////
//
//    drawing_group
//
/////////////////////////////////////////////////////////////

/**
 * @brief The OfficeArtRecordHeader fined on page 27 of the MSO-ODRAW specification:
 *   https://interoperability.blob.core.windows.net/files/MS-ODRAW/%5bMS-ODRAW%5d.pdf
 *
 * We'll use this to extract images found in office documents.
 */
struct OfficeArtRecordHeader_PackedLittleEndian {
    uint16_t recVerAndInstance; // 4 bytes for recVer, 12 bytes for recInstance
    uint16_t recType;
    uint32_t recLen;
};

/**
 * @brief The OfficeArtFBSE structure following its record header.
 * See section 2.2.32 OfficeArtFBSE in:
 *   https://interoperability.blob.core.windows.net/files/MS-ODRAW/%5bMS-ODRAW%5d.pdf
 *
 * Does not include the variable size nameData
 */
struct OfficeArtFBSE_PackedLittleEndian {
    uint8_t btWin32; // 1-byte enum containing a mso_blip_type value
    uint8_t btMacOS; // 1-byte enum containing a mso_blip_type value
    unsigned char rgbUid[16];
    uint16_t tag;
    uint32_t size;    // size of the Blip stream
    uint32_t cRef;    // number of references to the Blip
    uint32_t foDelay; // An MSOFOstructure, as defined in section 2.1.4, must be 0x00000000
    uint8_t unused1;  // unused
    uint8_t cbName;   // length of the name field, in bytes.
    uint8_t unused2;  // unused
    uint8_t unused3;  // unused
};

struct OfficeArtRecordHeader_Unpacked {
    uint16_t recVer;
    uint16_t recInstance;
    uint16_t recType;
    uint32_t recLen;
};

int append_to_file(ifile* file, const char* format, ...) {
    if (file == nullptr || nullptr == format) {
        return -1;
    }

    char buffer[256] = { 0 };

    va_list valist;
    va_start(valist, format);
    int nret = vsprintf(buffer, format, valist);
    va_end(valist);

    if (nret >= 0) {
        nret = file->write(-1, (unsigned char*)buffer, strlen(buffer));
    }

    return nret;
}

/**
* @brief Handle each type of Blip record. See section 2.2.23 OfficeArtBlip in:
* https://interoperability.blob.core.windows.net/files/MS-ODRAW/%5bMS-ODRAW%5d.pdf
*/
int process_blip_record(const cfb_ctx_t* ctx, struct OfficeArtRecordHeader_Unpacked* rh, const uint8_t* index, uint32_t remaining, ifile** out) {
    int nret = BQ::OK;

    uint32_t blip_bytes_before_image = 0; /* the number of bytes between the record header and the image */
    const uint8_t* start_of_image = NULL;
    uint32_t size_of_image = 0;
    const char* extracted_image_type = NULL;

    if (0x0 != rh->recVer) {
        MESSAGE_DEBUG(ctx, "[process_blip_record] Invalid recVer for Blip record header: %u\n", rh->recVer);
    }

    switch (rh->recType) {
    case 0xF01A: { /* OfficeArtBlipEMF */
        MESSAGE_DEBUG(ctx, "[process_blip_record] Found OfficeArtBlipEMF (Enhanced Metafile Format)\n");
        if (0x3D4 == rh->recInstance) {
            blip_bytes_before_image += 16 + 34; /* 1 16-byte UID + 34-byte metafile header */
        }
        else if (0x3D5 == rh->recInstance) {
            blip_bytes_before_image += 32 + 34; /* 2 16-byte UIDs + 34-byte metafile header */
        }
        else {
            MESSAGE_DEBUG(ctx, "[process_blip_record] Invalid recInstance for OfficeArtBlipEMF\n");
        }
        extracted_image_type = "EMF";
        break;
    }
    case 0xF01B: { /* OfficeArtBlipWMF */
        MESSAGE_DEBUG(ctx, "[process_blip_record] Found OfficeArtBlipWMF (Windows Metafile Format)\n");
        if (0x216 == rh->recInstance) {
            blip_bytes_before_image += 16 + 34; /* 1 16-byte UID + 34-byte metafile header */
        }
        else if (0x217 == rh->recInstance) {
            blip_bytes_before_image += 32 + 34; /* 2 16-byte UIDs + 34-byte metafile header */
        }
        else {
            MESSAGE_DEBUG(ctx, "[process_blip_record] Invalid recInstance for OfficeArtBlipWMF\n");
        }
        extracted_image_type = "WMF";
        break;
    }
    case 0xF01C: { /* OfficeArtBlipPICT */
        MESSAGE_DEBUG(ctx, "[process_blip_record] Found OfficeArtBlipPICT (Macintosh PICT)\n");
        if (0x542 == rh->recInstance) {
            blip_bytes_before_image += 16 + 34; /* 1 16-byte UID + 34-byte metafile header */
        }
        else if (0x543 == rh->recInstance) {
            blip_bytes_before_image += 32 + 34; /* 2 16-byte UIDs + 34-byte metafile header */
        }
        else {
            MESSAGE_DEBUG(ctx, "[process_blip_record] Invalid recInstance for OfficeArtBlipPICT\n");
        }
        extracted_image_type = "PICT";
        break;
    }
    case 0xF01D:   /* OfficeArtBlipJPEG */
    case 0xF02A: { /* OfficeArtBlipJPEG */
        MESSAGE_DEBUG(ctx, "[process_blip_record] Found OfficeArtBlipJPEG\n");
        if (0x46A == rh->recInstance || 0x6E2 == rh->recInstance) {
            blip_bytes_before_image += 16 + 1; /* 1 16-byte UID + 1-byte tag */
        }
        else if (0x46B == rh->recInstance || 0x6E3 == rh->recInstance) {
            blip_bytes_before_image += 32 + 1; /* 2 16-byte UIDs + 1-byte tag */
        }
        else {
            MESSAGE_DEBUG(ctx, "[process_blip_record] Invalid recInstance for OfficeArtBlipJPEG\n");
        }
        extracted_image_type = "JPEG";
        break;
    }
    case 0xF01E: { /* OfficeArtBlipPNG */
        MESSAGE_DEBUG(ctx, "[process_blip_record] Found OfficeArtBlipPNG\n");
        if (0x6E0 == rh->recInstance) {
            blip_bytes_before_image += 16 + 1; /* 1 16-byte UID + 1-byte tag */
        }
        else if (0x6E1 == rh->recInstance) {
            blip_bytes_before_image += 32 + 1; /* 2 16-byte UIDs + 1-byte tag */
        }
        else {
            MESSAGE_DEBUG(ctx, "[process_blip_record] Invalid recInstance for OfficeArtBlipPNG\n");
        }
        extracted_image_type = "PNG";
        break;
    }
    case 0xF01F: { /* OfficeArtBlipDIB */
        MESSAGE_DEBUG(ctx, "[process_blip_record] Found OfficeArtBlipDIB (device independent bitmap)\n");
        if (0x7A8 == rh->recInstance) {
            blip_bytes_before_image += 16 + 1; /* 1 16-byte UID + 1-byte tag */
        }
        else if (0x7A9 == rh->recInstance) {
            blip_bytes_before_image += 32 + 1; /* 2 16-byte UIDs + 1-byte tag */
        }
        else {
            MESSAGE_DEBUG(ctx, "[process_blip_record] Invalid recInstance for OfficeArtBlipDIB\n");
        }
        extracted_image_type = "DIB";
        break;
    }
    case 0xF029: { /* OfficeArtBlipTIFF */
        MESSAGE_DEBUG(ctx, "[process_blip_record] Found OfficeArtBlipTIFF\n");
        if (0x6E4 == rh->recInstance) {
            blip_bytes_before_image += 16 + 1; /* 1 16-byte UID + 1-byte tag */
        }
        else if (0x6E5 == rh->recInstance) {
            blip_bytes_before_image += 32 + 1; /* 2 16-byte UIDs + 1-byte tag */
        }
        else {
            MESSAGE_DEBUG(ctx, "[process_blip_record] Invalid recInstance for OfficeArtBlipTIFF\n");
        }
        extracted_image_type = "TIFF";
        break;
    }
    default: {
        MESSAGE_DEBUG(ctx, "[process_blip_record] Unknown OfficeArtBlip type!\n");
    }
    }

    if (0 == blip_bytes_before_image) {
        MESSAGE_DEBUG(ctx, "[process_blip_record] Was not able to identify the Blip type, skipping...\n");

    }
    else if (remaining < sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) + blip_bytes_before_image) {
        MESSAGE_DEBUG(ctx, "[process_blip_record] Not enough remaining bytes in blip array for image data\n");

    }
    else {
        start_of_image = index + sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) + blip_bytes_before_image;
        size_of_image = std::min<uint32_t>(rh->recLen, remaining - (sizeof(OfficeArtRecordHeader_PackedLittleEndian) + blip_bytes_before_image));

        MESSAGE_DEBUG(ctx, "[process_blip_record] Scanning extracted image of size %zu\n", size_of_image);

        if (ctx && ctx->file_creator) {
            // TODO: ctx->file_creator
            MESSAGE_DEBUG(ctx, "[process_blip_record] TODO: Create temp file \n");
        }
        else {
            MESSAGE_DEBUG(ctx, "[process_blip_record] File handler unavailable \n");
        }
    }

    if (remaining < sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) + rh->recLen) {
        remaining = 0;
    }
    else {
        remaining -= sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) + rh->recLen;
        index += sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) + rh->recLen;
    }

    nret = BQ::OK;
    return nret;
}

/**
 * @brief Read the office art record header information from a buffer
 *
 * @param data                      data buffer starting with the record header
 * @param data_size                 length of the buffer
 * @param[in,out] unpacked_header   fill this
 * @return int                      0 if successfull, else some error code
 */
int read_office_art_record_header(const uint8_t* data, uint32_t data_size, struct OfficeArtRecordHeader_Unpacked* unpacked_header, bool is_little_endian) {
    uint16_t recVerAndInstance;
    OfficeArtRecordHeader_PackedLittleEndian* rawHeader;

    if ((nullptr == data) || (sizeof(OfficeArtRecordHeader_PackedLittleEndian) > data_size) || (nullptr == unpacked_header)) {
        // invalid args
        return BQ::INVALID_ARG;
    }

    rawHeader = (OfficeArtRecordHeader_PackedLittleEndian*)data;

    recVerAndInstance = XsUtils::byteorder2host<uint16_t>(rawHeader->recVerAndInstance, is_little_endian);

    unpacked_header->recVer = recVerAndInstance & 0x000F;
    unpacked_header->recInstance = (recVerAndInstance & 0xFFF0) >> 4;
    unpacked_header->recType = XsUtils::byteorder2host<uint16_t>(rawHeader->recType, is_little_endian);
    unpacked_header->recLen = XsUtils::byteorder2host<uint32_t>(rawHeader->recLen, is_little_endian);

    return BQ::OK;
}

/**
 * @brief Process each Blip Store Container File Block in a Blip Store Container
 *
 * @param ctx
 * @param drawing_group
 * @param blip_store_container
 * @param blip_store_container_len
 * @param handler                     callback which will process the extracted files
 * @return int
 */
int process_blip_store_container(const cfb_ctx_t* ctx, file_obj_t* drawing_group, const uint8_t* blip_store_container, uint32_t blip_store_container_len, ifilehandler* handler, bool is_little_endian) {
    int nret = BQ::OK;

    struct OfficeArtRecordHeader_Unpacked rh;
    const unsigned char* index = blip_store_container;
    size_t remaining = blip_store_container_len;

    while (0 < remaining) {

        if (0 != read_office_art_record_header(index, remaining, &rh, is_little_endian)) {
            /* Failed to get header, abort. */
            goto done;
        }

        if (0x0 != rh.recVer) {
            MESSAGE_WARN(ctx, "[process_blip_store_container] Invalid recVer for Blip record header: %u\n", rh.recVer);
        }

        /*
         * Handle each type of Blip Store Container File Block. See section 2.2.22 OfficeArtBStoreContainerFileBlock in:
         * https://interoperability.blob.core.windows.net/files/MS-ODRAW/%5bMS-ODRAW%5d.pdf
         */
        if (0xF007 == rh.recType) {
            /* it's an OfficeArtFBSErecord */
            MESSAGE_DEBUG(ctx, "[process_blip_store_container] Found a File Blip Store Entry (FBSE) record\n");

            if (0x2 != rh.recVer) {
                MESSAGE_DEBUG(ctx, "[process_blip_store_container] Invalid recVer for OfficeArtFBSErecord: 0x%x\n", rh.recVer);
            }

            if (sizeof(OfficeArtFBSE_PackedLittleEndian) > remaining - sizeof(struct OfficeArtRecordHeader_PackedLittleEndian)) {
                MESSAGE_DEBUG(ctx, "[process_blip_store_container] Not enough bytes for FSBE record data\n");
            }
            else {
                struct OfficeArtFBSE_PackedLittleEndian* FBSE_record_data = (struct OfficeArtFBSE_PackedLittleEndian*)(index + sizeof(struct OfficeArtRecordHeader_PackedLittleEndian));

                if (FBSE_record_data->cbName > remaining - sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) - sizeof(struct OfficeArtFBSE_PackedLittleEndian)) {
                    MESSAGE_DEBUG(ctx, "[process_blip_store_container] Not enough bytes for FSBE record data + blip file name\n");
                }
                else {
                    struct OfficeArtRecordHeader_Unpacked embeddedBlip_rh;
                    const unsigned char* embeddedBlip;
                    size_t embeddedBlip_size;
                    char* blip_file_name = NULL;
                    char blip_name_buffer[255] = { 0 };

                    if (FBSE_record_data->cbName > 0) {
                        memcpy(blip_name_buffer,
                            (char*)(index +
                                sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) +
                                sizeof(struct OfficeArtFBSE_PackedLittleEndian)),
                            (size_t)FBSE_record_data->cbName);
                        blip_name_buffer[FBSE_record_data->cbName] = '\0';

                        blip_file_name = blip_name_buffer;
                        MESSAGE_DEBUG(ctx, "[process_blip_store_container] Blip file name: %s\n", blip_file_name);
                    }

                    embeddedBlip = (const unsigned char*)(index +
                        sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) +
                        sizeof(struct OfficeArtFBSE_PackedLittleEndian) +
                        (size_t)FBSE_record_data->cbName);

                    embeddedBlip_size = remaining -
                        sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) -
                        sizeof(struct OfficeArtFBSE_PackedLittleEndian) -
                        (size_t)FBSE_record_data->cbName;

                    uint32_t fbse_size = XsUtils::byteorder2host<>(FBSE_record_data->size, is_little_endian);
                    if (fbse_size > embeddedBlip_size) {
                        MESSAGE_DEBUG(ctx, "[process_blip_store_container] WARNING: The File Blip Store Entry claims that the Blip data is bigger than the remaining bytes in the record!\n");
                        MESSAGE_DEBUG(ctx, "[process_blip_store_container]    %d > %zu!\n", fbse_size, embeddedBlip_size);
                    }
                    else {
                        /* limit embeddedBlip_size to the size of what's actually left */
                        embeddedBlip_size = fbse_size;
                    }

                    if (0 != read_office_art_record_header(embeddedBlip, embeddedBlip_size, &embeddedBlip_rh, is_little_endian)) {
                        /* Failed to get header, abort. */
                        MESSAGE_DEBUG(ctx, "[process_blip_store_container] Failed to get header\n");
                        goto done;
                    }

                    ifile* extract = nullptr;
                    nret = process_blip_record(ctx, &embeddedBlip_rh, embeddedBlip, embeddedBlip_size, &extract);
                    if (nret == 0 && handler && extract) {
                        file_obj_t extract_obj{ extract , ft_xl4_drawing, 0, drawing_group };
                        nret = handler->handle(ctx, &extract_obj);
                    }
                    if (extract) {
                        extract->release();
                    }
                    if (nret != 0) {
                        goto done;
                    }
                }
            }
        }
        else if ((0xF018 <= rh.recType) && (0xF117 >= rh.recType)) {
            /* it's an OfficeArtBlip record */
            MESSAGE_DEBUG(ctx, "[process_blip_store_container] Found a Blip record\n");
            ifile* extract = nullptr;
            nret = process_blip_record(ctx, &rh, index, remaining, &extract);
            if (nret == 0 && handler && extract) {
                file_obj_t extract_obj{ extract , ft_xl4_drawing, 0, drawing_group };
                nret = handler->handle(ctx, &extract_obj);
            }
            if (extract) {
                extract->release();
            }
            if (nret != 0) {
                goto done;
            }
        }
        else {
            /* unexpected record type. */
            MESSAGE_DEBUG(ctx, "[process_blip_store_container] Unexpected record type\n");
        }

        if (remaining < sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) + rh.recLen) {
            remaining = 0;
        }
        else {
            remaining -= sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) + rh.recLen;
            index += sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) + rh.recLen;
        }
    }

    nret = 0;
done:
    return nret;
}

int xlm::extract_images_from_drawing_group(const cfb_ctx_t* ctx, file_obj_t* drawing_group, ifilehandler* handler) {
    int nret = BQ::OK;

    struct OfficeArtRecordHeader_Unpacked rh;
    const unsigned char* index = (const unsigned char*)drawing_group->file->mapping();
    uint32_t remaining = (uint32_t)drawing_group->file->size();

    if (nullptr == index || 0 == remaining || nullptr == handler) {
        goto done;
    }

    if (0 != read_office_art_record_header(index, remaining, &rh, is_little_endian)) {
        /* Failed to get header, abort. */
        goto done;
    }

    if (!((0xF == rh.recVer) && (0x000 == rh.recInstance) && (0xF000 == rh.recType))) {
        /* Invalid record values for drawing group record header */
        goto done;
    }

    if (rh.recLen > remaining) {
        /* Record header claims to be longer than our drawing group buffer */
    }

    /* Looks like we really found an Office Art Drawing Group (container).
     * See section 2.2.12 OfficeArtDggContainer in:
     * https://interoperability.blob.core.windows.net/files/MS-ODRAW/%5bMS-ODRAW%5d.pdf */
    MESSAGE_DEBUG(ctx, "[extract_images_from_drawing_group] Found drawing group of size %u bytes\n", rh.recLen);

    /* Just skip over this first header */
    if (remaining < sizeof(struct OfficeArtRecordHeader_PackedLittleEndian)) {
        remaining = 0;
    }
    else {
        remaining -= sizeof(struct OfficeArtRecordHeader_PackedLittleEndian);
        index += sizeof(struct OfficeArtRecordHeader_PackedLittleEndian);
    }

    while (0 < remaining) {
        if (0 != read_office_art_record_header(index, remaining, &rh, is_little_endian)) {
            /* Failed to get header, abort. */
            MESSAGE_DEBUG(ctx, "[extract_images_from_drawing_group] Failed to get header\n");
            goto done;
        }

        if (sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) > remaining) {
            MESSAGE_DEBUG(ctx, "[extract_images_from_drawing_group] Not enough data remaining for BLIP store.\n");
            goto done;
        }

        if ((0xF == rh.recVer) &&
            (0xF001 == rh.recType)) {
            /* Looks like we found a BLIP store container (array of OfficeArtBStoreContainerFileBlock records)
             * See section 2.2.20 OfficeArtBStoreContainer in:
             * https://interoperability.blob.core.windows.net/files/MS-ODRAW/%5bMS-ODRAW%5d.pdf */
            const unsigned char* start_of_blip_store_container = index + sizeof(struct OfficeArtRecordHeader_PackedLittleEndian);
            size_t blip_store_container_len = remaining - sizeof(struct OfficeArtRecordHeader_PackedLittleEndian);

            MESSAGE_DEBUG(ctx, "[extract_images_from_drawing_group] Found an OfficeArtBStoreContainerFileBlock (Blip store).\n");
            MESSAGE_DEBUG(ctx, "[extract_images_from_drawing_group] size: %u bytes, contains: %u file block records\n", rh.recLen, rh.recInstance);

            if (rh.recLen > blip_store_container_len) {
                MESSAGE_WARN(ctx, "[extract_images_from_drawing_group] WARNING: The blip store header claims to be bigger than the remaining bytes in the drawing group!\n");
                MESSAGE_WARN(ctx, "[extract_images_from_drawing_group]   %d > %zu!\n", rh.recLen, blip_store_container_len);
            }
            else {
                /* limit rgfb enumeration to the size of the blip store */
                blip_store_container_len = rh.recLen;
            }

            nret = process_blip_store_container(ctx, drawing_group, start_of_blip_store_container, blip_store_container_len, handler, is_little_endian);
            if (nret != 0) {
                goto done;
            }
        }

        if (remaining < sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) + rh.recLen) {
            remaining = 0;
        }
        else {
            remaining -= sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) + rh.recLen;
            index += sizeof(struct OfficeArtRecordHeader_PackedLittleEndian) + rh.recLen;
        }
    }

    nret = 0;
done:
    return nret;
}
