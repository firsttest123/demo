#ifndef _MODBUSDEFS_H
#define	_MODBUSDEFS_H
/*** BeginHeader */

/************************************ Copyright Notice **********************************************
 *																								 
 * This is a source file for the MODBUS Communication Master/Slave Driver Stack developed by Sunlux 
 * Technologies Ltd., Bangalore. Sunlux retains ownership of the products and as such exposes the 
 * code to registered customers who have purchased this Source Code Library from Sunlux. The 
 * customer is bound to protect the confidentiality of the source code and also bind all its 
 * employees and other related entities to protect the confidentiality of the source code. The 
 * source code cannot be sold for cost or free to any third party by the end customers. By the 
 * sale of this source code library Sunlux provides the customer license to port this SCL to the 
 * native platform of the customer and use it on one product only. The code is licensed 
 * royalty-free on multiple units of the same product. Use of the library on further products 
 * can be made only if further licenses are purchased by the customer from Sunlux. Infringement 
 * and/or violation of this Copyright in any form is an offence and is punishable under the 
 * Indian Copyright Laws. Jurisdiction: Bangalore, India
 * Sunlux Technologies Ltd. can be contacted at the following address:
 *	Sunlux Technologies Ltd., 497, 6th 'A' Main, H I G Colony, R M V II Stage
 *	Bangalore - 560 094, India
 *	Ph: ++91 80 23417073	Email: info@sunlux-india.com		Web: www.sunlux-india.com
 *
 ****************************************************************************************************/

/****************************************************************************
 * MBDefs.h:	This file contains the type definitions of some basic data 
 *				types and definitions of some symbolic literals.
 *
 * Edition History
 *
 * #  date       Comments                                       by
 * ------------------------------------------------------------------
 * 01 04/04/2000 Written										SRIKANTH
 * 02 02/10/2002 Updated for portability						RMD
 ****************************************************************************/


  /***************************************************************************/
  /*                                                                         */
  /*                            TYPE DEFINITIONS                             */
  /*                                                                         */
  /***************************************************************************/

  #define MBDEFS_NULL (void *)0   	 			 /* Generic NULL Pointer */
  #define MBDEFS_TRUE		1
  #define MBDEFS_FALSE		0
  #define MBDEFS_INVALID	2
  #define MBDEFS_GOOD		1
  #define MBDEFS_BAD		0

  typedef void * MBDEFS_VOIDPTR  ;
  #ifdef _RCM3200
  #define MBDEFS_NONE	void		/* Void return */
  #else
  typedef void			 MBDEFS_NONE; 	 /* Void return */
  #endif
  typedef unsigned char	 MBDEFS_BOOL;    /* Holds True or false */
  typedef          char  MBDEFS_CHAR;    /*           -128 to           127 */
  typedef          char  MBDEFS_INT8;    /*           -128 to           127 */
  typedef unsigned char  MBDEFS_UCHAR;   /*              0 to           255 */
  typedef unsigned char  MBDEFS_UINT8;   /*              0 to           255 */
  typedef          short MBDEFS_INT16;   /*        -32,768 to        32,767 */
  typedef unsigned short MBDEFS_USHORT;  /*              0 to        65,535 */
  typedef unsigned short MBDEFS_UINT16;  /*              0 to        65,535 */
  typedef          long  MBDEFS_LONG;    /* -2,147,483,648 to 2,147,483,647 */
  typedef unsigned long  MBDEFS_ULONG;   /*              0 to 4,294,967,295 */
  typedef          float MBDEFS_FLOAT;   /*  -3.4 * 10(38) to +3.4 * 10(38) */
                                         /*     32-bit short floating point */
                                         /*     number -- IEEE Standard 754 */
                                         /*       fraction = UI23[1..23]    */
                                         /*       exponent = UI8 [24..31]   */
                                         /*       sign     = BS1[32]        */
  typedef         double MBDEFS_DOUBLE;  /*  -3.4 * 10(38) to +3.4 * 10(38) */
                                         /*     32-bit short floating point */
                                         /*     number -- IEEE Standard 754 */
                                         /*       fraction = UI23[1..23]    */
                                         /*       exponent = UI8 [24..31]   */
                                         /*       sign     = BS1[32]        */


#endif /*(_MODBUSDEFS_H)*/

/*** EndHeader */
