//
//  Player.cpp
//  ClashStats
//
//  Created by Atlas on 12/12/15.
//  Copyright © 2015 JTJ. All rights reserved.
//

#include "Player.hpp"
#include "PlayerWarRecord.hpp"
#include "AttackRecord.hpp"
#include "DefendRecord.hpp"
#include "WarRecord.hpp"
#include "Database.hpp"
#include "PlayerRecord.hpp"
#include <iostream>
#include <fstream>

PlayerAttackSummary::PlayerAttackSummary()
: m_TotalAttacks(0)
, m_TotalStars(0)
, m_TotalPctDmg(0)
, m_TotalThreeStars(0)
, m_TotalNearMiss(0)			// 95% dmg >

// same TH vs TH level
, m_NumSaltAttacks(0)
, m_NumSaltStars(0)
, m_NumSaltPctDmg(0)
, m_NumSaltThreeStars(0)
, m_NumSaltFirstAttempts(0)		// 3-star on the first attempt

// same TH vs TH level
, m_NumBootAttacks(0)
, m_NumBootStars(0)
, m_NumBootPctDmg(0)
, m_NumBootThreeStars(0)
, m_NumBootFirstAttempts(0)

// player TH > opponent TH
, m_NumNukeAttacks(0)
, m_NumNukeStars(0)
, m_NumNukePctDmg(0)
, m_NumNukeThreeStars(0)
, m_NumNukeFirstAttempts(0)

// player TH < opponent TH
, m_NumSnipeAttacks(0)
, m_NumSnipeStars(0)
, m_NumSnipePctDmg(0)
, m_NumSnipeThreeStars(0)
, m_NumSnipeFirstAttempts(0)
{
	
}


PlayerDefendSummary::PlayerDefendSummary()
: m_TotalDefends(0)
, m_TotalStarsYielded(0)
, m_TotalDamage(0)
, m_TotalThreeStars(0)
, m_TotalMisses(0)

, m_NumPeerAttacks(0)
, m_NumPeerStarsYielded(0)
, m_NumPeerDamage(0)
, m_NumPeerThreeStars(0)
, m_NumPeerMisses(0)

, m_NumSnipeAttacks(0)
, m_NumSnipeStarsYielded(0)
, m_NumSnipeDamage(0)
, m_NumSnipeThreeStars(0)
, m_NumsnipeMisses(0)

, m_NumNukeAttacks(0)
, m_NumNukeStarsYielded(0)
, m_NumNukeDamage(0)
, m_NumNukeThreeStars(0)
, m_NumNukeMisses(0)
{
	
}

PlayerWarSummary::PlayerWarSummary()
: m_TotalWars(0)
, m_TotalCloserStars(0)
, m_TotalHolds(0)
, m_TotalBleeds(0)
, m_TotalNuked(0)
, m_TotalStars(0)
, m_TotalThreeStars(0)
{
	
}

////////////////////////////////////////////////////////////////////////////////

Player::Player()
: m_Key(0)
, m_Tag("")
, m_Name("")
{
	
}

Player::Player(const int pk)
{
	PlayerRecord rec;
	DATABASE::GetInstance().ReadPlayerRecord(pk, rec);
	
	m_Name = rec.playerName;
	m_Tag  = rec.playerTag;
	m_Key  = rec.pk;
}

Player::~Player()
{
	Reset();
	m_Name.clear();
}

void Player::Reset()
{
	m_WarRecordList.clear();
	m_AttackRecordList.clear();
	m_DefendRecordList.clear();
	
	for (int i = eTownHallLevel::kTH0; i < eTownHallLevel::kTH11; ++i)
	{
		memset(&m_AttackSummary[i], 0, sizeof(PlayerAttackSummary));
		memset(&m_DefendSummary[i], 0, sizeof(PlayerDefendSummary));
		memset(&m_WarSummary[i], 0, sizeof(PlayerWarSummary));
	}
}

void Player::GenerateSeasonHistoryWithMeta(std::string warMeta)
{
	Reset();
	
	DATABASE::GetInstance().ReadWarAttackData(m_Tag, warMeta, m_AttackRecordList);
	GenerateAttackSummaryData();
	
	DATABASE::GetInstance().ReadWarDefendData(m_Tag, warMeta, m_DefendRecordList);
	GenerateDefendSummaryData();
	
	DATABASE::GetInstance().ReadWarRecordData(m_Tag, warMeta, m_WarRecordList);
	GenerateWarSummaryData();
}

void Player::GenerateHistoryWithName(std::string name)
{
	m_Name = name;
	
	DATABASE::GetInstance().ReadAllPlayerAttackData(m_Name, m_AttackRecordList);
	GenerateAttackSummaryData();
	
	DATABASE::GetInstance().ReadAllPlayerDefendData(m_Name, m_DefendRecordList);
	GenerateDefendSummaryData();
	
	DATABASE::GetInstance().ReadAllPlayerWarRecordData(m_Name, m_WarRecordList);
	GenerateWarSummaryData();
}

void Player::GenerateAttackSummaryData()
{
	for (int i = 0; i < m_AttackRecordList.size(); ++i)
	{
		// get the attack data
		AttackRecord &rec = m_AttackRecordList[i];
		
		// find the appropriate player TH summary
		PlayerAttackSummary &summary = m_AttackSummary[rec.playerTH];
		
		summary.m_TotalAttacks++;
		summary.m_TotalStars		+= rec.starCount;
		summary.m_TotalPctDmg		+= rec.percentDmg;
		summary.m_TotalThreeStars	+= (rec.starCount == MAX_STARS_PER_ATTACK) ? 1 : 0;
		summary.m_TotalNearMiss		+= (rec.starCount != MAX_STARS_PER_ATTACK && rec.percentDmg >= NEAR_MISS_PCT_THRESHOLD) ? 1 : 0;
		summary.m_TotalCloserStars	+= (rec.isClose) ? MAX_STARS_PER_ATTACK : 0;
		
		if (rec.playerTH == rec.opponentTH)
		{
			if (rec.isSalt)
			{
				summary.m_NumSaltAttacks++;
				summary.m_NumSaltStars			+= rec.starCount;
				summary.m_NumSaltPctDmg			+= rec.percentDmg;
				summary.m_NumSaltThreeStars		+= (rec.starCount == MAX_STARS_PER_ATTACK) ? 1 : 0;
				summary.m_NumSaltFirstAttempts	+= (rec.isClose && rec.attemptNum == 1) ? 1 : 0;
				summary.m_NumSaltCloserStars	+= (rec.isClose) ? MAX_STARS_PER_ATTACK : 0;
			}
			else
			{
				summary.m_NumBootAttacks++;
				summary.m_NumBootStars			+= rec.starCount;
				summary.m_NumBootPctDmg			+= rec.percentDmg;
				summary.m_NumBootThreeStars		+= (rec.starCount == MAX_STARS_PER_ATTACK) ? 1 : 0;
				summary.m_NumBootFirstAttempts	+= (rec.isClose && rec.attemptNum == 1) ? 1 : 0;
				summary.m_NumBootCloserStars	+= (rec.isClose) ? MAX_STARS_PER_ATTACK : 0;
			}
		}
		// snipe
		else if (rec.playerTH < rec.opponentTH)
		{
			summary.m_NumSnipeAttacks++;
			summary.m_NumSnipeStars				+= rec.starCount;
			summary.m_NumSnipePctDmg			+= rec.percentDmg;
			summary.m_NumSnipeThreeStars		+= (rec.starCount == MAX_STARS_PER_ATTACK) ? 1 : 0;
			summary.m_NumSnipeFirstAttempts		+= (rec.isClose && rec.attemptNum == 1) ? 1 : 0;
			summary.m_NumSnipeCloserStars		+= (rec.isClose) ? MAX_STARS_PER_ATTACK : 0;
		}
		// nuke
		else if (rec.playerTH > rec.opponentTH)
		{
			summary.m_NumNukeAttacks++;
			summary.m_NumNukeStars				+= rec.starCount;
			summary.m_NumNukePctDmg				+= rec.percentDmg;
			summary.m_NumNukeThreeStars			+= (rec.starCount == MAX_STARS_PER_ATTACK) ? 1 : 0;
			summary.m_NumNukeFirstAttempts		+= (rec.isClose && rec.attemptNum == 1) ? 1 : 0;
			summary.m_NumNukeCloserStars		+= (rec.isClose) ? MAX_STARS_PER_ATTACK : 0;
		}
	}
}

void Player::GenerateDefendSummaryData()
{
	for (int i = 0; i < m_DefendRecordList.size(); ++i)
	{
		DefendRecord &rec = m_DefendRecordList[i];
		
		PlayerDefendSummary &summary = m_DefendSummary[rec.playerTH];
		
		summary.m_TotalDefends++;
		summary.m_TotalStarsYielded		+= rec.starCount;
		summary.m_TotalDamage			+= rec.percentDmg;
		summary.m_TotalThreeStars		+= (rec.starCount == MAX_STARS_PER_ATTACK) ? 1 : 0;
		summary.m_TotalMisses			+= (rec.starCount == 0) ? 1 : 0;
		
		if (rec.playerTH == rec.opponentTH)
		{
			summary.m_NumPeerAttacks++;
			summary.m_NumPeerStarsYielded		+= rec.starCount;
			summary.m_NumPeerDamage				+= rec.percentDmg;
			summary.m_NumPeerThreeStars			+= (rec.starCount == MAX_STARS_PER_ATTACK) ? 1 : 0;
			summary.m_NumPeerMisses				+= (rec.starCount == 0) ? 1 : 0;
		}
		// hit-down (nuke)
		else if (rec.playerTH < rec.opponentTH)
		{
			summary.m_NumNukeAttacks++;
			summary.m_NumNukeStarsYielded		+= rec.starCount;
			summary.m_NumNukeDamage				+= rec.percentDmg;
			summary.m_NumNukeThreeStars			+= (rec.starCount == MAX_STARS_PER_ATTACK) ? 1 : 0;
			summary.m_NumNukeMisses				+= (rec.starCount == 0) ? 1 : 0;
		}
		// hit-up (snipe)
		else if (rec.playerTH > rec.opponentTH)
		{
			summary.m_NumSnipeAttacks++;
			summary.m_NumSnipeStarsYielded		+= rec.starCount;
			summary.m_NumSnipeDamage			+= rec.percentDmg;
			summary.m_NumSnipeThreeStars		+= (rec.starCount == MAX_STARS_PER_ATTACK) ? 1 : 0;
			summary.m_NumsnipeMisses			+= (rec.starCount == 0) ? 1 : 0;
		}
	}
}

void Player::GenerateWarSummaryData()
{
	for (int i = 0; i < m_WarRecordList.size(); ++i)
	{
		PlayerWarRecord &rec = m_WarRecordList[i];
		
		PlayerWarSummary &summary = m_WarSummary[rec.playerTH];
		
		summary.m_TotalWars++;
		summary.m_TotalCloserStars			+= rec.closerStars;
		summary.m_TotalHolds				+= rec.holds;
		summary.m_TotalBleeds				+= rec.bleeds;
		summary.m_TotalNuked				+= rec.nuked;
		summary.m_TotalStars				+= rec.totalStars;
		summary.m_TotalThreeStars			+= rec.threeStars;
	}
}

void Player::AddWarRecord(const PlayerWarRecord &record)
{
	m_WarRecordList.push_back(record);
}

void Player::AddAttackRecord(const AttackRecord &record)
{
	m_AttackRecordList.push_back(record);
}

void Player::AddDefendRecord(const DefendRecord &record)
{
	m_DefendRecordList.push_back(record);
}

void Player::WritePlayerStatsData(std::ofstream &outputFile)
{
	const std::string delimiter = ",";
	std::string zero = "0";
	
	for (int thLvl = eTownHallLevel::kTH7; thLvl <= eTownHallLevel::kTH11; ++thLvl)
	{
		if (m_AttackSummary[thLvl].m_TotalAttacks > 0)
		{
			outputFile	<< m_Name																					<< delimiter
						<< thLvl																					<< delimiter
						<< m_WarSummary[thLvl].m_TotalWars															<< delimiter
						<< m_WarSummary[thLvl].m_TotalCloserStars													<< delimiter
						<< m_WarSummary[thLvl].m_TotalStars															<< delimiter
						<< (float)m_WarSummary[thLvl].m_TotalStars / (float)m_WarSummary[thLvl].m_TotalWars			<< delimiter
						<< m_WarSummary[thLvl].m_TotalThreeStars													<< delimiter
						<< m_WarSummary[thLvl].m_TotalHolds															<< delimiter
						<< m_WarSummary[thLvl].m_TotalBleeds														<< delimiter
						<< m_WarSummary[thLvl].m_TotalNuked															<< delimiter;
			
			if (m_AttackSummary[thLvl].m_TotalAttacks)
			{
				outputFile	<< m_AttackSummary[thLvl].m_TotalAttacks													<< delimiter
							<< m_AttackSummary[thLvl].m_TotalCloserStars												<< delimiter
							<< m_AttackSummary[thLvl].m_TotalStars														<< delimiter
							<< (float)m_AttackSummary[thLvl].m_TotalStars / (float)m_AttackSummary[thLvl].m_TotalAttacks<< delimiter
							<< (float)m_AttackSummary[thLvl].m_TotalPctDmg / (float)m_AttackSummary[thLvl].m_TotalAttacks		<< delimiter
							<< m_AttackSummary[thLvl].m_TotalThreeStars													<< delimiter
							<< (float)m_AttackSummary[thLvl].m_TotalThreeStars / (float)m_AttackSummary[thLvl].m_TotalAttacks			<< delimiter
							<< m_AttackSummary[thLvl].m_TotalNearMiss													<< delimiter;
			}
			else
			{
				outputFile	<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter;
			}
			
			if (m_AttackSummary[thLvl].m_NumSaltAttacks)
			{
				outputFile	<< m_AttackSummary[thLvl].m_NumSaltAttacks													<< delimiter
							<< m_AttackSummary[thLvl].m_NumSaltCloserStars												<< delimiter
							<< m_AttackSummary[thLvl].m_NumSaltStars													<< delimiter
							<< (float)m_AttackSummary[thLvl].m_NumSaltStars / (float)m_AttackSummary[thLvl].m_NumSaltAttacks			<< delimiter
							<< m_AttackSummary[thLvl].m_NumSaltThreeStars												<< delimiter
							<< (float)m_AttackSummary[thLvl].m_NumSaltThreeStars / (float)m_AttackSummary[thLvl].m_NumSaltAttacks		<< delimiter
							<< m_AttackSummary[thLvl].m_NumSaltFirstAttempts											<< delimiter
							<< (float)m_AttackSummary[thLvl].m_NumSaltFirstAttempts / (float)m_AttackSummary[thLvl].m_NumSaltAttacks	<< delimiter
							<< (float)m_AttackSummary[thLvl].m_NumSaltPctDmg / (float)m_AttackSummary[thLvl].m_NumSaltAttacks			<< delimiter;
			}
			else
			{
				outputFile	<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter;
			}
			
			if (m_AttackSummary[thLvl].m_NumBootAttacks)
			{
				outputFile	<< m_AttackSummary[thLvl].m_NumBootAttacks													<< delimiter
							<< m_AttackSummary[thLvl].m_NumBootCloserStars												<< delimiter
							<< m_AttackSummary[thLvl].m_NumBootStars													<< delimiter
							<< (float)m_AttackSummary[thLvl].m_NumBootStars / (float)m_AttackSummary[thLvl].m_NumBootAttacks			<< delimiter
							<< m_AttackSummary[thLvl].m_NumBootThreeStars												<< delimiter
							<< (float)m_AttackSummary[thLvl].m_NumBootThreeStars / (float)m_AttackSummary[thLvl].m_NumBootAttacks		<< delimiter
							<< m_AttackSummary[thLvl].m_NumBootFirstAttempts											<< delimiter
							<< (float)m_AttackSummary[thLvl].m_NumBootFirstAttempts / (float)m_AttackSummary[thLvl].m_NumBootAttacks  << delimiter
							<< (float)m_AttackSummary[thLvl].m_NumBootPctDmg / (float)m_AttackSummary[thLvl].m_NumBootAttacks			<< delimiter;
			}
			else
			{
				outputFile	<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter;
			}
			
			if (m_AttackSummary[thLvl].m_NumNukeAttacks)
			{
				outputFile	<< m_AttackSummary[thLvl].m_NumNukeAttacks													<< delimiter
							<< m_AttackSummary[thLvl].m_NumNukeCloserStars												<< delimiter
							<< m_AttackSummary[thLvl].m_NumNukeStars													<< delimiter
							<< (float)m_AttackSummary[thLvl].m_NumNukeStars / (float)m_AttackSummary[thLvl].m_NumNukeAttacks			<< delimiter
							<< m_AttackSummary[thLvl].m_NumNukeThreeStars												<< delimiter
							<< (float)m_AttackSummary[thLvl].m_NumNukeThreeStars / (float)m_AttackSummary[thLvl].m_NumNukeAttacks		<< delimiter
							<< m_AttackSummary[thLvl].m_NumNukeFirstAttempts											<< delimiter
							<< (float)m_AttackSummary[thLvl].m_NumNukeFirstAttempts / (float)m_AttackSummary[thLvl].m_NumNukeAttacks	<< delimiter
							<< (float)m_AttackSummary[thLvl].m_NumNukePctDmg / (float)m_AttackSummary[thLvl].m_NumNukeAttacks			<< delimiter;
			}
			else
			{
				outputFile	<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter;
			}
			
			if (m_AttackSummary[thLvl].m_NumSnipeAttacks)
			{
				outputFile	<< m_AttackSummary[thLvl].m_NumSnipeAttacks													<< delimiter
							<< m_AttackSummary[thLvl].m_NumSnipeCloserStars												<< delimiter
							<< m_AttackSummary[thLvl].m_NumSnipeStars													<< delimiter
							<< (float)m_AttackSummary[thLvl].m_NumSnipeStars / (float)m_AttackSummary[thLvl].m_NumSnipeAttacks		<< delimiter
							<< m_AttackSummary[thLvl].m_NumSnipeThreeStars												<< delimiter
							<< (float)m_AttackSummary[thLvl].m_NumSnipeThreeStars / (float)m_AttackSummary[thLvl].m_NumSnipeAttacks	<< delimiter
							<< m_AttackSummary[thLvl].m_NumSnipeFirstAttempts											<< delimiter
							<< (float)m_AttackSummary[thLvl].m_NumSnipeFirstAttempts / (float)m_AttackSummary[thLvl].m_NumSnipeAttacks<< delimiter
							<< (float)m_AttackSummary[thLvl].m_NumSnipePctDmg / (float)m_AttackSummary[thLvl].m_NumSnipeAttacks		<< delimiter;
			}
			else
			{
				outputFile	<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter;
			}
			
			if (m_DefendSummary[thLvl].m_TotalDefends)
			{
				outputFile	<< m_DefendSummary[thLvl].m_TotalDefends													<< delimiter
							<< m_DefendSummary[thLvl].m_TotalStarsYielded												<< delimiter
							<< (float)m_DefendSummary[thLvl].m_TotalStarsYielded / (float)m_DefendSummary[thLvl].m_TotalDefends		<< delimiter
							<< (float)m_DefendSummary[thLvl].m_TotalDamage / (float)m_DefendSummary[thLvl].m_TotalDefends				<< delimiter
							<< m_DefendSummary[thLvl].m_TotalThreeStars													<< delimiter
							<< m_DefendSummary[thLvl].m_TotalMisses														<< delimiter;
			}
			else
			{
				outputFile	<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter;
			}
			
			if (m_DefendSummary[thLvl].m_NumPeerAttacks)
			{
				outputFile	<< m_DefendSummary[thLvl].m_NumPeerAttacks													<< delimiter
							<< m_DefendSummary[thLvl].m_NumPeerStarsYielded												<< delimiter
							<< m_DefendSummary[thLvl].m_NumPeerThreeStars												<< delimiter
							<< (float)m_DefendSummary[thLvl].m_NumPeerStarsYielded / (float)m_DefendSummary[thLvl].m_NumPeerAttacks	<< delimiter
							<< (float)m_DefendSummary[thLvl].m_NumPeerDamage / (float)m_DefendSummary[thLvl].m_NumPeerAttacks			<< delimiter
							<< m_DefendSummary[thLvl].m_NumPeerMisses													<< delimiter;
			}
			else
			{
				outputFile	<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter;
			}
			
			if (m_DefendSummary[thLvl].m_NumNukeAttacks)
			{
				outputFile	<< m_DefendSummary[thLvl].m_NumNukeAttacks													<< delimiter
							<< m_DefendSummary[thLvl].m_NumNukeStarsYielded												<< delimiter
							<< m_DefendSummary[thLvl].m_NumNukeThreeStars												<< delimiter
							<< (float)m_DefendSummary[thLvl].m_NumNukeStarsYielded / (float)m_DefendSummary[thLvl].m_NumNukeAttacks	<< delimiter
							<< (float)m_DefendSummary[thLvl].m_NumNukeDamage / (float)m_DefendSummary[thLvl].m_NumNukeAttacks			<< delimiter
							<< m_DefendSummary[thLvl].m_NumNukeMisses													<< delimiter;
			}
			else
			{
				outputFile	<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter;
			}
			
			if (m_DefendSummary[thLvl].m_NumSnipeAttacks)
			{
				outputFile	<< m_DefendSummary[thLvl].m_NumSnipeAttacks													<< delimiter
							<< m_DefendSummary[thLvl].m_NumSnipeStarsYielded											<< delimiter
							<< m_DefendSummary[thLvl].m_NumSnipeThreeStars												<< delimiter
							<< (float)m_DefendSummary[thLvl].m_NumSnipeStarsYielded / (float)m_DefendSummary[thLvl].m_NumSnipeAttacks	<< delimiter
							<< (float)m_DefendSummary[thLvl].m_NumSnipeDamage / (float)m_DefendSummary[thLvl].m_NumSnipeAttacks		<< delimiter
							<< m_DefendSummary[thLvl].m_NumsnipeMisses													<< delimiter;
			}
			else
			{
				outputFile	<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter
							<< 0 << delimiter;
			}
			
			outputFile	<< "\n";
		}
	}
}
