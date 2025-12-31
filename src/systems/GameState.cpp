// Copyright 2025 Polar Bear Green World
// GameState.cpp
// Implementation of persistent game state management with encrypted binary format

#include "GameState.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <vector>

// Simple encryption key (can be changed for different games)
static const char SAVE_KEY[] = "WhiteBearGreenWorld2025";
static const uint32_t MAGIC_HEADER = 0x57424757;  // "WBGW" in hex

// Simple CRC32 implementation for checksum
uint32_t crc32(const uint8_t* data, size_t length)
{
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; ++i)
    {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j)
        {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    return ~crc;
}

// XOR encryption/decryption (symmetric)
void xorCrypt(uint8_t* data, size_t length, const char* key)
{
    size_t keyLen = std::strlen(key);
    for (size_t i = 0; i < length; ++i)
    {
        data[i] ^= key[i % keyLen];
    }
}

// Helper to write a string to binary stream
void writeString(std::vector<uint8_t>& buffer, const std::string& str)
{
    uint32_t len = str.length();
    buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&len), reinterpret_cast<uint8_t*>(&len) + sizeof(len));
    buffer.insert(buffer.end(), str.begin(), str.end());
}

// Helper to read a string from binary stream
std::string readString(const uint8_t*& ptr, const uint8_t* end)
{
    if (ptr + sizeof(uint32_t) > end) return "";
    
    uint32_t len;
    std::memcpy(&len, ptr, sizeof(len));
    ptr += sizeof(len);
    
    if (ptr + len > end) return "";
    
    std::string str(reinterpret_cast<const char*>(ptr), len);
    ptr += len;
    return str;
}

GameState::GameState()
{
    // Initialize with default values
    reset();
}

void GameState::reset()
{
    maxHealth = 3;
    hearts = 3;
    abilities = Abilities();
    armors = Armors();
    spiritsMet.clear();
    bossesDefeated.clear();
    collectiblesPerStage.clear();
}

bool GameState::saveToFile(const std::string& filepath) const
{
    try
    {
        std::vector<uint8_t> buffer;
        
        // Write magic header
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&MAGIC_HEADER), 
                     reinterpret_cast<const uint8_t*>(&MAGIC_HEADER) + sizeof(MAGIC_HEADER));
        
        // Write version number for future compatibility
        uint32_t version = 1;
        buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&version), 
                     reinterpret_cast<uint8_t*>(&version) + sizeof(version));
        
        // Write player stats
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&maxHealth), 
                     reinterpret_cast<const uint8_t*>(&maxHealth) + sizeof(maxHealth));
        
        // Write abilities (as a bitfield for efficiency)
        uint8_t abilityFlags = 0;
        if (abilities.hasSlash) abilityFlags |= (1 << 0);
        if (abilities.hasClimb) abilityFlags |= (1 << 1);
        if (abilities.hasDash) abilityFlags |= (1 << 2);
        if (abilities.hasIceBreath) abilityFlags |= (1 << 3);
        buffer.push_back(abilityFlags);
        
        // Write armors (as a bitfield)
        uint8_t armorFlags = 0;
        if (armors.hasWater) armorFlags |= (1 << 0);
        if (armors.hasFire) armorFlags |= (1 << 1);
        if (armors.hasEarth) armorFlags |= (1 << 2);
        if (armors.hasWind) armorFlags |= (1 << 3);
        buffer.push_back(armorFlags);
        
        // Write spirits met
        uint32_t spiritCount = spiritsMet.size();
        buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&spiritCount), 
                     reinterpret_cast<uint8_t*>(&spiritCount) + sizeof(spiritCount));
        for (const auto& spirit : spiritsMet)
        {
            writeString(buffer, spirit);
        }
        
        // Write bosses defeated
        uint32_t bossCount = bossesDefeated.size();
        buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&bossCount), 
                     reinterpret_cast<uint8_t*>(&bossCount) + sizeof(bossCount));
        for (const auto& boss : bossesDefeated)
        {
            writeString(buffer, boss);
        }
        
        // Write collectibles per stage
        uint32_t stageCount = collectiblesPerStage.size();
        buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&stageCount), 
                     reinterpret_cast<uint8_t*>(&stageCount) + sizeof(stageCount));
        for (const auto& [stageName, collectibles] : collectiblesPerStage)
        {
            writeString(buffer, stageName);
            uint32_t collectibleCount = collectibles.size();
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&collectibleCount), 
                         reinterpret_cast<uint8_t*>(&collectibleCount) + sizeof(collectibleCount));
            for (const auto& collectible : collectibles)
            {
                writeString(buffer, collectible);
            }
        }
        
        // Calculate checksum of data (excluding header and version)
        uint32_t checksum = crc32(buffer.data() + 8, buffer.size() - 8);
        
        // Encrypt data (excluding magic header and version)
        xorCrypt(buffer.data() + 8, buffer.size() - 8, SAVE_KEY);
        
        // Write checksum at the end
        buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&checksum), 
                     reinterpret_cast<uint8_t*>(&checksum) + sizeof(checksum));
        
        // Write to file
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open())
        {
            std::cerr << "Failed to open save file for writing: " << filepath << "\n";
            return false;
        }
        
        file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        file.close();
        
        std::cout << "Game state saved to: " << filepath << "\n";
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error saving game state: " << e.what() << "\n";
        return false;
    }
}

bool GameState::loadFromFile(const std::string& filepath)
{
    try
    {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
        {
            std::cerr << "Save file not found: " << filepath << "\n";
            return false;
        }
        
        // Read entire file
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        if (fileSize < 16)  // Minimum size: header + version + checksum
        {
            std::cerr << "Invalid save file: too small\n";
            return false;
        }
        
        std::vector<uint8_t> buffer(fileSize);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        file.close();
        
        // Verify magic header
        uint32_t magic;
        std::memcpy(&magic, buffer.data(), sizeof(magic));
        if (magic != MAGIC_HEADER)
        {
            std::cerr << "Invalid save file: wrong magic header\n";
            return false;
        }
        
        // Read version
        uint32_t version;
        std::memcpy(&version, buffer.data() + 4, sizeof(version));
        if (version != 1)
        {
            std::cerr << "Unsupported save file version: " << version << "\n";
            return false;
        }
        
        // Extract checksum from end
        uint32_t storedChecksum;
        std::memcpy(&storedChecksum, buffer.data() + fileSize - sizeof(uint32_t), sizeof(uint32_t));
        
        // Decrypt data (excluding header, version, and checksum)
        size_t dataSize = fileSize - 8 - sizeof(uint32_t);
        xorCrypt(buffer.data() + 8, dataSize, SAVE_KEY);
        
        // Verify checksum
        uint32_t calculatedChecksum = crc32(buffer.data() + 8, dataSize);
        if (calculatedChecksum != storedChecksum)
        {
            std::cerr << "Save file corrupted or tampered: checksum mismatch\n";
            return false;
        }
        
        // Parse data
        const uint8_t* ptr = buffer.data() + 8;
        const uint8_t* end = buffer.data() + fileSize - sizeof(uint32_t);
        
        // Read player stats
        if (ptr + sizeof(int) > end) return false;
        std::memcpy(&maxHealth, ptr, sizeof(maxHealth));
        ptr += sizeof(maxHealth);
        
        // Read abilities
        if (ptr + 1 > end) return false;
        uint8_t abilityFlags = *ptr++;
        abilities.hasSlash = (abilityFlags & (1 << 0)) != 0;
        abilities.hasClimb = (abilityFlags & (1 << 1)) != 0;
        abilities.hasDash = (abilityFlags & (1 << 2)) != 0;
        abilities.hasIceBreath = (abilityFlags & (1 << 3)) != 0;
        
        // Read armors
        if (ptr + 1 > end) return false;
        uint8_t armorFlags = *ptr++;
        armors.hasWater = (armorFlags & (1 << 0)) != 0;
        armors.hasFire = (armorFlags & (1 << 1)) != 0;
        armors.hasEarth = (armorFlags & (1 << 2)) != 0;
        armors.hasWind = (armorFlags & (1 << 3)) != 0;
        
        // Read spirits met
        if (ptr + sizeof(uint32_t) > end) return false;
        uint32_t spiritCount;
        std::memcpy(&spiritCount, ptr, sizeof(spiritCount));
        ptr += sizeof(spiritCount);
        
        spiritsMet.clear();
        for (uint32_t i = 0; i < spiritCount; ++i)
        {
            std::string spirit = readString(ptr, end);
            if (spirit.empty() && i < spiritCount - 1) return false;
            spiritsMet.insert(spirit);
        }
        
        // Read bosses defeated
        if (ptr + sizeof(uint32_t) > end) return false;
        uint32_t bossCount;
        std::memcpy(&bossCount, ptr, sizeof(bossCount));
        ptr += sizeof(bossCount);
        
        bossesDefeated.clear();
        for (uint32_t i = 0; i < bossCount; ++i)
        {
            std::string boss = readString(ptr, end);
            if (boss.empty() && i < bossCount - 1) return false;
            bossesDefeated.insert(boss);
        }
        
        // Read collectibles per stage
        if (ptr + sizeof(uint32_t) > end) return false;
        uint32_t stageCount;
        std::memcpy(&stageCount, ptr, sizeof(stageCount));
        ptr += sizeof(stageCount);
        
        collectiblesPerStage.clear();
        for (uint32_t i = 0; i < stageCount; ++i)
        {
            std::string stageName = readString(ptr, end);
            if (stageName.empty()) return false;
            
            if (ptr + sizeof(uint32_t) > end) return false;
            uint32_t collectibleCount;
            std::memcpy(&collectibleCount, ptr, sizeof(collectibleCount));
            ptr += sizeof(collectibleCount);
            
            for (uint32_t j = 0; j < collectibleCount; ++j)
            {
                std::string collectible = readString(ptr, end);
                if (collectible.empty() && j < collectibleCount - 1) return false;
                collectiblesPerStage[stageName].insert(collectible);
            }
        }
        
        std::cout << "Game state loaded from: " << filepath << "\n";
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error loading game state: " << e.what() << "\n";
        return false;
    }
}

// Spirit tracking
void GameState::markSpiritMet(const std::string& spiritName)
{
    spiritsMet.insert(spiritName);
}

bool GameState::hasMeetSpirit(const std::string& spiritName) const
{
    return spiritsMet.find(spiritName) != spiritsMet.end();
}

// Boss tracking
void GameState::markBossDefeated(const std::string& stageName, const std::string& bossName)
{
    std::string key = stageName + ":" + bossName;
    bossesDefeated.insert(key);
}

bool GameState::isBossDefeated(const std::string& stageName, const std::string& bossName) const
{
    std::string key = stageName + ":" + bossName;
    return bossesDefeated.find(key) != bossesDefeated.end();
}

// Collectible tracking
void GameState::markCollectibleFound(const std::string& stageName, const std::string& collectibleId)
{
    collectiblesPerStage[stageName].insert(collectibleId);
}

bool GameState::isCollectibleFound(const std::string& stageName, const std::string& collectibleId) const
{
    auto it = collectiblesPerStage.find(stageName);
    if (it == collectiblesPerStage.end())
        return false;
    
    return it->second.find(collectibleId) != it->second.end();
}
