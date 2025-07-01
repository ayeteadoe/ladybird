/*
 * Copyright (c) 2024, Andrew Kaster <akaster@serenityos.org>
 * Copyright (c) 2024, stelar7 <dudedbz@gmail.com>
 * Copyright (c) 2024, Jelle Raaijmakers <jelle@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/EnumBits.h>
#include <AK/String.h>
#include <LibCrypto/BigInt/UnsignedBigInteger.h>
#include <LibGC/Ptr.h>
#include <LibJS/Forward.h>
#include <LibWeb/Bindings/SubtleCryptoPrototype.h>
#include <LibWeb/Crypto/CryptoBindings.h>
#include <LibWeb/Crypto/CryptoKey.h>
#include <LibWeb/Forward.h>
#include <LibWeb/WebIDL/Buffers.h>
#include <LibWeb/WebIDL/ExceptionOr.h>
#include <LibWeb/WebIDL/Types.h>

namespace Web::Crypto {

using AlgorithmIdentifier = Variant<GC::Root<JS::Object>, String>;
using NamedCurve = String;
using KeyDataType = Variant<GC::Root<WebIDL::BufferSource>, Bindings::JsonWebKey>;

struct WEB_API HashAlgorithmIdentifier : public AlgorithmIdentifier {
    using AlgorithmIdentifier::AlgorithmIdentifier;

    JS::ThrowCompletionOr<String> name(JS::VM& vm) const
    {
        auto value = visit(
            [](String const& name) -> JS::ThrowCompletionOr<String> { return name; },
            [&](GC::Root<JS::Object> const& obj) -> JS::ThrowCompletionOr<String> {
                auto name_property = TRY(obj->get("name"_fly_string));
                return name_property.to_string(vm);
            });

        return value;
    }
};

// https://w3c.github.io/webcrypto/#algorithm-overview
struct WEB_API AlgorithmParams {
    virtual ~AlgorithmParams();
    explicit AlgorithmParams()
    {
    }

    // NOTE: this is initialized when normalizing the algorithm name as the spec requests.
    //       It must not be set in `from_value`.
    String name;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#aes-cbc
struct WEB_API AesCbcParams : public AlgorithmParams {
    virtual ~AesCbcParams() override;
    AesCbcParams(ByteBuffer iv)
        : iv(move(iv))
    {
    }

    ByteBuffer iv;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#dfn-AesCtrParams
struct WEB_API AesCtrParams : public AlgorithmParams {
    virtual ~AesCtrParams() override;
    AesCtrParams(ByteBuffer counter, u8 length)
        : counter(move(counter))
        , length(length)
    {
    }

    ByteBuffer counter;
    u8 length;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#dfn-AesGcmParams
struct WEB_API AesGcmParams : public AlgorithmParams {
    virtual ~AesGcmParams() override;
    AesGcmParams(ByteBuffer iv, Optional<ByteBuffer> additional_data, Optional<u8> tag_length)
        : iv(move(iv))
        , additional_data(move(additional_data))
        , tag_length(tag_length)
    {
    }

    ByteBuffer iv;
    Optional<ByteBuffer> additional_data;
    Optional<u8> tag_length;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#hkdf-params
struct WEB_API HKDFParams : public AlgorithmParams {
    virtual ~HKDFParams() override;
    HKDFParams(HashAlgorithmIdentifier hash, ByteBuffer salt, ByteBuffer info)
        : hash(move(hash))
        , salt(move(salt))
        , info(move(info))
    {
    }

    HashAlgorithmIdentifier hash;
    ByteBuffer salt;
    ByteBuffer info;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#pbkdf2-params
struct WEB_API PBKDF2Params : public AlgorithmParams {
    virtual ~PBKDF2Params() override;
    PBKDF2Params(ByteBuffer salt, u32 iterations, HashAlgorithmIdentifier hash)
        : salt(move(salt))
        , iterations(iterations)
        , hash(move(hash))
    {
    }

    ByteBuffer salt;
    u32 iterations;
    HashAlgorithmIdentifier hash;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#dfn-RsaKeyGenParams
struct WEB_API RsaKeyGenParams : public AlgorithmParams {
    virtual ~RsaKeyGenParams() override;

    RsaKeyGenParams(u32 modulus_length, ::Crypto::UnsignedBigInteger public_exponent)
        : modulus_length(modulus_length)
        , public_exponent(move(public_exponent))
    {
    }

    u32 modulus_length;
    // NOTE that the raw data is going to be in Big Endian u8[] format
    ::Crypto::UnsignedBigInteger public_exponent;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#dfn-RsaHashedKeyGenParams
struct WEB_API RsaHashedKeyGenParams : public RsaKeyGenParams {
    virtual ~RsaHashedKeyGenParams() override;

    RsaHashedKeyGenParams(u32 modulus_length, ::Crypto::UnsignedBigInteger public_exponent, HashAlgorithmIdentifier hash)
        : RsaKeyGenParams(modulus_length, move(public_exponent))
        , hash(move(hash))
    {
    }

    HashAlgorithmIdentifier hash;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#dfn-RsaHashedImportParams
struct WEB_API RsaHashedImportParams : public AlgorithmParams {
    virtual ~RsaHashedImportParams() override;

    RsaHashedImportParams(HashAlgorithmIdentifier hash)
        : hash(move(hash))
    {
    }

    HashAlgorithmIdentifier hash;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#dfn-RsaOaepParams
struct WEB_API RsaOaepParams : public AlgorithmParams {
    virtual ~RsaOaepParams() override;

    RsaOaepParams(ByteBuffer label)
        : label(move(label))
    {
    }

    ByteBuffer label;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#dfn-RsaPssParams
struct WEB_API RsaPssParams : public AlgorithmParams {
    virtual ~RsaPssParams() override;

    RsaPssParams(WebIDL::UnsignedLong salt_length)
        : salt_length(salt_length)
    {
    }

    WebIDL::UnsignedLong salt_length;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#dfn-EcdsaParams
struct WEB_API EcdsaParams : public AlgorithmParams {
    virtual ~EcdsaParams() override;

    EcdsaParams(HashAlgorithmIdentifier hash)
        : hash(move(hash))
    {
    }

    HashAlgorithmIdentifier hash;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#dfn-EcKeyGenParams
struct WEB_API EcKeyGenParams : public AlgorithmParams {
    virtual ~EcKeyGenParams() override;

    EcKeyGenParams(NamedCurve named_curve)
        : named_curve(move(named_curve))
    {
    }

    NamedCurve named_curve;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#dfn-AesKeyGenParams
struct WEB_API AesKeyGenParams : public AlgorithmParams {
    virtual ~AesKeyGenParams() override;

    AesKeyGenParams(u16 length)
        : length(length)
    {
    }

    u16 length;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#dfn-AesDerivedKeyParams
struct WEB_API AesDerivedKeyParams : public AlgorithmParams {
    virtual ~AesDerivedKeyParams() override;

    AesDerivedKeyParams(u16 length)
        : length(length)
    {
    }

    u16 length;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#hmac-importparams
struct WEB_API HmacImportParams : public AlgorithmParams {
    virtual ~HmacImportParams() override;

    HmacImportParams(HashAlgorithmIdentifier hash, Optional<WebIDL::UnsignedLong> length)
        : hash(move(hash))
        , length(length)
    {
    }

    HashAlgorithmIdentifier hash;
    Optional<WebIDL::UnsignedLong> length;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://w3c.github.io/webcrypto/#hmac-keygen-params
struct WEB_API HmacKeyGenParams : public AlgorithmParams {
    virtual ~HmacKeyGenParams() override;

    HmacKeyGenParams(HashAlgorithmIdentifier hash, Optional<WebIDL::UnsignedLong> length)
        : hash(move(hash))
        , length(length)
    {
    }

    HashAlgorithmIdentifier hash;
    Optional<WebIDL::UnsignedLong> length;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

class WEB_API AlgorithmMethods {
public:
    virtual ~AlgorithmMethods();

    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> encrypt(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&)
    {
        return WebIDL::NotSupportedError::create(m_realm, "encrypt is not supported"_string);
    }

    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> decrypt(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&)
    {
        return WebIDL::NotSupportedError::create(m_realm, "decrypt is not supported"_string);
    }

    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> sign(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&)
    {
        return WebIDL::NotSupportedError::create(m_realm, "sign is not supported"_string);
    }

    virtual WebIDL::ExceptionOr<JS::Value> verify(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&, ByteBuffer const&)
    {
        return WebIDL::NotSupportedError::create(m_realm, "verify is not supported"_string);
    }

    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> digest(AlgorithmParams const&, ByteBuffer const&)
    {
        return WebIDL::NotSupportedError::create(m_realm, "digest is not supported"_string);
    }

    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> derive_bits(AlgorithmParams const&, GC::Ref<CryptoKey>, Optional<u32>)
    {
        return WebIDL::NotSupportedError::create(m_realm, "deriveBits is not supported"_string);
    }

    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&)
    {
        return WebIDL::NotSupportedError::create(m_realm, "importKey is not supported"_string);
    }

    virtual WebIDL::ExceptionOr<Variant<GC::Ref<CryptoKey>, GC::Ref<CryptoKeyPair>>> generate_key(AlgorithmParams const&, bool, Vector<Bindings::KeyUsage> const&)
    {
        return WebIDL::NotSupportedError::create(m_realm, "generateKey is not supported"_string);
    }

    virtual WebIDL::ExceptionOr<GC::Ref<JS::Object>> export_key(Bindings::KeyFormat, GC::Ref<CryptoKey>)
    {
        return WebIDL::NotSupportedError::create(m_realm, "exportKey is not supported"_string);
    }

    virtual WebIDL::ExceptionOr<JS::Value> get_key_length(AlgorithmParams const&)
    {
        return WebIDL::NotSupportedError::create(m_realm, "getKeyLength is not supported"_string);
    }

    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> wrap_key(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&)
    {
        return WebIDL::NotSupportedError::create(m_realm, "wrapKey is not supported"_string);
    }

    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> unwrap_key(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&)
    {
        return WebIDL::NotSupportedError::create(m_realm, "unwwrapKey is not supported"_string);
    }

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new AlgorithmMethods(realm)); }

protected:
    explicit AlgorithmMethods(JS::Realm& realm)
        : m_realm(realm)
    {
    }

    GC::Ref<JS::Realm> m_realm;
};

class WEB_API RSAOAEP : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> encrypt(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> decrypt(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;

    virtual WebIDL::ExceptionOr<Variant<GC::Ref<CryptoKey>, GC::Ref<CryptoKeyPair>>> generate_key(AlgorithmParams const&, bool, Vector<Bindings::KeyUsage> const&) override;

    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::Object>> export_key(Bindings::KeyFormat, GC::Ref<CryptoKey>) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new RSAOAEP(realm)); }

private:
    explicit RSAOAEP(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API RSAPSS : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> sign(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;
    virtual WebIDL::ExceptionOr<JS::Value> verify(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&, ByteBuffer const&) override;

    virtual WebIDL::ExceptionOr<Variant<GC::Ref<CryptoKey>, GC::Ref<CryptoKeyPair>>> generate_key(AlgorithmParams const&, bool, Vector<Bindings::KeyUsage> const&) override;

    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::Object>> export_key(Bindings::KeyFormat, GC::Ref<CryptoKey>) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new RSAPSS(realm)); }

private:
    explicit RSAPSS(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API RSASSAPKCS1 : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> sign(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;
    virtual WebIDL::ExceptionOr<JS::Value> verify(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&, ByteBuffer const&) override;

    virtual WebIDL::ExceptionOr<Variant<GC::Ref<CryptoKey>, GC::Ref<CryptoKeyPair>>> generate_key(AlgorithmParams const&, bool, Vector<Bindings::KeyUsage> const&) override;

    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::Object>> export_key(Bindings::KeyFormat, GC::Ref<CryptoKey>) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new RSASSAPKCS1(realm)); }

private:
    explicit RSASSAPKCS1(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API AesCbc : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> encrypt(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> decrypt(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<Variant<GC::Ref<CryptoKey>, GC::Ref<CryptoKeyPair>>> generate_key(AlgorithmParams const&, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::Object>> export_key(Bindings::KeyFormat, GC::Ref<CryptoKey>) override;
    virtual WebIDL::ExceptionOr<JS::Value> get_key_length(AlgorithmParams const&) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new AesCbc(realm)); }

private:
    explicit AesCbc(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API AesCtr : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::Object>> export_key(Bindings::KeyFormat, GC::Ref<CryptoKey>) override;
    virtual WebIDL::ExceptionOr<JS::Value> get_key_length(AlgorithmParams const&) override;
    virtual WebIDL::ExceptionOr<Variant<GC::Ref<CryptoKey>, GC::Ref<CryptoKeyPair>>> generate_key(AlgorithmParams const&, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> encrypt(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> decrypt(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new AesCtr(realm)); }

private:
    explicit AesCtr(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API AesGcm : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<JS::Value> get_key_length(AlgorithmParams const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::Object>> export_key(Bindings::KeyFormat, GC::Ref<CryptoKey>) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> encrypt(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> decrypt(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;
    virtual WebIDL::ExceptionOr<Variant<GC::Ref<CryptoKey>, GC::Ref<CryptoKeyPair>>> generate_key(AlgorithmParams const&, bool, Vector<Bindings::KeyUsage> const&) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new AesGcm(realm)); }

private:
    explicit AesGcm(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API AesKw : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::Object>> export_key(Bindings::KeyFormat, GC::Ref<CryptoKey>) override;
    virtual WebIDL::ExceptionOr<JS::Value> get_key_length(AlgorithmParams const&) override;
    virtual WebIDL::ExceptionOr<Variant<GC::Ref<CryptoKey>, GC::Ref<CryptoKeyPair>>> generate_key(AlgorithmParams const&, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> wrap_key(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> unwrap_key(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new AesKw(realm)); }

private:
    explicit AesKw(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API HKDF : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> derive_bits(AlgorithmParams const&, GC::Ref<CryptoKey>, Optional<u32>) override;
    virtual WebIDL::ExceptionOr<JS::Value> get_key_length(AlgorithmParams const&) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new HKDF(realm)); }

private:
    explicit HKDF(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API PBKDF2 : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> derive_bits(AlgorithmParams const&, GC::Ref<CryptoKey>, Optional<u32>) override;
    virtual WebIDL::ExceptionOr<JS::Value> get_key_length(AlgorithmParams const&) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new PBKDF2(realm)); }

private:
    explicit PBKDF2(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API SHA : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> digest(AlgorithmParams const&, ByteBuffer const&) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new SHA(realm)); }

private:
    explicit SHA(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API ECDSA : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> sign(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;
    virtual WebIDL::ExceptionOr<JS::Value> verify(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&, ByteBuffer const&) override;

    virtual WebIDL::ExceptionOr<Variant<GC::Ref<CryptoKey>, GC::Ref<CryptoKeyPair>>> generate_key(AlgorithmParams const&, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::Object>> export_key(Bindings::KeyFormat, GC::Ref<CryptoKey>) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new ECDSA(realm)); }

private:
    explicit ECDSA(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API ECDH : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<Variant<GC::Ref<CryptoKey>, GC::Ref<CryptoKeyPair>>> generate_key(AlgorithmParams const&, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::Object>> export_key(Bindings::KeyFormat, GC::Ref<CryptoKey>) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> derive_bits(AlgorithmParams const&, GC::Ref<CryptoKey>, Optional<u32>) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new ECDH(realm)); }

private:
    explicit ECDH(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API ED25519 : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> sign(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;
    virtual WebIDL::ExceptionOr<JS::Value> verify(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&, ByteBuffer const&) override;

    virtual WebIDL::ExceptionOr<Variant<GC::Ref<CryptoKey>, GC::Ref<CryptoKeyPair>>> generate_key(AlgorithmParams const&, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::Object>> export_key(Bindings::KeyFormat, GC::Ref<CryptoKey>) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new ED25519(realm)); }

private:
    explicit ED25519(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API ED448 : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> sign(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;
    virtual WebIDL::ExceptionOr<JS::Value> verify(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&, ByteBuffer const&) override;

    virtual WebIDL::ExceptionOr<Variant<GC::Ref<CryptoKey>, GC::Ref<CryptoKeyPair>>> generate_key(AlgorithmParams const&, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::Object>> export_key(Bindings::KeyFormat, GC::Ref<CryptoKey>) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new ED448(realm)); }

private:
    explicit ED448(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API X25519 : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> derive_bits(AlgorithmParams const&, GC::Ref<CryptoKey>, Optional<u32>) override;
    virtual WebIDL::ExceptionOr<Variant<GC::Ref<CryptoKey>, GC::Ref<CryptoKeyPair>>> generate_key(AlgorithmParams const&, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::Object>> export_key(Bindings::KeyFormat, GC::Ref<CryptoKey>) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new X25519(realm)); }

private:
    explicit X25519(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API X448 : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> derive_bits(AlgorithmParams const&, GC::Ref<CryptoKey>, Optional<u32>) override;
    virtual WebIDL::ExceptionOr<Variant<GC::Ref<CryptoKey>, GC::Ref<CryptoKeyPair>>> generate_key(AlgorithmParams const&, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::Object>> export_key(Bindings::KeyFormat, GC::Ref<CryptoKey>) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new X448(realm)); }

private:
    explicit X448(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

class WEB_API HMAC : public AlgorithmMethods {
public:
    virtual WebIDL::ExceptionOr<GC::Ref<JS::ArrayBuffer>> sign(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&) override;
    virtual WebIDL::ExceptionOr<JS::Value> verify(AlgorithmParams const&, GC::Ref<CryptoKey>, ByteBuffer const&, ByteBuffer const&) override;
    virtual WebIDL::ExceptionOr<Variant<GC::Ref<CryptoKey>, GC::Ref<CryptoKeyPair>>> generate_key(AlgorithmParams const&, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<CryptoKey>> import_key(AlgorithmParams const&, Bindings::KeyFormat, CryptoKey::InternalKeyData, bool, Vector<Bindings::KeyUsage> const&) override;
    virtual WebIDL::ExceptionOr<GC::Ref<JS::Object>> export_key(Bindings::KeyFormat, GC::Ref<CryptoKey>) override;
    virtual WebIDL::ExceptionOr<JS::Value> get_key_length(AlgorithmParams const&) override;

    static NonnullOwnPtr<AlgorithmMethods> create(JS::Realm& realm) { return adopt_own(*new HMAC(realm)); }

private:
    explicit HMAC(JS::Realm& realm)
        : AlgorithmMethods(realm)
    {
    }
};

struct WEB_API EcdhKeyDeriveParams : public AlgorithmParams {
    virtual ~EcdhKeyDeriveParams() override;

    EcdhKeyDeriveParams(CryptoKey& public_key)
        : public_key(public_key)
    {
    }

    GC::Ref<CryptoKey> public_key;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

struct WEB_API EcKeyImportParams : public AlgorithmParams {
    virtual ~EcKeyImportParams() override;

    EcKeyImportParams(String named_curve)
        : named_curve(move(named_curve))
    {
    }

    String named_curve;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

// https://wicg.github.io/webcrypto-secure-curves/#dfn-Ed448Params
struct WEB_API Ed448Params : public AlgorithmParams {
    virtual ~Ed448Params() override;

    Ed448Params(Optional<ByteBuffer>& context)
        : context(context)
    {
    }

    Optional<ByteBuffer> context;

    static JS::ThrowCompletionOr<NonnullOwnPtr<AlgorithmParams>> from_value(JS::VM&, JS::Value);
};

WEB_API ErrorOr<String> base64_url_uint_encode(::Crypto::UnsignedBigInteger);
WEB_API WebIDL::ExceptionOr<ByteBuffer> base64_url_bytes_decode(JS::Realm&, String const& base64_url_string);
WEB_API WebIDL::ExceptionOr<::Crypto::UnsignedBigInteger> base64_url_uint_decode(JS::Realm&, String const& base64_url_string);

}
