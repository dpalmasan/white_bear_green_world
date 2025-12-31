// Copyright 2025 Polar Bear Green World
// GameState.h
// Manages persistent game state including abilities, armor, bosses defeated, and collectibles

#pragma once

#include <string>
#include <set>
#include <unordered_map>

// Persistent game state that tracks player progress
class GameState
{
public:
    GameState();
    
    // Abilities
    struct Abilities
    {
        bool hasSlash = true;      // Basic attack (unlocked by default)
        bool hasClimb = false;     // Climbing ability
        bool hasDash = false;      // Dash ability
        bool hasIceBreath = false; // Ice breath ability
    };
    
    // Elemental armors
    struct Armors
    {
        bool hasWater = false;
        bool hasFire = false;
        bool hasEarth = false;
        bool hasWind = false;
    };
    
    // Player stats
    int maxHealth = 3;  // Maximum health (default 3 hearts)
    int hearts = 3;     // Current health (default 3 hearts)
    
    // Abilities unlocked
    Abilities abilities;
    
    // Armors obtained
    Armors armors;
    
    // Spirits met (to avoid duplicate events)
    // Key format: "spirit_<element>" e.g., "spirit_water", "spirit_wind"
    std::set<std::string> spiritsMet;
    
    // Bosses defeated
    // Key format: "boss_<stage>_<name>" e.g., "boss_snowy-cliffs-boss_snow-robot"
    std::set<std::string> bossesDefeated;
    
    // Collectibles found per stage
    // Key: stage name, Value: set of collectible IDs (e.g., "heart_1", "heart_2")
    std::unordered_map<std::string, std::set<std::string>> collectiblesPerStage;
    
    // Helper methods for abilities
    void unlockSlash() { abilities.hasSlash = true; }
    void unlockClimb() { abilities.hasClimb = true; }
    void unlockDash() { abilities.hasDash = true; }
    void unlockIceBreath() { abilities.hasIceBreath = true; }
    
    bool hasSlash() const { return abilities.hasSlash; }
    bool hasClimb() const { return abilities.hasClimb; }
    bool hasDash() const { return abilities.hasDash; }
    bool hasIceBreath() const { return abilities.hasIceBreath; }
    
    // Helper methods for armors
    void unlockWaterArmor() { armors.hasWater = true; }
    void unlockFireArmor() { armors.hasFire = true; }
    void unlockEarthArmor() { armors.hasEarth = true; }
    void unlockWindArmor() { armors.hasWind = true; }
    
    bool hasWaterArmor() const { return armors.hasWater; }
    bool hasFireArmor() const { return armors.hasFire; }
    bool hasEarthArmor() const { return armors.hasEarth; }
    bool hasWindArmor() const { return armors.hasWind; }
    
    // Health management
    void increaseMaxHearts() { maxHealth++; hearts = maxHealth; }
    int getCurrentHearts() const { return hearts; }
    int getMaxHearts() const { return maxHealth; }
    void setHearts(int h) { hearts = h; }
    void restoreHealth() { hearts = maxHealth; }
    
    // Spirit tracking
    void markSpiritMet(const std::string& element);
    bool hasMeetSpirit(const std::string& element) const;
    
    // Boss tracking
    void markBossDefeated(const std::string& stageName, const std::string& bossName);
    bool isBossDefeated(const std::string& stageName, const std::string& bossName) const;
    
    // Collectible tracking
    void markCollectibleFound(const std::string& stageName, const std::string& collectibleId);
    bool isCollectibleFound(const std::string& stageName, const std::string& collectibleId) const;
    std::set<std::string> getCollectiblesForStage(const std::string& stageName) const;
    
    // Health management
    void increaseMaxHealth(int amount = 1);
    int getMaxHealth() const { return maxHealth; }
    void setMaxHealth(int health) { maxHealth = health; }
    
    // Save/Load
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    
    // Reset all progress (new game)
    void reset();
};
