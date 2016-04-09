#ifndef __NearUILayer_SCENE_H__
#define __NearUILayer_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocos-ext.h"
#include "ModelSprite.hpp"

using namespace cocos2d;
using namespace cocos2d::extension;

class NearUILayer : public cocos2d::Layer,
public cocos2d::extension::TableViewDataSource,
public cocos2d::extension::TableViewDelegate
{
public:
    
    NearUILayer() {};
    ~NearUILayer() {};
    virtual bool init();
    
    // implement the "static create()" method manually
    static NearUILayer* create();
    
    virtual void update(float delta);
    virtual void onExit();
    
    typedef enum : int {
        DNone = 0,
        DUp = 1,
        DDown = 2,
        DLeft = 3,
        DRight = 4
    } DGo;
    
    std::string _path = "";
    DGo _d_rotate = DGo::DNone;
    DGo _d_move = DGo::DNone;
    
    cocos2d::Node* _container = nullptr;
    
    cocos2d::Node* _n_control = nullptr;
    
    cocos2d::Node* _n_spheres = nullptr;
    
    cocos2d::ui::Text* _frame_count = nullptr;
    
    cocos2d::ui::TextField* _tf_scale = nullptr;
    
    cocos2d::ui::TextField* _start_frame = nullptr;
    
    cocos2d::ui::TextField* _end_frame = nullptr;
    
    cocos2d::ui::TextField* _speed_r = nullptr;
    
    cocos2d::ui::TextField* _speed_m = nullptr;
    
    cocos2d::Node* _tableBg = nullptr;
    
    cocos2d::ui::Button* _btn_list = nullptr;
    
    ModelSprite* _modleSprite = nullptr;
    
    cocos2d::extension::TableView* _fileTable = nullptr;
    
    cocos2d::Vector<cocos2d::Node*> _sphereList;
    
    long _selectedIdx = -1;
    
    std::vector<std::string> _pathList;
    
    float _line_step = 0;
    float _line_top = 0;
    
    void loadCollisionList();
    void saveCollisionList();
    
    void updateModel();
    bool addControl();
    bool addCollision();
    bool addTable();
    
    //TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx);
    virtual Size cellSizeForTable(TableView *table);
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
    virtual ssize_t numberOfCellsInTableView(TableView *table);
    
    // TableViewDelegate
    virtual void tableCellTouched(TableView* table, TableViewCell* cell);
    virtual void tableCellHighlight(TableView* table, TableViewCell* cell);
    virtual void tableCellUnhighlight(TableView* table, TableViewCell* cell);
    virtual void tableCellWillRecycle(TableView* table, TableViewCell* cell);
    
};

#endif // __NearUILayer_SCENE_H__
