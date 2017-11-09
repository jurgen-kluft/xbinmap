#include "xbase/x_allocator.h"
#include "xbase/x_debug.h"
#include "xbase/x_memory_std.h"
#include "xbase/x_integer.h"
#include "xbinmaps/merkle.h"
#include "xbinmaps/blake_256.h"
#include "xunittest/xunittest.h"

using namespace xcore;


extern xcore::x_iallocator* gTestAllocator;

class my_sigmap_allocator
{
	u32		sizeof_sig;

public:
	my_sigmap_allocator()
	{
		sizeof_sig  = sizeof(hash256);
	}

	virtual void	allocate(merkle::hash_t& s)
	{
		s.length_ = sizeof_sig;
		s.digest_ = (xbyte*)gTestAllocator->allocate(s.length_, 4);
	}

	virtual void	deallocate(merkle::hash_t& s)
	{
		gTestAllocator->deallocate(s.digest_);
		s.digest_ = 0;
	}
};

static void	combine(merkle::hash_t const& lhs, merkle::hash_t const& rhs, merkle::hash_t& result)
{
	ASSERT(lhs.length_ == rhs.length_);
	ASSERT(lhs.length_ == result.length_);
	blake256_state state;
	blake256_init(&state);
	blake256_update(&state, lhs.digest_, lhs.length_);
	blake256_update(&state, rhs.digest_, rhs.length_);
	hash256 hash;
	blake256_final(&state, hash);
	x_memcopy(result.digest_, hash.hash, result.length_);
};

static xbyte digests[] =
{
	0x42, 0x54, 0x87, 0xE4, 0x1F, 0x77, 0x19, 0x8B, 0xB7, 0xD3, 0x8C, 0xF7, 0xDC, 0x81, 0x35, 0xF5, 0xF6, 0x8E, 0x91, 0x28, 0x63, 0xAD, 0xEA, 0x7A, 0xFE, 0xBB, 0x0F, 0xE0, 0xD8, 0x4F, 0x7D, 0x63,
	0x0E, 0x43, 0x43, 0x5E, 0xE5, 0xF6, 0xDF, 0xCD, 0x17, 0xDF, 0x16, 0xF2, 0x88, 0x2C, 0x25, 0xF8, 0x45, 0xA6, 0xC8, 0xBF, 0xC1, 0x86, 0x87, 0x95, 0x6A, 0x0D, 0xE6, 0x63, 0x67, 0x38, 0xB2, 0x68,
	0x55, 0x28, 0xE8, 0x98, 0x3D, 0xD6, 0x10, 0xFF, 0xAF, 0xC3, 0x11, 0x83, 0x97, 0xF7, 0xF1, 0xD1, 0x96, 0x17, 0x9A, 0x32, 0x86, 0xBE, 0xA2, 0x88, 0xBB, 0xAE, 0xF3, 0xD8, 0x0F, 0x7B, 0x66, 0xDE,
	0x2C, 0xAD, 0x9C, 0x60, 0x44, 0x8C, 0xBA, 0x6E, 0x96, 0xC8, 0x0F, 0xFD, 0x75, 0x04, 0xFD, 0x72, 0x1B, 0x15, 0xCD, 0x4F, 0x9E, 0x80, 0xE2, 0x97, 0x86, 0x60, 0xDD, 0x8E, 0xE2, 0x38, 0xEF, 0x33,
	0x5D, 0x32, 0x56, 0xEA, 0x5A, 0x91, 0x8C, 0x0A, 0x5C, 0x2B, 0x9C, 0xBD, 0xAE, 0xA5, 0x12, 0xB7, 0xB1, 0x81, 0xBC, 0xFA, 0xAE, 0x16, 0x3F, 0xAC, 0xCB, 0x04, 0xD9, 0xE3, 0xAE, 0x2C, 0x8B, 0x5D,
	0xD9, 0xDC, 0x9F, 0x42, 0xAD, 0x13, 0x17, 0x8A, 0x88, 0xC2, 0xF3, 0x72, 0xE4, 0x24, 0xE9, 0xE1, 0x4D, 0x25, 0xDC, 0x61, 0x7C, 0xF5, 0x85, 0x47, 0xF2, 0xAA, 0x1F, 0x55, 0xB7, 0xA9, 0x5B, 0x30,
	0x9F, 0x3E, 0xE4, 0xD1, 0x05, 0xD0, 0x0A, 0x9E, 0x69, 0x32, 0x71, 0xEC, 0x23, 0xCD, 0xC7, 0xA6, 0x1B, 0xF8, 0x95, 0x88, 0x4F, 0x2A, 0xE1, 0xE4, 0x57, 0x58, 0x4F, 0x02, 0xF3, 0xC1, 0xD3, 0xDF,
	0x66, 0xC2, 0xD2, 0xA5, 0x40, 0x17, 0xDB, 0x7D, 0x1B, 0x3E, 0xF7, 0x49, 0x83, 0x99, 0x24, 0x25, 0xE2, 0x78, 0x3A, 0xBE, 0x13, 0xD2, 0xE0, 0x75, 0x5D, 0x00, 0x7A, 0xAD, 0x80, 0x07, 0xAA, 0xA8,
	0x5F, 0x91, 0x85, 0x60, 0xFA, 0x22, 0x66, 0x03, 0x1C, 0x17, 0x6D, 0x11, 0x32, 0x97, 0x60, 0xBD, 0x34, 0x2A, 0x20, 0x7B, 0x87, 0x14, 0xCC, 0x93, 0xE2, 0x5D, 0x48, 0x3F, 0x9D, 0x2E, 0xFD, 0x14,
	0x93, 0x4F, 0xEA, 0xF1, 0x13, 0xC3, 0x2A, 0xC0, 0x11, 0x36, 0xA1, 0xFB, 0x65, 0xCF, 0x42, 0xC5, 0x9F, 0xBC, 0x79, 0x56, 0xE7, 0x2F, 0x20, 0x4A, 0xC5, 0xF1, 0x7D, 0x9D, 0xD9, 0x26, 0x66, 0xEE,
	0x1A, 0x59, 0x7D, 0xE3, 0xAC, 0x6E, 0xAA, 0x70, 0xFF, 0x5F, 0x69, 0x50, 0x72, 0xD0, 0x2E, 0x12, 0x1A, 0x83, 0xA0, 0xE0, 0xB5, 0xA7, 0x2C, 0x63, 0x2E, 0xA0, 0x2C, 0x22, 0x45, 0x8D, 0xE6, 0xEB,
	0xD0, 0xB3, 0xE0, 0x0C, 0x51, 0x5F, 0x18, 0x30, 0x12, 0x08, 0x56, 0x87, 0xC6, 0xD0, 0xD7, 0xF4, 0xC3, 0x6B, 0x82, 0x56, 0x86, 0x86, 0x38, 0x73, 0x2C, 0x19, 0xB4, 0x0C, 0x42, 0x87, 0x2A, 0x72,
	0x78, 0x7E, 0x7F, 0xBF, 0xEF, 0x21, 0xC1, 0xBB, 0x6A, 0x6F, 0xCD, 0xAC, 0x50, 0xA5, 0x9D, 0x5D, 0xC8, 0x37, 0x5F, 0xDA, 0x0A, 0x90, 0x64, 0x4A, 0x7A, 0x6D, 0xE0, 0x37, 0xAB, 0x0C, 0xEE, 0x8A,
	0xDE, 0xE4, 0x0E, 0x47, 0x51, 0x18, 0x05, 0xA1, 0xA9, 0xC7, 0xEC, 0x17, 0xB8, 0x8C, 0x23, 0x87, 0x7C, 0x56, 0x44, 0x0B, 0x28, 0x4F, 0x67, 0xE8, 0x19, 0x35, 0x45, 0x82, 0x41, 0x89, 0x0F, 0x5D,
	0x81, 0x42, 0xCE, 0x6B, 0x96, 0x56, 0x0C, 0xAF, 0x32, 0xBE, 0x31, 0x42, 0xA3, 0x4D, 0x91, 0xEE, 0x4A, 0x59, 0xBB, 0x54, 0xC8, 0xE2, 0x62, 0x8F, 0x7D, 0x26, 0x2D, 0x1B, 0xD3, 0x82, 0x5F, 0x20,
	0xB7, 0x8E, 0x96, 0x76, 0xBE, 0xB8, 0xF0, 0x26, 0x49, 0x94, 0xD9, 0x6A, 0x1E, 0x25, 0x44, 0xCC, 0xA4, 0x56, 0x38, 0xE8, 0xCE, 0xBA, 0x95, 0x9E, 0xE0, 0x3E, 0xEE, 0x70, 0x28, 0xF1, 0xDC, 0xAF,
	0x5E, 0x11, 0x8A, 0x15, 0xB7, 0x1B, 0x30, 0x92, 0x68, 0x95, 0x0B, 0x33, 0x08, 0xF7, 0x71, 0x58, 0xE1, 0x61, 0x77, 0x7C, 0x4A, 0x76, 0x51, 0xFC, 0xF8, 0xF7, 0x95, 0x7F, 0xC9, 0x7F, 0xFD, 0x58,
	0xB8, 0x2A, 0x0B, 0x0D, 0x9A, 0x2F, 0x15, 0x88, 0x26, 0xA5, 0x80, 0x38, 0xAF, 0x62, 0x95, 0xE8, 0xCD, 0x96, 0x8F, 0xC6, 0x79, 0x5F, 0x64, 0xE8, 0xBF, 0x8D, 0x1C, 0xBF, 0x85, 0xB0, 0xC8, 0xE8,
	0x97, 0xB3, 0x7A, 0xDE, 0xEC, 0x64, 0xBD, 0xB2, 0x4B, 0xEE, 0x55, 0xA4, 0xF6, 0x19, 0x4D, 0x96, 0x61, 0xAD, 0xB1, 0x33, 0xF0, 0x37, 0xDE, 0x58, 0xC6, 0x23, 0x4D, 0x0C, 0x12, 0x55, 0x03, 0x76,
	0xDD, 0x97, 0xA1, 0xFC, 0xE7, 0xB7, 0xFC, 0x69, 0xD8, 0xDC, 0x7A, 0x8B, 0x09, 0xBF, 0xF1, 0xA6, 0x40, 0x85, 0x5E, 0xD7, 0xFF, 0x94, 0xF7, 0x60, 0xD4, 0x21, 0x18, 0xC8, 0x16, 0xB1, 0x9F, 0xF6,
	0xEB, 0xDF, 0xC7, 0x5F, 0x78, 0x50, 0xAF, 0xAA, 0xB1, 0x47, 0x5A, 0x14, 0xBC, 0x52, 0x4F, 0x06, 0x31, 0xA7, 0xCB, 0x8D, 0xF7, 0x2B, 0x77, 0x48, 0x9F, 0x40, 0x27, 0xC5, 0x9E, 0x33, 0x53, 0x27,
	0x3D, 0x84, 0xA0, 0x2A, 0xF8, 0x92, 0x2D, 0xEF, 0xD6, 0xB3, 0x26, 0xB7, 0x39, 0xE3, 0xA4, 0x82, 0xD2, 0x11, 0x96, 0x6E, 0xE5, 0x12, 0xFE, 0xBF, 0x68, 0x7E, 0xBF, 0x37, 0x29, 0xEA, 0x0C, 0x7E,
	0xF4, 0x33, 0xD6, 0x37, 0xB2, 0x25, 0x0E, 0x9F, 0x79, 0xDB, 0xB7, 0x44, 0x05, 0xBF, 0x0E, 0xBB, 0x9C, 0xA5, 0xF2, 0x8C, 0x9C, 0xE4, 0x41, 0x2B, 0x85, 0xE1, 0xC1, 0x79, 0xD3, 0xF6, 0xC6, 0xEB,
	0xA7, 0xA7, 0xC2, 0x06, 0x0D, 0x6C, 0xCF, 0xAD, 0x03, 0xF7, 0xF5, 0x0A, 0x5A, 0xBA, 0xB9, 0xA8, 0x70, 0x04, 0x9B, 0x84, 0xF7, 0x7F, 0xE1, 0x0F, 0xF2, 0xAB, 0xB2, 0xD1, 0x63, 0x95, 0xCC, 0x83,
	0x15, 0xCB, 0x9C, 0x5D, 0xFD, 0x08, 0xAE, 0x2F, 0xD5, 0x44, 0x89, 0xA5, 0xEF, 0x37, 0xD0, 0xCA, 0x27, 0x19, 0x08, 0xCF, 0x5A, 0x42, 0xF4, 0x28, 0xC6, 0xB2, 0x13, 0x62, 0xB9, 0xFE, 0x4A, 0x6E,
	0x2B, 0x71, 0xF4, 0x8F, 0x1E, 0x5B, 0xAB, 0x67, 0x55, 0x6B, 0xAE, 0xB2, 0xF6, 0x3C, 0x1F, 0xBB, 0xDA, 0xE8, 0xA9, 0x4A, 0x8C, 0xFC, 0x28, 0xD3, 0xD7, 0x74, 0x97, 0x4A, 0xF5, 0x4C, 0x76, 0xD6,
	0xFA, 0x1B, 0x79, 0x7E, 0x42, 0x41, 0xAC, 0xCB, 0x6D, 0xA1, 0x79, 0xEF, 0x93, 0x41, 0x32, 0x8A, 0x93, 0x57, 0x87, 0xD1, 0x54, 0x00, 0xE1, 0x2E, 0x49, 0x21, 0xB8, 0xA9, 0xC3, 0x60, 0x72, 0x09,
	0xF5, 0xD7, 0xB8, 0xDA, 0xC2, 0xA1, 0x79, 0x99, 0x3C, 0xDE, 0xDC, 0xEB, 0x83, 0x39, 0x3F, 0x95, 0x42, 0x0D, 0xB7, 0x9D, 0xC4, 0xA4, 0x82, 0xFA, 0x39, 0x4B, 0x34, 0x20, 0x88, 0x02, 0x07, 0x0C,
	0x2E, 0x6F, 0xFA, 0xBB, 0x88, 0x57, 0xC7, 0xF1, 0xDE, 0xEE, 0x95, 0x76, 0x23, 0xF6, 0xAB, 0x10, 0xE1, 0x22, 0x67, 0xB6, 0xA9, 0xD4, 0x45, 0x50, 0x55, 0xBE, 0xA1, 0x77, 0x6C, 0x58, 0x10, 0x39,
	0xD6, 0xCF, 0xBC, 0x13, 0x3F, 0x84, 0x0D, 0xEB, 0x46, 0xC6, 0xBF, 0xD6, 0x0F, 0x0C, 0xD6, 0x43, 0xA0, 0x24, 0x3F, 0x66, 0x9A, 0x51, 0x63, 0x5A, 0x2A, 0xA9, 0x65, 0xC9, 0xB6, 0x50, 0x84, 0x86,
	0xDD, 0x5B, 0x9F, 0x54, 0x39, 0x97, 0xD0, 0x5D, 0x15, 0x7C, 0x70, 0xE3, 0x15, 0x6F, 0x41, 0x49, 0xE8, 0x5D, 0xD7, 0x48, 0x18, 0xAA, 0x9F, 0x84, 0x6C, 0x62, 0xE9, 0x95, 0x1E, 0x43, 0x34, 0xF5,
	0x41, 0x39, 0xF8, 0xB3, 0x6C, 0x82, 0x09, 0x08, 0x53, 0xE1, 0x51, 0x1E, 0xE1, 0x7F, 0x45, 0xBB, 0x68, 0x1C, 0xE1, 0x58, 0x79, 0x3C, 0x6D, 0x32, 0xBC, 0x1B, 0xE7, 0xD7, 0xAD, 0x3B, 0x3F, 0xD1
};

UNITTEST_SUITE_BEGIN(merkle)
{
	UNITTEST_FIXTURE(main)
	{
		u32 data_size = 0;
		xbyte*	data1 = NULL;
		xbyte*	data2 = NULL;
		my_sigmap_allocator a;

		UNITTEST_FIXTURE_SETUP()
		{
			data_size = merkle::data_t::size_for(bin_t::to_root(8000), sizeof(hash256));
			data1 = (xbyte*)gTestAllocator->allocate(data_size, sizeof(void*));
			data2 = (xbyte*)gTestAllocator->allocate(data_size, sizeof(void*));
		}

		UNITTEST_FIXTURE_TEARDOWN()
		{
			gTestAllocator->deallocate(data1);
			gTestAllocator->deallocate(data2);
		}

		void clear_data()
		{
			x_memzero(data1, data_size);
			x_memzero(data2, data_size);
		}

		UNITTEST_TEST(construct)
		{
			merkle::tree sm;
		}

		UNITTEST_TEST(open_close)
		{
			clear_data();

			merkle::hash_t rootSignature;
			a.allocate(rootSignature);

			merkle::tree sm(merkle::user_data_t(bin_t::to_root(32), sizeof(hash256), data1), rootSignature, combine);

			a.deallocate(rootSignature);
		}

		UNITTEST_TEST(builder_open_submit_build_close1)
		{
			clear_data();

			my_sigmap_allocator a;

			merkle::hash_t rootSignature;
			a.allocate(rootSignature);

			merkle::hash_t lhs(&digests[0], rootSignature.length_);
			merkle::hash_t rhs(&digests[1], rootSignature.length_);
			combine(lhs, rhs, rootSignature);

			merkle::tree::builder smb(merkle::user_data_t(bin_t::to_root(2), sizeof(hash256), data1), rootSignature, combine);

			// submit two signatures, they will be merged
			merkle::hash_t signature1(&digests[0], rootSignature.length_);
			CHECK_EQUAL(1, smb.write(bin_t(0, 0), signature1));
			merkle::hash_t signature2(&digests[1], rootSignature.length_);
			CHECK_EQUAL(1, smb.write(bin_t(0, 1), signature2));

			CHECK_TRUE(smb.build());
			CHECK_TRUE(smb.build_and_verify(rootSignature));

			a.deallocate(rootSignature);
		}

		UNITTEST_TEST(builder_open_submit_build_close2)
		{
			clear_data();
			my_sigmap_allocator a;

			merkle::hash_t rootSignature;
			a.allocate(rootSignature);

			const s32 length = 512;
			// manually compute the root signature according to the merkle signature scheme
			xbyte	digests_data[length][32];
			merkle::hash_t signatures[length];
			for (s32 i=0; i<length; ++i)
			{
				signatures[i].digest_ = digests_data[i];
				signatures[i].length_ = rootSignature.length_;
				x_memcopy(signatures[i].digest_, &digests[i], rootSignature.length_);
			}
			for (s32 w=length; w>1; w=w/2)
			{
				for (s32 i=0; i<w; i+=2)
				{
					combine(signatures[i], signatures[i+1], signatures[i/2]);
				}
			}
			// The signature at index 0 is the root signature
			x_memcpy(rootSignature.digest_, signatures[0].digest_, rootSignature.length_);

			merkle::tree::builder smb(merkle::user_data_t(bin_t::to_root(length), sizeof(hash256), data1), rootSignature, combine);
			for (s32 i=0; i<length; ++i)
			{
				merkle::hash_t signature1(&digests[i], rootSignature.length_);
				CHECK_EQUAL(1, smb.write(bin_t(0, i), signature1));
			}

			CHECK_TRUE(smb.build());
			CHECK_TRUE(smb.build_and_verify(rootSignature));

			a.deallocate(rootSignature);
		}

		UNITTEST_TEST(open_builder_close)
		{
			clear_data();
			merkle::hash_t rootSignature;
			a.allocate(rootSignature);

			merkle::tree::builder smb(merkle::user_data_t(bin_t::to_root(32), sizeof(hash256), data1), rootSignature, combine);
			for (s32 i=0; i<512; ++i)
			{
				merkle::hash_t signature1(&digests[i], rootSignature.length_);
				smb.write(bin_t(0, i), signature1);
			}

			a.deallocate(rootSignature);
		}

		UNITTEST_TEST(open_readbranch_close)
		{
			clear_data();
			merkle::hash_t rootSignature;
			a.allocate(rootSignature);

			merkle::user_data_t sigdata(bin_t::to_root(32), sizeof(hash256), data1);
			merkle::tree::builder smb(sigdata, rootSignature, combine);

			// A branch starts at the root and goes down to the bin at the base level.
			// The signatures that are emitted along the way are:
			// 1. The root signature
			// 2. The signature of the child's sibling
			// 3. The signature of the designated bin
			for (s32 i=0; i<8; ++i)
			{
				merkle::hash_t signature1(&digests[i], rootSignature.length_);
				smb.write(bin_t(0, i), signature1);
			}
			CHECK_TRUE(smb.build());

			merkle::tree sm(sigdata, rootSignature, combine);

			const s32 max_branch_signatures = 5 + 1;
			// manually compute the root signature according to the merkle signature scheme
			xbyte	digests_data[max_branch_signatures][32];
			merkle::hash_t branch_signatures[max_branch_signatures];
			for (s32 i=0; i<max_branch_signatures; ++i)
			{
				branch_signatures[i].digest_ = digests_data[i];
				branch_signatures[i].length_ = rootSignature.length_;
			}
			merkle::branch_t my_branch(branch_signatures, max_branch_signatures);
			s32 const branch_len = sm.read(bin_t(0,0), my_branch);
			CHECK_EQUAL(6, branch_len);

			a.deallocate(rootSignature);
		}
	}
}
UNITTEST_SUITE_END
