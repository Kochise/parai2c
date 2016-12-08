/******************************************************************************/
/* @F_NAME:              i2cbus.cpp                                           */
/* @F_PURPOSE:           C++ I2C bus control manager (on parallel port)       */
/* @F_CREATED_BY:        D. KOCH (david.koch@libertysurf.fr)                  */
/* @F_CREATION_DATE:     2000/04/21                                           */
/* @F_MODIFIED_BY:       D. KOCH (david.koch@libertysurf.fr)                  */
/* @F_MODIFICATION_DATE: 2001/11/23 - programmersheaven special version       */
/* @F_LANGUAGE :         ANSI C++                                             */
/* @F_MPROC_TYPE:        ALL COMPUTER WITH PC-LIKE MAPPED PARALLEL DATA PORT  */
/***** (C) Copyright 2001 Programmer's Heaven (info@programmersheaven.com) ****/

// IF your compiler encounter some troubles due to the comment frame's pattern,
// just look for all ... and replace them by ... (JUST characters in brakets) !
//               1 : '/\'                    '**'
//               2 : '/*\*'                  '/***'
//               3 : '*/*/'                  '***/'
//               4 : '/*\ '                  '/*  '
//               5 : '/*/ '                  '/*  '
//               6 : ' /*/'                  '  */'
//               7 : ' \*/'                  '  */'

/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\ A great thank for all who sent me any comment, GOOD or BAD...            /*/
/*/ I changed many things, especialy the names of functions and variables !  \*/
/*\ I also changed the typing structure of the code, NOT the code structure  /*/
/*/ itselves, in order to make it 'more readable' (see the IFs and RETURNs). \*/
/*\                                                                          /*/
/*/ THIS FILE ONLY COMPILE WITH BORLAND TURBO C++ (Visual C++ doesn't works) \*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\ INFORMATIONS (SOME KINDA)                                                /*/
/*/                                                                          \*/
/*\ ======================================================================== /*/
/*/                                                                          \*/
/*\ PARALLEL PORT AND CONVERTER STUDY                                        /*/
/*/                                                                          \*/
/*\      | LPT1 | LPT2           WRITE D7   D6   D5   D4   D3   D2   D1   D0 /*/
/*/ -----+------+------                 x    x    x    x    x /SCL /SDA  PWR \*/
/*\ WRITE| 378h | 278h                                                       /*/
/*/ -----+------+------          READ  D7   D6   D5   D4   D3   D2   D1   D0 \*/
/*\ READ | 379h | 279h                  x    x  SCL  SDA    x    x    x  PWR /*/
/*/                                                                          \*/
/*\ ======================================================================== /*/
/*/                                                                          \*/
/*\ ETUDE DU BUS I2C de PHILIPS                                              /*/
/*/                                                                          \*/
/*\   TRAME : {Sleep}                   SDA -------- 1                       /*/
/*/           .                                      0                       \*/
/*\           .                         SCL -------- 1                       /*/
/*/           .                                      0                       \*/
/*\           .                                                              /*/
/*/           .                                                              \*/
/*\           {Start}                   SDA --,      1                       /*/
/*/           .                               |_____ 0                       \*/
/*\           .                         SCL -----,   1                       /*/
/*/           .                                  |__ 0                       \*/
/*\           .                                                              /*/
/*/           .                                                              \*/
/*\           {Active}                  SDA          1                       /*/
/*/           .                             ________ 0                       \*/
/*\           .                         SCL          1                       /*/
/*/           .                             ________ 0                       \*/
/*\           .                                                              /*/
/*/           .                                                              \*/
/*\           . {Adresse.7bits}{R/W.1bit}                                    /*/
/*/           . {Ackowledge.1bit}                                            \*/
/*\           .                                                              /*/
/*/           .                                                              \*/
/*\           .   {Sending 0}           SDA          1  Set by MASTER        /*/
/*/           .                             ________ 0                       \*/
/*\           .                         SCL   ,--,   1  Set by MASTER        /*/
/*/           .                             __|  |__ 0                       \*/
/*\           .                                                              /*/
/*/           .                                                              \*/
/*\           .   {Sending 1}           SDA  ,----,  1  Set by MASTER        /*/
/*/           .                             _|    |_ 0                       \*/
/*\           .                         SCL   ,--,   1  Set by MASTER        /*/
/*/           .                             __|  |__ 0                       \*/
/*\           .                                                              /*/
/*/           .                                                              \*/
/*\           .   {Receiving 0}         SDA ??       1  Set by SLAVE !!!     /*/
/*/           .                             ??______ 0                       \*/
/*\           .                         SCL   ,--,   1  Set by MASTER        /*/
/*/           .                             __|  |__ 0                       \*/
/*\           .                                                              /*/
/*/           .                                                              \*/
/*\           .   {Receiving 1}         SDA ??------ 1  Set by SLAVE !!!     /*/
/*/           .                             ??       0                       \*/
/*\           .                         SCL   ,--,   1  Set by MASTER        /*/
/*/           .                             __|  |__ 0                       \*/
/*\           .                                                              /*/
/*/           .                                                              \*/
/*\           .                                                              /*/
/*/           . {Donnee.8bits}                                               \*/
/*\           . {Ackowledge.1bit}                                            /*/
/*/           .                                                              \*/
/*/           .                                                              \*/
/*\           .   {ACK OK to SLAVE}     SDA          1  Set by MASTER        /*/
/*/           .                             ________ 0---> OK when 0         \*/
/*\           .   Always send OK (0)    SCL   ,--,   1  Set by MASTER        /*/
/*/           .   if you cannot check       __|  |__ 0                       \*/
/*\           .   if the transmition                                         /*/
/*/           .   was OK                                                     \*/
/*/           .                                                              \*/
/*\           .   {ACK from SLAVE}      SDA  ,?????? 1---> ERROR when 1      /*/
/*/           .                             _|?????? 0---> OK when 0         \*/
/*\           .                         SCL   ,--,   1  Set by MASTER        /*/
/*/           .                             __|  |__ 0                       \*/
/*\           .                                                              /*/
/*/           .  1/ MASTER set SDA to 1                                      \*/
/*\           .  2/ MASTER Set SCL to 1                                      /*/
/*/           .  3/ On rising front of SCL, SLAVE reply on SDA               \*/
/*\           .       IF an error occured, SLAVE keep SDA to 1               /*/
/*/           .       ELSE the SLAVE set SDA to 0                            \*/
/*\           .                                                              /*/
/*/           .                                                              \*/
/*\           {Stop}                    SDA      ,-- 1                       /*/
/*/           .                             _____|   0                       \*/
/*\           .                         SCL   ,----- 1                       /*/
/*/           .                             __|      0                       \*/
/*\           .                                                              /*/
/*/           .                                                              \*/
/*\           {Sleep}                   SDA -------- 1                       /*/
/*/                                                  0                       \*/
/*\                                     SCL -------- 1                       /*/
/*/                                                  0                       \*/
/*\                                                                          /*/
/*/                                                                          \*/
/*\ ======================================================================== /*/
/*/                                                                          \*/
/*\ FRAMES STRUCTURES                                                        /*/
/*/                                                                          \*/
/*\           Frame offset                                                   /*/
/*/                                                                          \*/
/*\   Module |    0 |     1 |     2 |     3 |     4 |     5 |     6 |     7  /*/
/*/   -------+------+-------+-------+-------+-------+-------+-------+------  \*/
/*\   Displ  W 0x70 |     0 | 0x077 | Digi1 | Digi2 | Digi3 | Digi4 |        /*/
/*/   -------+------+-------+-------+-------+-------+-------+-------+------  \*/
/*\   Clock  W 0xA2 |  SAh  | W@SAh |W@SAh+1|W@SAh+2|       |       |        /*/
/*/          R 0xA3 | R@00h | R@01h |   ... |   ... |   ... |   ... |   ...  \*/
/*\                                                                          /*/
/*/                                                                          \*/
/*\ ======================================================================== /*/
/*/                                                                          \*/
/*\ DISPLAY (ADRESS 0x070)                                                   /*/
/*/                                                                          \*/
/*\        bit0                                                              /*/
/*/        ----                                                              \*/
/*\   bit5|    |bit1                                                         /*/
/*/       |bit6|                                                             \*/
/*\        ----                                                              /*/
/*/   bit4|    |bit2                                                         \*/
/*\       |    |                                                             /*/
/*/        ----  o bit7                                                      \*/
/*\        bit3                                                              /*/
/*/                                                                          \*/
/*\ ======================================================================== /*/
/*/ Get more informations about the I2C bus on http://www.philips.com        \*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\ CODE OVERVIEW                                                            /*/
/*/                                                                          \*/
/*\   INCLUDES                                                               /*/
/*/   DEFINES (CONSTANTS, ...)                                               \*/
/*\   CLASS oFI2C DEFINITION                                                 /*/
/*/     CONSTRUCTORS AND DESTRUCTORS                                         \*/
/*\     LEVEL 0 METHODS (LOW LEVEL)                                          /*/
/*/       oFI2C_BUS_Init                                                     \*/
/*\       oFI2C_BUS_Off                                                      /*/
/*/       oFI2C_BUS_Test                                                     \*/
/*\       oFI2C_BIT_Set                                                      /*/
/*/       oFI2C_BIT_Clear                                                    \*/
/*\       oFI2C_BIT_Test                                                     /*/
/*/     LEVEL 1 METHODS (LOGIC LEVEL)                                        \*/
/*\       oFI2C_BUS_Start                                                    /*/
/*/       oFI2C_BUS_Stop                                                     \*/
/*\       oFI2C_ACK_Sending                                                  /*/
/*/       oFI2C_ACK_Receiving                                                \*/
/*\     LEVEL 2 METHODS (MACRO LEVEL)                                        /*/
/*/       oFI2C_DAT_Send                                                     \*/
/*\       oFI2C_DAT_Receive                                                  /*/
/*/       oFI2C_DAT_Convert                                                  \*/
/*\     LEVEL 3 METHODS (FUNCTIONNAL LEVEL)                                  /*/
/*/       oFI2C_FRM_Send                                                     \*/
/*\       oFI2C_FRM_Receive                                                  /*/
/*/                                                                          \*/
/*\   MAIN PROGRAM                                                           /*/
/*/                                                                          \*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\ 'INCLUDES'                                                               /*/
/*/                                                                          \*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

#include  <conio.h>
#include  <stdio.h>
#include  <dos.h>              // For 'delay'
#include  <iostream.h>         // For 'cout'

/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\ 'DEFINES'                                                                /*/
/*/                                                                          \*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

/*** cTRUE ********************************************************************/
/* Purpose : TRUE condition flag                                              */
/* Unit    : None                                                             */
/* Range   : TRUE / FALSE                                                     */
/* List    : None                                                             */
/* Example : None                                                             */
/* WARNING : ALWAYS (1==1)                                                    */
/******************************************************************************/
#define cTRUE (1==1) // ALWAYS generate a TRUE constant

/*** cFALSE *******************************************************************/
/* Purpose : FALSE condition flag                                             */
/* Unit    : None                                                             */
/* Range   : TRUE / FALSE                                                     */
/* List    : None                                                             */
/* Example : None                                                             */
/* WARNING : ALWAYS (1==0)                                                    */
/******************************************************************************/
#define cFALSE (1==0) // ALWAYS generate a FALSE constant

/*** tBIT *********************************************************************/
/* Purpose : BIT type definition                                              */
/* Unit    : None                                                             */
/* Range   : [0-1] - LIMITED selection                                        */
/* List    : None                                                             */
/* Example : None                                                             */
/******************************************************************************/
typedef bool tBIT;

/*** tUBYTE *******************************************************************/
/* Purpose : Unsigned BYTE type definition                                    */
/* Unit    : None                                                             */
/* Range   : [0-255] - LIMITED selection                                      */
/* List    : None                                                             */
/* Example : None                                                             */
/******************************************************************************/
typedef unsigned char tUBYTE;

/*** tSBYTE *******************************************************************/
/* Purpose : Signed BYTE type definition                                      */
/* Unit    : None                                                             */
/* Range   : [-128 - +127] - LIMITED selection                                */
/* List    : None                                                             */
/* Example : None                                                             */
/******************************************************************************/
typedef char tSBYTE;

/*** tUWORD *******************************************************************/
/* Purpose : Unsigned WORD type definition                                    */
/* Unit    : None                                                             */
/* Range   : [0-65535] - LIMITED selection                                    */
/* List    : None                                                             */
/* Example : None                                                             */
/******************************************************************************/
typedef unsigned int tUWORD;

/*** tSWORD *******************************************************************/
/* Purpose : Signed WORD type definition                                      */
/* Unit    : None                                                             */
/* Range   : [-32768 - +32767] - LIMITED selection                            */
/* List    : None                                                             */
/* Example : None                                                             */
/******************************************************************************/
typedef int tSWORD; // Signed LOW gives tSLOW (ROTFL ;)=)

/*** tULONG *******************************************************************/
/* Purpose : Unsigned LONG type definition                                    */
/* Unit    : None                                                             */
/* Range   : [0-4294967296] - 'LIMITED' selection                             */
/* List    : None                                                             */
/* Example : None                                                             */
/******************************************************************************/
typedef unsigned long tULONG;

/*** tSLONG *******************************************************************/
/* Purpose : Signed LONG type definition                                      */
/* Unit    : None                                                             */
/* Range   : [-2147483648 - +2147483647] - 'LIMITED' selection                */
/* List    : None                                                             */
/* Example : None                                                             */
/******************************************************************************/
typedef long tSLONG;

/*** TEMPO ********************************************************************/
/* Purpose : Enable/Disable TEMPO                                             */
/* Unit    : DEFINE selection                                                 */
/* Range   : None                                                             */
/* List    : None                                                             */
/* Example : //#define TEMPO -> Disable TEMPO                                 */
/******************************************************************************/
//#define TEMPO

/*** DISPLAY ******************************************************************/
/* Purpose : Enable/Disable DISPLAY                                           */
/* Unit    : DEFINE selection                                                 */
/* Range   : None                                                             */
/* List    : None                                                             */
/* Example : #define DISPLAY -> Enable DISPLAY module                         */
/******************************************************************************/
#define DISPLAY

/*** CLOCK ********************************************************************/
/* Purpose : Enable/Disable CLOCK                                             */
/* Unit    : DEFINE selection                                                 */
/* Range   : None                                                             */
/* List    : None                                                             */
/* Example : //#define CLOCK -> Disable CLOCK module                          */
/******************************************************************************/
//#define CLOCK

/*** cI2C_BIT *****************************************************************/
/* Purpose : I2C bit selection                                                */
/* Unit    : None                                                             */
/* Range   : enum LIST                                                        */
/* List    : 0 = cPWR                                                         */
/*           1 = cSCL                                                         */
/*           2 = cSDA                                                         */
/* Example : None                                                             */
/******************************************************************************/
enum
{
  cPWR,
  cSCL,
  cSDA
}cI2C_BIT;

/*** cI2C_ARRAY ***************************************************************/
/* Purpose : I2C bit bit array definition in oFI2C                            */
/* Unit    : None                                                             */
/* Range   : enum LIST                                                        */
/* List    : 0  = RPWR_BIT   1 = RPWR_LVL                                     */
/*           2  = RSCL_BIT   3 = RSCL_LVL                                     */
/*           4  = RSDA_BIT   5 = RSDA_LVL                                     */
/*           6  = WPWR_BIT   7 = WPWR_LVL                                     */
/*           8  = WSCL_BIT   9 = WSCL_LVL                                     */
/*           10 = WSDA_BIT  11 = WSDA_LVL                                     */
/* Example : None                                                             */
/******************************************************************************/
enum
{
  RPWR_BIT, RPWR_LVL,
  RSCL_BIT, RSCL_LVL,
  RSDA_BIT, RSDA_LVL,
  WPWR_BIT, WPWR_LVL,
  WSCL_BIT, WSCL_LVL,
  WSDA_BIT, WSDA_LVL
}cI2C_ARRAY;

/*** cADDR_BASE_WLPT **********************************************************/
/* Purpose : Base address of the LPT 1 port for writing                       */
/* Unit    : None                                                             */
/* Range   : Mem ADDR - FREE selection                                        */
/* List    : 0x0378 -> LPT1                                                   */
/*           0x0278 -> LPT2                                                   */
/* Example : #define cADDR_BASE_WLPT 0x0378 -> Writing addr of LPT1           */
/* WARNING : Due to the code structure, keep here >ALWAYS< 0x0378.            */
/******************************************************************************/
#define cADDR_BASE_WLPT 0x0378

/*** cADDR_BASE_RLPT **********************************************************/
/* Purpose : Base address of the LPT 1 port for reading                       */
/* Unit    : None                                                             */
/* Range   : Mem ADDR - FREE selection                                        */
/* List    : 0x0379 -> LPT1                                                   */
/*           0x0279 -> LPT2                                                   */
/* Example : #define cADDR_BASE_RLPT 0x0379 -> Reading addr of LPT1           */
/* WARNING : Keep here >ALWAYS< cADDR_BASE_WLPT+1 (code structure)            */
/******************************************************************************/
#define cADDR_BASE_RLPT cADDR_BASE_WLPT+1

/*** cDATA_LOOP_TRY ***********************************************************/
/* Purpose : Number of times the Receice routine will try to fetch the datas  */
/* Unit    : None                                                             */
/* Range   : [0-3-...] - FREE selection                                       */
/* List    : 0 -> Only 1 try, else error                                      */
/*           3 -> Will try 3 times before generating an error                 */
/* Example : #define cDATA_LOOP_TRY 3                                         */
/******************************************************************************/
#define cDATA_LOOP_TRY 3

/*** cDATA_TIME_TEMPO *********************************************************/
/* Purpose : Time between two I2C clock cycles                                */
/* Unit    : 1/1000 s                                                         */
/* Range   : [...-10-100-...] - FREE selection                                */
/* List    :  10 -> 100 Bauds                                                 */
/*           100 -> 10 Bauds                                                  */
/* Example : #define cDATA_TIME_TEMPO 1 -> 1000 Bauds                         */
/* WARNING : 0 works fine !                                                   */
/******************************************************************************/
#define cDATA_TIME_TEMPO 0

/*** cBITN_LPTP_POWER *********************************************************/
/* Purpose : Bit number of the Power line of the I2C Converter card           */
/* Unit    : Bit Number                                                       */
/* Range   : [0-7] - LIMITED selection                                        */
/* List    : Depend of the conversion card plugged on the parallel data port  */
/* Example : #define cBITN_LPTP_POWER 0                                       */
/******************************************************************************/
#define cBITN_LPTP_POWER 0

/*** cBITN_LPTP_WSDA **********************************************************/
/* Purpose : Bit number of the SDA line on the write port                     */
/* Unit    : Bit Number                                                       */
/* Range   : [0-7] - LIMITED selection                                        */
/* List    : Depend of the conversion card plugged on the parallel data port  */
/* Example : #define cBITN_LPTP_WSDA 1                                        */
/******************************************************************************/
#define cBITN_LPTP_WSDA 1

/*** cBITN_LPTP_WSCL **********************************************************/
/* Purpose : Bit number of the SCL line on the write port                     */
/* Unit    : Bit Number                                                       */
/* Range   : [0-7] - LIMITED selection                                        */
/* List    : Depend of the conversion card plugged on the parallel data port  */
/* Example : #define cBITN_LPTP_WSCL 2                                        */
/******************************************************************************/
#define cBITN_LPTP_WSCL 2

/*** cBITN_LPTP_RSDA **********************************************************/
/* Purpose : Bit number of the SDA line on the read port                      */
/* Unit    : Bit Number                                                       */
/* Range   : [0-7] - LIMITED selection                                        */
/* List    : Depend of the conversion card plugged on the parallel dat port   */
/* Example : #define cBITN_LPTP_RSDA 4                                        */
/******************************************************************************/
#define cBITN_LPTP_RSDA 4

/*** cBITN_LPTP_RSCL **********************************************************/
/* Purpose : Bit number of the SCL line on the read port                      */
/* Unit    : Bit Number                                                       */
/* Range   : [0-7] - LIMITED selection                                        */
/* List    : Depend of the conversion card plugged on the parallel dat port   */
/* Example : #define cBITN_LPTP_RSCL 5                                        */
/******************************************************************************/
#define cBITN_LPTP_RSCL 5

/*** cADDR_I2CM_DISP **********************************************************/
/* Purpose : I2C module address for the displayer                             */
/* Unit    : None                                                             */
/* Range   : Mem ADDR - FREE selection                                        */
/* List    : Depend on the displayer module configuration                     */
/* Example : #define cADDR_I2CM_DISP 0x070                                    */
/******************************************************************************/
#define cADDR_I2CM_DISP 0x070

/*** cADDR_I2CM_CLOCK *********************************************************/
/* Purpose : I2C module address for the Real-Time Clock                       */
/* Unit    : None                                                             */
/* Range   : Mem ADDR - FREE selection                                        */
/* List    : Depend on the RTC module configuration                           */
/* Example : #define cADDR_I2CM_CLOCK 0x0A2                                   */
/******************************************************************************/
#define cADDR_I2CM_CLOCK 0x0A2

/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\ 'CLASS/OBJECT' DEFINITION                                                /*/
/*/                                                                          \*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

class oFI2C // oFI2C object
{
  private: // Can ONLY be used in oFI2C's methods (as LOCAL variables)
    // 'Characteristics' of the oFI2C object /////////////////////////////
    tUBYTE vFI2C_LPT_DATA;  // DATA buffer
    tUBYTE vFI2C_LPT_MASK;  // MASK          (calculated once ! in oFI2C_BUS_Init)
    tUWORD vFI2C_LPT_RPORT; // READ  address (calculated once ! in oFI2C_BUS_Init)
    tUWORD vFI2C_LPT_WPORT; // WRITE address (calculated once ! in oFI2C_BUS_Init)
    tUWORD vFI2C_LPT_NPORT;

    tUBYTE cFI2C_LPT_MAPPING[]={ // BIT NUMBER, ACTIVE LEVEL <- according to
                                 cBITN_LPTP_POWER, 1, // RPWR    the conversion
                                 cBITN_LPTP_RSCL , 1, // RSCL    card
                                 cBITN_LPTP_RSDA , 1, // RSDA
                                 cBITN_LPTP_POWER, 1, // WPWR
                                 cBITN_LPTP_WSCL , 0, // WSCL
                                 cBITN_LPTP_WSDA , 0  // WSDA
                               };

  public:  // Can be used EVERYWHERE
    // 'Constructors/Destructors' ////////////////////////////////////////
    oFI2C  ();
    oFI2C  (int vPARAM_BUS_LPT_Port);
    ~oFI2C ();

    // 'Methods' /////////////////////////////////////////////////////////
      // LEVEL 0 METHODS (LOW LEVEL)
    void   oFI2C_BUS_Init      (tUWORD vPARAM_BUS_LPT_Port);
    void   oFI2C_BUS_Off       (void);
    tBIT   oFI2C_BUS_Test      (void);
    void   oFI2C_BIT_Set       (tUWORD vPARAM_BIT_ID);
    void   oFI2C_BIT_Clear     (tUWORD vPARAM_BIT_ID);
    tBIT   oFI2C_BIT_Test      (tUWORD vPARAM_BIT_ID);
      // LEVEL 1 METHODS (LOGIC LEVEL)
    void   oFI2C_BUS_Start     (void);
    void   oFI2C_BUS_Stop      (void);
    tBIT   oFI2C_ACK_Sending   (void);
    void   oFI2C_ACK_Receiving (void);
      // LEVEL 2 METHODS (MACRO LEVEL)
    void   oFI2C_DAT_Send      (tUBYTE vPARAM_DAT_Data_To_Send);
    tUBYTE oFI2C_DAT_Receive   (void);
    tUBYTE oFI2C_DAT_Convert   (tUBYTE vPARAM_DAT_ASCII_Char);
      // LEVEL 3 METHODS (FUNCTIONNAL LEVEL)
    tBIT   oFI2C_FRM_Send      (tUBYTE vPARAM_FRM_Frame_To_Send[]);
    tBIT   oFI2C_FRM_Receive   (tUBYTE vPARAM_FRM_Frame_To_Receive[]);
};

/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\ 'CONSTRUCTORS/DESTRUCTORS'                                               /*/
/*/                                                                          \*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C                                                               */
/* Role : Default constructor, when no parameter is given                     */
/* Interface : None                                                           */
/* Pre-condition : None                                                       */
/* Constraints : None                                                         */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Configure the parallel port LPT 1]                                    */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C                                                                 */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Configure the parallel port LPT 1] =                                    */
/*   DO                                                                       */
/*     [Call the initialisation routine for the LPT1 port]                    */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
oFI2C::oFI2C()
{
  oFI2C_BUS_Init(1);                // Port LPT1 par defaut
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C                                                               */
/* Role : Constructor with parameter                                          */
/* Interface : None                                                           */
/* Pre-condition : None                                                       */
/* Constraints : None                                                         */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Configure the selected parallel port]                                 */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C                                                                 */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Configure the selected parallel port] =                                 */
/*   DO                                                                       */
/*     [Call the initialisation routine with the selected port as parameter]  */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
oFI2C::oFI2C(int vPARAM_BUS_LPT_Port)
{
  oFI2C_BUS_Init(vPARAM_BUS_LPT_Port);
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C                                                               */
/* Role : Destructor automatically called at program exit                     */
/* Interface : None                                                           */
/* Pre-condition : None                                                       */
/* Constraints : None                                                         */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Restore the default/selected parallel port]                           */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C                                                                 */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Restore the default/selected parallel port] =                           */
/*   DO                                                                       */
/*     [Call the routine that will cut off the power of the conversion card]  */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
oFI2C::~oFI2C()
{
  oFI2C_BUS_Off();                     // A la fin, on COUPE tout...
}

/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\ 'METHODES'                                                               /*/
/*/                                                                          \*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C_BUS_Init                                                      */
/* Role : Switch on the card on and put it in STOP mode                       */
/* Interface :  IN AskedLptPort                                               */
/*                 1 - LPT 1                                                  */
/*                 2 - LPT 2                                                  */
/* Pre-condition : None                                                       */
/* Constraints : None                                                         */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Power ON and STOP condition]                                          */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C_BUS_Init                                                        */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Power ON and STOP condition] =                                          */
/*   DO                                                                       */
/*     [Calculate the READ address according to the selected LPT port]        */
/*     [Calculate the WRITE address according to the selected LPT port]       */
/*     [Calculate the MASK for faster access]                                 */
/*     [Set SDA and SCL to '1']                                               */
/*     [Power ON the conversion card, set POWER to '1']                       */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
void oFI2C::oFI2C_BUS_Init(tUWORD vPARAM_BUS_LPT_Port)
{
  vFI2C_LPT_NPORT = (vPARAM_BUS_LPT_Port - 1) & 1;
  vFI2C_LPT_RPORT = cADDR_BASE_RLPT - (0x0100 * vFI2C_LPT_NPORT);
  vFI2C_LPT_WPORT = cADDR_BASE_WLPT - (0x0100 * vFI2C_LPT_NPORT);

  vFI2C_LPT_MASK =   ((cFI2C_LPT_MAPPING[RPWR_LVL] & 1) << (cFI2C_LPT_MAPPING[RPWR_BIT] % 8))  // POWER
                   | ((cFI2C_LPT_MAPPING[RSCL_LVL] & 1) << (cFI2C_LPT_MAPPING[RSCL_BIT] % 8))  // RSCL
                   | ((cFI2C_LPT_MAPPING[RSDA_LVL] & 1) << (cFI2C_LPT_MAPPING[RSDA_BIT] % 8)); // RSDA
                   | ((cFI2C_LPT_MAPPING[WSCL_LVL] & 1) << (cFI2C_LPT_MAPPING[WSCL_BIT] % 8))  // WSCL
                   | ((cFI2C_LPT_MAPPING[WSDA_LVL] & 1) << (cFI2C_LPT_MAPPING[WSDA_BIT] % 8)); // WSDA
  
  vFI2C_LPT_DATA =   (1 << (cFI2C_LPT_MAPPING[WSCL_BIT] % 8))  // WSCL == 1
                   | (1 << (cFI2C_LPT_MAPPING[WSDA_BIT] % 8)); // WSDA == 1

  oFI2C_BIT_Set(cPWR); // Set PWR == 1, then put the whole BUFFER onto the PORT (WSCL, WSDA and WPWR)

#ifdef TEMPO
  delay(cDATA_TIME_TEMPO);
#endif // TEMPO
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C_BUS_Off                                                       */
/* Role : Restore I2C bus mode                                                */
/* Interface : None                                                           */
/* Pre-condition : None                                                       */
/* Constraints : None                                                         */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Clear the I2C and switch off the power]                               */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C_BUS_Off                                                         */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Clear the I2C and switch off the power] =                               */
/*   DO                                                                       */
/*     [Put the I2C bus lines to low level, set SDA and SCL to '0']           */
/*     [Switch off the power, set POWER to '0']                               */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
void oFI2C::oFI2C_BUS_Off(void)
{
  oFI2C_BIT_Clear(cSDA);
  oFI2C_BIT_Clear(cSCL);

  oFI2C_BIT_Clear(cPWR);

#ifdef TEMPO
  delay(cDATA_TIME_TEMPO);
#endif // TEMPO
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C_BUS_Test                                                      */
/* Role : Test if the I2C bus is FREE, in STOP mode (SDA and SCL to '1')      */
/* Interface : OUT oFI2C_BUS_Test                                             */
/*                 cFALSE - Error                                             */
/*                 cTRUE  - OK, bus FREE                                      */
/* Pre-condition : None                                                       */
/* Constraints : Cannot define in which context we are : Read, Write or Ack ? */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Test if the I2C bus is in STOP mode]                                  */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C_BUS_Test                                                        */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Test if the I2C bus is in STOP mode] =                                  */
/*   IF [SDA and SCL to '1', then the I2C bus is FREE, return 1] THEN         */
/*     [The I2C is FREE, return cTRUE]                                        */
/*   ELSE                                                                     */
/*     [The I2C is NOT FREE, return cFALSE]                                   */
/*   FI                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
bool oFI2C::oFI2C_BUS_Test(void)
{
  return(
             oFI2C_BIT_Test(cSDA)
          && oFI2C_BIT_Test(cSCL)
        );
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C_BIT_Set                                                       */
/* Role : Set a bit of the LPT data port to '1'                               */
/* Interface :  IN NbBit                                                      */
/*                 [0-7] - Bit number to set to '1'                           */
/* Pre-condition : None                                                       */
/* Constraints : None                                                         */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Set the selected bit to '1']                                          */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C_BIT_Set                                                         */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Set the selected bit to '1'] =                                          */
/*   DO                                                                       */
/*     [Set the bit to '1' in the LPT data buffer, using OR and ROL modulo 8] */
/*     [Output the buffer onto the selected LPT data port]                    */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
void oFI2C::oFI2C_BIT_Set(tUWORD vPARAM_BIT_ID)
{
  vFI2C_LPT_DATA |= (
                         1 
                      << (
                             cFI2C_LPT_MAPPING[(vPARAM_BIT_ID * 2) + 6]
                           % 8
                         )
                    );

  outportb(
            vFI2C_LPT_WPORT,
            (
                ( vFI2C_LPT_DATA &  vFI2C_LPT_MASK) // HIGH LEVEL
              | (~vFI2C_LPT_DATA & ~vFI2C_LPT_MASK) // LOW  LEVEL
            )
          );
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C_BIT_Clear                                                     */
/* Role : Set a bit of the LPT data port to '0'                               */
/* Interface :  IN NbBit                                                      */
/*                 [0-7] - Bit number to set to '0'                           */
/* Pre-condition : None                                                       */
/* Constraints : None                                                         */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Set the selected bit to '0']                                          */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C_BIT_Clear                                                       */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Set the selected bit to '0'] =                                          */
/*   DO                                                                       */
/*     [Set the bit to '0' in the LPT data buffer, AND, XOR and ROL modulo 8] */
/*     [Output the buffer onto the selected LPT data port]                    */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
void oFI2C::oFI2C_BIT_Clear(tUWORD vPARAM_BIT_ID)
{
  vFI2C_LPT_DATA &= (
                        (-1)
                      ^ (
                             1
                          << (
                                 cFI2C_LPT_MAPPING[(vPARAM_BIT_ID * 2) + 6]
                               % 8
                             )
                        )
                    );
  outportb(
            vFI2C_LPT_WPORT,
              ( vFI2C_LPT_DATA &  vFI2C_LPT_MASK) // HIGH LEVEL
            | (~vFI2C_LPT_DATA & ~vFI2C_LPT_MASK) // LOW  LEVEL
          );
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C_BIT_Test                                                      */
/* Role : Return the state of a selected bit of the LPT data port             */
/* Interface :  IN NbBit                                                      */
/*                 [0-7] - Bit number to set to '0'                           */
/*             OUT oFI2C_BIT_Test                                             */
/* Pre-condition : None                                                       */
/* Constraints : None                                                         */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Test the status of the selected bit]                                  */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C_BIT_Test                                                        */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Test the status of the selected bit] =                                  */
/*   DO                                                                       */
/*     [Return the selected bit state, using ROR modulo 8 and AND 1]          */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
tBIT oFI2C::oFI2C_BIT_Test(tUWORD vPARAM_BIT_ID)
{
  register tUBYTE vLOCAL_BIT_Test; // Force REGISTER usage instead of STACK

  vLOCAL_BIT_Test = inportb(vFI2C_LPT_RPORT);

  return(
            (
                 (
                     ( vLOCAL_BIT_Test &  vFI2C_LPT_MASK) // HIGH LEVEL
                   | (~vLOCAL_BIT_Test & ~vFI2C_LPT_MASK) // LOW  LEVEL
                 )
              >> (
                     cFI2C_LPT_MAPPING[vPARAM_BIT_ID * 2] 
                   % 8
                 )
            )
          & 1
        );
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C_BUS_Start                                                     */
/* Role : Create a START condition on the I2C bus                             */
/* Interface : None                                                           */
/* Pre-condition : MUST be in STOP mode                                       */
/* Constraints : None                                                         */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Put the I2C bus in START mode]                                        */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C_BUS_Start                                                       */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Put the I2C bus in START mode] =                                        */
/*   DO                                                                       */
/*     [Put SDA to '0' and SCL to '1', falling edge of SDA first]             */
/*     [Put SDA to '0' and SCL to '0', falling edge of SCL then]              */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
void oFI2C::oFI2C_BUS_Start(void)
{
  oFI2C_BIT_Clear(cSDA);
#ifdef TEMPO
  delay(cDATA_TIME_TEMPO);
#endif // TEMPO

  oFI2C_BIT_Clear(cSCL);
#ifdef TEMPO
  delay(cDATA_TIME_TEMPO);
#endif // TEMPO
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C_BUS_Stop                                                      */
/* Role : Create a STOP condition on the I2C bus                              */
/* Interface : None                                                           */
/* Pre-condition : None                                                       */
/* Constraints : None                                                         */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Put the I2C bus in STOP mode]                                         */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C_BUS_Stop                                                        */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Put the I2C bus in STOP mode] =                                         */
/*   DO                                                                       */
/*     [Put SDA to '0' and SCL to '1', rising edge of SCL first]              */
/*     [Put SDA to '1' and SCL to '1', rising edge of SDA then]               */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
void oFI2C::oFI2C_BUS_Stop(void)
{
  oFI2C_BIT_Set(cSCL);
#ifdef TEMPO
  delay(cDATA_TIME_TEMPO);
#endif // TEMPO

  oFI2C_BIT_Set(cSDA);
#ifdef TEMPO
  delay(cDATA_TIME_TEMPO);
#endif // TEMPO
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C_ACK_Sending                                                   */
/* Role : 'Acknowledge' condition returned by the Slave on emitting           */
/* Interface : OUT oFI2C_ACK_Sending                                          */
/*                 cFALSE - Error                                             */
/*                 cTRUE  - OK                                                */
/* Pre-condition : A byte MUST have been sent to a slave                      */
/* Constraints : None                                                         */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Return Slave's 'Acknowledge']                                         */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C_ACK_Sending                                                     */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Return Slave's 'Acknowledge'] =                                         */
/*   DO                                                                       */
/*     [Set SDA to '1', and keep SCL to '0']                                  */
/*     [Set SCL to '1', rising edge of SCL]                                   */
/*     [The Slave change the state of SDA according to the its receive state] */
/*     [Set SDA and SCL to '0']                                               */
/*     [Return the Slave's receive state]                                     */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/* Don't believe the WARNING issued there, da stuff is supposed to work FINE  */
/*--- END FUNCTION HEADER ----------------------------------------------------*/
tBIT oFI2C::oFI2C_ACK_Sending(void)
{
  tBIT vLOCAL_ACK_Slave_Reply;

  oFI2C_BIT_Set(cSDA);
#ifdef TEMPO
  delay(cDATA_TIME_TEMPO);
#endif // TEMPO

  oFI2C_BIT_Set(cSCL);
#ifdef TEMPO
  delay(cDATA_TIME_TEMPO);
#endif // TEMPO

  vLOCAL_ACK_Slave_Reply = (~oFI2C_BIT_Test(cSDA)) & 1;

  oFI2C_BIT_Clear(cSDA);
  oFI2C_BIT_Clear(cSCL);
#ifdef TEMPO
  delay(cDATA_TIME_TEMPO);
#endif // TEMPO

  return vLOCAL_ACK_Slave_Reply;
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C_ACK_Receiving                                                 */
/* Role : 'Acknowledge' condition returned TO the Slave on receiving from     */
/* Interface : None                                                           */
/* Pre-condition : None                                                       */
/* Constraints : ALWAYS return OK !                                           */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Return OK to the Slave]                                               */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C_ACK_Receiving                                                   */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Return OK to the Slave] =                                               */
/*   DO                                                                       */
/*     [Set SDA to '0', reception OK]                                         */
/*     [Validate with a complete clock cycle, SCL to '1', then '0']           */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
void oFI2C::oFI2C_ACK_Receiving(void)
{
  oFI2C_BIT_Clear(cSDA);
#ifdef TEMPO
  delay(cDATA_TIME_TEMPO);
#endif // TEMPO

  oFI2C_BIT_Set(cSCL);
#ifdef TEMPO
  delay(cDATA_TIME_TEMPO);
#endif // TEMPO

  oFI2C_BIT_Clear(cSCL);
#ifdef TEMPO
  delay(cDATA_TIME_TEMPO);
#endif // TEMPO
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C_DAT_Send                                                      */
/* Role : Send 8 bits on the I2C bus                                          */
/* Interface :  IN Data2Send                                                  */
/* Pre-condition : None                                                       */
/* Constraints : None                                                         */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Send 8 bits, from MSB to LSB]                                         */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C_DAT_Send                                                        */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Send 8 bits, from MSB to LSB] =                                         */
/*   DO                                                                       */
/*     WHILE [Counter = 7 to 0]                                               */
/*       DO                                                                   */
/*         [Set SDA to the bit number 'Counter' state]                        */
/*         [Validate with a complete clock cycle, SCL to '1', then '0']       */
/*       OD                                                                   */
/*     [At the end, set SDA and SCL to '0']                                   */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
void oFI2C::oFI2C_DAT_Send(tUBYTE vPARAM_DAT_Data_To_Send)
{
  tUBYTE vLOCAL_DAT_Send_Loop;

  // MSB -> LSB
  for(
       vLOCAL_DAT_Send_Loop = 7;
       vLOCAL_DAT_Send_Loop >= 0;
       vLOCAL_DAT_Send_Loop --
     )
  {
    if(
          (
               vPARAM_DAT_Data_To_Send
            >> vLOCAL_DAT_Send_Loop
          )
        & 1 // KEEP and SEND the BIT 0
      )
    {
      oFI2C_BIT_Set(cSDA);
    }
    else
    {
      oFI2C_BIT_Clear(cSDA);
    }
#ifdef TEMPO
    delay(cDATA_TIME_TEMPO);
#endif // TEMPO

    oFI2C_BIT_Set(cSCL);
#ifdef TEMPO
    delay(cDATA_TIME_TEMPO);
#endif // TEMPO

    oFI2C_BIT_Clear(cSCL);
#ifdef TEMPO
    delay(cDATA_TIME_TEMPO);
#endif // TEMPO
  }

  oFI2C_BIT_Clear(cSDA);
#ifdef TEMPO
  delay(cDATA_TIME_TEMPO);
#endif // TEMPO
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C_DAT_Receive                                                   */
/* Role : Receive 8 bits on the I2C bus                                       */
/* Interface :  OUT oFI2C_DAT_Receive                                         */
/* Pre-condition : None                                                       */
/* Constraints : None                                                         */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Receive 8 bits, from MSB to LSB]                                      */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C_DAT_Receive                                                     */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Receive 8 bits, from MSB to LSB] =                                      */
/*   DO                                                                       */
/*     WHILE [Counter = 7 to 0]                                               */
/*       DO                                                                   */
/*         [Set SCL to '1', rising edge of SCL]                               */
/*         [Set the buffetr bit number 'Counter' to SDA state]                */
/*         [Set SCL to '0', falling edge of SCL]                              */
/*         [Clear SDA and let the Slave set its state at the next loop]       */
/*       OD                                                                   */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
tUBYTE oFI2C::oFI2C_DAT_Receive(void)
{
  tUBYTE vLOCAL_DAT_Receive_Data = 0;
  tUWORD vLOCAL_DAT_Receive_Loop;

  // MSB -> LSB
  for(
       vLOCAL_DAT_Receive_Loop = 7;
       vLOCAL_DAT_Receive_Loop >= 0;
       vLOCAL_DAT_Receive_Loop --
     )
  {
    oFI2C_BIT_Set(cSCL);
#ifdef TEMPO
    delay(cDATA_TIME_TEMPO);
#endif // TEMPO

    vLOCAL_DAT_Receive_Data |= (
                                    oFI2C_BIT_Test(cSDA)
                                 << vLOCAL_DAT_Receive_Loop
                               );

    oFI2C_BIT_Clear(cSCL);
#ifdef TEMPO
    delay(cDATA_TIME_TEMPO);
#endif // TEMPO
  }

  oFI2C_BIT_Clear(cSDA);
#ifdef TEMPO
  delay(cDATA_TIME_TEMPO);
#endif // TEMPO

  return vLOCAL_DAT_Receive_Data;
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C_FRM_Send                                                      */
/* Role : Send a complete frame                                               */
/* Interface :  IN SendFlux's address (using pre-defined tables, see 'main')  */
/*             OUT oFI2C_FRM_Send                                             */
/*                 cFALSE - Error                                             */
/*                 cTRUE  - OK                                                */
/* Pre-condition : None                                                       */
/* Constraints : None                                                         */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Send a complete frame on the I2C bus]                                 */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C_FRM_Send                                                        */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Send a complete frame on the I2C bus] =                                 */
/*   DO                                                                       */
/*     [Set the I2C bus in START mode]                                        */
/*     [Print the selected LPT data port on which the data will be sent]      */
/*     [Print the I2C module address and the number of parameters to send]    */
/*     WHILE [Counter = 1 to SizeOfFrame+1] (skip number of params to send)   */
/*       DO                                                                   */
/*         [Clear the Error Counter]                                          */
/*           DO                                                               */
/*             [Send the byte number 'Counter' on the I2C bus]                */
/*             [Increase the Error Counter / Number of Try Counter]           */
/*           OD                                                               */
/*         WHILE [(NO Ack from Slave) AND (Error Counter < Number of try)]    */
/*         [Print the byte number sent, its value, and the number of try]     */
/*         IF [Error Counter = Number of Try] THEN                            */
/*           [Exit loop and finish the routine with a final test]             */
/*         ELSE                                                               */
/*           [Increase the number of bytes really sent]                       */
/*         FI                                                                 */
/*       OD                                                                   */
/*     [Set the I2C bus in STOP mode]                                         */
/*     IF [Number of bytes really sent = Number of byte to send] THEN         */
/*       [Return 'cTRUE']                                                     */
/*     ELSE                                                                   */
/*       [Return 'cFALSE']                                                    */
/*     FI                                                                     */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
tBIT oFI2C::oFI2C_FRM_Send(tUBYTE vPARAM_FRM_Frame_To_Send[])
{
  tUWORD vLOCAL_FRM_Sent_Counter = 0;
  tUWORD vLOCAL_FRM_Sent_Loop;
  tUWORD vLOCAL_FRM_Sent_Error;

  oFI2C_BUS_Start();
  
  printf(
          "Sending on LPT%d : ",
          (vFI2C_LPT_NPORT + 1)
        );

  // {'I2C module address'['Data tail length']}
  printf(
          "{$%x[%d]} ",
          (vPARAM_FRM_Frame_To_Send[1]),
          (vPARAM_FRM_Frame_To_Send[0])
        );

  // '1' because we shouda start sending the I2C module address first
  // '+1', the end of the I2C frame...
  //      IF 8 params, you have 10 bytes (+2 car TailLength + ModuleAddr), 
  //      and the array goes from 0 to 9, so end of params at 9 (8 + 1, TailLength + 1) !!!
  for(
       vLOCAL_FRM_Sent_Loop = 1;
       vLOCAL_FRM_Sent_Loop <= (vPARAM_FRM_Frame_To_Send[0] + 1);
       vLOCAL_FRM_Sent_Loop ++
     )
  {
    vLOCAL_FRM_Sent_Error = 0;

    do
    {
      oFI2C_DAT_Send(vPARAM_FRM_Frame_To_Send[vLOCAL_FRM_Sent_Loop]);
      vLOCAL_FRM_Sent_Error ++;
    }
    while( // IF error
              (!oFI2C_ACK_Sending())
           && (
                  vLOCAL_FRM_Sent_Error 
                < cDATA_LOOP_TRY
              )
         );

    // IF OK or TOO MUCH ERROR, exit loop
    // In CASE of TOO MUCH ERROR, the IF below will cheat the loop counter
    
    // ['ParamNumber'-$'HexByteValue'('ErrorNb')]
    // '-1' because ModuleAddr not took in account (9 - 1, 8 params, indexed on screen as byte 1 to 8)
    printf(
            "[%d-$%x(%d)]",
            (vLOCAL_FRM_Sent_Loop - 1),
            (vPARAM_FRM_Frame_To_Send[vLOCAL_FRM_Sent_Loop]),
            (vLOCAL_FRM_Sent_Error)
          );

    if(vLOCAL_FRM_Sent_Error == cDATA_LOOP_TRY)
    {
      vLOCAL_FRM_Sent_Loop = vPARAM_FRM_Frame_To_Send[0] + 1;          
      // Exit the loop by setting the loop counter at its maximum value
    }
    else
    {
      vLOCAL_FRM_Sent_Counter ++;                       // Number of byte sent
    }
  }

  oFI2C_BUS_Stop();
  printf("\r\n");

  // NbReallySent - 1 : '-1' because we skip the ModuleAddr...
  return(
             (vLOCAL_FRM_Sent_Counter - 1)
          == (vPARAM_FRM_Frame_To_Send[0])
        );
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C_FRM_Receive                                                   */
/* Role : Receive a complete frame                                            */
/* Interface :  IN ReceiveFlux's address (see 'main' for tables structure)    */
/*             OUT oFI2C_FRM_Send                                             */
/*                 cFALSE - Error                                             */
/*                 cTRUE  - OK                                                */
/* Interface : None                                                           */
/* Pre-condition : None                                                       */
/* Constraints : ALWAYS return OK to the Slave !                              */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Receive a complete frame on the I2C bus]                              */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C_FRM_Receive                                                     */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Receive a complete frame on the I2C bus] =                              */
/*   DO                                                                       */
/*     [Set the I2C bus in START mode]                                        */
/*     [Print the selcted LPT data port on which the data will be received]   */
/*     [Print the I2C module address and the number of parameters to receive] */
/*     [Send the module address with the read flag ON]                        */
/*     IF [Ack from Slave, the Slave is OK with the data request] THEN        */
/*       WHILE [Counter = 2 to SizeOfFrame+1] (skip nb params and mod addr)]  */
/*         DO                                                                 */
/*           [Receive the byte number 'Counter' on the I2C bus]               */
/*           [Send OK Ack for the Slave]                                      */
/*           [Print the byte number received and its value]                   */
/*           [Increase the number of bytes really received]                   */
/*         OD                                                                 */
/*     FI                                                                     */
/*     [Set the I2C bus in STOP mode]                                         */
/*     IF [Number of bytes really received = Number of byte to receive] THEN  */
/*       [Return 'cTRUE']                                                     */
/*     ELSE                                                                   */
/*       [Return 'cFALSE']                                                    */
/*     FI                                                                     */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
tBIT oFI2C::oFI2C_FRM_Receive(tUBYTE vPARAM_FRM_Frame_To_Receive[])
{
  tUWORD vLOCAL_FRM_Receive_Counter = 0;
  tUWORD vLOCAL_FRM_Receive_Loop;

  oFI2C_BUS_Start();

  printf(
          "Receiving on LPT%d : ",
          (vFI2C_LPT_NPORT + 1)
        );

  // {'I2C module address'['Data tail length']}
  printf(
          "{$%x[%d]} ",
          (vPARAM_FRM_Frame_To_Receive[1] | 1),
          (vPARAM_FRM_Frame_To_Receive[0])
        );

  oFI2C_DAT_Send(vPARAM_FRM_Frame_To_Receive[1] | 1); // '|1' set READ bit !!

  if(oFI2C_ACK_Sending())
  {
    // '2', because we start receiving at offset 2 (at '0' TailLength, at '1' ModuleAddr)
    // '+1' because end of frame when receiving
    //      IF 8 params, you have 10 bytes (+2 car TailLength + ModuleAddr), 
    //      and the array goes from 0 to 9, so end of params at 9 (8 + 1, TailLength + 1) !!!
    for(
         vLOCAL_FRM_Receive_Loop = 2;
         vLOCAL_FRM_Receive_Loop <= (vPARAM_FRM_Frame_To_Receive[0] + 1);
         vLOCAL_FRM_Receive_Loop ++
       )
    {
      vPARAM_FRM_Frame_To_Receive[vLOCAL_FRM_Receive_Loop] = oFI2C_DAT_Receive();
      oFI2C_ACK_Receiving();

      // ['ParamNumber'-$'HexByteValue'('ErrorNb')]
      // '-1' because ModuleAddr not took in account (9 - 1, 8 params, indexed on screen as byte 1 to 8)
      printf( 
              "[%d-$%x(1)]",
              (vLOCAL_FRM_Receive_Loop - 1),
              (vPARAM_FRM_Frame_To_Receive[vLOCAL_FRM_Receive_Loop])
            );

      vLOCAL_FRM_Receive_Counter ++;
    }
  }

  oFI2C_BUS_Stop();
  printf("\r\n");

  // Normally, ALWAYS cTRUE...
  return(
             vLOCAL_FRM_Receive_Counter 
          == (tUBYTE)vPARAM_FRM_Frame_To_Receive[0]
        );
}

/*--- START FUNCTION HEADER --------------------------------------------------*/
/* Name : oFI2C_DAT_Convert                                                   */
/* Role : Convert ASCII character to a displayable character on the disp mod  */
/* Interface :  IN Caract                                                     */
/*             OUT oFI2C_DAT_Convert                                          */
/* Pre-condition : None                                                       */
/* Constraints : None                                                         */
/* Behavior :                                                                 */
/*   DO                                                                       */
/*     [Convert the character]                                                */
/*   OD                                                                       */
/*----------------------------------------------------------------------------*/
/* PROC oFI2C_DAT_Convert                                                     */
/* (                                                                          */
/* )                                                                          */
/* DATA                                                                       */
/*   0x077 for A                                                              */
/*   0x07C for b                                                              */
/*   0x039 for c                                                              */
/*   0x05E for d                                                              */
/*   0x079 for E                                                              */
/*   0x071 for F                                                              */
/*   0x03D for G                                                              */
/*   0x076 for H                                                              */
/*   0x030 for I                                                              */
/*   0x01E for J                                                              */
/*   0x075 for k                                                              */
/*   0x038 for L                                                              */
/*   0x037 for M                                                              */
/*   0x054 for n                                                              */
/*   0x05C for o                                                              */
/*   0x073 for P                                                              */
/*   0x067 for q                                                              */
/*   0x050 for r                                                              */
/*   0x06D for S                                                              */
/*   0x078 for t                                                              */
/*   0x03E for U                                                              */
/*   0x01C for v                                                              */
/*   0x07E for W                                                              */
/*   0x076 for X                                                              */
/*   0x06E for y                                                              */
/*   0x05B for Z                                                              */
/*   0x03F for 0                                                              */
/*   0x006 for 1                                                              */
/*   0x05B for 2                                                              */
/*   0x04F for 3                                                              */
/*   0x066 for 4                                                              */
/*   0x06D for 5                                                              */
/*   0x07D for 6                                                              */
/*   0x007 for 7                                                              */
/*   0x07F for 8                                                              */
/*   0x06F for 9                                                              */
/* ATAD                                                                       */
/*                                                                            */
/* DO                                                                         */
/*   [Convert the character] =                                                */
/*   DO                                                                       */
/*     IF [Caract = [A-Z]] THEN                                               */
/*       [Select a charactere in [A-Z] in the display font]                   */
/*     ELSE IF [Caract = [a-z]] THEN                                          */
/*       [Select a charactere in [A-Z] in the display font]                   */
/*     ELSE IF [Caract = [0-9]] THEN                                          */
/*       [Select a charactere in [0-9] in the display font]                   */
/*     ELSE                                                                   */
/*       [Display a space charactere]                                         */
/*     FI                                                                     */
/*   OD                                                                       */
/* OD                                                                         */
/*----------------------------------------------------------------------------*/
/*--- END FUNCTION HEADER ----------------------------------------------------*/
tUBYTE oFI2C::oFI2C_DAT_Convert(tUBYTE vPARAM_DAT_ASCII_Char)
{
  tUBYTE oFI2C_DAT_Convert[]={
                               0x077,  // A
                               0x07C,  // b
                               0x039,  // c
                               0x05E,  // d
                               0x079,  // E
                               0x071,  // F
                               0x03D,  // G
                               0x076,  // H
                               0x030,  // I
                               0x01E,  // J
                               0x075,  // k
                               0x038,  // L
                               0x037,  // M
                               0x054,  // n
                               0x05C,  // o
                               0x073,  // P
                               0x067,  // q
                               0x050,  // r
                               0x06D,  // S
                               0x078,  // t
                               0x03E,  // U
                               0x01C,  // v
                               0x07E,  // W
                               0x076,  // X
                               0x06E,  // y
                               0x05B,  // Z
                               0x03F,  // 0
                               0x006,  // 1
                               0x05B,  // 2
                               0x04F,  // 3
                               0x066,  // 4
                               0x06D,  // 5
                               0x07D,  // 6
                               0x007,  // 7
                               0x07F,  // 8
                               0x06F   // 9
                             };

  if(
         (vPARAM_DAT_ASCII_Char >= 'A')
      && (vPARAM_DAT_ASCII_Char <= 'Z')
    )
  {
    return(oFI2C_DAT_Convert[vPARAM_DAT_ASCII_Char - 'A']);
  }
  else if(
              (vPARAM_DAT_ASCII_Char >= 'a')
           && (vPARAM_DAT_ASCII_Char <= 'z')
         )
  {
    return(oFI2C_DAT_Convert[vPARAM_DAT_ASCII_Char - 'a']);
  }
  else if(
              (vPARAM_DAT_ASCII_Char >= '0')
           && (vPARAM_DAT_ASCII_Char <= '9')
         )
  {
    return(oFI2C_DAT_Convert[vPARAM_DAT_ASCII_Char - '0' + 26]);
  }
  else
  {
    return(0); // Space
  }
}

/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\ MAIN CODE                                                                /*/
/*/                                                                          \*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/
int main(void)
{
// oFI2C FRAME STRUCTURE
// As the STANDARD I2C FRAME STRUCTURE, but with a 'PASCAL-like' header giving
// the length of the parameters tail of the I2C FRAME...

// Structure of a I2C frame
//   offset 0 - Length of the data tail after the I2C module address
//          1 - I2C module address (look into each I2C components documentation)
//          x - Data tail, module's parameters (look into each I2C components documentation)

#ifdef DISPLAY
  tUBYTE vLOCAL_MAIN_Display[] = {6, cADDR_I2CM_DISP, 0, 0x077, 1, 2, 4, 8};
#endif // DISPLAY

#ifdef CLOCK
  tUBYTE vLOCAL_MAIN_WClock[] = {2, cADDR_I2CM_CLOCK, 0, 0};
  tUBYTE vLOCAL_MAIN_RClock[] = {8, cADDR_I2CM_CLOCK, 0, 0, 0, 0, 0, 0, 0, 0};
#endif // CLOCK

  tUBYTE vLOCAL_MAIN_Keyb;                // Keyboard buffer

  oFI2C Flux1(1);                       // Creating object Flux1 on LPT1

  // ---------------------------------------------------------------------

  clrscr();

  if(Flux1.oFI2C_BUS_Test() == cTRUE)   // If I2C bus free
  {

#ifdef CLOCK
    // RTC Clock part    
    if(Flux1.oFI2C_FRM_Send(vLOCAL_MAIN_WClock) == cTRUE)
    {
      do
      {
        if(Flux1.oFI2C_FRM_Receive(vLOCAL_MAIN_RClock) = cTRUE)
        {
          // Some people could fine this way of typing a bit strange but don't worry,
          // in a while you'll not be able to read in another way anymore ;)
          // Thanks Marga.FP for the idea, it really makes things easier to read !
          vLOCAL_MAIN_Display[4]=Flux1.oFI2C_DAT_Convert(
                                                            (
                                                                (
                                                                     vLOCAL_MAIN_RClock[4]
                                                                  >> 4
                                                                )
                                                              & 3
                                                            ) 
                                                          + '0'
                                                        );
          vLOCAL_MAIN_Display[5]=Flux1.oFI2C_DAT_Convert( 
                                                            (
                                                                vLOCAL_MAIN_RClock[4] 
                                                              & 0x00F
                                                            )
                                                          + '0'
                                                        );
          vLOCAL_MAIN_Display[6]=Flux1.oFI2C_DAT_Convert(
                                                            (
                                                                (
                                                                     vLOCAL_MAIN_RClock[2]
                                                                  >> 4
                                                                ) 
                                                              & 0x00F
                                                            )
                                                          + '0'
                                                        );
          vLOCAL_MAIN_Display[7]=Flux1.oFI2C_DAT_Convert(
                                                            (
                                                                vLOCAL_MAIN_RClock[2]
                                                              & 0x00F
                                                            )
                                                          + '0'
                                                        );

          if(Flux1.oFI2C_FRM_Send(cADDR_I2CM_DISP) == cTRUE)
          {
            cout << "CLCK SEND OK" << endl;
          }
          else
          {
            cout << "CLCK SEND KO" << endl;
          }
        }
        else
        {
          cout << "CLCK RECEIVE KO" << endl;
        }
      }
      while(!kbhit());
    }
    else
    {
      cout << "CLCK RESET KO" << endl;
    }
#endif // CLOCK

#ifdef DISPLAY
    // 7 SEG Display part
    do
    {
      vLOCAL_MAIN_Keyb = getch();
      vLOCAL_MAIN_Display[4] = vLOCAL_MAIN_Display[5];
      vLOCAL_MAIN_Display[5] = vLOCAL_MAIN_Display[6];
      vLOCAL_MAIN_Display[6] = vLOCAL_MAIN_Display[7];
      vLOCAL_MAIN_Display[7] = Flux1.oFI2C_DAT_Convert(vLOCAL_MAIN_Keyb);

      if(Flux1.oFI2C_FRM_Send(vLOCAL_MAIN_Display) == cTRUE)
      {
        cout << "DISP SEND OK" << endl;
      }
      else
      {
        cout << "DISP SEND KO" << endl;
      }
    }
    while(vLOCAL_MAIN_Keyb != 27);      // [Esc]
#endif // DISPLAY

  }
  else
  {
    cout << "I2C bus already in use..." << endl;
  }

#ifdef RECEIVE
  if(Flux1.oFI2C_BUS_Test() == cTRUE)
  {
    if(Flux1.oFI2C_FRM_Receive(Flux) == cTRUE)
    {
      cout << "RECEIVE OK" << endl;
    }
    else
    {
      cout << "RECEIVE KO" << endl;
    }
  }
  else
  {
    cout << "I2C bus already in use..." << endl;
  }
#endif // RECEIVE
                                 
  getch();                    // Exit on 2nd keypress (after [ESC])

  return 0;
}

