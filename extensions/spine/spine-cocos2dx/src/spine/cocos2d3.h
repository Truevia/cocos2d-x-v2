#pragma once

#include "cocos2d.h"
#include <string.h>

#define CC_CONSTRUCTOR_ACCESS public
#define CCASSERT CCAssert

NS_CC_BEGIN

typedef CCBlendProtocol BlendProtocol ;
typedef CCTexture2D Texture2D ;
typedef ccBlendFunc BlendFunc ;
typedef ccV3F_C4B_T2F V3F_C4B_T2F;
typedef ccColor4F Color4F;
typedef CCPoint Vec2;

// Triangles structure to replace TrianglesCommand::Triangles
struct Triangles {
    ccV3F_C4B_T2F* verts;
    unsigned short* indices;
    int vertCount;
    int indexCount;
};

// BlendFunc::ALPHA_PREMULTIPLIED;
static const BlendFunc BlendFuncALPHA_PREMULTIPLIED = {GL_ONE, GL_ONE_MINUS_SRC_ALPHA};

NS_CC_END
