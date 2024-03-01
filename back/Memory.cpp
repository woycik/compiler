#include "Memory.hpp"

Memory::Memory() {}

Memory::~Memory() {}

void Memory::insertToMemory(long long newAddress, std::string parent, std::string identifier, bool isReference) {
    MemoryItem newItem(newAddress,parent, identifier,isReference);
    memoryArray.push_back(newItem);
    memoryPointer+=1;
}


long long Memory::getAddressForIdentifier(const std::string& parent, const std::string& identifier) {
    for (const auto& item : memoryArray) {
        if (item.identifier == identifier && item.parent == parent) {
            return item.address;
        }
    }
    std::cerr<<"Error, variable not declared"<<std::endl;
    exit(0);
    return -1;
}

void Memory::setMemoryItemAddress(const std::string& parent, const std::string& name, long long address) {
    if (memoryArray.empty()) {
        return;  
    }

    std::string lowercaseName = name;
    std::string lowercaseParent = parent;
    std::transform(lowercaseName.begin(), lowercaseName.end(), lowercaseName.begin(), ::tolower);
    std::transform(lowercaseParent.begin(), lowercaseParent.end(), lowercaseParent.begin(), ::tolower);

    auto foundItem = std::find_if(memoryArray.begin(), memoryArray.end(), [&](const MemoryItem& item) {
        std::string lowercaseIdentifier = item.identifier;
        std::transform(lowercaseIdentifier.begin(), lowercaseIdentifier.end(), lowercaseIdentifier.begin(), ::tolower);

        return lowercaseIdentifier == lowercaseName && item.parent == lowercaseParent;
    });

    if (foundItem != memoryArray.end()) {
        foundItem->address = address;
    }
}
long long Memory::getMemoryPointer(){
    return memoryPointer;
}


void Memory::setMemoryItemName(const std::string& parent, const std::string& currentName, const std::string& newName) {
    if (memoryArray.empty()) {
        return;  
    }

    std::string lowercaseParent = parent;
    std::transform(lowercaseParent.begin(), lowercaseParent.end(), lowercaseParent.begin(), ::tolower);

    auto foundItem = std::find_if(memoryArray.begin(), memoryArray.end(), [&](const MemoryItem& item) {
        return item.identifier == currentName && item.parent == lowercaseParent;
    });

    if (foundItem != memoryArray.end()) {
        foundItem->identifier = newName;
    }
}

std::vector<std::string> Memory::getItemsByParent(const std::string& parent) const {
        std::vector<std::string> result;

        std::string lowercaseParent = parent;
        std::transform(lowercaseParent.begin(), lowercaseParent.end(), lowercaseParent.begin(), ::tolower);

        for (const auto& item : memoryArray) {
            if (item.parent == lowercaseParent) {
                result.push_back(item.identifier);
            }
        }

        return result;
    }


bool Memory::isArgReference(const std::string& parent,const std::string& identifier){
    for(const auto& memoryItem : memoryArray){
        if(memoryItem.parent==parent && memoryItem.identifier==identifier){
            return memoryItem.isReference;
        }
    }
    return false;
}
void Memory::setArgReference(const std::string& parent,const std::string& identifier, bool reference){
    for(auto& memoryItem : memoryArray){
        if(memoryItem.parent==parent && memoryItem.identifier==identifier){
            memoryItem.isReference = reference;
            break;
        }
    }
}