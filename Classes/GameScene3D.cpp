#pragma execution_character_set("utf-8")
//
//  GameScene3D.cpp
//  fish_jinchan
//
//  Created by HanShaokun on 17/3/2016.
//
//

#include "GameScene3D.hpp"
#include "ui/CocosGUI.h"

GameScene3D::GameScene3D()
{
    
}

GameScene3D::~GameScene3D()
{
    
}

GameScene3D* GameScene3D::_instance = nullptr;
GameScene3D* GameScene3D::getInstance()
{
    if (!_instance) {
        _instance = new GameScene3D();
        if (!_instance->init()) {
            _instance->release();
            _instance = nullptr;
        }
    }
    return _instance;
}

void GameScene3D::destroyInstance()
{
    if (_instance) {
        _instance->release();
        _instance = nullptr;
    }
    //Sprite3DCache::getInstance()->removeAllSprite3DData();
    //Director::getInstance()->getTextureCache()->removeUnusedTextures();
    //Animation3DCache::getInstance()->removeAllAnimations();
    //AnimationCache::destroyInstance();
}

bool GameScene3D::init()
{
    bool ret = false;
    do {
        CC_BREAK_IF(!Scene::init());
        
        auto camera = this->getDefaultCamera();
        CCLOG("********************************");
        CCLOG("GameScene3D::init camera position x: %f y: %f z: %f", camera->getPositionX(), camera->getPositionY(), camera->getPositionZ());
        CCLOG("********************************");
        
        auto winSize = Director::getInstance()->getWinSize();
        
        // add ui layer
        _front_ui = NearUILayer::create();
        _front_ui->setAnchorPoint(Point::ANCHOR_MIDDLE);
        this->addChild(_front_ui);
        
        // add bg sprite
        _origin_node = Sprite3D::create();
        _origin_node->setPosition3D(Vec3(0, 0, 0));
        this->addChild(_origin_node);
        
        _origin_bg = Node::create();
        _origin_bg->setContentSize(winSize);
        _origin_bg->setPosition(Point(winSize.width / 2, winSize.height / 2));
        _origin_node->addChild(_origin_bg);
        
        auto bg_color = ui::ImageView::create("res/img/black.png");
        bg_color->setScale9Enabled(true);
        bg_color->setContentSize(winSize);
        _origin_bg->addChild(bg_color, -1, 1);

//        auto center_icon = Sprite::create("res/bg.png");
//        _origin_bg->addChild(center_icon);
        
        /*
         auto sprite = Sprite3D::create(fileName);
         sprite->setScale(0.1f);
         auto s = Director::getInstance()->getWinSize();
         sprite->setPosition(Vec2(s.width * 4.f / 5.f, s.height / 2.f));
         addChild(sprite);
         _sprite = sprite;
         auto animation = Animation3D::create(fileName);
         if (animation)
         {
         auto animate = Animate3D::create(animation, 0.f, 1.933f);
         _swim = RepeatForever::create(animate);
         sprite->runAction(_swim);
         
         _swim->retain();
         _hurt = Animate3D::create(animation, 1.933f, 2.8f);
         _hurt->retain();
         _state = State::SWIMMING;
         }
         
         */
        
        ret = true;
        
    } while (0);

    return ret;
}

void GameScene3D::onEnter()
{
    Scene::onEnter();
    if (_life_on_enter) {
        _life_on_enter();
    }
}

void GameScene3D::onExit()
{
    Scene::onExit();
    if (_life_on_exit) {
        _life_on_exit();
    }
}

