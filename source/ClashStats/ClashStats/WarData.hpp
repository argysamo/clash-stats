//
//  WarData.hpp
//  ClashStats
//
//  Created by Atlas on 11/14/15.
//  Copyright © 2015 JTJ. All rights reserved.
//

#ifndef WarData_hpp
#define WarData_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "DataObject.hpp"
#include "PlayerData.hpp"

class WarData : public DataObject
{
public:
	
protected:
	int							m_PlayerCount;
	std::string					m_OpponentName;
	std::string					m_DateStr;
	std::vector<PlayerData*>	m_ThemList;
	std::vector<PlayerData*>	m_UsList;
};

#endif /* WarData_hpp */
