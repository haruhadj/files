// === C++ RPG Project: Turn-Based Text Game with Dynamic Scaling ===
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <filesystem> // C++17
#include <algorithm>
using namespace std;
namespace fs = std::filesystem;

vector<vector<string>> records; // Each record: {name, age, gender, class, level}

// === Inventory and Items ===
struct Item
{
    string name;
    string type;
    int value;
    string description;
    int quantity;

    Item(string n, string t, int v, string d, int q) 
        : name(n), type(t), value(v), description(d), quantity(q) {}
};

struct Weapon {
    string name;
    int power;
    string description;
    int level;

    Weapon(string n = "Basic Sword", int p = 0, string d = "", int lvl = 1)
        : name(n), power(p), description(d), level(lvl) {}

    void upgrade() {
        level++;
        power += 5;
        cout << name << " upgraded to Level " << level 
             << "! Power is now " << power << ".\n";
    }

    int getGoldCost() const {
        return 10 + (level * 10);
    }

    int getXPCost() const {
        return 20 + (level * 10);
    }
};


// === Weapon Manager ===
class WeaponManager {
public:
    static vector<Weapon> getWeaponsForClass(const string& className) {
        if (className == "Warrior") {
            return {
                Weapon("Iron Greatsword", 15, "+15 Attack power for Warriors"),
                Weapon("Steel Axe", 25, "Heavy weapon with great damage"),
                Weapon("Titan Hammer", 40, "A legendary hammer that shakes the earth!")
            };
        } else if (className == "Mage") {
            return {
                Weapon("Fire Wand", 12, "+12 Magic power for Mages"),
                Weapon("Arcane Staff", 20, "High mana channeling staff"),
                Weapon("Storm Tome", 35, "Unleashes lightning on enemies")
            };
        } else if (className == "Archer") {
            return {
                Weapon("Longbow", 13, "+13 Ranged attack for Archers"),
                Weapon("Crossbow", 18, "Faster, powerful ranged weapon"),
                Weapon("Falcon Recurve", 30, "A lightweight bow that fires with precision")
            };
        }
        return {};
    }

    static Weapon getWeaponByName(const string& name) {
        vector<string> classes = {"Warrior", "Mage", "Archer"};
        for (const auto& cls : classes) {
            for (const auto& w : getWeaponsForClass(cls)) {
                if (w.name == name) return w;
            }
        }
        return Weapon(); // fallback
    }
};


// === Base Character Class ===
class Character
{
protected:
    string name;
    int level, hp, maxHP, mana, maxMana, xp, gold;
    Weapon weapon;
    int baseAttack; // New: base attack power

public:
    Character(string n) : name(n), level(1), hp(100), maxHP(100), mana(50), maxMana(50), xp(0), gold(0), weapon("Basic Sword", 0, "A simple sword"), baseAttack(10) {}
    virtual ~Character() {}
    virtual void attack() = 0;
    virtual string getClass() const = 0;
    virtual int getClassMultiplier() const = 0; // New: class-specific damage multiplier

    Weapon getWeapon() const { return weapon; }

    void showStats() const
    {
        cout << "Name: " << name << " (" << getClass() << ")\n"
             << "Level: " << level << "  HP: " << hp << "/" << maxHP << "  Mana: " << mana << "/" << maxMana << "\n"
             << "XP: " << xp << "  Gold: " << gold << "\n"
             << "Weapon: " << weapon.name << " (+" << weapon.power << " AP) - " << weapon.description << "\n";
    }

    // New: Calculate dynamic attack power
    int getAttackPower() const {
        return baseAttack + weapon.power + (level * getClassMultiplier());
    }

    void equipWeapon(const Weapon& newWeapon) {
        weapon = newWeapon;
        cout << name << " equipped " << weapon.name << "! Attack Power is now " << getAttackPower() << "\n";
    }

    // Getters/Setters
    string getName() const { return name; }
    int getHP() const { return hp; }
    void setHP(int h) { 
        hp = h; 
        if (hp > maxHP) hp = maxHP;
        if (hp < 0) hp = 0;
    }
    int getMaxHP() const { return maxHP; }
    int getMana() const { return mana; }
    void setMana(int m) { 
        mana = m; 
        if (mana > maxMana) mana = maxMana;
        if (mana < 0) mana = 0;
    }
    int getMaxMana() const { return maxMana; }
    int getXP() const { return xp; }
    void setXP(int x) { xp = x; }
    void addXP(int x) {
        xp += x;
        cout << "Gained " << x << " XP!\n";
        if (xp >= getXPNeeded()) levelUp();
    }
    int getGold() const { return gold; }
    void setGold(int g) { gold = g; }
    void addGold(int g) { 
        gold += g; 
        if (g > 0) cout << "Gained " << g << " gold!\n";
    }
    void setLevel(int l) { 
        level = l; 
        maxHP = getBaseHP() + ((level - 1) * 20);
        maxMana = getBaseMana() + ((level - 1) * 10);
        if (hp > maxHP) hp = maxHP;
        if (mana > maxMana) mana = maxMana;
    }
    void setWeapon(string w) { weapon = w; }
    int getLevel() const { return level; }

    // New: XP needed for next level scales with level
    int getXPNeeded() const {
        return 100 + (level * 25);
    }

    // New: Get base stats for each class
    virtual int getBaseHP() const = 0;
    virtual int getBaseMana() const = 0;

    void levelUp() {
        level++;
        int oldMaxHP = maxHP;
        int oldMaxMana = maxMana;
        
        maxHP = getBaseHP() + ((level - 1) * 20);
        maxMana = getBaseMana() + ((level - 1) * 10);
        
        // Heal player on level up (partial)
        hp += (maxHP - oldMaxHP) + 10; // Get the HP increase plus 10 bonus
        mana += (maxMana - oldMaxMana) + 5; // Get mana increase plus 5 bonus
        
        if (hp > maxHP) hp = maxHP;
        if (mana > maxMana) mana = maxMana;
        
        xp = 0;
        cout << "\n*** LEVEL UP! ***\n";
        cout << "You are now level " << level << "!\n";
        cout << "Max HP increased to " << maxHP << "\n";
        cout << "Max Mana increased to " << maxMana << "\n";
        cout << "Attack Power increased to " << getAttackPower() << "\n";
        cout << "Next level requires " << getXPNeeded() << " XP\n";
    }

    // New: Rest function to restore HP/Mana
    void rest() {
        hp = maxHP;
        mana = maxMana;
        cout << "You rest and recover all HP and Mana.\n";
    }
};

// === Derived Classes ===
class Warrior : public Character
{
public:
    Warrior(string n) : Character(n) {
        hp = maxHP = 120;
        mana = maxMana = 30;
        baseAttack = 15; // Higher base attack
    }
    void attack() override {
        cout << name << " swings sword with brute force!\n";
    }
    string getClass() const override { return "Warrior"; }
    int getClassMultiplier() const override { return 3; } // High damage scaling
    int getBaseHP() const override { return 120; }
    int getBaseMana() const override { return 30; }
};

class Mage : public Character
{
public:
    Mage(string n) : Character(n) {
        hp = maxHP = 80;
        mana = maxMana = 100;
        baseAttack = 8; // Lower base attack
    }
    void attack() override {
        if (mana >= 5) {
            cout << name << " casts a fireball!\n";
            mana -= 5;
        } else {
            cout << name << " swings staff (low mana)!\n";
        }
    }
    string getClass() const override { return "Mage"; }
    int getClassMultiplier() const override { return 4; } // Highest damage scaling (magic power)
    int getBaseHP() const override { return 80; }
    int getBaseMana() const override { return 100; }
};

class Archer : public Character
{
public:
    Archer(string n) : Character(n) {
        hp = maxHP = 90;
        mana = maxMana = 60;
        baseAttack = 12; // Balanced attack
    }
    void attack() override {
        cout << name << " shoots a precise arrow!\n";
    }
    string getClass() const override { return "Archer"; }
    int getClassMultiplier() const override { return 2; } // Moderate damage scaling
    int getBaseHP() const override { return 90; }
    int getBaseMana() const override { return 60; }
};

// === Enemy ===
class Enemy
{
public:
    string name;
    int hp, maxHP, attackPower;
    int level;

    Enemy(string n, int playerLevel) : name(n), level(playerLevel) {
        // Dynamic scaling based on player level
        maxHP = hp = 40 + (playerLevel * 15);
        attackPower = 8 + (playerLevel * 3);
        
        // Add some variety to different enemy types
        if (name == "Goblin") {
            maxHP = hp = 35 + (playerLevel * 12);
            attackPower = 6 + (playerLevel * 2);
        } else if (name == "Orc") {
            maxHP = hp = 55 + (playerLevel * 20);
            attackPower = 10 + (playerLevel * 4);
        } else if (name == "Dragon") {
            maxHP = hp = 100 + (playerLevel * 35);
            attackPower = 15 + (playerLevel * 6);
        }
    }

    void show() const {
        cout << "Enemy: " << name << " (Level " << level << ")  HP: " << hp << "/" << maxHP << "\n";
    }

    void showStats() const {
        cout << "Enemy: " << name << " (Level " << level << ")\n";
        cout << "HP: " << hp << "/" << maxHP << "\n";
        cout << "Attack Power: " << attackPower << "\n";
    }

    // New: Get different enemy types based on player level
    static string getEnemyType(int playerLevel) {
        vector<string> enemies;
        
        enemies.push_back("Goblin");
        if (playerLevel >= 2) enemies.push_back("Orc");
        if (playerLevel >= 5) enemies.push_back("Dragon");
        
        return enemies[rand() % enemies.size()];
    }
};



vector<Item> inventory;

// === Helper function to add items to inventory with stacking ===
void addItemToInventory(const Item& newItem) {
    // Check if item already exists in inventory
    for (auto& item : inventory) {
        if (item.name == newItem.name && item.type == newItem.type) {
            item.quantity += newItem.quantity;
            cout << "Added " << newItem.quantity << "x " << newItem.name << " to inventory. Total: " << item.quantity << "\n";
            return;
        }
    }
    // Item doesn't exist, add new entry
    inventory.push_back(newItem);
    cout << "Added " << newItem.quantity << "x " << newItem.name << " to inventory.\n";
}

void displayPlayerStats(shared_ptr<Character> player)
{
    player->showStats();
}

void inventoryMenu(shared_ptr<Character> player)
{
    while (true)
    {
        displayPlayerStats(player);
        if (inventory.empty())
        {
            cout << "\nInventory is empty.\n";
            return;
        }

        cout << "\n-- Inventory --\n";
        for (size_t i = 0; i < inventory.size(); ++i)
        {
            cout << i + 1 << ". " << inventory[i].name
                 << " x" << inventory[i].quantity << " (" << inventory[i].type << ") - "
                 << inventory[i].description << "\n";
        }
        cout << inventory.size() + 1 << ". Back\n";

        int choice;
        cout << "Choose an item to manage: ";
        cin >> choice;

        if (choice == inventory.size() + 1)
            return;

        if (choice < 1 || choice > inventory.size())
            continue;

        Item& item = inventory[choice - 1];

        cout << "\nSelected: " << item.name << " x" << item.quantity << "\n"
             << "1. Use\n"
             << "2. Sell (" << item.value / 2 << " gold each)\n"
             << "3. Discard\n"
             << "4. Back\n> ";

        int action;
        cin >> action;
        switch (action)
        {
        case 1: // Use
            if (item.type == "Health") {
                int oldHP = player->getHP();
                player->setHP(player->getHP() + item.value);
                cout << "You used the " << item.name << " and restored " << (player->getHP() - oldHP) << " HP.\n";
            } else if (item.type == "Mana") {
                int oldMana = player->getMana();
                player->setMana(player->getMana() + item.value);
                cout << "You used the " << item.name << " and restored " << (player->getMana() - oldMana) << " Mana.\n";
            } else if (item.type == "Weapon") {
                Weapon newWep = WeaponManager::getWeaponByName(item.name);
                player->equipWeapon(newWep);
            }
            item.quantity--;
            if (item.quantity <= 0) {
                inventory.erase(inventory.begin() + (choice - 1));
            }
            break;

        case 2: // Sell
            cout << "How many do you want to sell? (1-" << item.quantity << "): ";
            int sellAmount;
            cin >> sellAmount;
            if (sellAmount < 1 || sellAmount > item.quantity) {
                cout << "Invalid amount.\n";
                break;
            }
            player->addGold((item.value / 2) * sellAmount);
            item.quantity -= sellAmount;
            cout << "You sold " << sellAmount << "x " << item.name << " for " << (item.value / 2) * sellAmount << " gold.\n";
            if (item.quantity <= 0) {
                inventory.erase(inventory.begin() + (choice - 1));
            }
            break;

        case 3: // Discard
            cout << "How many do you want to discard? (1-" << item.quantity << "): ";
            int discardAmount;
            cin >> discardAmount;
            if (discardAmount < 1 || discardAmount > item.quantity) {
                cout << "Invalid amount.\n";
                break;
            }
            cout << "Are you sure you want to discard " << discardAmount << "x " << item.name << "? (y/n): ";
            char confirm;
            cin >> confirm;
            if (confirm == 'y' || confirm == 'Y')
            {
                item.quantity -= discardAmount;
                cout << "You discarded " << discardAmount << "x " << item.name << ".\n";
                if (item.quantity <= 0) {
                    inventory.erase(inventory.begin() + (choice - 1));
                }
            }
            break;

        case 4:
        default:
            break;
        }
    }
}

void upgradeWeaponMenu(shared_ptr<Character> player) {
    vector<int> weaponIndices;

    cout << "\n-- Weapon Upgrade Menu --\n";
    for (size_t i = 0; i < inventory.size(); ++i) {
        if (inventory[i].type == "Weapon") {
            weaponIndices.push_back(i);
            Weapon w = WeaponManager::getWeaponByName(inventory[i].name);
            cout << weaponIndices.size() << ". " << w.name 
                 << " (Power: " << w.power << ", Level: " << w.level << ") - " 
                 << w.description << "\n";
        }
    }

    if (weaponIndices.empty()) {
        cout << "You have no weapons to upgrade.\n";
        return;
    }

    cout << weaponIndices.size() + 1 << ". Back\n> ";
    int choice;
    cin >> choice;

    if (choice < 1 || choice > weaponIndices.size()) return;

    int index = weaponIndices[choice - 1];
    Item& item = inventory[index];
    Weapon w = WeaponManager::getWeaponByName(item.name);

    int goldCost = w.getGoldCost();
    int xpCost = w.getXPCost();

    cout << "Upgrade " << w.name << " to Level " << (w.level + 1) << "?\n";
    cout << "Cost: " << goldCost << " Gold, " << xpCost << " XP\n";
    cout << "Proceed? (y/n): ";
    char confirm;
    cin >> confirm;

    if (confirm != 'y' && confirm != 'Y') return;

    if (player->getGold() < goldCost || player->getXP() < xpCost) {
        cout << "Not enough resources to upgrade.\n";
        return;
    }

    // Deduct resources and apply upgrade
    player->addGold(-goldCost);
    player->setXP(player->getXP() - xpCost);
    w.upgrade();

    // Update inventory item
    item.value = w.power;
    item.description = w.description + " (Level " + to_string(w.level) + ")";
    cout << "Upgrade complete!\n";
}


// === Battle ===
void battle(shared_ptr<Character> player)
{
    displayPlayerStats(player);
    
    // Dynamic enemy selection and scaling
    string enemyType = Enemy::getEnemyType(player->getLevel());
    Enemy enemy(enemyType, player->getLevel());
    
    cout << "\nA wild " << enemy.name << " appears!\n";
    enemy.showStats();

    while (enemy.hp > 0 && player->getHP() > 0)
    {
        cout << "\nYour Turn:\n1. Attack\n2. Use Item\n3. Run Away\n> ";
        int action;
        cin >> action;
        
        if (action == 1)
        {
            player->attack();
            
            // Dynamic damage calculation
            int baseDamage = player->getAttackPower();
            int variation = rand() % 6 - 2; // -2 to +3 variation
            int finalDamage = max(1, baseDamage + variation);
            
            enemy.hp -= finalDamage;
            cout << "You deal " << finalDamage << " damage!\n";
            
            if (enemy.hp < 0) enemy.hp = 0;
        }
        else if (action == 2)
        {
            inventoryMenu(player);
            continue; // Don't let enemy attack if using item
        }
        else if (action == 3)
        {
            cout << "\nYou try to run away...\n";
            int escapeChance = 60 + (player->getLevel() * 5); // Higher level = better escape chance
            if (rand() % 100 < escapeChance)
            {
                cout << "You successfully escaped!\n";
                return;
            }
            else
            {
                cout << "You failed to escape! The enemy attacks you!\n";
            }
        }
        
        // Enemy's turn (if still alive)
        if (enemy.hp > 0)
        {
            int enemyDamage = enemy.attackPower + (rand() % 4 - 1); // Small variation
            enemyDamage = max(1, enemyDamage);
            
            cout << "\n" << enemy.name << " attacks you for " << enemyDamage << " damage!\n";
            player->setHP(player->getHP() - enemyDamage);
            
            if (player->getHP() <= 0)
            {
                cout << "\nGame Over! You have been defeated.\n";
                cout << "You lost half your gold in the defeat.\n";
                player->setGold(player->getGold() / 2);
                player->setHP(1); // Don't actually kill the player
                return;
            }
        }

        // Display current status
        cout << "\n--- Battle Status ---\n";
        cout << "Your HP: " << player->getHP() << "/" << player->getMaxHP() << "\n";
        cout << "Enemy HP: " << enemy.hp << "/" << enemy.maxHP << "\n";
    }
    
    if (player->getHP() > 0 && enemy.hp <= 0)
    {
        cout << "\nVictory! You defeated the " << enemy.name << "!\n";
        
        // Dynamic rewards based on enemy level
        int xpReward = 30 + (enemy.level * 20);
        int goldReward = 15 + (enemy.level * 10);
        
        // Bonus for fighting higher level enemies
        if (enemy.name == "Dragon") {
            xpReward = (int)(xpReward * 1.5);
            goldReward = (int)(goldReward * 1.5);
        }
        
        player->addXP(xpReward);
        player->addGold(goldReward);
        
         // Chance for item drops
        int dropChance = 25 + (enemy.level * 5);
        if (rand() % 100 < dropChance) {
            Item drop("Health Potion", "Health", 30 + (enemy.level * 5), 
                     "Restores " + to_string(30 + (enemy.level * 5)) + " HP", 1);
            if (rand() % 2 == 0) {
                drop = Item("Mana Potion", "Mana", 20 + (enemy.level * 3), 
                           "Restores " + to_string(20 + (enemy.level * 3)) + " Mana", 1);
            }
            
            addItemToInventory(drop);
            cout << "The " << enemy.name << " dropped a " << drop.name << "!\n";
        }
    }
}

// === Shop ===
void shop(shared_ptr<Character> player)
{
    displayPlayerStats(player);
    
    // Dynamic shop prices based on player level
    int healthPotionCost = 10 + (player->getLevel() * 2);
    int manaPotionCost = 8 + (player->getLevel() * 2);
    int healthPotionValue = 30 + (player->getLevel() * 5);
    int manaPotionValue = 20 + (player->getLevel() * 3);
    
    cout << "\n-- Shop --\n";
    cout << "1. Health Potion (" << healthPotionCost << "g) - Restores " << healthPotionValue << " HP\n";
    cout << "2. Mana Potion (" << manaPotionCost << "g) - Restores " << manaPotionValue << " Mana\n";
    cout << "3. Rest at Inn (5g) - Restore all HP and Mana\n";

    // Class-specific weapons
    auto classWeapons = WeaponManager::getWeaponsForClass(player->getClass());
    for (size_t i = 0; i < classWeapons.size(); ++i) {
        cout << 4 + i << ". " << classWeapons[i].name << " (30g) - " << classWeapons[i].description << "\n";
    }
    cout << 4 + classWeapons.size() << ". Exit\n> ";
    
    int choice;
    cin >> choice;

    int weaponIndex;
    
    if (choice == 1 && player->getGold() >= healthPotionCost)
    {
        Item healthPotion("Health Potion", "Health", healthPotionValue, 
                         "Restores " + to_string(healthPotionValue) + " HP", 1);
        addItemToInventory(healthPotion);
        player->addGold(-healthPotionCost);
        cout << "Purchased Health Potion!\n";
    }
    else if (choice == 2 && player->getGold() >= manaPotionCost)
    {
        Item manaPotion("Mana Potion", "Mana", manaPotionValue, 
                       "Restores " + to_string(manaPotionValue) + " Mana", 1);
        addItemToInventory(manaPotion);
        player->addGold(-manaPotionCost);
        cout << "Purchased Mana Potion!\n";
    }
    else if (choice == 3 && player->getGold() >= 5)
    {
        player->addGold(-5);
        player->rest();
    }
    else if (choice == 4)
        weaponIndex = choice - 4;
        if (weaponIndex >= 0 && weaponIndex < classWeapons.size()) {
            if (player->getGold() >= 30) {
                Weapon chosen = classWeapons[weaponIndex];
                addItemToInventory(Item(chosen.name, "Weapon", chosen.power, chosen.description, 1));
                player->addGold(-30);
                cout << "Purchased " << chosen.name << "!\n";
            } else {
                cout << "Not enough gold.\n";
            }
        } else if (choice == 4 + classWeapons.size()) {
            return; // Exit
        } else {
            cout << "Invalid choice.\n";
        }
}

// === Save / Load ===
void saveGame(shared_ptr<Character> player, const string& age, const string& gender)
{
    ofstream file(player->getName() + ".txt");
    if (!file.is_open()) {
        cout << "Error: Could not save game.\n";
        return;
    }
    
    file << player->getName() << "\n";
    file << player->getClass() << "\n";
    file << player->getLevel() << "\n";
    file << player->getHP() << "\n";
    file << player->getMaxHP() << "\n";
    file << player->getMana() << "\n";
    file << player->getMaxMana() << "\n";
    file << player->getGold() << "\n";
    file << player->getXP() << "\n";
    file << player->getWeapon().name << "," 
         << player->getWeapon().power << "," 
         << player->getWeapon().description << "," 
         << player->getWeapon().level << "\n"; // NEW
    file << age << "\n";
    file << gender << "\n";

    // Save inventory with quantities
    file << inventory.size() << "\n";
    for (const auto& item : inventory) {
        file << item.name << "," << item.type << "," << item.value << "," 
            << item.description << "," << item.quantity;

        if (item.type == "Weapon") {
            Weapon w = WeaponManager::getWeaponByName(item.name);
            file << "," << w.level;
        }
        file << "\n";
    }

    file.close();
    cout << "Game saved successfully.\n";
}


shared_ptr<Character> loadGame(const string &username, string& age, string& gender)
{
    ifstream file(username + ".txt");
    if (!file.is_open()) {
        cout << "Error: Save file not found for username '" << username << "'.\n";
        return nullptr;
    }

    string name, classType;
    int level, hp, maxHP, mana, maxMana, gold, xp;

    file >> name >> classType >> level >> hp >> maxHP >> mana >> maxMana >> gold >> xp;

    file.ignore();
    string weaponLine;
    getline(file, weaponLine);

    Weapon equippedWep;
    {
        istringstream iss(weaponLine);
        string name, powerStr, desc, levelStr;
        getline(iss, name, ',');
        getline(iss, powerStr, ',');
        getline(iss, desc, ',');
        getline(iss, levelStr);

        int power = stoi(powerStr);
        int level = stoi(levelStr);

        equippedWep = Weapon(name, power, desc, level);
    }


    file >> ws;
    getline(file, age);
    getline(file, gender);

    shared_ptr<Character> player;
    if (classType == "Warrior")
        player = make_shared<Warrior>(name);
    else if (classType == "Mage")
        player = make_shared<Mage>(name);
    else
        player = make_shared<Archer>(name);

    player->setLevel(level);
    player->setHP(hp);
    player->setMana(mana);
    player->setGold(gold);
    player->setXP(xp);

    player->equipWeapon(equippedWep);

    // Load inventory with quantities
    inventory.clear();
    int itemCount;
    file >> itemCount;
    file.ignore();

    for (int i = 0; i < itemCount; ++i)
    {
        string line;
        getline(file, line);
        if (line.empty()) continue;
        
        istringstream iss(line);
        string name, type, valStr, description, quantityStr = "1", levelStr = "1";
        getline(iss, name, ',');
        getline(iss, type, ',');
        getline(iss, valStr, ',');
        getline(iss, description, ',');
        getline(iss, quantityStr, ',');
        getline(iss, levelStr);
        
        try {
            int value = stoi(valStr);
            int quantity = quantityStr.empty() ? 1 : stoi(quantityStr);

            // If it's a weapon, recalculate power based on level
            if (type == "Weapon") {
                int level = levelStr.empty() ? 1 : stoi(levelStr);
                int basePower = value - (5 * (level - 1)); // reverse upgrade
                Weapon w(name, basePower, description, level);
                value = basePower + (level - 1) * 5; // recalc to match upgrade()
                description = w.description + " (Level " + to_string(level) + ")";
            }

            inventory.push_back(Item(name, type, value, description, quantity));

        } catch (const invalid_argument& e) {
            cout << "Warning: Invalid item data in save file, skipping item.\n";
        }
    }

    file.close();
    cout << "Game loaded successfully.\n";
    return player;
}

//////////////////////////////////////////////////////////////////////
void displayRecords() {
    cout << "\n[All Records]\n";
    if (records.empty()) {
        cout << "No records available.\n";
        return;
    }
    cout << "Index\tName\t\tAge\tGender\tClass\tLevel\n";
    for (size_t i = 0; i < records.size(); ++i) {
        cout << i + 1 << "\t" << records[i][0] << "\t\t" << records[i][1] << "\t" << records[i][2] << "\t\t" << records[i][3] << "\t" << records[i][4] << "\n";
    }
}

void editRecord() {
    displayRecords();
    if (records.empty()) return;
    
    int index;
    cout << "\nEnter record index to edit: ";
    cin >> index;
    if (index < 1 || index > records.size()) {
        cout << "Invalid index.\n";
        return;
    }
    index--;
    cout << "Editing record #" << index + 1 << ":\n";
    cout << "New Name (leave blank to keep current): ";
    cin.ignore();
    string name;
    getline(cin, name);
    if (!name.empty()) records[index][0] = name;

    cout << "New Age (leave blank to keep current): ";
    string age;
    getline(cin, age);
    if (!age.empty()) records[index][1] = age;

    cout << "New Gender (leave blank to keep current): ";
    string gender;
    getline(cin, gender);
    if (!gender.empty()) records[index][2] = gender;

    cout << "Record updated!\n";
}

void deleteRecord() {
    displayRecords();
    if (records.empty()) return;

    int index;
    cout << "\nEnter record index to delete: ";
    cin >> index;
    if (index < 1 || index > records.size()) {
        cout << "Invalid index.\n";
        return;
    }
    records.erase(records.begin() + (index - 1));
    cout << "Record deleted!\n";
}

void manageRecordsMenu() {
    while (true) {
        cout << "\n-- Manage Records --\n";
        cout << "1. Display Records\n";
        cout << "2. Edit Record\n";
        cout << "3. Delete Record\n";
        cout << "4. Back to Game Menu\n> ";
        int choice;
        cin >> choice;
        switch (choice) {
            case 1: displayRecords(); break;
            case 2: editRecord(); break;
            case 3: deleteRecord(); break;
            case 4: return;
            default: cout << "Invalid choice.\n";
        }
    }
}

void saveRecords() {
    ofstream file("records.txt");
    if (!file.is_open()) {
        cout << "Warning: Could not save records.\n";
        return;
    }
    
    for (const auto& record : records) {
        for (const auto& field : record) {
            file << field << "\t";
        }
        file << "\n";
    }
    file.close();
}

void loadRecords() {
    ifstream file("records.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            
            istringstream iss(line);
            vector<string> record;
            string field;
            while (iss >> field) {
                record.push_back(field);
            }
            if (record.size() >= 4) {
                records.push_back(record);
            }
        }
        file.close();
    }
}

vector<string> listSaveFiles() {
    vector<string> saves;
    
    try {
        if (!fs::exists(".")) {
            cout << "Debug: Current directory not accessible via filesystem.\n";
            return saves;
        }
        
        for (const auto& entry : fs::directory_iterator(".")) {
            if (entry.is_regular_file()) {
                string filename = entry.path().filename().string();
                string extension = entry.path().extension().string();
                
                if (extension == ".txt" && filename != "records.txt") {
                    string name = entry.path().stem().string();
                    saves.push_back(name);
                    cout << "Debug: Found save file: " << filename << "\n";
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        cout << "Debug: Filesystem error: " << e.what() << "\n";
        cout << "Falling back to simple file check method...\n";
        
        vector<string> commonNames = {"player1", "player2", "test", "save1", "save2"};
        for (const string& name : commonNames) {
            ifstream testFile(name + ".txt");
            if (testFile.is_open()) {
                saves.push_back(name);
                testFile.close();
            }
        }
    } catch (const exception& e) {
        cout << "Debug: General error in listSaveFiles: " << e.what() << "\n";
    }
    
    return saves;
}


//////////////////////////////////////////////////////////////////////

// === Menu ===
void mainMenu(shared_ptr<Character> player)
{
    while (true)
    {
        player->showStats();
        if (player->getHP() <= 0)
        {
            cout << "\nYou are badly wounded but not defeated. Rest at an inn to recover.\n";
        }

        cout << "\n-- Main Menu --\n";
        cout << "1. Battle\n";
        cout << "2. Inventory\n";
        cout << "3. Upgrade Weapon\n";
        cout << "4. Shop\n";
        cout << "5. Save\n";
        cout << "6. Exit\n";
        cout << "7. Manage Records\n";
        cout << "Next level needs " << (player->getXPNeeded() - player->getXP()) << " more XP\n> ";
        
        int choice;
        cin >> choice;
        string age = "?", gender = "?";
        
        switch (choice)
        {
        case 1:
            if (player->getHP() <= 0) {
                cout << "You are too wounded to fight! Visit the shop to rest.\n";
            } else {
                battle(player);
            }
            break;
        case 2:
            inventoryMenu(player);
            break;
        case 3:
            upgradeWeaponMenu(player);
            break;

        case 4:
            shop(player);
            break;
        case 5:
            for (const auto& r : records) {
                if (r[0] == player->getName()) {
                    age = r[1];
                    gender = r[2];
                    break;
                }
            }
            saveGame(player, age, gender);
            break;
        case 6:
            return;
        case 7:
            manageRecordsMenu();
            break;
        default:
            cout << "Invalid choice.\n";
        }
    }
}

int main()
{
    loadRecords();
    srand(time(0));
    cout << "=== RPG Game ===\n";
    cout << "1. New Game\n2. Load Game\n> ";
    int option;
    cin >> option;

    string name, age, gender;
    shared_ptr<Character> player;
    
    if (option == 1)
    {
        cout << "Enter your name: ";
        cin >> ws;
        getline(cin, name);

        cout << "Enter your age: ";
        getline(cin, age);

        cout << "Enter your gender: ";
        getline(cin, gender);

        cout << "Choose class:\n";
        cout << "1. Warrior (High HP, Strong attacks)\n";
        cout << "2. Mage (High Mana, Magical attacks)\n";
        cout << "3. Archer (Balanced, Precise attacks)\n> ";
        
        int cls;
        cin >> cls;
        if (cls == 1)
            player = make_shared<Warrior>(name);
        else if (cls == 2)
            player = make_shared<Mage>(name);
        else
            player = make_shared<Archer>(name);

        bool alreadyExists = false;
        for (const auto& r : records) {
            if (r[0] == name) {
                alreadyExists = true;
                break;
            }
        }
        if (!alreadyExists) {
            records.push_back({name, age, gender, player->getClass(), to_string(1)});
        }
    }
    else if (option == 2)
    {
        cout << "Searching for save files...\n";
        vector<string> saves = listSaveFiles();
        
        if (saves.empty()) {
            cout << "No save files found. Starting new game instead.\n";
            
            cout << "Enter your name: ";
            cin >> ws;
            getline(cin, name);

            cout << "Enter your age: ";
            getline(cin, age);

            cout << "Enter your gender: ";
            getline(cin, gender);

            cout << "Choose class:\n";
            cout << "1. Warrior (High HP, Strong attacks)\n";
            cout << "2. Mage (High Mana, Magical attacks)\n";
            cout << "3. Archer (Balanced, Precise attacks)\n> ";
            
            int cls;
            cin >> cls;
            if (cls == 1)
                player = make_shared<Warrior>(name);
            else if (cls == 2)
                player = make_shared<Mage>(name);
            else
                player = make_shared<Archer>(name);

            bool alreadyExists = false;
            for (const auto& r : records) {
                if (r[0] == name) {
                    alreadyExists = true;
                    break;
                }
            }
            if (!alreadyExists) {
                records.push_back({name, age, gender, player->getClass(), to_string(1)});
            }
        } else {
            cout << "Available save files:\n";
            for (size_t i = 0; i < saves.size(); ++i) {
                cout << i + 1 << ". " << saves[i] << "\n";
            }
            cout << "Select a save file: ";
            int choice;
            cin >> choice;
            if (choice < 1 || choice > saves.size()) {
                cout << "Invalid choice. Exiting.\n";
                return 1;
            }
            string selectedName = saves[choice - 1];
            player = loadGame(selectedName, age, gender);
            if (!player) {
                cout << "Failed to load save. Starting new game.\n";
                player = make_shared<Warrior>(selectedName);
            } else {
                bool found = false;
                for (auto& r : records) {
                    if (r[0] == selectedName) {
                        r[1] = age;
                        r[2] = gender;
                        r[3] = player->getClass();
                        r[4] = to_string(player->getLevel());
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    records.push_back({selectedName, age, gender, player->getClass(), to_string(player->getLevel())});
                }
            }
        }
    }
    else {
        cout << "Invalid option. Exiting.\n";
        return 1;
    }

    if (player) {
        cout << "\n=== Welcome to the RPG World! ===\n";
        cout << "Enemies and rewards scale with your level.\n";
        cout << "Good luck on your adventure!\n\n";
        
        mainMenu(player);
        saveRecords();
        
        // Find age/gender for final save
        for (const auto& r : records) {
            if (r[0] == player->getName()) {
                age = r[1];
                gender = r[2];
                break;
            }
        }
        saveGame(player, age, gender);
    }
    
    return 0;
}