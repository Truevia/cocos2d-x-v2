#include "PolygonBatch.h"

// Claude
namespace spine {

PolygonBatch* PolygonBatch::createWithCapacity(int capacity) {
    PolygonBatch* batch = new PolygonBatch();
    if (batch && batch->initWithCapacity(capacity)) {
        batch->autorelease();
        return batch;
    }
    CC_SAFE_DELETE(batch);
    return nullptr;
}

PolygonBatch::PolygonBatch() :
    capacity(0),
    vertices(nullptr), verticesCount(0),
    triangles(nullptr), trianglesCount(0),
    texture(nullptr)
{}

bool PolygonBatch::initWithCapacity(int capacity) {
    // 32767是最大索引，所以32767 / 3 - (32767 / 3 % 3) = 10920
    CCAssert(capacity <= 10920, "capacity cannot be > 10920");
    CCAssert(capacity >= 0, "capacity cannot be < 0");
    
    this->capacity = capacity;
    vertices = new ccV2F_C4B_T2F[capacity];
    triangles = new GLushort[capacity * 3];
    
    if (!vertices || !triangles) {
        CC_SAFE_DELETE_ARRAY(vertices);
        CC_SAFE_DELETE_ARRAY(triangles);
        CCLOG("PolygonBatch: failed to allocate memory");
        return false;
    }
    
    return true;
}

PolygonBatch::~PolygonBatch() {
    CC_SAFE_DELETE_ARRAY(vertices);
    CC_SAFE_DELETE_ARRAY(triangles);
}

void PolygonBatch::add(CCTexture2D* addTexture,
                    const float* addVertices, const float* uvs, int addVerticesCount,
                    const unsigned short* addTriangles, int addTrianglesCount,
                    ccColor4B* color) {
    // 确保输入参数有效
    CCAssert(addTexture, "texture cannot be null");
    CCAssert(addVertices, "vertices cannot be null");
    CCAssert(uvs, "uvs cannot be null");
    CCAssert(addTriangles, "triangles cannot be null");
    CCAssert(color, "color cannot be null");
    
    // 检查容量
    int numVertices = addVerticesCount / 2; // 每个顶点有 (x,y) 两个坐标
    
    // 如果纹理改变或容量不足，先刷新当前批次
    if (addTexture != texture ||
        verticesCount + numVertices > capacity ||
        trianglesCount + addTrianglesCount > capacity * 3) {
        this->flush();
        texture = addTexture;
    }
    
    // 保存当前顶点索引作为偏移量
    int indexOffset = verticesCount;
    
    // 添加顶点数据
    for (int i = 0; i < addVerticesCount; i += 2) {
        ccV2F_C4B_T2F* vertex = vertices + verticesCount;
        vertex->vertices.x = addVertices[i];
        vertex->vertices.y = addVertices[i + 1];
        vertex->colors = *color;
        vertex->texCoords.u = uvs[i];
        vertex->texCoords.v = uvs[i + 1];
        verticesCount++;
    }
    
    // 添加三角形索引，使用正确的偏移量
    for (int i = 0; i < addTrianglesCount; ++i) {
        triangles[trianglesCount++] = addTriangles[i] + indexOffset;
    }
}

void PolygonBatch::flush() {
    // 如果没有要渲染的顶点或纹理，直接返回
    if (verticesCount == 0 || trianglesCount == 0 || !texture) {
        verticesCount = 0;
        trianglesCount = 0;
        return;
    }

    // 设置纹理参数
    ccTexParams texParams = {GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE};
    texture->setTexParameters(&texParams);
    
    // 绑定纹理
    ccGLBindTexture2D(texture->getName());
    
    // 启用顶点属性
    ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position | kCCVertexAttribFlag_Color | kCCVertexAttribFlag_TexCoords);
    
    // 设置顶点数据
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, sizeof(ccV2F_C4B_T2F), &vertices[0].vertices);
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ccV2F_C4B_T2F), &vertices[0].colors);
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(ccV2F_C4B_T2F), &vertices[0].texCoords);

    // 绘制三角形
    glDrawElements(GL_TRIANGLES, trianglesCount, GL_UNSIGNED_SHORT, triangles);

    CC_INCREMENT_GL_DRAWS(1);
    
    // 重置计数器，准备下一批
    verticesCount = 0;
    trianglesCount = 0;

    // 检查OpenGL错误
    CHECK_GL_ERROR_DEBUG();
}

} // namespace spine {
