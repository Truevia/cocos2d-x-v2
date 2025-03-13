#pragma once

#include <spine/spine-cocos2dx.h>

USING_NS_CC;
namespace spine {

/**
 * PolygonBatch 类用于高效批处理渲染多边形（三角形）
 * 它减少了 OpenGL 状态切换和绘制调用，提高渲染性能
 */
class PolygonBatch : public CCObject {
    public:
        /**
         * 创建一个指定容量的 PolygonBatch 实例
         * @param capacity 最大可容纳的顶点数量
         * @return 创建的 PolygonBatch 实例
         */
        static PolygonBatch* createWithCapacity(int capacity);
        
        PolygonBatch();
        virtual ~PolygonBatch();
        
        /**
         * 使用指定容量初始化 PolygonBatch
         * @param capacity 最大可容纳的顶点数量
         * @return 初始化是否成功
         */
        bool initWithCapacity(int capacity);
        
        /**
         * 添加一组三角形到批处理中
         * @param texture 使用的纹理
         * @param vertices 顶点位置数组 [x1, y1, x2, y2, ...]
         * @param uvs 纹理坐标数组 [u1, v1, u2, v2, ...]
         * @param verticesCount 顶点数组长度 (通常是顶点数 * 2)
         * @param triangles 三角形索引数组
         * @param trianglesCount 三角形索引数组长度
         * @param color 顶点颜色
         */
        void add(CCTexture2D* texture,
                 const float* vertices, const float* uvs, int verticesCount,
                 const unsigned short* triangles, int trianglesCount,
                 ccColor4B* color);
                
        /**
         * 强制刷新当前批次，绘制所有累积的三角形
         */
        void flush();
        
        /**
         * 获取当前批次的顶点数量
         * @return 当前批次的顶点数量
         */
        inline int getVerticesCount() const { return verticesCount; }
        
        /**
         * 获取当前批次的三角形索引数量
         * @return 当前批次的三角形索引数量
         */
        inline int getTrianglesCount() const { return trianglesCount; }
        
        /**
         * 获取当前绑定的纹理
         * @return 当前绑定的纹理
         */
        inline CCTexture2D* getTexture() const { return texture; }
        
    private:
        int capacity;               // 最大容量（顶点数）
        ccV2F_C4B_T2F* vertices;    // 顶点数组，包含位置、颜色和纹理坐标
        int verticesCount;          // 当前顶点数量
        GLushort* triangles;        // 三角形索引数组
        int trianglesCount;         // 当前三角形索引数量
        CCTexture2D* texture;       // 当前绑定的纹理
    };

} // spine
