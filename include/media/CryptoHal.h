/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CRYPTO_HAL_H_

#define CRYPTO_HAL_H_

#include <android/hardware/drm/1.0/ICryptoFactory.h>
#include <android/hardware/drm/1.0/ICryptoPlugin.h>
#include <media/ICrypto.h>
#include <utils/KeyedVector.h>
#include <utils/threads.h>

#include "SharedLibrary.h"

class IMemoryHeap;

namespace android {

struct CryptoHal : public BnCrypto {
    CryptoHal();
    virtual ~CryptoHal();

    virtual status_t initCheck() const;

    virtual bool isCryptoSchemeSupported(const uint8_t uuid[16]);

    virtual status_t createPlugin(
            const uint8_t uuid[16], const void *data, size_t size);

    virtual status_t destroyPlugin();

    virtual bool requiresSecureDecoderComponent(
            const char *mime) const;

    virtual void notifyResolution(uint32_t width, uint32_t height);

    virtual status_t setMediaDrmSession(const Vector<uint8_t> &sessionId);

    virtual ssize_t decrypt(const uint8_t key[16], const uint8_t iv[16],
            CryptoPlugin::Mode mode, const CryptoPlugin::Pattern &pattern,
            const sp<IMemory> &source, size_t offset,
            const CryptoPlugin::SubSample *subSamples, size_t numSubSamples,
            const ICrypto::DestinationBuffer &destination,
            AString *errorDetailMsg);

private:
    mutable Mutex mLock;

    sp<SharedLibrary> mLibrary;
    sp<::android::hardware::drm::V1_0::ICryptoFactory> mFactory;
    sp<::android::hardware::drm::V1_0::ICryptoPlugin> mPlugin;

    /**
     * mInitCheck is:
     *   NO_INIT if a plugin hasn't been created yet
     *   ERROR_UNSUPPORTED if a plugin can't be created for the uuid
     *   OK after a plugin has been created and mPlugin is valid
     */
    status_t mInitCheck;

    KeyedVector<void *, uint32_t> mHeapBases;
    uint32_t mNextBufferId;

    sp<::android::hardware::drm::V1_0::ICryptoFactory>
            makeCryptoFactory();
    sp<::android::hardware::drm::V1_0::ICryptoPlugin>
            makeCryptoPlugin(const uint8_t uuid[16], const void *initData,
                size_t size);

    void setHeapBase(const sp<IMemoryHeap>& heap);

    status_t toSharedBuffer(const sp<IMemory>& memory,
            ::android::hardware::drm::V1_0::SharedBuffer* buffer);

    DISALLOW_EVIL_CONSTRUCTORS(CryptoHal);
};

}  // namespace android

#endif  // CRYPTO_HAL_H_
