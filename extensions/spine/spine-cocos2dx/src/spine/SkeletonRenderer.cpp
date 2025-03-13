/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated May 1, 2019. Replaces all prior versions.
 *
 * Copyright (c) 2013-2019, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software
 * or otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THIS SOFTWARE IS PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, BUSINESS
 * INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include <spine/spine-cocos2dx.h>
#include <spine/SkeletonRenderer.h>
#include <spine/Extension.h>
//#include <spine/SkeletonBatch.h>
//#include <spine/SkeletonTwoColorBatch.h>
#include <spine/AttachmentVertices.h>
#include <algorithm>
#include <string.h>

#define INITIAL_WORLD_VERTICES_LENGTH 1000
// Used for transforming attachments for bounding boxes & debug rendering
static float* worldVertices = nullptr;
static size_t worldVerticesLength = 0;

void ensureWorldVerticesCapacity(size_t capacity) {
	if (worldVerticesLength < capacity) {
		float* newWorldVertices = new float[capacity];
		memcpy(newWorldVertices, worldVertices, worldVerticesLength * sizeof(float));
		delete[] worldVertices;
		worldVertices = newWorldVertices;
		worldVerticesLength = capacity;
	}
}

USING_NS_CC;
using std::min;
using std::max;

namespace spine {
	
	static Cocos2dTextureLoader textureLoader;
	
	void SkeletonRenderer::destroyScratchBuffers() {
		if (worldVertices) {
			delete[] worldVertices;
			worldVertices = nullptr;
			worldVerticesLength = 0;
		}
	}
	
	SkeletonRenderer* SkeletonRenderer::createWithSkeleton(Skeleton* skeleton, bool ownsSkeleton, bool ownsSkeletonData) {
		SkeletonRenderer* node = new SkeletonRenderer(skeleton, ownsSkeleton, ownsSkeletonData);
		node->autorelease();
		return node;
	}
	
	SkeletonRenderer* SkeletonRenderer::createWithData (SkeletonData* skeletonData, bool ownsSkeletonData) {
		SkeletonRenderer* node = new SkeletonRenderer(skeletonData, ownsSkeletonData);
		node->autorelease();
		return node;
	}
	
	SkeletonRenderer* SkeletonRenderer::createWithFile (const std::string& skeletonDataFile, Atlas* atlas, float scale) {
		SkeletonRenderer* node = new SkeletonRenderer(skeletonDataFile, atlas, scale);
		node->autorelease();
		return node;
	}
	
	SkeletonRenderer* SkeletonRenderer::createWithFile (const std::string& skeletonDataFile, const std::string& atlasFile, float scale) {
		SkeletonRenderer* node = new SkeletonRenderer(skeletonDataFile, atlasFile, scale);
		node->autorelease();
		return node;
	}
	
	void SkeletonRenderer::initialize () {
		if (!worldVertices) {
			worldVertices = new float[INITIAL_WORLD_VERTICES_LENGTH];
			worldVerticesLength = INITIAL_WORLD_VERTICES_LENGTH;
		}
		
		_clipper = new (__FILE__, __LINE__) SkeletonClipping();
		
		_blendFunc = BlendFuncALPHA_PREMULTIPLIED;
		setOpacityModifyRGB(true);
		
		setupGLProgramState(false);
		
		_skeleton->setToSetupPose();
		_skeleton->updateWorldTransform();
	}
	
	void SkeletonRenderer::setupGLProgramState (bool twoColorTintEnabled) {
		if (twoColorTintEnabled) {
//			setGLProgramState(SkeletonTwoColorBatch::getInstance()->getTwoColorTintProgramState());
			return;
		}
		
		Texture2D *texture = nullptr;
		for (int i = 0, n = _skeleton->getSlots().size(); i < n; i++) {
			Slot* slot = _skeleton->getDrawOrder()[i];
			if (!slot->getAttachment()) continue;
			if (slot->getAttachment()->getRTTI().isExactly(RegionAttachment::rtti)) {
				RegionAttachment* attachment = (RegionAttachment*)slot->getAttachment();
				texture = static_cast<AttachmentVertices*>(attachment->getRendererObject())->_texture;
			} else if (slot->getAttachment()->getRTTI().isExactly(MeshAttachment::rtti)) {
				MeshAttachment* attachment = (MeshAttachment*)slot->getAttachment();
				texture = static_cast<AttachmentVertices*>(attachment->getRendererObject())->_texture;
			} else {
				continue;
			}
			
			if (texture != nullptr) {
				break;
			}
		}
        //@fixme
//		setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP, texture));
        setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor));

	}
	
	void SkeletonRenderer::setSkeletonData (SkeletonData *skeletonData, bool ownsSkeletonData) {
		_skeleton = new (__FILE__, __LINE__) Skeleton(skeletonData);
		_ownsSkeletonData = ownsSkeletonData;
	}
	
	SkeletonRenderer::SkeletonRenderer ()
	: _atlas(nullptr), _attachmentLoader(nullptr), _debugSlots(false), _debugBones(false), _debugMeshes(false), _timeScale(1), _effect(nullptr), _startSlotIndex(-1), _endSlotIndex(-1) {
	}
	
	SkeletonRenderer::SkeletonRenderer(Skeleton* skeleton, bool ownsSkeleton, bool ownsSkeletonData, bool ownsAtlas)
	: _atlas(nullptr), _attachmentLoader(nullptr), _debugSlots(false), _debugBones(false), _debugMeshes(false), _timeScale(1), _effect(nullptr), _startSlotIndex(-1), _endSlotIndex(-1) {
		initWithSkeleton(skeleton, ownsSkeleton, ownsSkeletonData, ownsAtlas);
	}
	
	SkeletonRenderer::SkeletonRenderer (SkeletonData *skeletonData, bool ownsSkeletonData)
	: _atlas(nullptr), _attachmentLoader(nullptr), _debugSlots(false), _debugBones(false), _debugMeshes(false), _timeScale(1), _effect(nullptr), _startSlotIndex(-1), _endSlotIndex(-1) {
		initWithData(skeletonData, ownsSkeletonData);
	}
	
	SkeletonRenderer::SkeletonRenderer (const std::string& skeletonDataFile, Atlas* atlas, float scale)
	: _atlas(nullptr), _attachmentLoader(nullptr), _debugSlots(false), _debugBones(false), _debugMeshes(false), _timeScale(1), _effect(nullptr), _startSlotIndex(-1), _endSlotIndex(-1) {
		initWithJsonFile(skeletonDataFile, atlas, scale);
	}
	
	SkeletonRenderer::SkeletonRenderer (const std::string& skeletonDataFile, const std::string& atlasFile, float scale)
	: _atlas(nullptr), _attachmentLoader(nullptr), _debugSlots(false), _debugBones(false), _debugMeshes(false), _timeScale(1), _effect(nullptr), _startSlotIndex(-1), _endSlotIndex(-1) {
		initWithJsonFile(skeletonDataFile, atlasFile, scale);
	}
	
	SkeletonRenderer::~SkeletonRenderer () {
		if (_ownsSkeletonData) delete _skeleton->getData();
		if (_ownsSkeleton) delete _skeleton;
		if (_ownsAtlas && _atlas) delete _atlas;
		if (_attachmentLoader) delete _attachmentLoader;
		delete _clipper;
	}
	
	void SkeletonRenderer::initWithSkeleton(Skeleton* skeleton, bool ownsSkeleton, bool ownsSkeletonData, bool ownsAtlas) {
		_skeleton = skeleton;
		_ownsSkeleton = ownsSkeleton;
		_ownsSkeletonData = ownsSkeletonData;
		_ownsAtlas = ownsAtlas;
		
		initialize();
	}
	
	void SkeletonRenderer::initWithData (SkeletonData* skeletonData, bool ownsSkeletonData) {
		_ownsSkeleton = true;
		setSkeletonData(skeletonData, ownsSkeletonData);
		initialize();
	}
	
	void SkeletonRenderer::initWithJsonFile (const std::string& skeletonDataFile, Atlas* atlas, float scale) {
		_atlas = atlas;
		_attachmentLoader = new (__FILE__, __LINE__) Cocos2dAtlasAttachmentLoader(_atlas);
		
		SkeletonJson* json = new (__FILE__, __LINE__) SkeletonJson(_attachmentLoader);
		json->setScale(scale);
		SkeletonData* skeletonData = json->readSkeletonDataFile(skeletonDataFile.c_str());
		CCASSERT(skeletonData, !json->getError().isEmpty() ? json->getError().buffer() : "Error reading skeleton data.");
		delete json;
		
		_ownsSkeleton = true;
		setSkeletonData(skeletonData, true);
		
		initialize();
	}
	
	void SkeletonRenderer::initWithJsonFile (const std::string& skeletonDataFile, const std::string& atlasFile, float scale) {
		_atlas = new (__FILE__, __LINE__) Atlas(atlasFile.c_str(), &textureLoader);
		CCASSERT(_atlas, "Error reading atlas file.");
		
		_attachmentLoader = new (__FILE__, __LINE__) Cocos2dAtlasAttachmentLoader(_atlas);
		
		SkeletonJson* json = new (__FILE__, __LINE__) SkeletonJson(_attachmentLoader);
		json->setScale(scale);
		SkeletonData* skeletonData = json->readSkeletonDataFile(skeletonDataFile.c_str());
		CCASSERT(skeletonData, !json->getError().isEmpty() ? json->getError().buffer() : "Error reading skeleton data.");
		delete json;
		
		_ownsSkeleton = true;
		_ownsAtlas = true;
		setSkeletonData(skeletonData, true);
		
		initialize();
	}
	
	void SkeletonRenderer::initWithBinaryFile (const std::string& skeletonDataFile, Atlas* atlas, float scale) {
		_atlas = atlas;
		_attachmentLoader = new (__FILE__, __LINE__) Cocos2dAtlasAttachmentLoader(_atlas);
		
		SkeletonBinary* binary = new (__FILE__, __LINE__) SkeletonBinary(_attachmentLoader);
		binary->setScale(scale);
		SkeletonData* skeletonData = binary->readSkeletonDataFile(skeletonDataFile.c_str());
		CCASSERT(skeletonData, !binary->getError().isEmpty() ? binary->getError().buffer() : "Error reading skeleton data.");
		delete binary;
		_ownsSkeleton = true;
		setSkeletonData(skeletonData, true);
		
		initialize();
	}
	
	void SkeletonRenderer::initWithBinaryFile (const std::string& skeletonDataFile, const std::string& atlasFile, float scale) {
		_atlas = new (__FILE__, __LINE__) Atlas(atlasFile.c_str(), &textureLoader);
		CCASSERT(_atlas, "Error reading atlas file.");
		
		_attachmentLoader = new (__FILE__, __LINE__) Cocos2dAtlasAttachmentLoader(_atlas);
		
		SkeletonBinary* binary = new (__FILE__, __LINE__) SkeletonBinary(_attachmentLoader);
		binary->setScale(scale);
		SkeletonData* skeletonData = binary->readSkeletonDataFile(skeletonDataFile.c_str());
		CCASSERT(skeletonData, !binary->getError().isEmpty() ? binary->getError().buffer() : "Error reading skeleton data.");
		delete binary;
		_ownsSkeleton = true;
		_ownsAtlas = true;
		setSkeletonData(skeletonData, true);
		
		initialize();
	}
	
	void SkeletonRenderer::update (float deltaTime) {
		CCNode::update(deltaTime);
		if (_ownsSkeleton) _skeleton->update(deltaTime * _timeScale);
	}

    void SkeletonRenderer::draw() {
        CC_PROFILER_START_CATEGORY(kCCProfilerCategorySpine, "SkeletonRenderer::draw");
        
        if (!_skeleton) {
            return;
        }
        
        // 如果骨架不可见则提前退出
        if (getDisplayedOpacity() == 0 || _skeleton->getColor().a == 0) {
            return;
        }
        
        CC_NODE_DRAW_SETUP();
        
        // 应用节点变换
        kmGLPushMatrix();
    //    kmGLMultMatrix(&_modelViewTransform);
        
        if (_effect) _effect->begin(*_skeleton);
        
        Color4F nodeColor;
        nodeColor.r = getDisplayedColor().r / (float)255;
        nodeColor.g = getDisplayedColor().g / (float)255;
        nodeColor.b = getDisplayedColor().b / (float)255;
        nodeColor.a = getDisplayedOpacity() / (float)255;
        
        Color4F color;
        Color4F darkColor;
        float darkPremultipliedAlpha = _premultipliedAlpha ? 255 : 0;
        AttachmentVertices* attachmentVertices = nullptr;
        
        // 确保启用纹理
    //    glEnable(GL_TEXTURE_2D);
        
        bool inRange = _startSlotIndex != -1 || _endSlotIndex != -1 ? false : true;
        for (int i = 0, n = _skeleton->getSlots().size(); i < n; ++i) {
            Slot* slot = _skeleton->getDrawOrder()[i];
            
            if (_startSlotIndex >= 0 && _startSlotIndex == slot->getData().getIndex()) {
                inRange = true;
            }
            
            if (!inRange) {
                _clipper->clipEnd(*slot);
                continue;
            }
            
            if (_endSlotIndex >= 0 && _endSlotIndex == slot->getData().getIndex()) {
                inRange = false;
            }
            
            if (!slot->getAttachment()) {
                _clipper->clipEnd(*slot);
                continue;
            }
            
            // 如果插槽不可见则提前退出
            if (slot->getColor().a == 0) {
                _clipper->clipEnd(*slot);
                continue;
            }
            
            // 存储渲染数据的临时变量
            float* vertices = nullptr;
            float* uvs = nullptr;
            unsigned short* indices = nullptr;
            int vertexCount = 0;
            int indexCount = 0;
            CCTexture2D* texture = nullptr;
            
            if (slot->getAttachment()->getRTTI().isExactly(RegionAttachment::rtti)) {
                RegionAttachment* attachment = (RegionAttachment*)slot->getAttachment();
                attachmentVertices = (AttachmentVertices*)attachment->getRendererObject();
                
                if (!attachmentVertices) {
                    CCLOG("Error: No attachment vertices for region attachment!");
                    _clipper->clipEnd(*slot);
                    continue;
                }
                
                texture = attachmentVertices->_texture;
                
                // 如果附件不可见则提前退出
                if (attachment->getColor().a == 0) {
                    _clipper->clipEnd(*slot);
                    continue;
                }
                
                // 准备顶点和UV数据
                vertexCount = 4; // 四边形有4个顶点
                vertices = new float[vertexCount * 2]; // 每个顶点2个浮点数
                uvs = new float[vertexCount * 2]; // 每个顶点2个UV坐标
                
                // 获取世界坐标
                attachment->computeWorldVertices(slot->getBone(), vertices, 0, 2);
                
                // 获取UV坐标
                float* attachmentUVs = attachment->getUVs().buffer();
                for (int j = 0; j < 8; j++) {
                    uvs[j] = attachmentUVs[j];
                }
                
                // 设置索引 (两个三角形组成一个四边形)
                indexCount = 6;
                indices = new unsigned short[indexCount];
                indices[0] = 0; indices[1] = 1; indices[2] = 2;
                indices[3] = 2; indices[4] = 3; indices[5] = 0;
                
                color.r = attachment->getColor().r;
                color.g = attachment->getColor().g;
                color.b = attachment->getColor().b;
                color.a = attachment->getColor().a;
            }
            else if (slot->getAttachment()->getRTTI().isExactly(MeshAttachment::rtti)) {
                MeshAttachment* attachment = (MeshAttachment*)slot->getAttachment();
                attachmentVertices = (AttachmentVertices*)attachment->getRendererObject();
                
                if (!attachmentVertices) {
                    CCLOG("Error: No attachment vertices for mesh attachment!");
                    _clipper->clipEnd(*slot);
                    continue;
                }
                
                texture = attachmentVertices->_texture;
                
                // 如果附件不可见则提前退出
                if (attachment->getColor().a == 0) {
                    _clipper->clipEnd(*slot);
                    continue;
                }
                
                // 准备顶点和UV数据
                int verticesLength = attachment->getWorldVerticesLength();
                vertexCount = verticesLength / 2;
                
                vertices = new float[verticesLength];
                attachment->computeWorldVertices(*slot, 0, verticesLength, vertices, 0, 2);
                
                uvs = new float[verticesLength];
                for (int j = 0; j < vertexCount; j++) {
                    uvs[j * 2] = attachment->getUVs()[j * 2];
                    uvs[j * 2 + 1] = attachment->getUVs()[j * 2 + 1];
                }
                
                // 设置索引
                indexCount = attachment->getTriangles().size();
                indices = new unsigned short[indexCount];
                memcpy(indices, attachment->getTriangles().buffer(), indexCount * sizeof(unsigned short));
                
                color.r = attachment->getColor().r;
                color.g = attachment->getColor().g;
                color.b = attachment->getColor().b;
                color.a = attachment->getColor().a;
            }
            else if (slot->getAttachment()->getRTTI().isExactly(ClippingAttachment::rtti)) {
                ClippingAttachment* clip = (ClippingAttachment*)slot->getAttachment();
                _clipper->clipStart(*slot, clip);
                continue;
            } else {
                _clipper->clipEnd(*slot);
                continue;
            }
            
            // 检查纹理是否有效
            if (!texture) {
                CCLOG("Error: Attachment has no texture!");
                if (vertices) delete[] vertices;
                if (uvs) delete[] uvs;
                if (indices) delete[] indices;
                _clipper->clipEnd(*slot);
                continue;
            }
            
            float alpha = nodeColor.a * _skeleton->getColor().a * slot->getColor().a * color.a * 255;
            // 如果这个附件的颜色为0则跳过渲染
            if (alpha == 0) {
                _clipper->clipEnd(*slot);
                if (vertices) delete[] vertices;
                if (uvs) delete[] uvs;
                if (indices) delete[] indices;
                continue;
            }
            
            float multiplier = _premultipliedAlpha ? alpha : 255;
            float red = nodeColor.r * _skeleton->getColor().r * slot->getColor().r * color.r * multiplier;
            float green = nodeColor.g * _skeleton->getColor().g * slot->getColor().g * color.g * multiplier;
            float blue = nodeColor.b * _skeleton->getColor().b * slot->getColor().b * color.b * multiplier;
            
            // 预处理顶点颜色数据
            ccColor4B vertexColor = ccc4((GLubyte)red, (GLubyte)green, (GLubyte)blue, (GLubyte)alpha);
            
            // 设置混合模式
            ccBlendFunc blendFunc = getBlendFunc();
            switch (slot->getData().getBlendMode()) {
                case BlendMode_Additive:
                    blendFunc.src = _premultipliedAlpha ? GL_ONE : GL_SRC_ALPHA;
                    blendFunc.dst = GL_ONE;
                    break;
                case BlendMode_Multiply:
                    blendFunc.src = GL_DST_COLOR;
                    blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
                    break;
                case BlendMode_Screen:
                    blendFunc.src = GL_ONE;
                    blendFunc.dst = GL_ONE_MINUS_SRC_COLOR;
                    break;
                default:
                    blendFunc.src = _premultipliedAlpha ? GL_ONE : GL_SRC_ALPHA;
                    blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
            }
            
            // 应用混合模式
            ccGLBlendFunc(blendFunc.src, blendFunc.dst);
            
            // 如果有裁剪
            if (_clipper->isClipping()) {
                _clipper->clipTriangles(vertices, indices, indexCount, uvs, 2);
                
                // 清理原始数据，因为我们将使用裁剪后的数据
                if (vertices) delete[] vertices;
                if (uvs) delete[] uvs;
                if (indices) delete[] indices;
                
                if (_clipper->getClippedTriangles().size() == 0) {
                    _clipper->clipEnd(*slot);
                    continue;
                }
                
                // 使用裁剪后的数据
                vertices = _clipper->getClippedVertices().buffer();
                vertexCount = _clipper->getClippedVertices().size() >> 1;
                uvs = _clipper->getClippedUVs().buffer();
                indices = _clipper->getClippedTriangles().buffer();
                indexCount = _clipper->getClippedTriangles().size();
            }
            
            // 现在进行实际的渲染
            
            // 绑定纹理
            ccGLBindTexture2D(texture->getName());
            
            // 创建顶点数组
            ccV3F_C4B_T2F* quadVertices = new ccV3F_C4B_T2F[vertexCount];
            for (int v = 0; v < vertexCount; v++) {
                quadVertices[v].vertices = vertex3(vertices[v * 2], vertices[v * 2 + 1], 1);
                quadVertices[v].colors = vertexColor;
                quadVertices[v].texCoords = tex2(uvs[v * 2], uvs[v * 2 + 1]);
            }
            
            // 启用顶点属性
            ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position | kCCVertexAttribFlag_Color | kCCVertexAttribFlag_TexCoords);
            
            // 设置顶点数据
            glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, sizeof(ccV3F_C4B_T2F), &quadVertices[0].vertices);
            glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ccV3F_C4B_T2F), &quadVertices[0].colors);
            glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(ccV3F_C4B_T2F), &quadVertices[0].texCoords);
            
            // 设置纹理参数
            ccTexParams texParams = {GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE};
            texture->setTexParameters(&texParams);
            
            // 绘制三角形
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, indices);
            
            // 清理
            delete[] quadVertices;
            
            // 如果使用了裁剪，不删除数据，因为它们由裁剪器管理
            if (!_clipper->isClipping()) {
                if (vertices) delete[] vertices;
                if (uvs) delete[] uvs;
                if (indices) delete[] indices;
            }
            
            _clipper->clipEnd(*slot);
        }
        
        _clipper->clipEnd();
        
        if (_effect) _effect->end();
        
        // 绘制调试信息
        if (_debugSlots || _debugBones || _debugMeshes) {
    //        drawDebug();
        }
        
        kmGLPopMatrix();
        
        // 增加draw次数
		CC_INCREMENT_GL_DRAWS(1);

        CC_PROFILER_STOP_CATEGORY(kCCProfilerCategorySpine, "SkeletonRenderer::draw");
    }

	CCRect SkeletonRenderer::boundingBox () {
		float minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;
		float scaleX = getScaleX(), scaleY = getScaleY();
		for (int i = 0; i < _skeleton->getSlots().size(); ++i) {
			Slot* slot = _skeleton->getSlots()[i];
			if (!slot->getAttachment()) continue;
			int verticesCount;
			if (slot->getAttachment()->getRTTI().isExactly(RegionAttachment::rtti)) {
				RegionAttachment* attachment = (RegionAttachment*)slot->getAttachment();
				attachment->computeWorldVertices(slot->getBone(), worldVertices, 0, 2);
				verticesCount = 8;
			} else if (slot->getAttachment()->getRTTI().isExactly(MeshAttachment::rtti)) {
				MeshAttachment* mesh = (MeshAttachment*)slot->getAttachment();
				ensureWorldVerticesCapacity(mesh->getWorldVerticesLength());
				mesh->computeWorldVertices(*slot, 0, mesh->getWorldVerticesLength(), worldVertices, 0, 2);
				verticesCount = mesh->getWorldVerticesLength();
			} else
				continue;
			for (int ii = 0; ii < verticesCount; ii += 2) {
				float x = worldVertices[ii] * scaleX, y = worldVertices[ii + 1] * scaleY;
				minX = min(minX, x);
				minY = min(minY, y);
				maxX = max(maxX, x);
				maxY = max(maxY, y);
			}
		}
		Vec2 position = getPosition();
		if (minX == FLT_MAX) minX = minY = maxX = maxY = 0;
		return CCRect(position.x + minX, position.y + minY, maxX - minX, maxY - minY);
	}
	
	// --- Convenience methods for Skeleton_* functions.
	
	void SkeletonRenderer::updateWorldTransform () {
		_skeleton->updateWorldTransform();
	}
	
	void SkeletonRenderer::setToSetupPose () {
		_skeleton->setToSetupPose();
	}
	void SkeletonRenderer::setBonesToSetupPose () {
		_skeleton->setBonesToSetupPose();
	}
	void SkeletonRenderer::setSlotsToSetupPose () {
		_skeleton->setSlotsToSetupPose();
	}
	
	Bone* SkeletonRenderer::findBone (const std::string& boneName) const {
		return _skeleton->findBone(boneName.c_str());
	}
	
	Slot* SkeletonRenderer::findSlot (const std::string& slotName) const {
		return _skeleton->findSlot(slotName.c_str());
	}
	
	void SkeletonRenderer::setSkin (const std::string& skinName) {
		_skeleton->setSkin(skinName.empty() ? 0 : skinName.c_str());
	}
	void SkeletonRenderer::setSkin (const char* skinName) {
		_skeleton->setSkin(skinName);
	}
	
	Attachment* SkeletonRenderer::getAttachment (const std::string& slotName, const std::string& attachmentName) const {
		return _skeleton->getAttachment(slotName.c_str(), attachmentName.c_str());
	}
	bool SkeletonRenderer::setAttachment (const std::string& slotName, const std::string& attachmentName) {
		return _skeleton->getAttachment(slotName.c_str(), attachmentName.empty() ? 0 : attachmentName.c_str()) ? true : false;
	}
	bool SkeletonRenderer::setAttachment (const std::string& slotName, const char* attachmentName) {
		return _skeleton->getAttachment(slotName.c_str(), attachmentName) ? true : false;
	}
	
	void SkeletonRenderer::setTwoColorTint(bool enabled) {
		setupGLProgramState(enabled);
	}
	
	bool SkeletonRenderer::isTwoColorTint() {
		return false;
	}
	
	void SkeletonRenderer::setVertexEffect(VertexEffect *effect) {
		this->_effect = effect;
	}
	
	void SkeletonRenderer::setSlotsRange(int startSlotIndex, int endSlotIndex) {
		this->_startSlotIndex = startSlotIndex;
		this->_endSlotIndex = endSlotIndex;
	}
	
	Skeleton* SkeletonRenderer::getSkeleton () {
		return _skeleton;
	}
	
	void SkeletonRenderer::setTimeScale (float scale) {
		_timeScale = scale;
	}
	float SkeletonRenderer::getTimeScale () const {
		return _timeScale;
	}
	
	void SkeletonRenderer::setDebugSlotsEnabled (bool enabled) {
		_debugSlots = enabled;
	}
	bool SkeletonRenderer::getDebugSlotsEnabled () const {
		return _debugSlots;
	}
	
	void SkeletonRenderer::setDebugBonesEnabled (bool enabled) {
		_debugBones = enabled;
	}
	bool SkeletonRenderer::getDebugBonesEnabled () const {
		return _debugBones;
	}
	
	void SkeletonRenderer::setDebugMeshesEnabled (bool enabled) {
		_debugMeshes = enabled;
	}
	bool SkeletonRenderer::getDebugMeshesEnabled () const {
		return _debugMeshes;
	}
	
	void SkeletonRenderer::onEnter () {
#if CC_ENABLE_SCRIPT_BINDING
		if (_scriptType == kScriptTypeJavascript && ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnEnter)) return;
#endif
		CCNode::onEnter();
		scheduleUpdate();
	}
	
	void SkeletonRenderer::onExit () {
#if CC_ENABLE_SCRIPT_BINDING
		if (_scriptType == kScriptTypeJavascript && ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnExit)) return;
#endif
		CCNode::onExit();
		unscheduleUpdate();
	}
	
	// --- CCBlendProtocol
	
	BlendFunc SkeletonRenderer::getBlendFunc () {
		return _blendFunc;
	}
	
	void SkeletonRenderer::setBlendFunc (BlendFunc blendFunc) {
		_blendFunc = blendFunc;
	}
	
	void SkeletonRenderer::setOpacityModifyRGB (bool value) {
		_premultipliedAlpha = value;
	}
	
	bool SkeletonRenderer::isOpacityModifyRGB () {
		return _premultipliedAlpha;
	}
	
}
