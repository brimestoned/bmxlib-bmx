/*
 * Copyright (C) 2013, British Broadcasting Corporation
 * All Rights Reserved.
 *
 * Author: Philip de Nier
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the British Broadcasting Corporation nor the names
 *       of its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BMX_AVC_MXF_DESCRIPTOR_HELPER_H_
#define BMX_AVC_MXF_DESCRIPTOR_HELPER_H_


#include <bmx/mxf_helper/PictureMXFDescriptorHelper.h>
#include <bmx/essence_parser/AVCEssenceParser.h>



namespace bmx
{


class AVCMXFDescriptorHelper : public PictureMXFDescriptorHelper
{
public:
    static EssenceType IsSupported(mxfpp::FileDescriptor *file_descriptor, mxfUL alternative_ec_label);
    static bool IsSupported(EssenceType essence_type);

    static void MapColorPrimaries(uint8_t avc_value, PictureMXFDescriptorHelper *pict_helper);
    static void MapTransferCharacteristic(uint8_t avc_value, PictureMXFDescriptorHelper *pict_helper);
    static void MapMatrixCoefficients(uint8_t avc_value, PictureMXFDescriptorHelper *pict_helper);

public:
    AVCMXFDescriptorHelper();
    virtual ~AVCMXFDescriptorHelper();

public:
    // initialize from existing descriptor
    virtual void Initialize(mxfpp::FileDescriptor *file_descriptor, uint16_t mxf_version, mxfUL alternative_ec_label);

public:
    // configure and create new descriptor
    virtual void SetEssenceType(EssenceType essence_type);

    virtual mxfpp::FileDescriptor* CreateFileDescriptor(mxfpp::HeaderMetadata *header_metadata);
    virtual void UpdateFileDescriptor();
    virtual void UpdateFileDescriptor(mxfpp::FileDescriptor *file_desc_in);
    void UpdateFileDescriptor(AVCEssenceParser *essence_parser);

    mxfpp::AVCSubDescriptor* GetAVCSubDescriptor() const { return mAVCSubDescriptor; }

protected:
    virtual mxfUL ChooseEssenceContainerUL() const;

private:
    void UpdateEssenceIndex();

private:
    size_t mEssenceIndex;
    mxfpp::AVCSubDescriptor *mAVCSubDescriptor;
};


};



#endif

