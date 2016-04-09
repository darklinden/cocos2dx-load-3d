#pragma execution_character_set("utf-8")
//
//  ModelSprite.cpp
//  fish_jinchan
//
//  Created by HanShaokun on 18/3/2016.
//
//

#include "ModelSprite.hpp"

void ModelSprite::onEnter()
{
    Sprite3D::onEnter();
    this->scheduleUpdate();
    
    _debugDraw = DrawNode3D::create();
    this->addChild(_debugDraw);
}

void ModelSprite::onExit()
{
    Sprite3D::onExit();
    this->unscheduleUpdate();
    
    if (_debugDraw) {
        _debugDraw->removeFromParent();
        _debugDraw = nullptr;
    }
}

void ModelSprite::update(float delta)
{
    if (_debugDraw) {
        _debugDraw->clear();
        
        for (auto i = 0; i < _collision_spheres.size(); i++) {
            auto b = _collision_spheres[i];
            
            if (b.radius) {
                _debugDraw->drawSphere(b.pos, b.radius, Color4F(1, 0, 0, 1));
            }
        }
    }
}

void ModelSprite::playAnimate(int startFrame, int endFrame, bool repeat)
{
    this->stopAllActions();

    auto animation = Animation3D::create(_c3b);
    if (animation) {
        auto animate = Animate3D::createWithFrames(animation, startFrame, endFrame);

        // speed up animation
        auto duration = animate->getDuration();
        animate->setDuration(duration / 2);

        if (repeat) {
            this->runAction(RepeatForever::create(animate));
        }
        else {
            this->runAction(animate);
        }
    }
}