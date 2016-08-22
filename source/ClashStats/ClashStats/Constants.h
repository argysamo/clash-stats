//
//  Constants.h
//  ClashStats
//
//  Created by Atlas on 11/25/15.
//  Copyright © 2015 JTJ. All rights reserved.
//

#ifndef Constants_h
#define Constants_h

// town hall levels
enum eTownHallLevel
{
	kTH0 = 0,
	kTH1,
	kTH2,
	kTH3,
	kTH4,
	kTH5,
	kTH6,
	kTH7,
	kTH8,
	kTH9,
	kTH10,
	kTH11,
};

#define MAX_STARS_PER_WAR			(6)
#define MAX_STARS_PER_ATTACK		(3)
#define MIN_STARS_PER_ATTACK		(0)

#define MIN_PCT_DAMAGE				(0)
#define MAX_PCT_DAMAGE				(100)
#define NEAR_MISS_PCT_THRESHOLD		(95)

#define MIN_ATTACKER_ID				(1)

static int GetMaxTHWeight(const int thLvl)
{
	switch (thLvl)
	{
		case kTH11:			return 110;
		case kTH10:			return 90;
		case kTH9:			return 70;
		case kTH8:			return 60;
			
		case kTH7:
		default:			return 50;
	}
}

static int GetMinTHWeight(const int thLvl)
{
	switch (thLvl)
	{
		case kTH11:			return 100;
		case kTH10:			return 80;
		case kTH9:			return 60;
		case kTH8:			return 50;
			
		case kTH7:
		default:			return 0;
	}
}


#endif /* Constants_h */
