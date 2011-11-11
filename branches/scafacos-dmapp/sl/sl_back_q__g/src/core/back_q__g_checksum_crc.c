/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/core/checksum_crc.c
 *  timestamp: 2010-10-13 08:51:13 +0200
 *  
 */


#include "sl_common.h"


/* sl_func icrc1 icrc reflect crcInit crcFast */

/* Given a remainder up to now, return the new CRC after one character is added. This routine
is functionally equivalent to icrc(,,1,-1,1), but slower. It is used by icrc to initialize its
table. */
unsigned short icrc1(unsigned short crc, unsigned char onech)
{
  int i;
  unsigned short ans = (crc ^ onech << 8);

  for (i=0; i<8; i++)
  {
    /* Here is where 8 one-bit shifts, and some XORs with the generator polynomial, are done. */
/*    if (ans & 0x8000) ans = (ans <<= 1) ^ 4129;*/
    if (ans & 0x8000) ans = (ans << 1) ^ 4129;
    else ans <<= 1;
  }

  return ans;
}

#define LOBYTE(x) ((unsigned char)((x) & 0xFF))
#define HIBYTE(x) ((unsigned char)((x) >> 8))

/* Computes a 16-bit Cyclic Redundancy Check for a byte array bufptr[1..len], using any
of several conventions as determined by the settings of jinit and jrev (see accompanying
table). If jinit is negative, then crc is used on input to initialize the remainder register, in
effect (for crc set to the last returned value) concatenating bufptr to the previous call. */
unsigned short icrc(unsigned short crc, unsigned char *bufptr, unsigned long len, short jinit, int jrev)
{
  unsigned short icrc1(unsigned short crc, unsigned char onech);
  static unsigned short icrctb[256],init=0;
  static unsigned char rchr[256];
  unsigned short j, cword = crc;
  /* Table of 4-bit bit-reverses. */
  static unsigned char it[16]={0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15};

  if (!init) /* Do we need to initialize tables? */
  {
    init=1;
    for (j = 0; j <= 255; j++)
    {
      /* The two tables are: CRCs of all characters, and bit-reverses of all characters. */
      icrctb[j] = icrc1(j << 8, (unsigned char) 0);
      rchr[j] = (unsigned char) (it[j & 0xF] << 4 | it[j >> 4]);
    }
  }

  if (jinit >= 0) cword=((unsigned char) jinit) | (((unsigned char) jinit) << 8); /* Initialize the remainder register. */
  else if (jrev < 0) cword=rchr[HIBYTE(cword)] | rchr[LOBYTE(cword)] << 8; /* If not initializing, do we reverse the register? */

  /* Main loop over the characters in the array. */
  for (j=0;j<len;j++) cword=icrctb[(jrev < 0 ? rchr[bufptr[j]] : bufptr[j]) ^ HIBYTE(cword)] ^ LOBYTE(cword) << 8;

  /* Do we need to reverse the output? */
  return (jrev >= 0 ? cword : rchr[HIBYTE(cword)] | rchr[LOBYTE(cword)] << 8);
}


/* generate an "order-invariant" checksum over given elements_t
   using a 16-bit crc taken from "numerical recipes in c" */
unsigned short elements_crc16(elements_t *s, slint n, slint keys, slint data) /* sl_proto, sl_func elements_crc16 */
{
  unsigned short crc = 0;
  slint i, j;

  for (j = 0; j < n; j++)
  for (i = 0; i < s[j].size; i++)
  {
    if (keys) crc += icrc(0, (unsigned char *) &s[j].keys[i], key_byte, 0, 0);

    if (data)
    {
#define xelem_key_not
#define xelem_call \
      crc += icrc(0, (unsigned char *) xelem_buf_at(&s[j], i), xelem_byte, 0, 1);
#include "sl_xelem_call.h"
    }
  }

  return crc;
}


#define CRC_NAME          "CRC-32"
#define POLYNOMIAL        0x04C11DB7UL
#define INITIAL_REMAINDER 0xFFFFFFFFUL
#define FINAL_XOR_VALUE   0xFFFFFFFFUL
#define REFLECT_DATA      TRUE
#define REFLECT_REMAINDER TRUE
#define CHECK_VALUE       0xCBF43926UL

#define WIDTH             (8 * sizeof(unsigned long))
#define TOPBIT            (1UL << (WIDTH - 1))

#if (REFLECT_DATA == TRUE)
#undef  REFLECT_DATA
#define REFLECT_DATA(X)   ((unsigned char) reflect((X), 8))
#else
#undef  REFLECT_DATA
#define REFLECT_DATA(X)   (X)
#endif

#if (REFLECT_REMAINDER == TRUE)
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)  ((unsigned long) reflect((X), WIDTH))
#else
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)  (X)
#endif

static unsigned long reflect(unsigned long data, unsigned char nBits)
{
  unsigned long  reflection = 0x00000000;
  unsigned char  bit;

  /*
   * Reflect the data about the center bit.
   */
  for (bit = 0; bit < nBits; ++bit)
  {
    /*
     * If the LSB bit is set, set the reflection of it.
     */
    if (data & 0x01)
    {
      reflection |= (1 << ((nBits - 1) - bit));
    }

    data = (data >> 1);
  }

  return (reflection);

} /* reflect() */



/*********************************************************************
 *
 * Function:    crcInit()
 *
 * Description: Populate the partial CRC lookup table.
 *
 * Notes:   This function must be rerun any time the CRC standard
 *        is changed.  If desired, it can be run "offline" and
 *        the table results stored in an embedded system's ROM.
 *
 * Returns:   None defined.
 *
 *********************************************************************/
void crcInit(unsigned long *crcTable)
{
  unsigned long remainder;
  unsigned long dividend;
  unsigned char bit;

    /*
     * Compute the remainder of each possible dividend.
     */
    for (dividend = 0; dividend < 256; ++dividend)
    {
        /*
         * Start with the dividend followed by zeros.
         */
        remainder = dividend << (WIDTH - 8);

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */

            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }

        /*
         * Store the result into the table.
         */
        crcTable[dividend] = remainder;
    }

}   /* crcInit() */


/*********************************************************************
 *
 * Function:    crcFast()
 *
 * Description: Compute the CRC of a given message.
 *
 * Notes:   crcInit() must be called first.
 *
 * Returns:   The CRC of the message.
 *
 *********************************************************************/
unsigned long crcFast(unsigned char const message[], int nBytes, unsigned long *crcTable)
{
  unsigned long remainder = INITIAL_REMAINDER;
  unsigned char data;
  int           byte;


    /*
     * Divide the message by the polynomial, a byte at a time.
     */
    for (byte = 0; byte < nBytes; ++byte)
    {
        data = REFLECT_DATA(message[byte]) ^ (remainder >> (WIDTH - 8));
      remainder = crcTable[data] ^ (remainder << 8);
    }

    /*
     * The final remainder is the CRC.
     */
    return (REFLECT_REMAINDER(remainder) ^ FINAL_XOR_VALUE);

}   /* crcFast() */


unsigned int elements_crc32(elements_t *s, slint n, slint keys, slint data) /* sl_proto, sl_func elements_crc32 */
{
  unsigned long crcTable[256];
  unsigned long crc = 0, crcp;
  slint i, j;

  crcInit(crcTable);

  for (j = 0; j < n; j++)
  {
    crcp = 0;
    for (i = 0; i < s[j].size; i++)
    {
      if (keys) crcp += crcFast((unsigned char *) &s[j].keys[i], key_byte, crcTable);

      if (data)
      {
#define xelem_key_not
#define xelem_call \
        crcp += crcFast((unsigned char *) xelem_buf_at(&s[j], i), xelem_byte, crcTable);
#include "sl_xelem_call.h"
      }
    }
    crc += crcp;
  }

  return crc;
}
