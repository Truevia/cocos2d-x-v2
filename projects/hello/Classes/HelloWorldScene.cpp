#include "HelloWorldScene.h"

#include <math.h>

#include "cocos-ext.h"

USING_NS_CC;

void shader_setGray(cocos2d::CCNode* node)
{
    cocos2d::CCGLProgram* shaderProgram = new cocos2d::CCGLProgram();
    shaderProgram->initWithVertexShaderFilename("shader/shader_gray.vsh", "shader/shader_gray.fsh");
    shaderProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
    shaderProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
    shaderProgram->link();
    shaderProgram->updateUniforms();

    GLint grayUniformLocation = glGetUniformLocation(shaderProgram->getProgram(), "u_gray");
    GLint u_alphaUniformLocation = glGetUniformLocation(shaderProgram->getProgram(), "u_alpha");
    node->setShaderProgram(shaderProgram);
    node->getShaderProgram()->use();
    node->getShaderProgram()->setUniformLocationWith1f(grayUniformLocation, 1.0f);
    //node->getShaderProgram()->setUniformLocationWith1f(u_alphaUniformLocation, 200.0/255.0);
}

void shader_shadow(cocos2d::CCNode* node)
{
    cocos2d::CCGLProgram* shaderProgram = new cocos2d::CCGLProgram();
    shaderProgram->initWithVertexShaderFilename("shader/shader_gray.vsh", "shader/shader_gray.fsh");
    shaderProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
    shaderProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
    shaderProgram->link();
    shaderProgram->updateUniforms();

    GLint u_shadowColor = glGetUniformLocation(shaderProgram->getProgram(), "u_shadowColor");
    GLint u_shadowOffset = glGetUniformLocation(shaderProgram->getProgram(), "u_shadowOffset");
    node->setShaderProgram(shaderProgram);
    node->getShaderProgram()->use();
    node->getShaderProgram()->setUniformLocationWith4f(u_shadowColor, 0.0, 0.0, 0.0, 10.0/200.0);
    node->getShaderProgram()->setUniformLocationWith2f(u_shadowOffset, 0.0, -0.05);
    //node->getShaderProgram()->setUniformLocationWith1f(u_alphaUniformLocation, 200.0/255.0);
}


CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();

    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

void HelloWorld::testSpine()
{
    auto skeletonNode = spine::SkeletonAnimation::createWithFile("spine37/spineboy.json", "spine37/spineboy.atlas", 0.6f);
//
//    skeletonNode->startListener = [this, skeletonNode](int trackIndex) {
//        spTrackEntry* entry = spAnimationState_getCurrent(skeletonNode->state, trackIndex);
//        const char* animationName = (entry && entry->animation) ? entry->animation->name : 0;
//        CCLog("%d start: %s", trackIndex, animationName);
//    };
//    skeletonNode->endListener = [](int trackIndex) {
//        CCLog("%d end", trackIndex);
//    };
//    skeletonNode->completeListener = [](int trackIndex, int loopCount) {
//        CCLog("%d complete: %d", trackIndex, loopCount);
//    };
//    skeletonNode->eventListener = [](int trackIndex, spEvent* event) {
//        CCLog("%d event: %s, %d, %f, %s", trackIndex, event->data->name, event->intValue, event->floatValue, event->stringValue);
//    };

    skeletonNode->setMix("walk", "jump", 0.2f);
    skeletonNode->setMix("jump", "run", 0.2f);
    skeletonNode->setAnimation(0, "walk", true);
//    spTrackEntry* jumpEntry = skeletonNode->addAnimation(0, "jump", false, 3);
    skeletonNode->addAnimation(0, "run", true);


//    skeletonNode->setStartListener(jumpEntry, [](int trackIndex) {
//        CCLog("jumped!");
//        });

    // skeletonNode->addAnimation(1, "test", true);
    // skeletonNode->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1), FadeIn::create(1), DelayTime::create(5), NULL)));

    CCSize windowSize = CCDirector::sharedDirector()->getWinSize();
    skeletonNode->setPosition(ccp(windowSize.width / 2 - 100, 20));
    addChild(skeletonNode);
//    skeletonNode->setOpacity(127);
//    skeletonNode->setColor(ccc3(255, 0, 0));
    
    // test 2
    {
        auto skeletonNode = spine::SkeletonAnimation::createWithFile("spine37/goblins.json",
            "spine37/goblins.atlas", 1.5f);
        skeletonNode->setAnimation(0, "walk", true);
        skeletonNode->setSkin("goblin");

        CCSize windowSize = CCDirector::sharedDirector()->getWinSize();
        skeletonNode->setPosition(ccp(windowSize.width / 2 + 200, 20));
        addChild(skeletonNode);
//        skeletonNode->setColor(ccc3(255, 0, 255));
    }
    
    CCLayerColor *colorLayer = CCLayerColor::create(ccc4(255, 255, 0, 170), 800, 300);
    colorLayer->setPosition(100, 50);
    addChild(colorLayer);
    
    // test 3
    {
        auto skeletonNode = spine::SkeletonAnimation::createWithBinaryFile("spine37/tank.skel",
            "spine37/tank.atlas", 0.2);
        skeletonNode->setAnimation(0, "drive", true);
//        skeletonNode->setSkin("goblin");

        CCSize windowSize = CCDirector::sharedDirector()->getWinSize();
        skeletonNode->setPosition(ccp(windowSize.width / 2 + 200, 20));
        addChild(skeletonNode);
    }
    
    // test 4
    {
        auto skeletonNode = spine::SkeletonAnimation::createWithBinaryFile("spine37/coin.skel",
            "spine37/coin.atlas", 0.5);
        skeletonNode->setAnimation(0, "animation", true);

        CCSize windowSize = CCDirector::sharedDirector()->getWinSize();
        skeletonNode->setPosition(ccp(windowSize.width / 2 + 200, 100));
        colorLayer->addChild(skeletonNode);
    }
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    auto layer = CCLayerColor::create(ccc4(255, 255, 255, 255), visibleSize.width, visibleSize.height);
    addChild(layer);

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback));

	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Arial", 24);

    // position the label on the center of the screen
    pLabel->setPosition(ccp(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - pLabel->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(pLabel, 1);

    // add "HelloWorld" splash screen"
    CCSprite* pSprite = CCSprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    pSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);

    //shader_setGray(pSprite);

    testSpine();

    //testRenderTexture();

    return true;
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    exit(0);
#endif
#endif
}

void HelloWorld::testRenderTexture()
{
    auto layer = CCLayerColor::create(ccc4(255,255,255,255), 640, 960);
    addChild(layer);

    auto rect = CCSprite::create("100x50.png");
    rect->setPosition(ccp(100, 100));
    layer->addChild(rect);

    auto size = rect->getContentSize();
    auto rt = CCRenderTexture::create(size.width, size.height);
    layer->addChild(rt);
    rt->setPosition(500, 200);
    rt->getSprite()->setColor(ccc3(255, 0, 0));

    rt->beginWithClear(0, 255, 0, 255);
    {
        /*
        * -------(w,h)
        * |         |
        * (0,0)---->
        */
        auto p = rect->getPosition();
        rect->setPosition(size / 2);
        rect->setRotation(90);
        rect->visit();
        rect->setPosition(p);
    }
    rt->end();

    
}
