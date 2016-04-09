#pragma execution_character_set("utf-8")
#include "NearUILayer.hpp"
#include "GameScene3D.hpp"
#include "cocostudio/CocoStudio.h"
#include "FileUtilsExtension.h"
#include <json/document.h>
#include "ModelSprite.hpp"

USING_NS_CC;

using namespace cocostudio::timeline;

NearUILayer* NearUILayer::create()
{
    NearUILayer *pRet = new(std::nothrow) NearUILayer();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

// on "init" you need to initialize your instance
bool NearUILayer::init()
{
    // 1. super init first
    if (!Layer::init()) {
        return false;
    }
    
    auto winSize = Director::getInstance()->getWinSize();
    
    bool ret = false;
    
    do {
        
        _container = CSLoader::createNode("res/control.csb");
        _container->setPosition(winSize / 2);
        this->addChild(_container);
        
        CC_BREAK_IF(!_container);
        
        CC_BREAK_IF(!addTable());
        
        CC_BREAK_IF(!addControl());
        
        CC_BREAK_IF(!addCollision());
        
        this->scheduleUpdate();
        
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [=](Touch* t, Event* e) -> bool {
            return true;
        };
        
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
        _touchListener = listener;
        
        ret = true;
        
    } while (false);
    
    
    return ret;
}

bool NearUILayer::addControl()
{
    bool ret = false;
    do {
        
        auto winSize = Director::getInstance()->getWinSize();
        auto n = _container->getChildByName("n_control");
        _n_control = n;
        CC_BREAK_IF(!_n_control);
        
        auto nbg = n->getChildByName("sp_bg_0");
        CC_BREAK_IF(!nbg);
        
        auto nbgw = nbg->getContentSize().width + 20;
        
        n->setPosition((winSize.width / 2.0) - nbgw, 0);
        
        auto btn_switch = n->getChildByName<ui::Text*>("btn_switch");
        CC_BREAK_IF(!btn_switch);
        
        btn_switch->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            if (ui::Widget::TouchEventType::ENDED == t) {
                auto pos = n->getPosition();
                if (pos.x >= (winSize.width / 2.0) - 5) {
                    n->setPositionX((winSize.width / 2.0) - nbgw);
                }
                else {
                    n->setPositionX((winSize.width / 2.0));
                }
            }
        });
        
        _tf_scale = n->getChildByName<ui::TextField*>("tf_scale");
        CC_BREAK_IF(!_tf_scale);
        
        _frame_count = n->getChildByName<ui::Text*>("frame_count");
        CC_BREAK_IF(!_frame_count);
        
        _start_frame = n->getChildByName<ui::TextField*>("start_frame");
        CC_BREAK_IF(!_start_frame);
        
        _end_frame = n->getChildByName<ui::TextField*>("end_frame");
        CC_BREAK_IF(!_end_frame);
        
        _btn_list = n->getChildByName<ui::Button*>("btn_list");
        CC_BREAK_IF(!_btn_list);
        
        _btn_list->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            if (ui::Widget::TouchEventType::ENDED == t) {
                if (_tableBg->isVisible()) {
                    _tableBg->setVisible(false);
                    _btn_list->setTitleText("显示列表");
                }
                else {
                    _tableBg->setVisible(true);
                    _btn_list->setTitleText("隐藏列表");
                    if (_modleSprite) {
                        _modleSprite->removeFromParent();
                        _modleSprite = nullptr;
                    }
#ifdef WIN32
                    auto path = FileUtilsExtension::res_path() + "\\model";
#else
                    auto path = FileUtils::getInstance()->fullPathForFilename("model");
#endif
                    _pathList = FileUtilsExtension::content_of_folder(path, "c3b");
                    _selectedIdx = -1;
                    
                    _fileTable->reloadData();
                    
                    if (_sphereList.size()) {
                        for (int i = 0; i < _sphereList.size(); i++) {
                            auto sn = _sphereList.at(i);
                            sn->removeFromParent();
                        }
                        _sphereList.clear();
                    }
                }
            }
        });
        
        auto btn_scale = n->getChildByName<ui::Button*>("btn_scale");
        CC_BREAK_IF(!btn_scale);
        
        btn_scale->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            if (ui::Widget::TouchEventType::ENDED == t) {
                float s = atof(_tf_scale->getString().c_str());
                if (s > 0 && _modleSprite) {
                    _modleSprite->setScale(s);
                }
            }
        });
        
        auto btn_bg = n->getChildByName<ui::Button*>("btn_bg");
        CC_BREAK_IF(!btn_bg);
        
        btn_bg->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            if (ui::Widget::TouchEventType::ENDED == t) {
                static int bg_id = 0;
                
                auto on = GameScene3D::getInstance()->getOriginBg();
                auto bg_color = dynamic_cast<ui::ImageView*>(on->getChildByTag(1));
                
                if (!bg_color) {
                    return;
                }
                
                switch (bg_id) {
                    case 0:
                    {
                        bg_id = 1;
                        bg_color->loadTexture("res/img/white.png");
                    }
                        break;
                    case 1:
                    {
                        bg_id = 2;
                        bg_color->loadTexture("res/img/gray.png");
                    }
                        break;
                    default:
                    {
                        bg_id = 0;
                        bg_color->loadTexture("res/img/black.png");
                    }
                        break;
                }
            }
        });
        
        auto btn_play = n->getChildByName<ui::Button*>("btn_play");
        CC_BREAK_IF(!btn_play);
        
        btn_play->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            if (ui::Widget::TouchEventType::ENDED == t) {
                if (_path.length()) {
                    this->updateModel();
                    _modleSprite->stopAllActions();
                    _modleSprite->playAnimate(atoi(_start_frame->getString().c_str()),
                                              atoi(_end_frame->getString().c_str()));
                }
            }
        });
        
        auto btn_repeat = n->getChildByName<ui::Button*>("btn_repeat");
        CC_BREAK_IF(!btn_repeat);
        
        btn_repeat->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            if (ui::Widget::TouchEventType::ENDED == t) {
                if (_path.length()) {
                    this->updateModel();
                    _modleSprite->stopAllActions();
                    _modleSprite->playAnimate(atoi(_start_frame->getString().c_str()),
                                              atoi(_end_frame->getString().c_str()),
                                              true);
                }
            }
        });
        
        auto btn_pause = n->getChildByName<ui::Button*>("btn_pause");
        CC_BREAK_IF(!btn_pause);
        
        btn_pause->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            if (ui::Widget::TouchEventType::ENDED == t) {
                _modleSprite->stopAllActions();
            }
        });
        
        auto btn_up = n->getChildByName<ui::Button*>("btn_up");
        CC_BREAK_IF(!btn_up);
        
        btn_up->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            switch (t) {
                case ui::Widget::TouchEventType::BEGAN:
                case ui::Widget::TouchEventType::MOVED:
                {
                    _d_rotate = DGo::DUp;
                }
                    break;
                case ui::Widget::TouchEventType::ENDED:
                case ui::Widget::TouchEventType::CANCELED:
                {
                    _d_rotate = DGo::DNone;
                }
                    break;
                default:
                    break;
            }
        });
        
        auto btn_down = n->getChildByName<ui::Button*>("btn_down");
        CC_BREAK_IF(!btn_down);
        
        btn_down->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            switch (t) {
                case ui::Widget::TouchEventType::BEGAN:
                case ui::Widget::TouchEventType::MOVED:
                {
                    _d_rotate = DGo::DDown;
                }
                    break;
                case ui::Widget::TouchEventType::ENDED:
                case ui::Widget::TouchEventType::CANCELED:
                {
                    _d_rotate = DGo::DNone;
                }
                    break;
                default:
                    break;
            }
        });
        
        auto btn_left = n->getChildByName<ui::Button*>("btn_left");
        CC_BREAK_IF(!btn_left);
        
        btn_left->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            switch (t) {
                case ui::Widget::TouchEventType::BEGAN:
                case ui::Widget::TouchEventType::MOVED:
                {
                    _d_rotate = DGo::DLeft;
                }
                    break;
                case ui::Widget::TouchEventType::ENDED:
                case ui::Widget::TouchEventType::CANCELED:
                {
                    _d_rotate = DGo::DNone;
                }
                    break;
                default:
                    break;
            }
        });
        
        auto btn_right = n->getChildByName<ui::Button*>("btn_right");
        CC_BREAK_IF(!btn_right);
        
        btn_right->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            switch (t) {
                case ui::Widget::TouchEventType::BEGAN:
                case ui::Widget::TouchEventType::MOVED:
                {
                    _d_rotate = DGo::DRight;
                }
                    break;
                case ui::Widget::TouchEventType::ENDED:
                case ui::Widget::TouchEventType::CANCELED:
                {
                    _d_rotate = DGo::DNone;
                }
                    break;
                default:
                    break;
            }
        });
        
        /////////////////

        auto btn_up_m = n->getChildByName<ui::Button*>("btn_up_m");
        CC_BREAK_IF(!btn_up_m);
        
        btn_up_m->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            switch (t) {
                case ui::Widget::TouchEventType::BEGAN:
                case ui::Widget::TouchEventType::MOVED:
                {
                    _d_move = DGo::DUp;
                }
                    break;
                case ui::Widget::TouchEventType::ENDED:
                case ui::Widget::TouchEventType::CANCELED:
                {
                    _d_move = DGo::DNone;
                }
                    break;
                default:
                    break;
            }
        });
        
        auto btn_down_m = n->getChildByName<ui::Button*>("btn_down_m");
        CC_BREAK_IF(!btn_down_m);
        
        btn_down_m->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            switch (t) {
                case ui::Widget::TouchEventType::BEGAN:
                case ui::Widget::TouchEventType::MOVED:
                {
                    _d_move = DGo::DDown;
                }
                    break;
                case ui::Widget::TouchEventType::ENDED:
                case ui::Widget::TouchEventType::CANCELED:
                {
                    _d_move = DGo::DNone;
                }
                    break;
                default:
                    break;
            }
        });
        
        auto btn_left_m = n->getChildByName<ui::Button*>("btn_left_m");
        CC_BREAK_IF(!btn_left_m);
        
        btn_left_m->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            switch (t) {
                case ui::Widget::TouchEventType::BEGAN:
                case ui::Widget::TouchEventType::MOVED:
                {
                    _d_move = DGo::DLeft;
                }
                    break;
                case ui::Widget::TouchEventType::ENDED:
                case ui::Widget::TouchEventType::CANCELED:
                {
                    _d_move = DGo::DNone;
                }
                    break;
                default:
                    break;
            }
        });
        
        auto btn_right_m = n->getChildByName<ui::Button*>("btn_right_m");
        CC_BREAK_IF(!btn_right_m);
        
        btn_right_m->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            switch (t) {
                case ui::Widget::TouchEventType::BEGAN:
                case ui::Widget::TouchEventType::MOVED:
                {
                    _d_move = DGo::DRight;
                }
                    break;
                case ui::Widget::TouchEventType::ENDED:
                case ui::Widget::TouchEventType::CANCELED:
                {
                    _d_move = DGo::DNone;
                }
                    break;
                default:
                    break;
            }
        });
        
        auto btn_reset = n->getChildByName<ui::Button*>("btn_reset");
        CC_BREAK_IF(!btn_reset);
        
        btn_reset->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            if (ui::Widget::TouchEventType::ENDED == t) {
                
                auto aabb = _modleSprite->getOriAABB();
                auto w = aabb._min.distance(aabb._max);
                auto winSize = Director::getInstance()->getWinSize();
                auto camera_pos = GameScene3D::getInstance()->getDefaultCamera()->getPosition3D();
                _modleSprite->setScale(winSize.height * 0.8 / w);
                
                _tf_scale->setString(StringUtils::format("%0.2f", _modleSprite->getScale()));
                
                _modleSprite->setPosition3D(Vec3(camera_pos.x, camera_pos.y, camera_pos.z * 0.4));
                _modleSprite->setRotation3D(cocos2d::Vec3::ZERO);
            }
        });
        
        _speed_r = n->getChildByName<ui::TextField*>("speed");
        CC_BREAK_IF(!_speed_r);
        
        _speed_m = n->getChildByName<ui::TextField*>("speed_m");
        CC_BREAK_IF(!_speed_m);
        
        ret = true;
        
    } while (false);
    
    return ret;
}

void NearUILayer::loadCollisionList()
{
    if (!_path.length()) {
        return;
    }
    
    if (!_modleSprite) {
        return;
    }
    
    auto json_path = FileUtilsExtension::string_by_delete_path_extension(_path) + ".json";
    auto jstr = FileUtils::getInstance()->getStringFromFile(json_path);
    
    if (!jstr.length()) {
        return;
    }
    /*
    [
     {
     "x":0,
     "y":0,
     "z":0,
     "r":0
     }
    ]
    */
    rapidjson::Document doc;
    doc.Parse<0>(jstr.c_str());
    
    if (doc.HasParseError()) {
        return;
    }
    
    std::vector<ModelCollisionSphere> collisionSpheres;
    if (doc.IsArray()) {
        
        for (auto i = 0; i < doc.Size(); i++) {
            rapidjson::Value& jo = doc[i];
            ModelCollisionSphere mcs;
            
            if (jo.HasMember("x") && jo["x"].IsNumber()) {
                mcs.pos.x = jo["x"].GetDouble();
            }
            else {
                continue;
            }
            
            if (jo.HasMember("y") && jo["y"].IsNumber()) {
                mcs.pos.y = jo["y"].GetDouble();
            }
            else {
                continue;
            }
            
            if (jo.HasMember("z") && jo["z"].IsNumber()) {
                mcs.pos.z = jo["z"].GetDouble();
            }
            else {
                continue;
            }
            
            if (jo.HasMember("r") && jo["r"].IsNumber()) {
                mcs.radius = jo["r"].GetDouble();
            }
            else {
                continue;
            }
            
            collisionSpheres.push_back(mcs);
        }
        
        if (collisionSpheres.size()) {
            _modleSprite->setCollisionSphere(collisionSpheres);
        }
    }
    
    for (auto n : _sphereList) {
        n->removeFromParent();
    }
    _sphereList.clear();
    
    for (auto i = 0; i < collisionSpheres.size(); i++) {
        auto mcs = collisionSpheres.at(i);
        auto line = CSLoader::createNode("res/line.csb");
        CC_BREAK_IF(!line);
        
        line->setPosition(Vec2(-50, _line_top - (_sphereList.size() * _line_step)));
        _n_spheres->addChild(line);
        
        auto tf_x = line->getChildByName<ui::TextField*>("tf_x");
        CC_BREAK_IF(!tf_x);
        
        auto tf_y = line->getChildByName<ui::TextField*>("tf_y");
        CC_BREAK_IF(!tf_y);
        
        auto tf_z = line->getChildByName<ui::TextField*>("tf_z");
        CC_BREAK_IF(!tf_z);
        
        auto tf_r = line->getChildByName<ui::TextField*>("tf_r");
        CC_BREAK_IF(!tf_r);
        
        auto btn_delete = line->getChildByName<ui::Button*>("btn_delete");
        CC_BREAK_IF(!btn_delete);
        
        tf_x->setString(StringUtils::format("%.2f", mcs.pos.x));
        tf_y->setString(StringUtils::format("%.2f", mcs.pos.y));
        tf_z->setString(StringUtils::format("%.2f", mcs.pos.z));
        tf_r->setString(StringUtils::format("%.2f", mcs.radius));
        
        btn_delete->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            if (ui::Widget::TouchEventType::ENDED == t) {
                _sphereList.at(i)->removeFromParent();
                _sphereList.erase(i);
            }
        });
        
        _sphereList.pushBack(line);
    }
}

void NearUILayer::saveCollisionList()
{
    if (!_path.length()) {
        return;
    }
    
    if (!_modleSprite) {
        return;
    }
    
    auto json_path = FileUtilsExtension::string_by_delete_path_extension(_path) + ".json";

    /*
     [
     {
     "x":0,
     "y":0,
     "z":0,
     "r":0
     }
     ]
     */
    
    std::string all = "";
    all += "[";
    auto collisionSpheres = _modleSprite->getCollisionSphere();
    
    for (auto i = 0; i < collisionSpheres.size(); i++) {
        auto mcs = collisionSpheres.at(i);
        std::string tmp = "{";
        tmp += StringUtils::format("\"x\":%.2f", mcs.pos.x) + ",";
        tmp += StringUtils::format("\"y\":%.2f", mcs.pos.y) + ",";
        tmp += StringUtils::format("\"z\":%.2f", mcs.pos.z) + ",";
        tmp += StringUtils::format("\"r\":%.2f", mcs.radius);
        
        tmp += "}";
        
        if (i != collisionSpheres.size() - 1) {
            all += tmp + ",";
        }
        else {
            all += tmp;
        }
    }
    all += "]";
    
    FileUtils::getInstance()->writeStringToFile(all, json_path);
}

bool NearUILayer::addCollision()
{
    bool ret = false;
    do {
        _n_spheres = _n_control->getChildByName("n_spheres");
        CC_BREAK_IF(!_n_spheres);
        
        auto n = _n_spheres;
        n->setVisible(false);
        
        auto nbg = n->getChildByName("sp_bg");
        CC_BREAK_IF(!nbg);
        
        auto nSize = nbg->getContentSize();
        
        auto btn_collision = _n_control->getChildByName<ui::Button*>("btn_collision");
        CC_BREAK_IF(!btn_collision);
        
        btn_collision->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            if (ui::Widget::TouchEventType::ENDED == t) {
                if (_modleSprite && _path.length()) {
                    _n_spheres->setVisible(true);
                }
            }
        });
        
        auto btn_close = n->getChildByName<ui::Button*>("btn_close");
        CC_BREAK_IF(!btn_close);
        
        btn_close->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            if (ui::Widget::TouchEventType::ENDED == t) {
                _n_spheres->setVisible(false);
            }
        });
        
        //
        auto btn_add = n->getChildByName<ui::Button*>("btn_add");
        CC_BREAK_IF(!btn_add);

        _line_step = 40;
        _line_top = btn_add->getPosition().y - (btn_add->getContentSize().height / 2) - _line_step;
        
        btn_add->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            if (ui::Widget::TouchEventType::ENDED == t) {

                auto line = CSLoader::createNode("res/line.csb");

                line->setPosition(Vec2(-50, _line_top - (_sphereList.size() * _line_step)));
                _n_spheres->addChild(line);
                int i = _sphereList.size();
                auto btn_delete = line->getChildByName<ui::Button *>("btn_delete");
                btn_delete->addTouchEventListener([=](Ref *r, ui::Widget::TouchEventType t) {
                    if (ui::Widget::TouchEventType::ENDED == t) {
                        _sphereList.at(i)->removeFromParent();
                        _sphereList.erase(i);
                    }
                });

                _sphereList.pushBack(line);
            }
        });
        
        auto btn_load = n->getChildByName<ui::Button*>("btn_load");
        CC_BREAK_IF(!btn_load);
        
        btn_load->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            if (ui::Widget::TouchEventType::ENDED == t) {
                this->loadCollisionList();
            }
        });
        
        
        auto btn_save = n->getChildByName<ui::Button*>("btn_save");
        CC_BREAK_IF(!btn_save);
        
        btn_save->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            if (ui::Widget::TouchEventType::ENDED == t) {
                this->saveCollisionList();
            }
        });
        
        auto btn_refresh = n->getChildByName<ui::Button*>("btn_refresh");
        CC_BREAK_IF(!btn_refresh);
        
        btn_refresh->addTouchEventListener([=](Ref* r, ui::Widget::TouchEventType t) {
            if (ui::Widget::TouchEventType::ENDED == t) {
                
                std::vector<ModelCollisionSphere> collisionSpheres;
                for (auto i = 0; i < _sphereList.size(); i++) {
                    auto line = _sphereList.at(i);
                    ModelCollisionSphere mcs;
                    
                    auto tf_x = line->getChildByName<ui::TextField*>("tf_x");
                    CC_BREAK_IF(!tf_x);
                    
                    auto tf_y = line->getChildByName<ui::TextField*>("tf_y");
                    CC_BREAK_IF(!tf_y);
                    
                    auto tf_z = line->getChildByName<ui::TextField*>("tf_z");
                    CC_BREAK_IF(!tf_z);
                    
                    auto tf_r = line->getChildByName<ui::TextField*>("tf_r");
                    CC_BREAK_IF(!tf_r);
                    
                    mcs.pos.x = atof(tf_x->getString().c_str());
                    mcs.pos.y = atof(tf_y->getString().c_str());
                    mcs.pos.z = atof(tf_z->getString().c_str());
                    mcs.radius = atof(tf_r->getString().c_str());
                    
                    collisionSpheres.push_back(mcs);
                }
                
                if (collisionSpheres.size()) {
                    _modleSprite->setCollisionSphere(collisionSpheres);
                }
            }
        });
        
        ret = true;
        
    } while (false);
    return ret;
}

void NearUILayer::updateModel()
{
    if (_path.length()) {
        _tableBg->setVisible(false);
        _btn_list->setTitleText("显示列表");
        
        auto texture_path = FileUtilsExtension::string_by_delete_path_extension(_path) + ".png";
        
        if (_modleSprite) {
            if (_modleSprite->get_c3b() == _path && _modleSprite->get_png() == texture_path) {
                // pass
            }
            else {
                _modleSprite->removeFromParent();
                _modleSprite = nullptr;
            }
        }
        
        if (!_modleSprite) {
            _modleSprite = ModelSprite::create(_path, texture_path);
            
            auto aabb = _modleSprite->getOriAABB();
            auto w = aabb._min.distance(aabb._max);
            auto winSize = Director::getInstance()->getWinSize();
            auto camera_pos = GameScene3D::getInstance()->getDefaultCamera()->getPosition3D();
            _modleSprite->setScale(winSize.height * 0.8 / w);
            
            _tf_scale->setString(StringUtils::format("%0.2f", _modleSprite->getScale()));
            
            _modleSprite->setPosition3D(Vec3(camera_pos.x, camera_pos.y, camera_pos.z * 0.4));
            
            GameScene3D::getInstance()->addChild(_modleSprite);
        }
        
        auto animation = Animation3D::create(_path);
        if (animation) {
            _frame_count->setString(StringUtils::toString(round(animation->getDuration() * 30.0)));
            _start_frame->setString(StringUtils::toString(0));
            _end_frame->setString(StringUtils::toString(round(animation->getDuration() * 30.0)));
        }
        
        CCLOG("%f %f %f", _modleSprite->getPosition3D().x, _modleSprite->getPosition3D().y,
              _modleSprite->getPosition3D().z);
    }
}

bool NearUILayer::addTable()
{
    bool ret = false;
    do {
        _tableBg = _container->getChildByName<Node*>("table_list");
        CC_BREAK_IF(!_tableBg);
        
        auto tb_bg = _tableBg->getChildByName<Node*>("Image_3");
        CC_BREAK_IF(!tb_bg);
        
        auto size = tb_bg->getContentSize();
        
        _fileTable = TableView::create(this, size);
        _fileTable->setDelegate(this);
        _fileTable->setLocalZOrder(2);
        _fileTable->setVerticalFillOrder(cocos2d::extension::TableView::VerticalFillOrder::TOP_DOWN);
        _tableBg->addChild(_fileTable);
        
#ifdef WIN32
        auto path = FileUtilsExtension::res_path() + "\\model";
#else
        auto path = FileUtils::getInstance()->fullPathForFilename("model");
#endif
        _pathList = FileUtilsExtension::content_of_folder(path, "c3b");
        _selectedIdx = -1;
        
        _fileTable->reloadData();

        auto mouseListener = EventListenerMouse::create();
        mouseListener->onMouseScroll = [=] (EventMouse* event) {
            auto mouseEvent = static_cast<EventMouse*>(event);
            float moveY = mouseEvent->getScrollY() * 20;
            
            auto minOffset = _fileTable->minContainerOffset();
            auto maxOffset = _fileTable->maxContainerOffset();
            
            auto offset = _fileTable->getContentOffset();
            offset.y += moveY;
            
            if (offset.y < minOffset.y)
            {
                offset.y = minOffset.y;
            }
            else if (offset.y > maxOffset.y)
            {
                offset.y = maxOffset.y;
            }
            
            _fileTable->setContentOffset(offset);
        };
        
        _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
        
        ret = true;
        
    } while (false);
    
    return ret;
}

void NearUILayer::update(float delta)
{
    if (!_speed_r) {
        return;
    }
    
    float distance = delta * atof(_speed_r->getString().c_str());
    switch (_d_rotate) {
        case DGo::DUp:
        {
            auto v3 = _modleSprite->getRotation3D();
            _modleSprite->setRotation3D(cocos2d::Vec3(v3.x - distance, v3.y, v3.z));
        }
            break;
        case DGo::DDown:
        {
            auto v3 = _modleSprite->getRotation3D();
            _modleSprite->setRotation3D(cocos2d::Vec3(v3.x + distance, v3.y, v3.z));
        }
            break;
        case DGo::DLeft:
        {
            auto v3 = _modleSprite->getRotation3D();
            _modleSprite->setRotation3D(cocos2d::Vec3(v3.x, v3.y - distance, v3.z));
        }
            break;
        case DGo::DRight:
        {
            auto v3 = _modleSprite->getRotation3D();
            _modleSprite->setRotation3D(cocos2d::Vec3(v3.x, v3.y + distance, v3.z));
        }
            break;
        default:
            break;
    }
    
    if (!_speed_m) {
        return;
    }
    
    float distance_m = delta * atof(_speed_m->getString().c_str());
    switch (_d_move) {
        case DGo::DUp:
        {
            auto v3 = _modleSprite->getPosition3D();
            _modleSprite->setPosition3D(Vec3(v3.x, v3.y + distance_m, v3.z));
        }
            break;
        case DGo::DDown:
        {
            auto v3 = _modleSprite->getPosition3D();
            _modleSprite->setPosition3D(Vec3(v3.x, v3.y - distance_m, v3.z));
        }
            break;
        case DGo::DLeft:
        {
            auto v3 = _modleSprite->getPosition3D();
            _modleSprite->setPosition3D(Vec3(v3.x - distance_m, v3.y, v3.z));
        }
            break;
        case DGo::DRight:
        {
            auto v3 = _modleSprite->getPosition3D();
            _modleSprite->setPosition3D(Vec3(v3.x + distance_m, v3.y, v3.z));
        }
            break;
        default:
            break;
    }
}

void NearUILayer::onExit()
{
    Layer::onExit();
    //_eventDispatcher->removeEventListener(_touchListener);
    //_touchListener = nullptr;
    //if (_modleSprite) {
    //    _modleSprite->removeFromParentAndCleanup(true);
    //}
    
    Sprite3DCache::getInstance()->removeAllSprite3DData();
    Director::getInstance()->getTextureCache()->removeAllTextures();
}

//////////////////////////////// table
Size NearUILayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    return cellSizeForTable(table);
};

Size NearUILayer::cellSizeForTable(TableView *table)
{
    return Size(0, 20);
};

TableViewCell* NearUILayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    Label* name = nullptr;
    Label* path = nullptr;
    ui::ImageView* bg = nullptr;
    
    TableViewCell* cell = table->dequeueCell();
    if (!cell) {
        cell = TableViewCell::create();
        
        name = Label::createWithSystemFont("", "", 20);
        path = Label::createWithSystemFont("", "", 20);
        
        name->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        path->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        
        name->setPosition(Point(30, 10));
        path->setPosition(Point(200, 10));
        
        cell->addChild(name, 2, 2);
        cell->addChild(path, 3, 3);
        
        bg = ui::ImageView::create("res/img/white.png");
        bg->setScale9Enabled(true);
        bg->setContentSize(Size(table->getBoundingBox().size.width, 20));
        bg->setPosition(Point(table->getBoundingBox().size.width / 2, 10));
        cell->addChild(bg, 1, 1);
    }
    else {
        bg = dynamic_cast<ui::ImageView*>(cell->getChildByTag(1));
        name = dynamic_cast<Label*>(cell->getChildByTag(2));
        path = dynamic_cast<Label*>(cell->getChildByTag(3));
    }
    
    if (idx == _selectedIdx) {
        name->setColor(cocos2d::Color3B::BLACK);
        path->setColor(cocos2d::Color3B::BLACK);
        bg->setVisible(true);
    }
    else {
        name->setColor(cocos2d::Color3B::WHITE);
        path->setColor(cocos2d::Color3B::WHITE);
        bg->setVisible(false);
    }
    
    name->setString(FileUtilsExtension::file_name_for_path(_pathList[idx]));
    auto p = _pathList[idx];
    auto s = p.substr(FileUtilsExtension::res_path().length(), p.length() - FileUtilsExtension::res_path().length());
    path->setString(s);
    
    return cell;
}

ssize_t NearUILayer::numberOfCellsInTableView(TableView *table)
{
    return _pathList.size();
}

// TableViewDelegate
void NearUILayer::tableCellTouched(TableView* table, TableViewCell* cell)
{
    auto old = _selectedIdx;
    _selectedIdx = cell->getIdx();
    table->updateCellAtIndex(old);
    table->updateCellAtIndex(_selectedIdx);
    
#ifdef WIN32
    _path = _pathList[_selectedIdx];
#else
    _path = FileUtils::getInstance()->fullPathForFilename("model/" + _pathList[_selectedIdx]);
#endif

}

void NearUILayer::tableCellHighlight(TableView* table, TableViewCell* cell)
{
    cell->getChildByTag(1)->setVisible(true);
}

void NearUILayer::tableCellUnhighlight(TableView* table, TableViewCell* cell)
{
    cell->getChildByTag(1)->setVisible(false);
}

void NearUILayer::tableCellWillRecycle(TableView* table, TableViewCell* cell)
{
    
}