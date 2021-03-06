//
//  ParserFactory.hpp
//  ClashStats
//
//  Created by Atlas on 11/14/15.
//  Copyright © 2015 JTJ. All rights reserved.
//

#ifndef ParserFactory_hpp
#define ParserFactory_hpp

#include "Singleton.h"

class Parser;

class ParserFactory
{
public:
	Parser* getParserForFile(const char *fileName);
	
protected:
};

typedef atSingleton<ParserFactory> PARSER;

#endif /* ParserFactory_hpp */
