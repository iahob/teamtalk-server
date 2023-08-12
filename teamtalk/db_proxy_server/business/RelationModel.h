/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：RelationModel.h
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#ifndef RELATION_SHIP_H_
#define RELATION_SHIP_H_

#include <list>

#include "base/util.h"
#include "base/ImPduBase.h"
#include "IM.BaseDefine.pb.h"

using namespace std;


class CRelationModel {
public:
	virtual ~CRelationModel()=default;

	static CRelationModel* getInstance(){
        static CRelationModel m_pInstance;
        return &m_pInstance;
    };
    uint32_t getRelationId(uint32_t nUserAId, uint32_t nUserBId, bool bAdd);
    bool updateRelation(uint32_t nRelationId, uint32_t nUpdateTime);
    bool removeRelation(uint32_t nRelationId);
    
private:
    CRelationModel ()= default;
    CRelationModel (const CRelationModel &)= delete;
    CRelationModel & operator=(const CRelationModel &)= delete;
    uint32_t addRelation(uint32_t nSmallId, uint32_t nBigId);
 };
#endif
