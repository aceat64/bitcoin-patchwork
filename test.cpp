// Copyright (c) 2009-2010 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.
#include <assert.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <db_cxx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <assert.h>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <sys/resource.h>
#include <sys/time.h>
using namespace std;
using namespace boost;
#include "cryptopp/sha.h"
#include "strlcpy.h"
#include "serialize.h"
#include "uint256.h"
#include "bignum.h"

#undef printf
	template <size_t nBytes, typename T>
T* alignup(T* p)
{
	union
	{   
		T* ptr;
		size_t n;
	} u;
	u.ptr = p;
	u.n = (u.n + (nBytes-1)) & ~(nBytes-1);
	return u.ptr;
}

int FormatHashBlocks(void* pbuffer, unsigned int len) 
{
	unsigned char* pdata = (unsigned char*)pbuffer;
	unsigned int blocks = 1 + ((len + 8) / 64); 
	unsigned char* pend = pdata + 64 * blocks;
	memset(pdata + len, 0, 64 * blocks - len);
	pdata[len] = 0x80;
	unsigned int bits = len * 8; 
	pend[-1] = (bits >> 0) & 0xff;
	pend[-2] = (bits >> 8) & 0xff;
	pend[-3] = (bits >> 16) & 0xff;
	pend[-4] = (bits >> 24) & 0xff;
	return blocks;
}

using CryptoPP::ByteReverse;
static int detectlittleendian = 1;

#define NPAR 32 

extern void Double_BlockSHA256(const void* pin, void* pout, const void *pinit, unsigned int hash[8][NPAR], const void *init2);

using CryptoPP::ByteReverse;

static const unsigned int pSHA256InitState[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

inline void SHA256Transform(void* pstate, void* pinput, const void* pinit)
{
	memcpy(pstate, pinit, 32); 
	CryptoPP::SHA256::Transform((CryptoPP::word32*)pstate, (CryptoPP::word32*)pinput);
}

void BitcoinTester(char *filename)
{
	printf("SHA256 test started\n");

	struct tmpworkspace
	{
		struct unnamed2
		{
			int nVersion;
			uint256 hashPrevBlock;
			uint256 hashMerkleRoot;
			unsigned int nTime;
			unsigned int nBits;
			unsigned int nNonce;
		}
		block;
		unsigned char pchPadding0[64];
		uint256 hash1;
		unsigned char pchPadding1[64];
	};
	char tmpbuf[sizeof(tmpworkspace)+16];
	tmpworkspace& tmp = *(tmpworkspace*)alignup<16>(tmpbuf);


	char line[180];
	ifstream fin(filename);
	char *p;
	unsigned long int totalhashes= 0;
	unsigned long int found = 0;
	clock_t start, end;
	unsigned long int cpu_time_used;
	unsigned int tnonce;
	start = clock();

	while( fin.getline(line, 180)) 
	{
		string in(line);
		//printf("%s\n", in.c_str());
		tmp.block.nVersion       = strtol(in.substr(0,8).c_str(), &p, 16);
		tmp.block.hashPrevBlock.SetHex(in.substr(8,64));
		tmp.block.hashMerkleRoot.SetHex(in.substr(64+8,64));
		tmp.block.nTime          = strtol(in.substr(128+8,8).c_str(), &p, 16);
		tmp.block.nBits          = strtol(in.substr(128+16,8).c_str(), &p, 16);
		tnonce = strtol(in.substr(128+24,8).c_str(), &p, 16);
		tmp.block.nNonce         = tnonce;

		unsigned int nBlocks0 = FormatHashBlocks(&tmp.block, sizeof(tmp.block));
		unsigned int nBlocks1 = FormatHashBlocks(&tmp.hash1, sizeof(tmp.hash1));

		// Byte swap all the input buffer
		for (int i = 0; i < sizeof(tmp)/4; i++) 
			((unsigned int*)&tmp)[i] = ByteReverse(((unsigned int*)&tmp)[i]);

		// Precalc the first half of the first hash, which stays constant
		uint256 midstatebuf[2];
		uint256& midstate = *alignup<16>(midstatebuf);
		SHA256Transform(&midstate, &tmp.block, pSHA256InitState);


		uint256 hashTarget = CBigNum().SetCompact(ByteReverse(tmp.block.nBits)).getuint256();
		//	printf("target %s\n", hashTarget.GetHex().c_str());
		uint256 hash;
		uint256 hashbuf[2];
		uint256& refhash = *alignup<16>(hashbuf);

		unsigned int thash[8][NPAR];
		int done = 0;
		unsigned int i, j;

		/* reference */
		SHA256Transform(&tmp.hash1, (char*)&tmp.block + 64, &midstate);
		SHA256Transform(&refhash, &tmp.hash1, pSHA256InitState);
		for (int i = 0; i < sizeof(refhash)/4; i++)
			((unsigned int*)&refhash)[i] = ByteReverse(((unsigned int*)&refhash)[i]);

		//printf("reference nonce %08x:\n%s\n\n", tnonce, refhash.GetHex().c_str());

		tmp.block.nNonce = ByteReverse(tnonce) & 0xfffff000;


		for(;;)
		{

			Double_BlockSHA256((char*)&tmp.block + 64, &tmp.hash1, &midstate, thash, pSHA256InitState);

			for(i = 0; i<NPAR; i++) {
				/* fast hash checking */
				if(thash[7][i] == 0) {
			//		printf("found something... ");

					for(j = 0; j<8; j++) ((unsigned int *)&hash)[j] = ByteReverse((unsigned int)thash[j][i]);
				//	printf("%s\n", hash.GetHex().c_str());

					if (hash <= hashTarget)
					{
						found++;
						if(tnonce == ByteReverse(tmp.block.nNonce + i) ) {
							if(hash == refhash) {
								printf("\r%lu", found);
								totalhashes += NPAR;
								done = 1;
							} else {
								printf("Hashes do not match!\n");
							}
						} else {
							printf("nonce does not match. %08x != %08x\n", tnonce, ByteReverse(tmp.block.nNonce + i));
						}
						break;
					}
				}
			}
			if(done) break;

			tmp.block.nNonce+=NPAR;
			totalhashes += NPAR;
			if(tmp.block.nNonce == 0) {
				printf("ERROR: Hash not found for:\n%s\n", in.c_str());
				return;
			}
		}
	}
	printf("\n");
	end = clock();
	cpu_time_used += (unsigned int)(end - start);
	cpu_time_used /= ((CLOCKS_PER_SEC)/1000);
	printf("found solutions = %lu\n", found);
	printf("total hashes = %lu\n", totalhashes);
	printf("total time = %lu ms\n", cpu_time_used);
	printf("average speed: %lu khash/s\n", (totalhashes)/cpu_time_used);
}

int main(int argc, char* argv[]) {
	if(argc == 2) {
		BitcoinTester(argv[1]);
	} else 
		printf("Missing filename!\n");
	return 0;
}
