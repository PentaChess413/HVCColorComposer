#ifndef	__rsa_c__
#define	__rsa_c__

#include <gmp.h>

#ifdef __cplusplus

	template<typename T, void (*dispose_func)(T*)>
	class	Auto_Key : public T
	{
	public:
					Auto_Key()								: T(empty()) { }
					~Auto_Key()								{ (*dispose_func)(this); }

	private:
		inline T	empty()									{ T result = { 0 }; return result; }
	};

	extern "C"
	{
#endif

typedef struct	RSAPublicKey
{
	unsigned int	bits;
	MP_INT			n, e;
} RSAPublicKey;

/* Frees any memory associated with the public key. */

void rsa_clear_public_key(RSAPublicKey *pub);

#ifndef RSAREF

/* Performs a public-key RSA operation (encrypt/decrypt). */

void rsa_public(MP_INT *output, const MP_INT *input, const RSAPublicKey *pub);

#endif // !RSAREF

#ifdef __cplusplus
	}

	typedef Auto_Key<RSAPublicKey, rsa_clear_public_key>	Auto_Public_Key;

#endif

#endif
