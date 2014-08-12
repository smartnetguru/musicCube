/* Automatically generated.  Do not edit */
/* See the mkopcodeh.awk script for details */
#define OP_MemLoad                              1
#define OP_HexBlob                            134   /* same as TK_BLOB     */
#define OP_Column                               2
#define OP_SetCookie                            3
#define OP_IfMemPos                             4
#define OP_Real                               133   /* same as TK_FLOAT    */
#define OP_Sequence                             5
#define OP_MoveGt                               6
#define OP_Ge                                  80   /* same as TK_GE       */
#define OP_RowKey                               7
#define OP_Eq                                  76   /* same as TK_EQ       */
#define OP_OpenWrite                            8
#define OP_NotNull                             74   /* same as TK_NOTNULL  */
#define OP_If                                   9
#define OP_ToInt                               10
#define OP_String8                             95   /* same as TK_STRING   */
#define OP_Pop                                 11
#define OP_CollSeq                             12
#define OP_OpenRead                            13
#define OP_Expire                              14
#define OP_AutoCommit                          15
#define OP_Gt                                  77   /* same as TK_GT       */
#define OP_IntegrityCk                         16
#define OP_Sort                                17
#define OP_Function                            18
#define OP_And                                 68   /* same as TK_AND      */
#define OP_Subtract                            87   /* same as TK_MINUS    */
#define OP_Noop                                19
#define OP_Return                              20
#define OP_Remainder                           90   /* same as TK_REM      */
#define OP_NewRowid                            21
#define OP_Multiply                            88   /* same as TK_STAR     */
#define OP_Variable                            22
#define OP_String                              23
#define OP_ParseSchema                         24
#define OP_Close                               25
#define OP_CreateIndex                         26
#define OP_IsUnique                            27
#define OP_IdxIsNull                           28
#define OP_NotFound                            29
#define OP_Int64                               30
#define OP_MustBeInt                           31
#define OP_Halt                                32
#define OP_Rowid                               33
#define OP_IdxLT                               34
#define OP_AddImm                              35
#define OP_Statement                           36
#define OP_RowData                             37
#define OP_MemMax                              38
#define OP_Push                                39
#define OP_Or                                  67   /* same as TK_OR       */
#define OP_NotExists                           40
#define OP_MemIncr                             41
#define OP_Gosub                               42
#define OP_Divide                              89   /* same as TK_SLASH    */
#define OP_Integer                             43
#define OP_ToNumeric                           44
#define OP_Prev                                45
#define OP_Concat                              91   /* same as TK_CONCAT   */
#define OP_BitAnd                              82   /* same as TK_BITAND   */
#define OP_CreateTable                         46
#define OP_Last                                47
#define OP_IsNull                              73   /* same as TK_ISNULL   */
#define OP_IdxRowid                            48
#define OP_MakeIdxRec                          49
#define OP_ShiftRight                          85   /* same as TK_RSHIFT   */
#define OP_ResetCount                          50
#define OP_FifoWrite                           51
#define OP_Callback                            52
#define OP_ContextPush                         53
#define OP_DropTrigger                         54
#define OP_DropIndex                           55
#define OP_IdxGE                               56
#define OP_IdxDelete                           57
#define OP_Vacuum                              58
#define OP_MoveLe                              59
#define OP_IfNot                               60
#define OP_DropTable                           61
#define OP_MakeRecord                          62
#define OP_ToBlob                              63
#define OP_Delete                              64
#define OP_AggFinal                            65
#define OP_ShiftLeft                           84   /* same as TK_LSHIFT   */
#define OP_Dup                                 66
#define OP_Goto                                70
#define OP_FifoRead                            71
#define OP_Clear                               72
#define OP_IdxGT                               81
#define OP_MoveLt                              93
#define OP_Le                                  78   /* same as TK_LE       */
#define OP_VerifyCookie                        96
#define OP_AggStep                             97
#define OP_Pull                                98
#define OP_ToText                              99
#define OP_Not                                 69   /* same as TK_NOT      */
#define OP_SetNumColumns                      100
#define OP_AbsValue                           101
#define OP_Transaction                        102
#define OP_Negative                            92   /* same as TK_UMINUS   */
#define OP_Ne                                  75   /* same as TK_NE       */
#define OP_ContextPop                         103
#define OP_BitOr                               83   /* same as TK_BITOR    */
#define OP_Next                               104
#define OP_IdxInsert                          105
#define OP_Distinct                           106
#define OP_Lt                                  79   /* same as TK_LT       */
#define OP_Insert                             107
#define OP_Destroy                            108
#define OP_ReadCookie                         109
#define OP_ForceInt                           110
#define OP_LoadAnalysis                       111
#define OP_OpenVirtual                        112
#define OP_Explain                            113
#define OP_OpenPseudo                         114
#define OP_Null                               115
#define OP_Blob                               116
#define OP_Add                                 86   /* same as TK_PLUS     */
#define OP_MemStore                           117
#define OP_Rewind                             118
#define OP_MoveGe                             119
#define OP_BitNot                              94   /* same as TK_BITNOT   */
#define OP_Found                              120
#define OP_NullRow                            121

/* The following opcode values are never used */
#define OP_NotUsed_122                        122
#define OP_NotUsed_123                        123
#define OP_NotUsed_124                        124
#define OP_NotUsed_125                        125
#define OP_NotUsed_126                        126
#define OP_NotUsed_127                        127
#define OP_NotUsed_128                        128
#define OP_NotUsed_129                        129
#define OP_NotUsed_130                        130
#define OP_NotUsed_131                        131
#define OP_NotUsed_132                        132

#define NOPUSH_MASK_0 65368
#define NOPUSH_MASK_1 47898
#define NOPUSH_MASK_2 47069
#define NOPUSH_MASK_3 49148
#define NOPUSH_MASK_4 65403
#define NOPUSH_MASK_5 30719
#define NOPUSH_MASK_6 53215
#define NOPUSH_MASK_7 997
#define NOPUSH_MASK_8 0
#define NOPUSH_MASK_9 0
