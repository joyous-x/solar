#pragma once
#include "../3rds/md5.h"
#include "../3rds/sha1.h"
#include "../3rds/sha256.h"
#include "../3rds/sha512.h"
#include "../utils/utils.h"

class ihash {
public:
    virtual ~ihash() {};
    virtual ihash* add(const uint8_t* data, uint32_t size) = 0;
    virtual ihash* finalize() = 0;
    virtual const uint8_t* bytes(uint32_t* size = 0) = 0;
};

class md5er : public ihash {
public:
    virtual ~md5er() { };
    virtual ihash* add(const uint8_t* data, uint32_t size) {
        hash.Update((unsigned char*)data, size);
        return this;
    }
    virtual ihash* finalize() {
        hash.Final();
        return this;
    }
    virtual const uint8_t* bytes(uint32_t* size = 0) {
        if (size) {
            *size = sizeof(hash.digestRaw);
        }
        return hash.digestRaw;
    }
protected:
    md5::MD5 hash;
};

class sha1er : public ihash {
public:
    virtual ~sha1er() {}
    virtual ihash* add(const uint8_t* data, uint32_t size) {
        hash.add(data, size);
        return this;
    }
    virtual ihash* finalize() {
        hash.finalize();
        return this;
    }
    virtual const uint8_t* bytes(uint32_t* size = 0) {
        if (size) {
            *size = sizeof(hash.digist);
        }
        return hash.digest();
    }

protected:
    sha1 hash;
};

class sha256er : public ihash {
public:
    sha256er() {
        hash.init();
        memset(digist, 0, sizeof(digist));
    }
    virtual ~sha256er() {
    }
    virtual ihash* add(const uint8_t* data, uint32_t size) {
        hash.update(data, size);
        return this;
    }
    virtual ihash* finalize() {
        hash.final(digist);
        return this;
    }
    virtual const uint8_t* bytes(uint32_t* size = 0) {
        if (size) {
            *size = sizeof(digist);
        }
        return digist;
    }

protected:
    SHA256      hash;
    uint8_t     digist[SHA256::DIGEST_SIZE];
};

class sha512er : public ihash {
public:
    sha512er() {
        hash.init();
        memset(digist, 0, sizeof(digist));
    }
    virtual ~sha512er() {
    }
    virtual ihash* add(const uint8_t* data, uint32_t size) {
        hash.update(data, size);
        return this;
    }
    virtual ihash* finalize() {
        hash.final(digist);
        return this;
    }
    virtual const uint8_t* bytes(uint32_t* size = 0) {
        if (size) {
            *size = sizeof(digist);
        }
        return digist;
    }

protected:
    SHA512      hash;
    uint8_t     digist[SHA512::DIGEST_SIZE];
};

class Hash {
public:
    Hash(const char* name) {
        hash = nullptr;
        if (0 == ucs::icmp<char>(name, "sha1")) {
            hash = new sha1er();
        } else if (0 == ucs::icmp<char>(name, "sha256")) {
            hash = new sha256er();
        } else if (0 == ucs::icmp<char>(name, "sha512")) {
            hash = new sha512er();
        } else if (0 == ucs::icmp<char>(name, "md5")) {
            hash = new md5er();
        }
    }

    ~Hash() {
        if (hash) {
            delete hash;
            hash = nullptr;
        }
    }

    Hash& add(const uint8_t* data, uint32_t size) {
        if (hash) hash->add(data, size);
        return *this;
    }

    Hash& finalize() {
        if (hash) hash->finalize();
        return *this;
    }

    const uint8_t* bytes(uint32_t* size = 0) {
        if (hash) {
            return hash->bytes(size);
        }
        return nullptr;
    }

protected:
    ihash* hash{ nullptr };
};