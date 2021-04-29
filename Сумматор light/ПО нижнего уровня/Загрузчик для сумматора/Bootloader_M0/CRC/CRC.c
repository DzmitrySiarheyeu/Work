static const unsigned short crcPoly = 0x8005;

unsigned short CRC(unsigned char ch, unsigned short oldCRC)
{
	int n;
	unsigned long m;

	m = ((unsigned long)oldCRC << 8) | ch;
	for (n = 0; n < 8; n++)
		if ((m <<= 1) & 0x1000000)
			m ^= ((unsigned int)crcPoly << 8);

	return (unsigned short)(m >> 8);
}
