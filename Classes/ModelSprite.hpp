#pragma execution_character_set("utf-8")
//
//  ModelSprite.hpp
//  fish_jinchan
//
//  Created by HanShaokun on 18/3/2016.
//
//

#ifndef ModelSprite_hpp
#define ModelSprite_hpp

#include "cocos2d.h"
#include "DrawNode3D.h"

using namespace cocos2d;

typedef struct {
    Vec3  pos;
    float radius;
} ModelCollisionSphere;

class ModelSprite :public cocos2d::Sprite3D {
public:
    
    static ModelSprite* create(const std::string& c3b, const std::string& png)
    {
        auto sprite = new (std::nothrow) ModelSprite();
        if (sprite && sprite->initWithFile(c3b))
        {
            sprite->setTexture(png);
            sprite->_c3b = c3b;
            sprite->_png = png;
            sprite->_aabbtOri = sprite->getAABB();
            sprite->_obbtOri = OBB(sprite->_aabbtOri);
            sprite->autorelease();
        }
        else {
            CC_SAFE_DELETE(sprite);
        }
        
        return sprite;
    }
    
    virtual void onEnter();
    virtual void onExit();
    virtual void update(float delta);
    
    CC_SYNTHESIZE(std::string, _c3b, _c3b);
    CC_SYNTHESIZE(std::string, _png, _png);
    
    void playAnimate(int startFrame, int endFrame, bool repeat = false);
    
    void setOriAABB(const cocos2d::AABB &aabb) { _aabbtOri = aabb; };
    cocos2d::AABB getOriAABB() { return _aabbtOri; }
    
    void setOriOBB(const cocos2d::OBB &obb) { _obbtOri = obb; };
    cocos2d::OBB getOriOBB() { return _obbtOri; }
    
    std::vector<ModelCollisionSphere> getCollisionSphere() { return _collision_spheres; };
    void setCollisionSphere(const std::vector<ModelCollisionSphere> &collisionSphere) { _collision_spheres = collisionSphere; };
    
private:
    
    std::vector<ModelCollisionSphere> _collision_spheres;
    
    cocos2d::OBB _obbtOri;
    
    cocos2d::AABB _aabbtOri;
    
    cocos2d::DrawNode3D* _debugDraw = nullptr;
    
    
};

#endif /* ModelSprite_hpp */
