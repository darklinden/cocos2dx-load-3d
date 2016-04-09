#pragma execution_character_set("utf-8")
//
//  GameScene3D.hpp
//  fish_jinchan
//
//  Created by HanShaokun on 17/3/2016.
//
//

#ifndef GameScene3D_hpp
#define GameScene3D_hpp

#define CPP_GAME_SCENE_DEBUG 1

#include "cocos2d.h"
#include "NearUILayer.hpp"

using namespace cocos2d;

class GameScene3D : public cocos2d::Scene {
public:
    
    GameScene3D();
    ~GameScene3D();
    
    static GameScene3D* getInstance();
    static void destroyInstance();
    
    NearUILayer* getUILayer() { return _front_ui; };
    
    // 自定义背景 目前处于z-106位置
    Sprite3D* getOriginNode() { return _origin_node; };
    Node* getOriginBg() { return _origin_bg; };
    
    virtual bool init();
    
    void setLifeCircleCallback(std::function<void(void)> enter, std::function<void(void)> exit) {
        _life_on_enter = enter;
        _life_on_exit = exit;
    };
    
    virtual void onEnter();
    virtual void onExit();
    
private:
    
    static GameScene3D* _instance;

    // add 2d ui layer
    NearUILayer* _front_ui = nullptr;
    
    // add 3d node to hold 2d bg sprite
    Sprite3D* _origin_node = nullptr;
    Node* _origin_bg = nullptr;
    
    std::function<void(void)> _life_on_enter = nullptr;
    std::function<void(void)> _life_on_exit = nullptr;
};

#endif /* GameScene3D_hpp */
